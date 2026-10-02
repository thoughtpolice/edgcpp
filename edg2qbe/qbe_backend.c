#include "qbe_codegen.h"
#include "cmd_line.h"
#include "error.h"
#include "assemble.h"
#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>


static NORETURN void output_failure(const char *reason, const char *path)
{
  size_t length = strlen(reason) + strlen(path) + 3;
  char *message = alloc_general((sizeof_t)length);
  (void)snprintf(message, length, "%s: %s", reason, path);
  str_catastrophe(ec_qbe_backend_error, message);
}

/* Resolve new outputs as well as existing files, including dangling symlinks. */
static a_boolean canonical_output_depth(const char *path,
                                        char result[PATH_MAX], unsigned depth)
{
  char parent[PATH_MAX], link[PATH_MAX], expanded[PATH_MAX];
  const char *base;
  char *slash;
  size_t length = strlen(path);
  ssize_t link_length;
  if (depth == 40) return FALSE;
  if (realpath(path, result)) return TRUE;
  if (length >= sizeof(parent)) return FALSE;
  memcpy(parent, path, length + 1);
  slash = strrchr(parent, '/');
  if (slash) {
    base = path + (slash - parent) + 1;
    if (slash == parent) slash[1] = '\0';
    else *slash = '\0';
  } else {
    base = path;
    strcpy(parent, ".");
  }
  if (!realpath(parent, result)) return FALSE;
  link_length = readlink(path, link, sizeof(link) - 1);
  if (link_length >= 0) {
    link[link_length] = '\0';
    if (link[0] == '/')
      return canonical_output_depth(link, result, depth + 1);
    length = strlen(result);
    if (length + (size_t)link_length + 2 > sizeof(expanded)) return FALSE;
    memcpy(expanded, result, length);
    expanded[length++] = '/';
    memcpy(expanded + length, link, (size_t)link_length + 1);
    return canonical_output_depth(expanded, result, depth + 1);
  }
  length = strlen(result);
  if (length + strlen(base) + 2 > PATH_MAX) return FALSE;
  result[length++] = '/';
  strcpy(result + length, base);
  return TRUE;
}

static a_boolean canonical_output(const char *path, char result[PATH_MAX])
{
  return canonical_output_depth(path, result, 0);
}

static a_boolean paths_alias(const char *left, const char *right)
{
  char left_path[PATH_MAX], right_path[PATH_MAX];
  if (strcmp(left, right) == 0) return TRUE;
#if UNIQUE_FILE_IDENTIFIER_AVAILABLE
  {
    a_unique_file_id left_id, right_id;
    clear_unique_file_id(&left_id);
    clear_unique_file_id(&right_id);
    get_unique_id_for_file(left, &left_id);
    get_unique_id_for_file(right, &right_id);
    if (same_unique_file_ids(&left_id, &right_id)) return TRUE;
  }
#endif
  return canonical_output(left, left_path) &&
         canonical_output(right, right_path) &&
         strcmp(left_path, right_path) == 0;
}

static void save_qbe(FILE *input, FILE *output)
{
  FILE *saved;
  char chunk[8192];
  size_t n;
  if (fseek(input, 0, SEEK_SET) != 0) {
    (void)fclose(input);
    (void)fclose(output);
    output_failure("cannot rewind QBE output", il2qbe_saved_qbe_path);
  }
  saved = fopen(il2qbe_saved_qbe_path, "wb");
  if (!saved) {
    (void)fclose(input);
    (void)fclose(output);
    output_failure("cannot open saved QBE output", il2qbe_saved_qbe_path);
  }
  while ((n = fread(chunk, 1, sizeof(chunk), input)) != 0) {
    if (fwrite(chunk, 1, n, saved) != n) {
      (void)fclose(saved);
      (void)fclose(input);
      (void)fclose(output);
      output_failure("cannot write saved QBE output", il2qbe_saved_qbe_path);
    }
  }
  if (ferror(input)) {
    (void)fclose(saved);
    (void)fclose(input);
    (void)fclose(output);
    output_failure("cannot read QBE output", il2qbe_saved_qbe_path);
  }
  if (fclose(saved) != 0) {
    (void)fclose(input);
    (void)fclose(output);
    output_failure("cannot write saved QBE output", il2qbe_saved_qbe_path);
  }
}
void back_end(void)
{
  const char *output_name;
  a_boolean aliases_source;
  FILE *output;
  FILE *qbe_output;
  QbeBackend backend;

  if (!primary_source_file_name)
    str_catastrophe(ec_qbe_backend_error, "missing primary source file name");
  output_name = qbe_output_file_name;
  if (!output_name)
    output_name = derived_name(primary_source_file_name,
                               il2qbe_emit_qbe_only ? ".qbe" : ".S");
  aliases_source = paths_alias(output_name, primary_source_file_name);
  if (aliases_source)
    str_catastrophe(ec_qbe_backend_error,
                    "output file aliases the input source file");
  if (il2qbe_saved_qbe_path &&
      (paths_alias(il2qbe_saved_qbe_path, primary_source_file_name) ||
       paths_alias(il2qbe_saved_qbe_path, output_name)))
    str_catastrophe(ec_qbe_backend_error,
                    "saved QBE file aliases the source or assembly output");
  output = fopen(output_name, "wb");
  if (!output)
    output_failure("cannot open output file", output_name);
  qbe_output = il2qbe_emit_qbe_only ? output : tmpfile();
  if (!qbe_output) {
    (void)fclose(output);
    output_failure("cannot create backend temporary stream", output_name);
  }
  qbe_backend_init(&backend, qbe_output);
  qbe_module(&backend);
  qbe_backend_destroy(&backend);
  if (ferror(qbe_output) || fflush(qbe_output) != 0) {
    if (qbe_output != output) (void)fclose(qbe_output);
    (void)fclose(output);
    output_failure("cannot write QBE output", output_name);
  }
  if (il2qbe_saved_qbe_path) save_qbe(qbe_output, output);
  if (!il2qbe_emit_qbe_only) {
    if (fseek(qbe_output, 0, SEEK_SET) != 0) {
      (void)fclose(qbe_output);
      (void)fclose(output);
      output_failure("cannot rewind backend temporary stream", output_name);
    }
    if (il2qbe_assemble(qbe_output, output) != 0) {
      (void)fclose(qbe_output);
      (void)fclose(output);
      output_failure("embedded QBE assembly generation failed", output_name);
    }
    if (fclose(qbe_output) != 0) {
      (void)fclose(output);
      output_failure("cannot close backend temporary stream", output_name);
    }
  }
  if (fclose(output) != 0)
    output_failure("cannot write output file", output_name);
}
