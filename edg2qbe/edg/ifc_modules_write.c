/*
Part of the EDG Compiler Project, under the Apache License v2.0 with LLVM
Exceptions.
See https://edgcpp.org/LICENSE.txt for license information.
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
*/

/*

ifc_modules_write.c -- IFC writing code.

*/

/* Header files common to all files. */
#include "fe_common.h"

#include <errno.h>

/* Additional header files. */
#include "ifc_modules.h"
#include "ifc_map_functions.h"
#include "ifc_modules_internal.h"
#include "il_def.h"
#include "il_walk.h"


/* Writer-owned storage has explicit lifetime, including scope-member arrays. */
typedef Edg_array an_ifc_output_buffer;
typedef Edg_array a_scope_member_array;
typedef uint32_t a_seq_number_key;
typedef struct an_ifc_output_partition {
  size_t part_name_offset, element_size;
  an_ifc_output_buffer contents;
  a_byte *content_start;
} an_ifc_output_partition;
typedef struct an_ifc_output_state {
  an_ifc_module_file *output_file;
  an_ifc_scope_offset global_scope;
  an_ifc_output_buffer string_table;
  an_ifc_unit_sort unit_sort;
  a_boolean internal_partition;
  size_t module_name_offset, source_file_name_offset;
  an_ifc_output_partition *partitions[IFC_PARTITION_COUNT];
} an_ifc_output_state;
typedef struct an_ifc_output_token_cache {
  Edg_array basic_tokens, complex_tokens;
} an_ifc_output_token_cache;
typedef struct an_ifc_il_map {
  an_ifc_output_state *output_state;
  Edg_map string_table_map, textual_token_map, src_file_map, seq_number_map;
  Edg_map type_map, scope_map, il_entry_to_decl;
  an_ifc_type_index fund_class_type, fund_namespace_type, fund_alias_typedef_type;
  an_ifc_type_index fund_scoped_enum_type, fund_struct_type, fund_union_type;
  an_ifc_type_index fund_unscoped_enum_type;
  Edg_array scope_members;
} an_ifc_il_map;
typedef struct an_ifc_output_partition_metadata {
  an_ifc_partition_kind kind;
  size_t relative_offset;
} an_ifc_output_partition_metadata;
typedef struct an_ifc_output_metadata {
  Edg_array used_partitions;
  size_t num_partition_bytes, num_string_table_bytes;
} an_ifc_output_metadata;
typedef struct an_ifc_output_stream {
  an_ifc_module_file *output_file;
  a_sha256_hash hash_state;
#if CHECKING
  a_boolean checksum_computed;
#endif
} an_ifc_output_stream;

NORETURN static void ifc_write_catastrophe(an_error_code reason)
{
  a_diagnostic_ptr diag = start_catastrophe(ec_ifc_creation_failure);
  add_diag_info(diag, reason);
  end_diagnostic(diag);
  exit_compilation(es_internal_error);
}

static void writer_partition_init(an_ifc_output_partition *p,
                                  size_t name, size_t element_size)
{
  p->part_name_offset = name;
  p->element_size = element_size;
  p->content_start = NULL;
  edg_array_init(&p->contents, sizeof(a_byte), EDG_ALLOC_GENERAL, 0, NULL, NULL);
}
static void writer_partition_destroy(an_ifc_output_partition *p)
{
  if (p != NULL) {
    edg_array_destroy(&p->contents);
    free_general((char *)p, sizeof(*p));
  }
}
static size_t writer_partition_new_elements(an_ifc_output_partition *p,
                                           size_t num_nodes)
{
  size_t orig_size = p->contents.len;
  edg_array_resize(&p->contents, orig_size + p->element_size * num_nodes);
  p->content_start = p->contents.data;
  return orig_size / p->element_size;
}
static size_t writer_add_string_bytes(an_ifc_output_state *s,
                                     a_const_char *bytes, size_t num_bytes)
{
  size_t start = s->string_table.len;
  edg_array_insert(&s->string_table, start, bytes, num_bytes);
  return start;
}
static size_t writer_add_string(an_ifc_output_state *s, a_const_char *text)
{
  return writer_add_string_bytes(s, text, strlen(text) + 1);
}
static size_t writer_add_string_view(an_ifc_output_state *s, a_string_view text)
{
  size_t start = writer_add_string_bytes(s, text.data, text.len);
  a_byte zero = 0;
  edg_array_push(&s->string_table, &zero);
  return start;
}
static void writer_state_init(an_ifc_output_state *s, an_ifc_module_file *file)
{
  an_ifc_module_file_write_state *write_state;
  memset(s, 0, sizeof(*s));
  s->output_file = file;
  s->unit_sort = ifc_us_header;
  edg_array_init(&s->string_table, sizeof(a_byte), EDG_ALLOC_GENERAL, 0, NULL, NULL);
  (void)writer_add_string_bytes(s, "", 1);
  write_state = an_ifc_module_file_get_write_state(file);
  if (write_state->source_module != NULL) {
    a_module *src_mod = write_state->source_module;
    if (src_mod->kind == mk_unit) s->unit_sort = ifc_us_primary;
    else if (src_mod->kind == mk_unit_partition) s->unit_sort = ifc_us_partition;
    if (src_mod->kind == mk_unit || src_mod->kind == mk_unit_partition) {
      s->module_name_offset = writer_add_string_view(s, module_name_of(src_mod));
    }
    if (src_mod->variant.unit_partition.is_internal) {
      s->internal_partition = TRUE;
    }
  }
  if (write_state->source_file_name != NULL) {
    s->source_file_name_offset = writer_add_string(s, write_state->source_file_name);
  }
}
static void writer_state_destroy(an_ifc_output_state *s)
{
  size_t i;
  for (i = 0; i < IFC_PARTITION_COUNT; ++i) {
    writer_partition_destroy(s->partitions[i]);
    s->partitions[i] = NULL;
  }
  edg_array_destroy(&s->string_table);
}
static an_ifc_output_partition *writer_get_or_init_partition(
                       an_ifc_output_state *s, an_ifc_partition_kind kind)
{
  an_ifc_output_partition *p = s->partitions[kind - 1];
  if (p == NULL) {
    size_t element_size = get_ifc_partition_element_size_an_ifc_partition_kind(s->output_file, kind);
    size_t name = writer_add_string_view(s, get_partition_name_from_kind(kind));
    p = (an_ifc_output_partition *)alloc_general(sizeof(*p));
    writer_partition_init(p, name, element_size);
    s->partitions[kind - 1] = p;
  }
  return p;
}
typedef struct writer_node_ops {
  an_ifc_partition_kind (*kind)(void);
  void (*bind)(void *, an_ifc_module_file *, a_byte **, size_t);
} writer_node_ops;
static void writer_bind_an_ifc_chart_unilevel(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_chart_unilevel *)out = an_ifc_chart_unilevel_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_chart_unilevel = {
  get_ifc_partition_kind_an_ifc_chart_unilevel, writer_bind_an_ifc_chart_unilevel
};
static void writer_bind_an_ifc_decl_alias(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_decl_alias *)out = an_ifc_decl_alias_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_decl_alias = {
  get_ifc_partition_kind_an_ifc_decl_alias, writer_bind_an_ifc_decl_alias
};
static void writer_bind_an_ifc_decl_barren(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_decl_barren *)out = an_ifc_decl_barren_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_decl_barren = {
  get_ifc_partition_kind_an_ifc_decl_barren, writer_bind_an_ifc_decl_barren
};
static void writer_bind_an_ifc_decl_constructor(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_decl_constructor *)out = an_ifc_decl_constructor_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_decl_constructor = {
  get_ifc_partition_kind_an_ifc_decl_constructor, writer_bind_an_ifc_decl_constructor
};
static void writer_bind_an_ifc_decl_destructor(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_decl_destructor *)out = an_ifc_decl_destructor_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_decl_destructor = {
  get_ifc_partition_kind_an_ifc_decl_destructor, writer_bind_an_ifc_decl_destructor
};
static void writer_bind_an_ifc_decl_enumeration(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_decl_enumeration *)out = an_ifc_decl_enumeration_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_decl_enumeration = {
  get_ifc_partition_kind_an_ifc_decl_enumeration, writer_bind_an_ifc_decl_enumeration
};
static void writer_bind_an_ifc_decl_enumerator(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_decl_enumerator *)out = an_ifc_decl_enumerator_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_decl_enumerator = {
  get_ifc_partition_kind_an_ifc_decl_enumerator, writer_bind_an_ifc_decl_enumerator
};
static void writer_bind_an_ifc_decl_field(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_decl_field *)out = an_ifc_decl_field_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_decl_field = {
  get_ifc_partition_kind_an_ifc_decl_field, writer_bind_an_ifc_decl_field
};
static void writer_bind_an_ifc_decl_function(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_decl_function *)out = an_ifc_decl_function_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_decl_function = {
  get_ifc_partition_kind_an_ifc_decl_function, writer_bind_an_ifc_decl_function
};
static void writer_bind_an_ifc_decl_method(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_decl_method *)out = an_ifc_decl_method_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_decl_method = {
  get_ifc_partition_kind_an_ifc_decl_method, writer_bind_an_ifc_decl_method
};
static void writer_bind_an_ifc_decl_parameter(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_decl_parameter *)out = an_ifc_decl_parameter_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_decl_parameter = {
  get_ifc_partition_kind_an_ifc_decl_parameter, writer_bind_an_ifc_decl_parameter
};
static void writer_bind_an_ifc_decl_scope(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_decl_scope *)out = an_ifc_decl_scope_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_decl_scope = {
  get_ifc_partition_kind_an_ifc_decl_scope, writer_bind_an_ifc_decl_scope
};
static void writer_bind_an_ifc_decl_template(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_decl_template *)out = an_ifc_decl_template_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_decl_template = {
  get_ifc_partition_kind_an_ifc_decl_template, writer_bind_an_ifc_decl_template
};
static void writer_bind_an_ifc_dir_using(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_dir_using *)out = an_ifc_dir_using_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_dir_using = {
  get_ifc_partition_kind_an_ifc_dir_using, writer_bind_an_ifc_dir_using
};
static void writer_bind_an_ifc_edg_constant_integer(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_edg_constant_integer *)out = an_ifc_edg_constant_integer_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_edg_constant_integer = {
  get_ifc_partition_kind_an_ifc_edg_constant_integer, writer_bind_an_ifc_edg_constant_integer
};
static void writer_bind_an_ifc_edg_constant_integer_word(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_edg_constant_integer_word *)out = an_ifc_edg_constant_integer_word_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_edg_constant_integer_word = {
  get_ifc_partition_kind_an_ifc_edg_constant_integer_word, writer_bind_an_ifc_edg_constant_integer_word
};
static void writer_bind_an_ifc_edg_extension_type(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_edg_extension_type *)out = an_ifc_edg_extension_type_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_edg_extension_type = {
  get_ifc_partition_kind_an_ifc_edg_extension_type, writer_bind_an_ifc_edg_extension_type
};
static void writer_bind_an_ifc_edg_heap_complex_token(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_edg_heap_complex_token *)out = an_ifc_edg_heap_complex_token_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_edg_heap_complex_token = {
  get_ifc_partition_kind_an_ifc_edg_heap_complex_token, writer_bind_an_ifc_edg_heap_complex_token
};
static void writer_bind_an_ifc_edg_heap_template_argument(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_edg_heap_template_argument *)out = an_ifc_edg_heap_template_argument_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_edg_heap_template_argument = {
  get_ifc_partition_kind_an_ifc_edg_heap_template_argument, writer_bind_an_ifc_edg_heap_template_argument
};
static void writer_bind_an_ifc_edg_template_argument_non_type(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_edg_template_argument_non_type *)out = an_ifc_edg_template_argument_non_type_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_edg_template_argument_non_type = {
  get_ifc_partition_kind_an_ifc_edg_template_argument_non_type, writer_bind_an_ifc_edg_template_argument_non_type
};
static void writer_bind_an_ifc_edg_template_argument_template(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_edg_template_argument_template *)out = an_ifc_edg_template_argument_template_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_edg_template_argument_template = {
  get_ifc_partition_kind_an_ifc_edg_template_argument_template, writer_bind_an_ifc_edg_template_argument_template
};
static void writer_bind_an_ifc_edg_template_argument_type(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_edg_template_argument_type *)out = an_ifc_edg_template_argument_type_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_edg_template_argument_type = {
  get_ifc_partition_kind_an_ifc_edg_template_argument_type, writer_bind_an_ifc_edg_template_argument_type
};
static void writer_bind_an_ifc_edg_token_basic(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_edg_token_basic *)out = an_ifc_edg_token_basic_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_edg_token_basic = {
  get_ifc_partition_kind_an_ifc_edg_token_basic, writer_bind_an_ifc_edg_token_basic
};
static void writer_bind_an_ifc_edg_token_cache(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_edg_token_cache *)out = an_ifc_edg_token_cache_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_edg_token_cache = {
  get_ifc_partition_kind_an_ifc_edg_token_cache, writer_bind_an_ifc_edg_token_cache
};
static void writer_bind_an_ifc_edg_token_constant(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_edg_token_constant *)out = an_ifc_edg_token_constant_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_edg_token_constant = {
  get_ifc_partition_kind_an_ifc_edg_token_constant, writer_bind_an_ifc_edg_token_constant
};
static void writer_bind_an_ifc_edg_token_identifier(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_edg_token_identifier *)out = an_ifc_edg_token_identifier_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_edg_token_identifier = {
  get_ifc_partition_kind_an_ifc_edg_token_identifier, writer_bind_an_ifc_edg_token_identifier
};
static void writer_bind_an_ifc_edg_token_textual(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_edg_token_textual *)out = an_ifc_edg_token_textual_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_edg_token_textual = {
  get_ifc_partition_kind_an_ifc_edg_token_textual, writer_bind_an_ifc_edg_token_textual
};
static void writer_bind_an_ifc_edg_trait_class_template_definition(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_edg_trait_class_template_definition *)out = an_ifc_edg_trait_class_template_definition_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_edg_trait_class_template_definition = {
  get_ifc_partition_kind_an_ifc_edg_trait_class_template_definition, writer_bind_an_ifc_edg_trait_class_template_definition
};
static void writer_bind_an_ifc_edg_trait_function_definition(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_edg_trait_function_definition *)out = an_ifc_edg_trait_function_definition_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_edg_trait_function_definition = {
  get_ifc_partition_kind_an_ifc_edg_trait_function_definition, writer_bind_an_ifc_edg_trait_function_definition
};
static void writer_bind_an_ifc_edg_type_substituted(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_edg_type_substituted *)out = an_ifc_edg_type_substituted_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_edg_type_substituted = {
  get_ifc_partition_kind_an_ifc_edg_type_substituted, writer_bind_an_ifc_edg_type_substituted
};
static void writer_bind_an_ifc_heap_type(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_heap_type *)out = an_ifc_heap_type_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_heap_type = {
  get_ifc_partition_kind_an_ifc_heap_type, writer_bind_an_ifc_heap_type
};
static void writer_bind_an_ifc_name_source_file(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_name_source_file *)out = an_ifc_name_source_file_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_name_source_file = {
  get_ifc_partition_kind_an_ifc_name_source_file, writer_bind_an_ifc_name_source_file
};
static void writer_bind_an_ifc_scope_descriptor(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_scope_descriptor *)out = an_ifc_scope_descriptor_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_scope_descriptor = {
  get_ifc_partition_kind_an_ifc_scope_descriptor, writer_bind_an_ifc_scope_descriptor
};
static void writer_bind_an_ifc_scope_member(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_scope_member *)out = an_ifc_scope_member_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_scope_member = {
  get_ifc_partition_kind_an_ifc_scope_member, writer_bind_an_ifc_scope_member
};
static void writer_bind_an_ifc_source_line(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_source_line *)out = an_ifc_source_line_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_source_line = {
  get_ifc_partition_kind_an_ifc_source_line, writer_bind_an_ifc_source_line
};
static void writer_bind_an_ifc_type_designated(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_type_designated *)out = an_ifc_type_designated_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_type_designated = {
  get_ifc_partition_kind_an_ifc_type_designated, writer_bind_an_ifc_type_designated
};
static void writer_bind_an_ifc_type_forall(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_type_forall *)out = an_ifc_type_forall_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_type_forall = {
  get_ifc_partition_kind_an_ifc_type_forall, writer_bind_an_ifc_type_forall
};
static void writer_bind_an_ifc_type_function(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_type_function *)out = an_ifc_type_function_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_type_function = {
  get_ifc_partition_kind_an_ifc_type_function, writer_bind_an_ifc_type_function
};
static void writer_bind_an_ifc_type_fundamental(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_type_fundamental *)out = an_ifc_type_fundamental_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_type_fundamental = {
  get_ifc_partition_kind_an_ifc_type_fundamental, writer_bind_an_ifc_type_fundamental
};
static void writer_bind_an_ifc_type_lvalue_reference(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_type_lvalue_reference *)out = an_ifc_type_lvalue_reference_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_type_lvalue_reference = {
  get_ifc_partition_kind_an_ifc_type_lvalue_reference, writer_bind_an_ifc_type_lvalue_reference
};
static void writer_bind_an_ifc_type_method(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_type_method *)out = an_ifc_type_method_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_type_method = {
  get_ifc_partition_kind_an_ifc_type_method, writer_bind_an_ifc_type_method
};
static void writer_bind_an_ifc_type_pointer(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_type_pointer *)out = an_ifc_type_pointer_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_type_pointer = {
  get_ifc_partition_kind_an_ifc_type_pointer, writer_bind_an_ifc_type_pointer
};
static void writer_bind_an_ifc_type_qualified(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_type_qualified *)out = an_ifc_type_qualified_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_type_qualified = {
  get_ifc_partition_kind_an_ifc_type_qualified, writer_bind_an_ifc_type_qualified
};
static void writer_bind_an_ifc_type_rvalue_reference(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_type_rvalue_reference *)out = an_ifc_type_rvalue_reference_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_type_rvalue_reference = {
  get_ifc_partition_kind_an_ifc_type_rvalue_reference, writer_bind_an_ifc_type_rvalue_reference
};
static void writer_bind_an_ifc_type_syntactic(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_type_syntactic *)out = an_ifc_type_syntactic_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_type_syntactic = {
  get_ifc_partition_kind_an_ifc_type_syntactic, writer_bind_an_ifc_type_syntactic
};
static void writer_bind_an_ifc_type_tor(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_type_tor *)out = an_ifc_type_tor_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_type_tor = {
  get_ifc_partition_kind_an_ifc_type_tor, writer_bind_an_ifc_type_tor
};
static void writer_bind_an_ifc_type_tuple(void *out, an_ifc_module_file *file,
                            a_byte **start, size_t offset)
{
  *(an_ifc_type_tuple *)out = an_ifc_type_tuple_from_indirect(file, start, offset);
}
static const writer_node_ops writer_ops_an_ifc_type_tuple = {
  get_ifc_partition_kind_an_ifc_type_tuple, writer_bind_an_ifc_type_tuple
};
#define WRITER_NODE_OPS(node) _Generic((node), \
  an_ifc_chart_unilevel *: &writer_ops_an_ifc_chart_unilevel, \
  an_ifc_decl_alias *: &writer_ops_an_ifc_decl_alias, \
  an_ifc_decl_barren *: &writer_ops_an_ifc_decl_barren, \
  an_ifc_decl_constructor *: &writer_ops_an_ifc_decl_constructor, \
  an_ifc_decl_destructor *: &writer_ops_an_ifc_decl_destructor, \
  an_ifc_decl_enumeration *: &writer_ops_an_ifc_decl_enumeration, \
  an_ifc_decl_enumerator *: &writer_ops_an_ifc_decl_enumerator, \
  an_ifc_decl_field *: &writer_ops_an_ifc_decl_field, \
  an_ifc_decl_function *: &writer_ops_an_ifc_decl_function, \
  an_ifc_decl_method *: &writer_ops_an_ifc_decl_method, \
  an_ifc_decl_parameter *: &writer_ops_an_ifc_decl_parameter, \
  an_ifc_decl_scope *: &writer_ops_an_ifc_decl_scope, \
  an_ifc_decl_template *: &writer_ops_an_ifc_decl_template, \
  an_ifc_dir_using *: &writer_ops_an_ifc_dir_using, \
  an_ifc_edg_constant_integer *: &writer_ops_an_ifc_edg_constant_integer, \
  an_ifc_edg_constant_integer_word *: &writer_ops_an_ifc_edg_constant_integer_word, \
  an_ifc_edg_extension_type *: &writer_ops_an_ifc_edg_extension_type, \
  an_ifc_edg_heap_complex_token *: &writer_ops_an_ifc_edg_heap_complex_token, \
  an_ifc_edg_heap_template_argument *: &writer_ops_an_ifc_edg_heap_template_argument, \
  an_ifc_edg_template_argument_non_type *: &writer_ops_an_ifc_edg_template_argument_non_type, \
  an_ifc_edg_template_argument_template *: &writer_ops_an_ifc_edg_template_argument_template, \
  an_ifc_edg_template_argument_type *: &writer_ops_an_ifc_edg_template_argument_type, \
  an_ifc_edg_token_basic *: &writer_ops_an_ifc_edg_token_basic, \
  an_ifc_edg_token_cache *: &writer_ops_an_ifc_edg_token_cache, \
  an_ifc_edg_token_constant *: &writer_ops_an_ifc_edg_token_constant, \
  an_ifc_edg_token_identifier *: &writer_ops_an_ifc_edg_token_identifier, \
  an_ifc_edg_token_textual *: &writer_ops_an_ifc_edg_token_textual, \
  an_ifc_edg_trait_class_template_definition *: &writer_ops_an_ifc_edg_trait_class_template_definition, \
  an_ifc_edg_trait_function_definition *: &writer_ops_an_ifc_edg_trait_function_definition, \
  an_ifc_edg_type_substituted *: &writer_ops_an_ifc_edg_type_substituted, \
  an_ifc_heap_type *: &writer_ops_an_ifc_heap_type, \
  an_ifc_name_source_file *: &writer_ops_an_ifc_name_source_file, \
  an_ifc_scope_descriptor *: &writer_ops_an_ifc_scope_descriptor, \
  an_ifc_scope_member *: &writer_ops_an_ifc_scope_member, \
  an_ifc_source_line *: &writer_ops_an_ifc_source_line, \
  an_ifc_type_designated *: &writer_ops_an_ifc_type_designated, \
  an_ifc_type_forall *: &writer_ops_an_ifc_type_forall, \
  an_ifc_type_function *: &writer_ops_an_ifc_type_function, \
  an_ifc_type_fundamental *: &writer_ops_an_ifc_type_fundamental, \
  an_ifc_type_lvalue_reference *: &writer_ops_an_ifc_type_lvalue_reference, \
  an_ifc_type_method *: &writer_ops_an_ifc_type_method, \
  an_ifc_type_pointer *: &writer_ops_an_ifc_type_pointer, \
  an_ifc_type_qualified *: &writer_ops_an_ifc_type_qualified, \
  an_ifc_type_rvalue_reference *: &writer_ops_an_ifc_type_rvalue_reference, \
  an_ifc_type_syntactic *: &writer_ops_an_ifc_type_syntactic, \
  an_ifc_type_tor *: &writer_ops_an_ifc_type_tor, \
  an_ifc_type_tuple *: &writer_ops_an_ifc_type_tuple)

static size_t writer_alloc_node_impl(an_ifc_output_state *s, void *out,
                                    const writer_node_ops *ops)
{
  an_ifc_output_partition *p = writer_get_or_init_partition(s, ops->kind());
  size_t index = writer_partition_new_elements(p, 1);
  ops->bind(out, s->output_file, &p->content_start, index * p->element_size);
  return index;
}
#define writer_alloc_node(s, out) \
  writer_alloc_node_impl((s), (out), WRITER_NODE_OPS(out))
static size_t writer_alloc_node_block(an_ifc_output_state *s,
                                     an_ifc_partition_kind kind, size_t count)
{
  return count ? writer_partition_new_elements(writer_get_or_init_partition(s, kind), count) : 0;
}
static void writer_fetch_node_impl(an_ifc_output_state *s, void *out,
                                  size_t index, const writer_node_ops *ops)
{
  an_ifc_output_partition *p = s->partitions[ops->kind() - 1];
  ops->bind(out, s->output_file, &p->content_start, index * p->element_size);
}
#define writer_fetch_node(s, out, index) \
  writer_fetch_node_impl((s), (out), (index), WRITER_NODE_OPS(out))
static void writer_fetch_decl_impl(an_ifc_output_state *s, void *out,
                              an_ifc_decl_index index, const writer_node_ops *ops)
{
  check_assertion(to_partition_kind_an_ifc_decl_sort(index.sort) == ops->kind());
  writer_fetch_node_impl(s, out, index.value, ops);
}
#define writer_fetch_decl(s, out, index) \
  writer_fetch_decl_impl((s), (out), (index), WRITER_NODE_OPS(out))
#define writer_alloc_decl_trait(s, index, out) \
  ((void)writer_alloc_node((s), (out)), set_ifc_decl((out), (index)))
static an_ifc_chart_index writer_alloc_chart_impl(an_ifc_output_state *s,
                                  void *out, const writer_node_ops *ops)
{
  size_t index = writer_alloc_node_impl(s, out, ops);
  return (an_ifc_chart_index){s->output_file, to_chart_sort_an_ifc_partition_kind(ops->kind()), index};
}
#define writer_alloc_chart(s, out) \
  writer_alloc_chart_impl((s), (out), WRITER_NODE_OPS(out))
static an_ifc_edg_constant_index writer_alloc_constant_impl(an_ifc_output_state *s,
                                  void *out, const writer_node_ops *ops)
{
  size_t index = writer_alloc_node_impl(s, out, ops);
  return (an_ifc_edg_constant_index){s->output_file, to_edg_constant_sort_an_ifc_partition_kind(ops->kind()), index};
}
#define writer_alloc_constant(s, out) \
  writer_alloc_constant_impl((s), (out), WRITER_NODE_OPS(out))
static an_ifc_decl_index writer_alloc_decl_impl(an_ifc_output_state *s,
                                  void *out, const writer_node_ops *ops)
{
  size_t index = writer_alloc_node_impl(s, out, ops);
  return (an_ifc_decl_index){s->output_file, to_decl_sort_an_ifc_partition_kind(ops->kind()), index};
}
#define writer_alloc_decl(s, out) \
  writer_alloc_decl_impl((s), (out), WRITER_NODE_OPS(out))
static an_ifc_dir_index writer_alloc_dir_impl(an_ifc_output_state *s,
                                  void *out, const writer_node_ops *ops)
{
  size_t index = writer_alloc_node_impl(s, out, ops);
  return (an_ifc_dir_index){s->output_file, to_dir_sort_an_ifc_partition_kind(ops->kind()), index};
}
#define writer_alloc_dir(s, out) \
  writer_alloc_dir_impl((s), (out), WRITER_NODE_OPS(out))
static an_ifc_name_index writer_alloc_name_impl(an_ifc_output_state *s,
                                  void *out, const writer_node_ops *ops)
{
  size_t index = writer_alloc_node_impl(s, out, ops);
  return (an_ifc_name_index){s->output_file, to_name_sort_an_ifc_partition_kind(ops->kind()), index};
}
#define writer_alloc_name(s, out) \
  writer_alloc_name_impl((s), (out), WRITER_NODE_OPS(out))
static an_ifc_edg_complex_token_index writer_alloc_complex_token_impl(an_ifc_output_state *s,
                                  void *out, const writer_node_ops *ops)
{
  size_t index = writer_alloc_node_impl(s, out, ops);
  return (an_ifc_edg_complex_token_index){s->output_file, to_edg_complex_token_sort_an_ifc_partition_kind(ops->kind()), index};
}
#define writer_alloc_complex_token(s, out) \
  writer_alloc_complex_token_impl((s), (out), WRITER_NODE_OPS(out))

static an_ifc_type_index writer_alloc_type_impl(an_ifc_output_state *s,
                                  void *out, const writer_node_ops *ops)
{
  an_ifc_partition_kind kind = ops->kind();
  size_t index = writer_alloc_node_impl(s, out, ops);
  if (is_edg_type_sort_an_ifc_partition_kind(kind)) {
    an_ifc_edg_extension_type extension;
    size_t offset = writer_alloc_node(s, &extension);
    an_ifc_edg_type_index edg_index = {s->output_file, to_edg_type_sort_an_ifc_partition_kind(kind), index};
    set_ifc_value(&extension, edg_index);
    return (an_ifc_type_index){s->output_file, ifc_ts_type_vendor_extension, offset + 1};
  }
  return (an_ifc_type_index){s->output_file, to_type_sort_an_ifc_partition_kind(kind), index};
}
#define writer_alloc_type(s, out) \
  writer_alloc_type_impl((s), (out), WRITER_NODE_OPS(out))
static void writer_cache_init(an_ifc_output_token_cache *cache)
{
  edg_array_init(&cache->basic_tokens, sizeof(an_ifc_edg_basic_token_sort), EDG_ALLOC_GENERAL, 0, NULL, NULL);
  edg_array_init(&cache->complex_tokens, sizeof(an_ifc_edg_complex_token_index), EDG_ALLOC_GENERAL, 0, NULL, NULL);
}
static void writer_cache_destroy(an_ifc_output_token_cache *cache)
{
  edg_array_destroy(&cache->basic_tokens);
  edg_array_destroy(&cache->complex_tokens);
}
static void writer_cache_add_basic(an_ifc_output_token_cache *cache,
                                   an_ifc_edg_basic_token_sort token)
{
  check_assertion(token != ifc_ebts_complex);
  edg_array_push(&cache->basic_tokens, &token);
}
static void writer_cache_add_complex(an_ifc_output_token_cache *cache,
                                     an_ifc_edg_complex_token_index token)
{
  an_ifc_edg_basic_token_sort kind = ifc_ebts_complex;
  edg_array_push(&cache->basic_tokens, &kind);
  edg_array_push(&cache->complex_tokens, &token);
}
static void writer_scope_members_copy(void *dst, const void *src)
{
  memset(dst, 0, sizeof(a_scope_member_array));
  edg_array_copy(dst, src);
}
static void writer_scope_members_destroy(void *obj)
{
  edg_array_destroy(obj);
}
static size_t writer_token_kind_hash(const void *key)
{ return edg_hash_bytes(key, sizeof(a_token_kind)); }
static a_boolean writer_token_kind_equal(const void *a, const void *b)
{ return *(const a_token_kind *)a == *(const a_token_kind *)b; }
static size_t writer_seq_hash(const void *key)
{ return edg_hash_bytes(key, sizeof(a_seq_number_key)); }
static a_boolean writer_seq_equal(const void *a, const void *b)
{ return *(const a_seq_number_key *)a == *(const a_seq_number_key *)b; }
static size_t writer_tagged_hash(const void *key)
{
  const a_tagged_pointer *p = key;
  return edg_hash_pointer(&p->ptr) ^ edg_hash_bytes(&p->kind, sizeof(p->kind));
}
static a_boolean writer_tagged_equal(const void *a, const void *b)
{
  const a_tagged_pointer *p = a, *q = b;
  return p->ptr == q->ptr && p->kind == q->kind;
}
static size_t writer_get_string_table_map(an_ifc_il_map *m, a_string_view key)
{
  const size_t *value = edg_map_get(&m->string_table_map, &key);
  return value != NULL ? *value : (size_t){0};
}
static void writer_set_string_table_map(an_ifc_il_map *m, a_string_view key, size_t value)
{
  (void)edg_map_set(&m->string_table_map, &key, &value);
}
static size_t writer_get_textual_token_map(an_ifc_il_map *m, a_token_kind key)
{
  const size_t *value = edg_map_get(&m->textual_token_map, &key);
  return value != NULL ? *value : (size_t){0};
}
static void writer_set_textual_token_map(an_ifc_il_map *m, a_token_kind key, size_t value)
{
  (void)edg_map_set(&m->textual_token_map, &key, &value);
}
static an_ifc_name_index writer_get_src_file_map(an_ifc_il_map *m, a_source_file_ptr key)
{
  const an_ifc_name_index *value = edg_map_get(&m->src_file_map, &key);
  return value != NULL ? *value : (an_ifc_name_index){0};
}
static void writer_set_src_file_map(an_ifc_il_map *m, a_source_file_ptr key, an_ifc_name_index value)
{
  (void)edg_map_set(&m->src_file_map, &key, &value);
}
static an_ifc_line_offset writer_get_seq_number_map(an_ifc_il_map *m, a_seq_number_key key)
{
  const an_ifc_line_offset *value = edg_map_get(&m->seq_number_map, &key);
  return value != NULL ? *value : (an_ifc_line_offset){0};
}
static void writer_set_seq_number_map(an_ifc_il_map *m, a_seq_number_key key, an_ifc_line_offset value)
{
  (void)edg_map_set(&m->seq_number_map, &key, &value);
}
static an_ifc_type_index writer_get_type_map(an_ifc_il_map *m, a_type_ptr key)
{
  const an_ifc_type_index *value = edg_map_get(&m->type_map, &key);
  return value != NULL ? *value : (an_ifc_type_index){0};
}
static void writer_set_type_map(an_ifc_il_map *m, a_type_ptr key, an_ifc_type_index value)
{
  (void)edg_map_set(&m->type_map, &key, &value);
}
static an_ifc_scope_offset writer_get_scope_map(an_ifc_il_map *m, a_scope_ptr key)
{
  const an_ifc_scope_offset *value = edg_map_get(&m->scope_map, &key);
  return value != NULL ? *value : (an_ifc_scope_offset){0};
}
static void writer_set_scope_map(an_ifc_il_map *m, a_scope_ptr key, an_ifc_scope_offset value)
{
  (void)edg_map_set(&m->scope_map, &key, &value);
}
static an_ifc_decl_index writer_get_il_entry_to_decl(an_ifc_il_map *m, a_tagged_pointer key)
{
  const an_ifc_decl_index *value = edg_map_get(&m->il_entry_to_decl, &key);
  return value != NULL ? *value : (an_ifc_decl_index){0};
}
static void writer_set_il_entry_to_decl(an_ifc_il_map *m, a_tagged_pointer key, an_ifc_decl_index value)
{
  (void)edg_map_set(&m->il_entry_to_decl, &key, &value);
}
static void writer_il_map_init(an_ifc_il_map *m, an_ifc_output_state *s)
{
  memset(m, 0, sizeof(*m));
  m->output_state = s;
  edg_map_init(&m->string_table_map, sizeof(a_string_view), sizeof(size_t), EDG_ALLOC_GENERAL, 10, edg_string_view_hash, edg_string_view_equal, NULL, NULL, NULL, NULL);
  edg_map_init(&m->textual_token_map, sizeof(a_token_kind), sizeof(size_t), EDG_ALLOC_GENERAL, 10, writer_token_kind_hash, writer_token_kind_equal, NULL, NULL, NULL, NULL);
  edg_map_init(&m->src_file_map, sizeof(a_source_file_ptr), sizeof(an_ifc_name_index), EDG_ALLOC_GENERAL, 10, edg_hash_pointer, edg_equal_pointer, NULL, NULL, NULL, NULL);
  edg_map_init(&m->seq_number_map, sizeof(a_seq_number_key), sizeof(an_ifc_line_offset), EDG_ALLOC_GENERAL, 10, writer_seq_hash, writer_seq_equal, NULL, NULL, NULL, NULL);
  edg_map_init(&m->type_map, sizeof(a_type_ptr), sizeof(an_ifc_type_index), EDG_ALLOC_GENERAL, 10, edg_hash_pointer, edg_equal_pointer, NULL, NULL, NULL, NULL);
  edg_map_init(&m->scope_map, sizeof(a_scope_ptr), sizeof(an_ifc_scope_offset), EDG_ALLOC_GENERAL, 10, edg_hash_pointer, edg_equal_pointer, NULL, NULL, NULL, NULL);
  edg_map_init(&m->il_entry_to_decl, sizeof(a_tagged_pointer), sizeof(an_ifc_decl_index), EDG_ALLOC_GENERAL, 10, writer_tagged_hash, writer_tagged_equal, NULL, NULL, NULL, NULL);
  edg_array_init(&m->scope_members, sizeof(a_scope_member_array), EDG_ALLOC_GENERAL, 0, writer_scope_members_copy, writer_scope_members_destroy);
}
static void writer_il_map_destroy(an_ifc_il_map *m)
{
  edg_map_destroy(&m->string_table_map);
  edg_map_destroy(&m->textual_token_map);
  edg_map_destroy(&m->src_file_map);
  edg_map_destroy(&m->seq_number_map);
  edg_map_destroy(&m->type_map);
  edg_map_destroy(&m->scope_map);
  edg_map_destroy(&m->il_entry_to_decl);
  edg_array_destroy(&m->scope_members);
}

static an_ifc_type_index writer_map_type_index(an_ifc_il_map *m,
                                 a_type_ptr type, an_ifc_type_index index)
{
  writer_set_type_map(m, type, index);
  return index;
}
static an_ifc_decl_index writer_map_decl_index(an_ifc_il_map *m,
                               a_tagged_pointer entity, an_ifc_decl_index index)
{
  writer_set_il_entry_to_decl(m, entity, index);
  return index;
}
#define writer_il_map_new_type(m, type, node) \
  writer_map_type_index((m), (type), writer_alloc_type((m)->output_state, (node)))
#define writer_il_map_new_decl(m, entity, node) \
  writer_map_decl_index((m), make_tagged_ptr(entity), writer_alloc_decl((m)->output_state, (node)))
#define writer_il_entity_name_as_text_offset(m, entity) \
  writer_il_string_as_text_offset((m), (entity)->source_corresp.name)
#define writer_il_entity_name_as_name_index(m, entity) \
  writer_il_string_as_name_index((m), (entity)->source_corresp.name)
#define writer_il_find_or_enter_entity_pos(m, entity) \
  writer_il_find_or_enter_pos((m), &(entity)->source_corresp.decl_position)
#define writer_il_associate_entity_home_scope(m, entity) \
  writer_associate_home_scope((m), make_tagged_ptr(entity), (entity)->source_corresp.parent_scope)
#define access_specifier_of(entity) \
  writer_access_specifier_of(&(entity)->source_corresp)

typedef struct writer_trait_order {
  uint64_t encoded_decl;
  size_t index;
} writer_trait_order;
static int writer_compare_trait_order(const void *left, const void *right)
{
  const writer_trait_order *a = left, *b = right;
  if (a->encoded_decl < b->encoded_decl) return -1;
  if (a->encoded_decl > b->encoded_decl) return 1;
  return a->index < b->index ? -1 : a->index > b->index;
}
static void writer_sort_traits(an_ifc_output_state *s)
{
  an_ifc_partition_kind kind = get_ifc_partition_kind_an_ifc_edg_trait_function_definition();
  an_ifc_output_partition *p = s->partitions[kind - 1];
  if (p != NULL) {
    size_t count = p->contents.len / p->element_size;
    size_t i;
    Edg_array order;
    an_ifc_output_partition *replacement;
    edg_array_init(&order, sizeof(writer_trait_order), EDG_ALLOC_GENERAL, count, NULL, NULL);
    for (i = 0; i < count; ++i) {
      an_ifc_edg_trait_function_definition node;
      writer_trait_order item;
      writer_fetch_node(s, &node, i);
      item.encoded_decl = get_ifc_encoded_decl_an_ifc_edg_trait_function_definition(node).value;
      item.index = i;
      edg_array_push(&order, &item);
    }
    if (count > 1) qsort(order.data, count, sizeof(writer_trait_order), writer_compare_trait_order);
    replacement = (an_ifc_output_partition *)alloc_general(sizeof(*replacement));
    writer_partition_init(replacement, p->part_name_offset, p->element_size);
    (void)writer_partition_new_elements(replacement, count);
    for (i = 0; i < count; ++i) {
      const writer_trait_order *item = edg_array_at(&order, i);
      memcpy(replacement->content_start + i * p->element_size,
             p->content_start + item->index * p->element_size, p->element_size);
    }
    s->partitions[kind - 1] = replacement;
    writer_partition_destroy(p);
    edg_array_destroy(&order);
  }
}

/* Private writer call graph declarations. */
static an_ifc_decl_index writer_associate_home_scope(an_ifc_il_map *self,
                                        a_tagged_pointer entity, a_scope_ptr scope);
NORETURN static void ifc_write_error(void);
static void writer_stream_write_bytes_at(an_ifc_output_stream *self, size_t file_pos, a_byte const *bytes, size_t num_bytes);
static void writer_stream_write_bytes(an_ifc_output_stream *self, a_byte const *bytes, size_t num_bytes, a_boolean add_to_checksum);
static a_sha256_digest writer_stream_finish_checksum(an_ifc_output_stream *self);
static void write_ifc_magic_bytes(an_ifc_output_stream *output_stream);
static an_ifc_architecture_sort get_target_ifc_architecture(void);
static size_t get_partitions_start(an_ifc_module_file *file, const an_ifc_output_metadata *metadata);
static size_t get_string_table_start(an_ifc_module_file *file, const an_ifc_output_metadata *metadata);
static size_t get_toc_start(an_ifc_module_file *file, const an_ifc_output_metadata *metadata);
static void write_ifc_header(an_ifc_output_stream *output_stream, an_ifc_unit_sort unit_sort, a_boolean is_internal, size_t module_name, size_t source_file_name, an_ifc_scope_offset global_scope, const an_ifc_output_metadata *metadata);
static void write_ifc_partition(an_ifc_output_stream *output_stream, const an_ifc_output_partition *partition);
static void write_string_table(an_ifc_output_stream *output_stream, const an_ifc_output_buffer *string_table_bytes);
static void write_table_of_contents_entry( an_ifc_output_stream *output_stream, size_t start_of_partitions, const an_ifc_output_partition *partition, const an_ifc_output_partition_metadata *partition_metadata);
static void write_ifc_checksum(an_ifc_output_stream *output_stream);
static an_ifc_output_metadata make_output_metadata( an_ifc_output_partition **partitions, const an_ifc_output_buffer *string_table);
static void writer_write(an_ifc_output_state *self);
static an_ifc_edg_token_cache_offset writer_alloc_token_cache(an_ifc_output_state *self, const an_ifc_output_token_cache *output_cache);
static an_ifc_expr_index writer_alloc_token_cache_expr(an_ifc_output_state *self, const an_ifc_output_token_cache *output_cache);
static an_ifc_type_index writer_il_find_or_enter_type(an_ifc_il_map *self, a_type_ptr type);
static inline a_boolean typeref_has_any_common_qualifiers(a_type_ptr type);
static inline a_boolean typeref_has_any_extended_qualifiers(a_type_ptr type);
static an_ifc_type_index writer_il_enter_type(an_ifc_il_map *self, a_type_ptr type);
static an_ifc_scope_offset writer_il_find_or_enter_scope(an_ifc_il_map *self, a_scope_ptr scope);
static an_ifc_scope_offset writer_il_enter_scope(an_ifc_il_map *self, a_scope_ptr scope);
static inline an_ifc_access_sort convert_access_specifier( an_access_specifier access);
static inline an_ifc_access_sort writer_access_specifier_of( a_source_correspondence *scp);
static an_ifc_decl_index writer_il_find_or_enter_class_struct_union(an_ifc_il_map *self, a_type_ptr type);
static an_ifc_decl_index writer_il_enter_class_struct_union(an_ifc_il_map *self, a_type_ptr type);
static an_ifc_decl_index writer_il_find_or_enter_enum(an_ifc_il_map *self, a_type_ptr type);
static an_ifc_decl_index writer_il_enter_enum(an_ifc_il_map *self, a_type_ptr type);
static an_ifc_decl_index writer_il_find_or_enter_namespace(an_ifc_il_map *self, a_namespace_ptr nsp);
static an_ifc_decl_index writer_il_enter_namespace(an_ifc_il_map *self, a_namespace_ptr nsp);
static an_ifc_decl_index writer_il_find_or_enter_routine(an_ifc_il_map *self, a_routine_ptr rp);
static an_ifc_decl_index writer_il_enter_routine(an_ifc_il_map *self, a_routine_ptr rp);
static an_ifc_decl_index writer_il_find_or_enter_template(an_ifc_il_map *self, a_template_ptr templ);
static inline a_boolean is_alias_template(a_template_ptr templ);
static an_ifc_decl_index writer_il_enter_template(an_ifc_il_map *self, a_template_ptr templ);
static an_ifc_decl_index writer_il_find_or_enter_using_directive(an_ifc_il_map *self, a_using_decl_ptr udp, a_scope_ptr scope);
static an_ifc_decl_index writer_il_enter_using_directive(an_ifc_il_map *self, a_using_decl_ptr udp, a_scope_ptr scope);
static an_ifc_text_offset writer_il_string_as_text_offset_bytes(an_ifc_il_map *self, a_const_char *str, size_t str_len);
static an_ifc_text_offset writer_il_string_as_text_offset(an_ifc_il_map *self, a_const_char *str);
static an_ifc_name_index writer_il_string_as_name_index(an_ifc_il_map *self, a_const_char *str);
static an_ifc_name_index writer_il_find_or_enter_src_file(an_ifc_il_map *self, a_source_file_ptr file);
static an_ifc_name_index writer_il_enter_src_file(an_ifc_il_map *self, a_source_file_ptr file);
static inline a_seq_number_key make_seq_num_map_key(const a_source_position *pos);
static an_ifc_source_location writer_il_find_or_enter_null_pos(an_ifc_il_map *self);
static an_ifc_source_location writer_il_find_or_enter_pos(an_ifc_il_map *self, const a_source_position *pos);
static an_ifc_source_location writer_il_enter_pos(an_ifc_il_map *self, const a_source_position *pos);
static an_ifc_type_index writer_il_enter_class_struct_union_type(an_ifc_il_map *self, a_type_ptr type);
static an_ifc_type_index writer_il_enter_constructor_type(an_ifc_il_map *self, a_type_ptr type);
static an_ifc_type_index writer_il_enter_enum_type(an_ifc_il_map *self, a_type_ptr type);
static an_ifc_type_index writer_il_enter_float_type(an_ifc_il_map *self, a_type_ptr type);
static an_ifc_type_index writer_il_enter_free_function_type(an_ifc_il_map *self, a_type_ptr type);
static an_ifc_type_precision_sort type_size_to_precision(a_type_ptr type);
static an_ifc_type_index writer_il_enter_integer_type(an_ifc_il_map *self, a_type_ptr type);
static an_ifc_type_index writer_il_enter_member_function_type(an_ifc_il_map *self, a_type_ptr type);
static an_ifc_type_index writer_il_enter_nullptr_type(an_ifc_il_map *self, a_type_ptr type);
static an_ifc_type_index writer_il_enter_pointer_type(an_ifc_il_map *self, a_type_ptr type);
static an_ifc_type_index writer_il_enter_routine_params_type(an_ifc_il_map *self, a_type_ptr type);
static an_ifc_type_index writer_il_enter_routine_type(an_ifc_il_map *self, a_type_ptr type);
static an_ifc_type_index writer_il_enter_template_type_param_type(an_ifc_il_map *self, a_type_ptr type, an_ifc_decl_index param_decl_idx);
static an_ifc_type_index writer_il_enter_typedef_type(an_ifc_il_map *self, a_type_ptr type);
static an_ifc_type_index writer_il_enter_void_type(an_ifc_il_map *self, a_type_ptr type);
static an_ifc_output_token_cache writer_il_copy_template_body_to_cache(an_ifc_il_map *self, a_template_ptr templ);
static a_constant_ptr enumerator_constants_for_type(a_type_ptr type);
static an_ifc_sequence writer_il_enter_enumerators(an_ifc_il_map *self, a_type_ptr type);
static an_ifc_chart_index writer_il_enter_routine_params(an_ifc_il_map *self, a_routine_ptr rp);
static an_ifc_edg_template_argument_index writer_il_enter_template_argument(an_ifc_il_map *self, a_template_arg_ptr templ_arg);
static an_ifc_chart_index writer_il_enter_template_params(an_ifc_il_map *self, a_template_ptr templ);
static an_ifc_type_index writer_il_enter_template_template_param_type(an_ifc_il_map *self, a_template_ptr templ);
static void writer_il_set_template_param_coordinates(an_ifc_il_map *self, an_ifc_decl_parameter *param_decl, const a_template_param_coordinate *coordinates);
static void writer_il_set_up_template_non_type_param(an_ifc_il_map *self, an_ifc_decl_parameter *param_decl, a_template_parameter_ptr templ_param);
static void writer_il_set_up_template_template_param(an_ifc_il_map *self, an_ifc_decl_parameter *param_decl, an_ifc_decl_index param_decl_idx, a_template_parameter_ptr templ_param);
static void writer_il_set_up_template_type_param(an_ifc_il_map *self, an_ifc_decl_parameter *param_decl, an_ifc_decl_index param_decl_idx, a_template_parameter_ptr templ_param);
static an_ifc_edg_constant_index writer_il_enter_constant(an_ifc_il_map *self, const a_constant *cp);
static inline an_ifc_edg_basic_token_sort token_to_basic_token_kind( a_token_kind token);
static inline an_ifc_edg_constant_token_sort token_to_constant_token_kind( a_token_kind token);
static an_ifc_edg_complex_token_index writer_il_find_or_enter_textual_token(an_ifc_il_map *self, a_shared_token tok);
static void writer_il_enter_basic_token_to_cache(an_ifc_il_map *self, an_ifc_output_token_cache *ifc_cache, a_shared_token tok);
static an_ifc_edg_complex_token_index writer_il_enter_textual_token(an_ifc_il_map *self, a_shared_token tok);
static void writer_il_enter_textual_token_to_cache(an_ifc_il_map *self, an_ifc_output_token_cache *ifc_cache, a_shared_token tok);
static an_ifc_edg_complex_token_index writer_il_enter_constant_token(an_ifc_il_map *self, an_ifc_edg_constant_token_sort token_kind, an_ifc_edg_constant_index constant_idx);
static void writer_il_enter_constant_token_to_cache(an_ifc_il_map *self, an_ifc_output_token_cache *ifc_cache, a_shared_token tok);
static void writer_il_enter_extracted_body_to_cache(an_ifc_il_map *self, an_ifc_output_token_cache *ifc_cache, a_shared_token tok);
static void writer_il_enter_identifier_token_to_cache(an_ifc_il_map *self, an_ifc_output_token_cache *ifc_cache, a_shared_token tok);
static a_boolean should_token_be_simplified(a_shared_token tok);
static void writer_il_enter_token_cache(an_ifc_il_map *self, an_ifc_output_token_cache *ifc_cache, a_token_cache *fe_cache);
static an_ifc_type_index writer_il_find_or_enter_class_scope_type(an_ifc_il_map *self);
static an_ifc_type_index writer_il_find_or_enter_namespace_scope_type(an_ifc_il_map *self);
static an_ifc_type_index writer_il_find_or_enter_alias_typedef_type(an_ifc_il_map *self);
static an_ifc_type_index writer_il_find_or_enter_scoped_enum_type(an_ifc_il_map *self);
static an_ifc_type_index writer_il_find_or_enter_struct_scope_type(an_ifc_il_map *self);
static an_ifc_type_index writer_il_find_or_enter_union_scope_type(an_ifc_il_map *self);
static an_ifc_type_index writer_il_find_or_enter_unscoped_enum_type(an_ifc_il_map *self);
static an_ifc_decl_index writer_il_enter_alias_template(an_ifc_il_map *self, a_template_ptr templ);
static an_ifc_decl_index writer_il_enter_class_template(an_ifc_il_map *self, a_template_ptr templ);
static INLINE an_ifc_function_traits_bitfield build_function_traits( an_ifc_module_file *file, a_routine_ptr rp);
static an_ifc_decl_index writer_il_enter_constructor(an_ifc_il_map *self, a_routine_ptr rp);
static an_ifc_decl_index writer_il_enter_destructor(an_ifc_il_map *self, a_routine_ptr rp);
static an_ifc_decl_index writer_il_enter_field(an_ifc_il_map *self, a_field_ptr field);
static an_ifc_decl_index writer_il_enter_free_function(an_ifc_il_map *self, a_routine_ptr rp);
static an_ifc_decl_index writer_il_enter_function_template(an_ifc_il_map *self, a_template_ptr templ);
static an_ifc_decl_index writer_il_enter_member_function(an_ifc_il_map *self, a_routine_ptr rp);
static an_ifc_decl_index writer_il_enter_namespace_scope(an_ifc_il_map *self, a_scope_ptr scope);
static an_ifc_decl_index writer_il_enter_typedef(an_ifc_il_map *self, a_type_ptr type);
static an_ifc_decl_index writer_il_find_or_enter_home_scope(an_ifc_il_map *self, a_scope_ptr scope);
static an_ifc_decl_index writer_il_enter_home_scope(an_ifc_il_map *self, a_scope_ptr scope);
static void writer_il_map_scope_member(an_ifc_il_map *self, a_scope_ptr scope, an_ifc_decl_index decl);
static void dump_scope_types(an_ifc_il_map *il_map, a_scope_ptr scope);
static void dump_scope_templates(an_ifc_il_map *il_map, a_scope_ptr scope);
static void dump_scope_routines(an_ifc_il_map *il_map, a_scope_ptr scope);
static void dump_scope_namespaces(an_ifc_il_map *il_map, a_scope_ptr scope);
static void dump_scope_using_directives(an_ifc_il_map *il_map, a_scope_ptr scope);
static void dump_scope_recursively(an_ifc_il_map *il_map, a_scope_ptr scope);
static void complete_scope_info(an_ifc_output_state *output_state, an_ifc_il_map *il_map);
static an_error_code module_to_error_code(a_module_ptr mod);

NORETURN static void ifc_write_error(void)
/*
Issue a catastrophic diagnostic that the IFC file could not be created and
terminate the compilation.  This routine does not return.
*/
{
  error_position = null_source_position;
  file_write_error(ec_ifc, errno);
}  /* ifc_write_error */

static void writer_stream_write_bytes_at(an_ifc_output_stream *self, size_t       file_pos,
                                          a_byte const *bytes,
                                          size_t       num_bytes)
/*
Write the given bytes (counted by num_bytes) at the given file position.  The
file position must already exist and the file must already have at least
num_bytes bytes following the specified file position.
*/
{
  long orig_file_pos = ftell(self->output_file->f_module);

  if (orig_file_pos < 0) {
    ifc_write_error();
  }  /* if */
  /* If this assertion fails, this write operation would increase the size of
     the file.  write_bytes should be used for append operations. */
  check_assertion(file_pos + num_bytes < size_t_arg(orig_file_pos));
  { /* Move the cursor to the desired write position. */
    int seek_result = fseek(self->output_file->f_module,
                            (long)file_pos, SEEK_SET);

    if (seek_result != 0) {
      ifc_write_error();
    }  /* if */
  }
  writer_stream_write_bytes(self, bytes, num_bytes, /*add_to_checksum=*/FALSE);
  { /* Restore the cursor to its original position. */
    int seek_result = fseek(self->output_file->f_module, orig_file_pos,
                            SEEK_SET);
    if (seek_result != 0) {
      ifc_write_error();
    }  /* if */
  }
}  /* writer_stream_write_bytes_at */

static void writer_stream_write_bytes(an_ifc_output_stream *self, a_byte const *bytes,
                                       size_t       num_bytes,
                     a_boolean    add_to_checksum)
/*
Append the given bytes (counted by num_bytes) to the underlying file.  If
add_to_checksum is TRUE, the bytes will be factored into the SHA-2 256 checksum
associated with the file.
*/
{
  /* The write must be either performed without modifying the checksum
     or the checksum must not yet have been calculated. */
  check_assertion(!add_to_checksum || !self->checksum_computed);
  size_t num_written = fwrite(bytes, num_bytes, /*num_to_write=*/1,
                              self->output_file->f_module);

  if (add_to_checksum) {
    edg_sha256_update(&self->hash_state, bytes, num_bytes);
  }  /* if */
  if (num_written != 1) {
    ifc_write_error();
  }  /* if */
}  /* writer_stream_write_bytes */

static a_sha256_digest writer_stream_finish_checksum(an_ifc_output_stream *self)
/*
Compute and return the checksum of all bytes written to the file that
should be considered part of the checksum.
*/
{
#if CHECKING
  /* The output stream can only be "finished" once. */
  check_assertion(!self->checksum_computed);
  self->checksum_computed = TRUE;
#endif /* CHECKING */
  return edg_sha256_compute_digest(&self->hash_state);
}  /* writer_stream_finish_checksum */

static void write_ifc_magic_bytes(an_ifc_output_stream *output_stream)
/*
Write the series of magic bytes identifying the IFC file.
*/
{
  /* If this assertion fails the front end is presumably trying to write a
     Microsoft compatible IFC file and that's not currently possible. */
  check_assertion(output_stream->output_file->module_kind == mfk_edg_ifc);
  /* FIXME: If the file header ever changes size, we'll need to change this. */
  writer_stream_write_bytes(output_stream, edg_ifc_magic_numbers,
                             sizeof(edg_ifc_magic_numbers),
                             /*add_to_checksum=*/FALSE);
}  /* write_ifc_magic_bytes */




static an_ifc_architecture_sort get_target_ifc_architecture(void)
/*
Return the IFC architecture sort value corresponding to the current target.
*/
{
  an_ifc_architecture_sort result = {0};

  if (target_is_x86_based()) {
    if (target_is_64_bits()) {
      result = ifc_as_x64;
    } else {
      result = ifc_as_x86;
    }  /* if */
  } else if (target_is_arm_based()) {
    if (target_is_x86_compatible()) {
      if (target_is_64_bits()) {
        result = ifc_as_hybrid_x86_arm64;
      } else {
        ifc_write_catastrophe(ec_unsupported_il_to_ifc);
      }  /* if */
    } else if (target_is_64_bits()) {
      result = ifc_as_arm64;
    } else {
      result = ifc_as_arm32;
    }  /* if */
  } else {
    ifc_write_catastrophe(ec_unsupported_il_to_ifc);
  }  /* if */
  return result;
}  /* get_target_ifc_architecture */




static size_t get_partitions_start(an_ifc_module_file           *file,
                                   const an_ifc_output_metadata *metadata)
/*
Return the byte offset for the start of the partitions.
*/
{
  size_t result = 0;

  /* Add the bytes for the magic numbers. */
  result += sizeof(edg_ifc_magic_numbers);
  /* Add the bytes for the file header. */
  result += get_ifc_buffer_size_an_ifc_file_header_storage(file);
  return result;
}  /* get_partitions_start */




static size_t get_string_table_start(an_ifc_module_file           *file,
                                     const an_ifc_output_metadata *metadata)
/*
Return the byte offset for the start of the string table given the associated
module file and output metadata.
*/
{
  size_t result = 0;

  /* Add the bytes for the start of the partitions. */
  result += get_partitions_start(file, metadata);
  /* Add the bytes for the partitions themselves. */
  result += metadata->num_partition_bytes;
  return result;
}  /* get_string_table_start */




static size_t get_toc_start(an_ifc_module_file           *file,
                            const an_ifc_output_metadata *metadata)
/*
Return the byte offset for the start of the table of contents given the
associated module file and output metadata.
*/
{
  size_t result = 0;

  /* Add the bytes for the start of the string table. */
  result += get_string_table_start(file, metadata);
  /* Add the bytes for the string table. */
  result += metadata->num_string_table_bytes;
  return result;
}  /* get_toc_start */




static void write_ifc_header(an_ifc_output_stream         *output_stream,
                             an_ifc_unit_sort             unit_sort,
                             a_boolean                    is_internal,
                             size_t                       module_name,
                             size_t                       source_file_name,
                             an_ifc_scope_offset          global_scope,
                             const an_ifc_output_metadata *metadata)
/*
Use the associated offset of the source file name into the string table, global
scope offset, and metadata to form the IFC file header.  Write the created file
header into the given output stream.
*/
{
  /* Create an instance of an_ifc_file_header_storage on the stack and
     use it to set up, then write, the file header. */
  an_ifc_module_file *file = output_stream->output_file;
  an_ifc_file_header file_header = an_ifc_file_header_init(file);
  an_ifc_version ifc_major_version = (an_ifc_version){file, file->version_major};
  an_ifc_version ifc_minor_version = (an_ifc_version){file, file->version_minor};

  /* Set the IFC version information. */
  set_ifc_major_version(&file_header, ifc_major_version);
  set_ifc_minor_version(&file_header, ifc_minor_version);
  /* Set the ABI version. */
  set_ifc_arch(&file_header, get_target_ifc_architecture());

  /* Set the string table information. */
  an_ifc_byte_offset ifc_string_table_start = (an_ifc_byte_offset){file,
                                            get_string_table_start(file, metadata)};
  an_ifc_cardinality ifc_string_table_length = (an_ifc_cardinality){file,
                                             metadata->num_string_table_bytes};
  set_ifc_string_table_bytes(&file_header, ifc_string_table_start);
  set_ifc_string_table_size(&file_header, ifc_string_table_length);

  /* Set the unit information. */
  an_ifc_unit_index ifc_unit_idx = (an_ifc_unit_index){file, unit_sort, module_name};
  set_ifc_unit(&file_header, ifc_unit_idx);

  /* Set the source path information. */
  an_ifc_text_offset ifc_source_path = (an_ifc_text_offset){file, source_file_name};
  set_ifc_src_path(&file_header, ifc_source_path);

  /* Set the primary scope information. */
  set_ifc_global_scope(&file_header, global_scope);

  /* Set the table of contents offset. */
  an_ifc_byte_offset ifc_toc_start = (an_ifc_byte_offset){file, get_toc_start(file, metadata)};
  set_ifc_toc(&file_header, ifc_toc_start);

  /* Set the number of partitions. */
  an_ifc_cardinality ifc_partition_count = (an_ifc_cardinality){
               file,
               (an_ifc_cardinality_storage)metadata->used_partitions.len};
  set_ifc_partition_count(&file_header, ifc_partition_count);

  /* Set whether or not this is an internal partition. */
  an_ifc_bool ifc_is_internal = (an_ifc_bool){file, is_internal};
  set_ifc_internal(&file_header, ifc_is_internal);
  /* Do the append. */

  /* Write the file header's SHA-2 256 bit checksum as a placeholder. */
  size_t sha256_len = get_ifc_buffer_size_an_ifc_sha256_storage(file);
  writer_stream_write_bytes(output_stream, (const a_byte *)an_ifc_file_header_get_storage(&file_header), sha256_len,
                             /*add_to_checksum=*/FALSE);

  /* Write the remaining file header contents (included in the checksum). */
  size_t header_len = get_ifc_buffer_size_an_ifc_file_header_storage(file);
  writer_stream_write_bytes(output_stream, (const a_byte *)an_ifc_file_header_get_storage(&file_header) + sha256_len,
                             header_len - sha256_len, TRUE);
}  /* write_ifc_header */




static void write_ifc_partition(an_ifc_output_stream          *output_stream,
                                const an_ifc_output_partition *partition)
/*
Append the contents of the given partition to the given module output stream.
*/
{
  writer_stream_write_bytes(output_stream, partition->contents.data, partition->contents.len, TRUE);
}  /* write_ifc_partition */




static void write_string_table(an_ifc_output_stream       *output_stream,
                               const an_ifc_output_buffer *string_table_bytes)
/*
Append the contents of the given string table to the given module output
stream.
*/
{
  writer_stream_write_bytes(output_stream, string_table_bytes->data,
                             string_table_bytes->len, TRUE);
}  /* write_string_table */




static void write_table_of_contents_entry(
                   an_ifc_output_stream                   *output_stream,
                   size_t                                 start_of_partitions,
                   const an_ifc_output_partition *partition,
                   const an_ifc_output_partition_metadata *partition_metadata)
/*
Write the table of contents entry for the given partition into the given module
output stream.

The start of partitions value should be the byte offset from the start of the
file to where the partition bytes have been written.  Partition metadata is the
associated partition metadata for the partition.
*/
{
  an_ifc_module_file *file = output_stream->output_file;
  an_ifc_partition toc_entry = an_ifc_partition_init(file);
  an_ifc_text_offset ifc_name_offset = (an_ifc_text_offset){file, partition->part_name_offset};

  /* Set the name of the partition. */
  set_ifc_name(&toc_entry, ifc_name_offset);

  /* Set the offset where this partition starts in the file. */
  an_ifc_byte_offset ifc_part_offset = (an_ifc_byte_offset){file,
                                     (start_of_partitions +
                                      partition_metadata->relative_offset)};
  set_ifc_offset(&toc_entry, ifc_part_offset);

  /* Set the number of elements in this partition. */
  size_t             num_elements = (partition->contents.len /
                                     partition->element_size);
  an_ifc_cardinality ifc_num_elements = (an_ifc_cardinality){file, num_elements};
  set_ifc_cardinality(&toc_entry, ifc_num_elements);

  /* Set the entry size. */
  size_t             entity_size = partition->element_size;
  an_ifc_entity_size ifc_entity_size = (an_ifc_entity_size){file, entity_size};
  set_ifc_entry_size(&toc_entry, ifc_entity_size);

  size_t storage_len = get_ifc_buffer_size_an_ifc_partition_storage(file);
  writer_stream_write_bytes(output_stream, (const a_byte *)an_ifc_partition_get_storage(&toc_entry), storage_len, TRUE);
}  /* write_table_of_contents_entry */




static void write_ifc_checksum(an_ifc_output_stream *output_stream)
/*
Create and write out the IFC file header for the given IFC module file using
the associated offset of the source file name into the string table, global
scope offset, and metadata.
*/
{
  an_ifc_module_file *file = output_stream->output_file;
  a_sha256_digest    digest = writer_stream_finish_checksum(output_stream);

  size_t sha256_size = get_ifc_buffer_size_an_ifc_sha256_storage(file);
  check_assertion(sha256_size == 32);
  writer_stream_write_bytes_at(output_stream, /*file_pos=*/4, digest.bytes, sha256_size);
}  /* write_ifc_checksum */




static an_ifc_output_metadata make_output_metadata(
                                     an_ifc_output_partition    **partitions,
                                     const an_ifc_output_buffer *string_table)
/*
Given the array of partitions and string table for an IFC output state,
return the computed IFC output metadata.
*/
{
  an_ifc_output_metadata result = {0};
  edg_array_init(&result.used_partitions, sizeof(an_ifc_output_partition_metadata), EDG_ALLOC_GENERAL, 0, NULL, NULL);
  size_t                 relative_start = 0;

  for (size_t i = 0; i < IFC_PARTITION_COUNT; ++i) {
    const an_ifc_output_partition *partition = partitions[i];

    if (partition == NULL) {
      continue;
    }  /* if */

    an_ifc_output_partition_metadata part_metadata = {0};
    /* Add the partition metadata for this partition. */
    part_metadata.kind = (an_ifc_partition_kind)(i + 1);
    part_metadata.relative_offset = relative_start;
    edg_array_push(&result.used_partitions, &part_metadata);

    /* Update the relative_start for the next partition. */
    size_t bytes_in_partition = partition->contents.len;
    relative_start += bytes_in_partition;
    /* Update the total number of bytes used by all partitions. */
    result.num_partition_bytes += bytes_in_partition;
  }  /* for */
  result.num_string_table_bytes =
                            (an_ifc_cardinality_storage)string_table->len;
  return result;
}  /* make_output_metadata */



static void writer_write(an_ifc_output_state *self)
/*
Write the IFC output state to the file associated with the output state's
output file (i.e., an_ifc_output_state.output_file).
*/
{
  an_ifc_output_stream output_stream = {0};
  output_stream.output_file = self->output_file;
  edg_sha256_init(&output_stream.hash_state);
  an_ifc_output_metadata
                metadata = make_output_metadata(self->partitions, &self->string_table);

  write_ifc_magic_bytes(&output_stream);
  write_ifc_header(&output_stream, self->unit_sort, self->internal_partition, self->module_name_offset, self->source_file_name_offset, self->global_scope, &metadata);
  for (size_t part_i = 0; part_i < metadata.used_partitions.len; ++part_i) {
    const an_ifc_output_partition_metadata *part_metadata = edg_array_at(&metadata.used_partitions, part_i);
    an_ifc_output_partition
                *partition = self->partitions[part_metadata->kind - 1];

    write_ifc_partition(&output_stream, partition);
  }  /* for */
  write_string_table(&output_stream, &self->string_table);

  size_t start_of_partitions = get_partitions_start(self->output_file, &metadata);
  for (size_t part_i = 0; part_i < metadata.used_partitions.len; ++part_i) {
    const an_ifc_output_partition_metadata *part_metadata = edg_array_at(&metadata.used_partitions, part_i);
    an_ifc_output_partition
                *partition = self->partitions[part_metadata->kind - 1];

    write_table_of_contents_entry(&output_stream, start_of_partitions, partition, part_metadata);
  }  /* for */
  write_ifc_checksum(&output_stream);
  edg_array_destroy(&metadata.used_partitions);
}  /* writer_write */

static an_ifc_edg_token_cache_offset writer_alloc_token_cache(an_ifc_output_state *self, 
                                 const an_ifc_output_token_cache *output_cache)
/*
Allocate and populate the token cache with the information tracked by the given
IFC output token cache.  Return the IFC token cache offset of the created token
cache.
*/
{
  /* Allocate the token cache. */
  an_ifc_edg_token_cache token_cache = {0};
  size_t        token_cache_offset = writer_alloc_node(self, &token_cache);
  an_ifc_edg_token_cache_offset result = (an_ifc_edg_token_cache_offset){token_cache.file, token_cache_offset + 1};
  /* Allocate the basic and complex token blocks. */
  size_t        num_basic_tokens = output_cache->basic_tokens.len;
  size_t        basic_tokens_start = writer_alloc_node_block(self, get_ifc_partition_kind_an_ifc_edg_token_basic(), 
                                                             num_basic_tokens);
  size_t        num_complex_tokens = output_cache->complex_tokens.len;
  size_t        complex_tokens_start = writer_alloc_node_block(self, get_ifc_partition_kind_an_ifc_edg_heap_complex_token(), 
                                                           num_complex_tokens);
  /* Associate the basic tokens with the token cache. */
  an_ifc_edg_token_basic_offset ifc_basic_tokens_start = (an_ifc_edg_token_basic_offset){token_cache.file,
                                       basic_tokens_start};
  an_ifc_cardinality ifc_num_basic_tokens = (an_ifc_cardinality){token_cache.file, num_basic_tokens};
  set_ifc_tokens(&token_cache, ifc_basic_tokens_start);
  set_ifc_num_tokens(&token_cache, ifc_num_basic_tokens);

  /* Associate the complex tokens with the token cache. */
  an_ifc_edg_heap_complex_token_offset ifc_complex_tokens_start = (an_ifc_edg_heap_complex_token_offset){token_cache.file,
                                         complex_tokens_start};
  an_ifc_cardinality ifc_num_complex_tokens = (an_ifc_cardinality){token_cache.file,
                                       num_complex_tokens};
  set_ifc_complex_tokens(&token_cache, ifc_complex_tokens_start);
  set_ifc_num_complex_tokens(&token_cache, ifc_num_complex_tokens);

  /* Complete the basic tokens. */
  for (size_t i = 0; i < num_basic_tokens; ++i) {
    an_ifc_edg_token_basic basic_token = {0};

    writer_fetch_node(self, &basic_token, basic_tokens_start + i);
    set_ifc_kind(&basic_token, (*(an_ifc_edg_basic_token_sort *)edg_array_at(&output_cache->basic_tokens, i)));
  }  /* for */
  /* Complete the complex tokens. */
  for (size_t i = 0; i < num_complex_tokens; ++i) {
    an_ifc_edg_heap_complex_token complex_token = {0};

    writer_fetch_node(self, &complex_token, complex_tokens_start + i);
    set_ifc_index(&complex_token, (*(an_ifc_edg_complex_token_index *)edg_array_at(&output_cache->complex_tokens, i)));
  }  /* for */
  return result;
}  /* writer_alloc_token_cache */

static an_ifc_expr_index writer_alloc_token_cache_expr(an_ifc_output_state *self, 
                                 const an_ifc_output_token_cache *output_cache)
/*
Allocate and populate the token cache with the information tracked by the given
IFC output token cache.  Return the IFC expression index of the created token
cache.
*/
{
  an_ifc_edg_token_cache_offset
                token_cache_offset = writer_alloc_token_cache(self, output_cache);

  return (an_ifc_expr_index){token_cache_offset.file,
                           ifc_es_expr_vendor_extension,
                           token_cache_offset.value};
}  /* writer_alloc_token_cache_expr */

static an_ifc_type_index writer_il_find_or_enter_type(an_ifc_il_map *self, a_type_ptr type)
/*
For the given type find or enter the type into the IFC output state.  Return
the type index for the type file.
*/
{
  an_ifc_type_index result = writer_get_type_map(self, type);

  if (is_null_index(result)) {
    result = writer_il_enter_type(self, type);
  }  /* if */
  return result;
}  /* writer_il_find_or_enter_type */

static const a_type_qualifier_set
                ifc_common_qualifiers = TQ_CONST | TQ_VOLATILE | TQ_RESTRICT;
                        /* These are the qualifiers natively representable by
                           the IFC TypeSort::Qualified type. */


static inline a_boolean typeref_has_any_common_qualifiers(a_type_ptr type)
/*
If the type has any of the common type qualifiers supported natively by the IFC
format (const, volatile, or __restrict) return TRUE; otherwise, return FALSE.
*/
{
  check_assertion(type->kind == tk_typeref);

  return (type->variant.typeref.qualifiers & ifc_common_qualifiers) != 0;
}  /* typeref_has_any_common_qualifiers */


static inline a_boolean typeref_has_any_extended_qualifiers(a_type_ptr type)
/*
If the type has any of type qualifiers supported by EDG but not natively by the
IFC format return TRUE; otherwise, return FALSE.
*/
{
  return (type->variant.typeref.qualifiers & ~ifc_common_qualifiers) != 0;
}  /* typeref_has_any_extended_qualifiers */


static an_ifc_type_index writer_il_enter_type(an_ifc_il_map *self, a_type_ptr type)
/*
For the given type enter the type into the IFC output state.  Return the type
index for the type file.
*/
{
  check_assertion(is_null_index(writer_get_type_map(self, type)));
  an_ifc_type_index result = {0};

  switch (type->kind) {
    case tk_class:
    case tk_struct:
    case tk_union:
      result = writer_il_enter_class_struct_union_type(self, type);
      break;
    case tk_float:
      result = writer_il_enter_float_type(self, type);
      break;
    case tk_integer:
      if (is_enum_type(type)) {
        result = writer_il_enter_enum_type(self, type);
      } else {
        result = writer_il_enter_integer_type(self, type);
      }  /* if */
      break;
    case tk_nullptr:
      result = writer_il_enter_nullptr_type(self, type);
      break;
    case tk_pointer:
      result = writer_il_enter_pointer_type(self, type);
      break;
    case tk_routine:
      result = writer_il_enter_routine_type(self, type);
      break;
    case tk_template_param:
      /* This type should be entered when the template parameter chart
         is set up in enter_template_params. */
      unexpected_condition();
      break;
    case tk_typeref:
      if (typeref_is_typedef(type)) {
        /* If a typedef has its own qualifiers the IL type will be mapped to
           two distinct IFC types.  Thus, if this case is encountered either
           the typedef shouldn't have been directly given qualifiers or this
           code needs to be updated. */
        check_assertion(type->variant.typeref.qualifiers == TQ_NONE);
        result = writer_il_enter_typedef_type(self, type);
      } else if (is_typeref_kind(type, trk_is_decltype)) {
        /* FIXME: This is almost definitely incomplete (e.g., dependent cases).
           The IFC has a decltype type node for representing decltype
           expressions (presumably we only need this in dependent cases?). */
        a_type_ptr underlying_type = type->variant.typeref.type;

        result = writer_il_find_or_enter_type(self, underlying_type);
      } else if (is_typeref_kind(type, trk_none)) {
        a_type_ptr underlying_type = type->variant.typeref.type;

        result = writer_il_find_or_enter_type(self, underlying_type);
      } else {
        /* FIXME: Handle other kinds of typerefs. */
        ifc_write_catastrophe(ec_unsupported_il_to_ifc);
      }  /* if */
      /* If the typeref adds qualifiers, add a qualifying type. */
      if (typeref_has_any_extended_qualifiers(type)) {
        /* FIXME: Handle the extended qualifiers. */
        ifc_write_catastrophe(ec_unsupported_il_to_ifc);
      } else if (typeref_has_any_common_qualifiers(type)) {
        an_ifc_type_qualified qualified_type = {0};
        an_ifc_type_index
                unqualified_type = result;

        /* Update the result and type mapping to include the qualifiers. */
        result = writer_il_map_new_type(self, type, &qualified_type);
        set_ifc_unqualified(&qualified_type, unqualified_type);

        /* Apply the appropriate qualifiers. */
        an_ifc_qualifier_bitfield_query
                qualifers = (an_ifc_qualifier_bitfield_query)0;
        if (type->variant.typeref.qualifiers & TQ_CONST) {
          qualifers = qualifers | ifc_qb_const;
        }  /* if */
        if (type->variant.typeref.qualifiers & TQ_VOLATILE) {
          qualifers = qualifers | ifc_qb_volatile;
        }  /* if */
        if (type->variant.typeref.qualifiers & TQ_RESTRICT) {
          qualifers = qualifers | ifc_qb_restrict;
        }  /* if */

        an_ifc_qualifier_bitfield_storage
                ifc_raw_qualifiers = to_bitmask_an_ifc_qualifier_bitfield_query(qualified_type.file,
                                                qualifers);
        an_ifc_qualifier_bitfield ifc_qualifiers = (an_ifc_qualifier_bitfield){qualified_type.file, ifc_raw_qualifiers};
        set_ifc_qualifiers(&qualified_type, ifc_qualifiers);
      }  /* if */
      break;
    case tk_void:
      result = writer_il_enter_void_type(self, type);
      break;
    case tk_error:
#if FIXED_POINT_ALLOWED
    case tk_fixed_point:
#endif /* FIXED_POINT_ALLOWED */
#if C99_IL_EXTENSIONS_SUPPORTED
    case tk_imaginary:
    case tk_complex:
#endif /* C99_IL_EXTENSIONS_SUPPORTED */
    case tk_array:
    case tk_ptr_to_member:
#if GNU_VECTOR_TYPES_ALLOWED
    case tk_vector:
    case tk_scalable_vector:
    case tk_scalable_vector_count:
    case tk_riscv_vector:
    case tk_mfp8:
    case tk_float8e4m3:
    case tk_float8e5m2:
#endif /* GNU_VECTOR_TYPES_ALLOWED */
    case tk_reflection:
    case tk_unknown:
      break;
    default_is_unexpected();
  }  /* switch */
  return result;
}  /* writer_il_enter_type */

static an_ifc_scope_offset writer_il_find_or_enter_scope(an_ifc_il_map *self, a_scope_ptr scope)
/*
For the given scope enter the scope into the IFC output state.  Return the
scope offset for the type file.
*/
{
  an_ifc_scope_offset result = writer_get_scope_map(self, scope);

  if (is_null_index(result)) {
    result = writer_il_enter_scope(self, scope);
  }  /* if */
  return result;
}  /* writer_il_find_or_enter_scope */

static an_ifc_scope_offset writer_il_enter_scope(an_ifc_il_map *self, a_scope_ptr scope)
/*
For the given scope enter the scope into the IFC output state.  Return the
scope offset for the type file.
*/
{
  check_assertion(is_null_index(writer_get_scope_map(self, scope)));
  an_ifc_scope_descriptor descriptor = {0};
  size_t        part_offset = writer_alloc_node(self->output_state, &descriptor);
  an_ifc_scope_offset result = (an_ifc_scope_offset){descriptor.file, part_offset + 1};

  writer_set_scope_map(self, scope, result);
  {
    a_scope_member_array members;
    edg_array_init(&members, sizeof(an_ifc_decl_index), EDG_ALLOC_GENERAL, 0, NULL, NULL);
    edg_array_push(&self->scope_members, &members);
    edg_array_destroy(&members);
  }
  /* If this assertion fails a scope member array is missing for one or more
     IFC scope descriptors. */
  check_assertion((size_t)self->scope_members.len == (part_offset + 1));
  return result;
}  /* writer_il_enter_scope */

static inline an_ifc_access_sort convert_access_specifier(
                                                    an_access_specifier access)
/*
Given a front end access specifier return the corresponding IFC access sort.
*/
{
  an_ifc_access_sort result = ifc_as_none;

  switch (access) {
    case as_public:
      result = ifc_as_public;
      break;
    case as_protected:
      result = ifc_as_protected;
      break;
    case as_private:
      result = ifc_as_private;
      break;
    case as_inaccessible:
      /* The inaccessible access level should not make it to IFC writing.  If
         an inaccessible access level needs written to the IFC this would
         require a vendor extension (most likely in the form of some sort of
         optional IFC trait) to be implemented for both the writer and
         reader. */
      unexpected_condition();
    default_is_unexpected();
  }  /* switch */
  return result;
}  /* convert_access_specifier */


static inline an_ifc_access_sort writer_access_specifier_of(
                                                  a_source_correspondence *scp)
/*
Given a front end source correspondence return the corresponding IFC access
sort.
*/
{
  return convert_access_specifier((an_access_specifier)scp->access);
}  /* access_specifier_of */


static an_ifc_decl_index writer_il_find_or_enter_class_struct_union(an_ifc_il_map *self, 
                                                               a_type_ptr type)
/*
For the given class, struct, or union type find or enter the respective
declaration into the IFC output state.  Return the declaration index for the
entered declaration.
*/
{
  an_ifc_decl_index result = writer_get_il_entry_to_decl(self, make_tagged_ptr(type));

  if (is_null_index(result)) {
    result = writer_il_enter_class_struct_union(self, type);
  }  /* if */
  return result;
}  /* writer_il_find_or_enter_class_struct_union */

static an_ifc_decl_index writer_il_enter_class_struct_union(an_ifc_il_map *self, a_type_ptr type)
/*
Enter the given class, struct, or union type into the IFC output state.  Return
the declaration index for the entered declaration.
*/
{
  check_assertion(is_class_struct_union_type(type));
  an_ifc_decl_scope scope_decl = {0};
  an_ifc_decl_index
                result = writer_il_map_new_decl(self, type, &scope_decl);

  /* Set the name information. */
  an_ifc_name_index
                ifc_name_index = writer_il_entity_name_as_name_index(self, type);
  set_ifc_name(&scope_decl, ifc_name_index);

  /* Set the source location information. */
  an_ifc_source_location
                ifc_src_pos = writer_il_find_or_enter_entity_pos(self, type);
  set_ifc_locus(&scope_decl, ifc_src_pos);

  /* Set the IFC fundamental type to indicate this is a class scope. */
  an_ifc_type_index ifc_scope_type = {0};
  switch (type->kind) {
    case tk_class:
      ifc_scope_type = writer_il_find_or_enter_class_scope_type(self);
      break;
    case tk_struct:
      ifc_scope_type = writer_il_find_or_enter_struct_scope_type(self);
      break;
    case tk_union:
      ifc_scope_type = writer_il_find_or_enter_union_scope_type(self);
      break;
    default:
      ifc_write_catastrophe(ec_unsupported_il_to_ifc);
      break;
  }  /* switch */
  set_ifc_type(&scope_decl, ifc_scope_type);

  /* FIXME: Set base. */
  /* Associate the class scope decl with its contents. */
  a_scope_ptr   class_scope = class_type_supp(type)->assoc_scope;
  a_template    *assoc_templ = class_type_supp(type)->assoc_template;
  if (class_scope != NULL && assoc_templ == NULL) {
    /* This traversal is not performed for class templates.  The class template
       definition will be associated below via a trait. */
    an_ifc_scope_offset
                initializer = writer_il_find_or_enter_scope(self, class_scope);

    set_ifc_initializer(&scope_decl, initializer);
    for (a_field_ptr fp = fields_of(type); fp != NULL; fp = fp->next) {
      (void)writer_il_enter_field(self, fp);
    }  /* for */
    /* Traverse the class scope's contents. */
    dump_scope_recursively(self, class_scope);
  }  /* if */

  /* Set the scope information. */
  an_ifc_decl_index
                scope_decl_idx = writer_il_associate_entity_home_scope(self, type);
  set_ifc_home_scope(&scope_decl, scope_decl_idx);
  /* FIXME: Set alignment. */
  /* FIXME: Set pack size. */
  /* FIXME: Set specifiers. */
  /* FIXME: Set traits. */

  /* Set the access specifier. */
  an_ifc_access_sort ifc_access = access_specifier_of(type);
  set_ifc_access(&scope_decl, ifc_access);

  /* Set the properties. */
  an_ifc_reachable_properties_bitfield_query
                properties = (an_ifc_reachable_properties_bitfield_query)0;
  /* Flag that an initializer is present if relevant. */
  if (assoc_templ != NULL) {
    an_ifc_edg_trait_class_template_definition def_trait = {0};
    writer_alloc_decl_trait(self->output_state, result, &def_trait);

    /* Create a token cache representation of the initializing constant. */
    an_ifc_output_token_cache
                init_token_cache = writer_il_copy_template_body_to_cache(self, 
                                                                  assoc_templ);
    an_ifc_edg_token_cache_offset
                token_cache_offset = writer_alloc_token_cache(self->output_state, &init_token_cache);
writer_cache_destroy(&init_token_cache);
    set_ifc_initializer(&def_trait, token_cache_offset);
    /* Apply the initializer flag to indicate the presence of this
       definition. */
    properties = properties | ifc_rpb_initializer;
  } else if (!type->incomplete) {
    properties = properties | ifc_rpb_initializer;
  }  /* if */

  an_ifc_reachable_properties_bitfield_storage
                ifc_raw_properties = to_bitmask_an_ifc_reachable_properties_bitfield_query(scope_decl.file,
                                                properties);
  an_ifc_reachable_properties_bitfield ifc_properties = (an_ifc_reachable_properties_bitfield){scope_decl.file, ifc_raw_properties};
  set_ifc_properties(&scope_decl, ifc_properties);
  return result;
}  /* writer_il_enter_class_struct_union */

static an_ifc_decl_index writer_il_find_or_enter_enum(an_ifc_il_map *self, a_type_ptr type)
/*
For the given enumeration type find or enter the enumeration into the IFC
output state.  Return the declaration index for the enumeration.
*/
{
  an_ifc_decl_index result = writer_get_il_entry_to_decl(self, make_tagged_ptr(type));

  if (is_null_index(result)) {
    result = writer_il_enter_enum(self, type);
  }  /* if */
  return result;
}  /* writer_il_find_or_enter_enum */

static an_ifc_decl_index writer_il_enter_enum(an_ifc_il_map *self, a_type_ptr type)
/*
For the given enumeration type enter the enumeration into the IFC output state.
Return the declaration index for the enumeration.
*/
{
  check_assertion(type->kind == tk_enum && type->variant.integer.enum_type);
  an_ifc_decl_enumeration enum_decl = {0};
  an_ifc_decl_index
                result = writer_il_map_new_decl(self, type, &enum_decl);
  /* Set the name information. */
  an_ifc_text_offset
                ifc_name_offset = writer_il_entity_name_as_text_offset(self, type);

  set_ifc_name(&enum_decl, ifc_name_offset);

  /* Set the source location information. */
  an_ifc_source_location
                ifc_src_pos = writer_il_find_or_enter_entity_pos(self, type);
  set_ifc_locus(&enum_decl, ifc_src_pos);

  /* Set the type information. */
  an_ifc_type_index type_idx = {0};
  if (type->variant.integer.is_scoped_enum) {
    type_idx = writer_il_find_or_enter_scoped_enum_type(self);
  } else {
    type_idx = writer_il_find_or_enter_unscoped_enum_type(self);
  }  /* if */
  set_ifc_type(&enum_decl, type_idx);

  /* Set the base type. */
  an_integer_kind
                base_int_kind = type->variant.integer.int_kind;
  a_type_ptr    base_type = integer_type(base_int_kind);
  an_ifc_type_index
                base_type_idx = writer_il_find_or_enter_type(self, base_type);
  set_ifc_base(&enum_decl, base_type_idx);

  /* Construct and set the initializer to provide the enumerators. */
  an_ifc_sequence seq = writer_il_enter_enumerators(self, type);
  set_ifc_initializer(&enum_decl, seq);

  /* Set the scope information. */
  an_ifc_decl_index
                scope_decl_idx = writer_il_associate_entity_home_scope(self, type);
  set_ifc_home_scope(&enum_decl, scope_decl_idx);
  /* FIXME: Set alignment. */
  /* FIXME: Set specifiers. */

  /* Set the access specifier. */
  an_ifc_access_sort ifc_access = access_specifier_of(type);
  set_ifc_access(&enum_decl, ifc_access);
  /* FIXME: Set properties. */
  return result;
}  /* writer_il_enter_enum */

static an_ifc_decl_index writer_il_find_or_enter_namespace(an_ifc_il_map *self, a_namespace_ptr nsp)
/*
For the given namespace find or enter the respective declaration into the IFC
output state.  Return the declaration index for the entered declaration.
*/
{
  an_ifc_decl_index result = writer_get_il_entry_to_decl(self, make_tagged_ptr(nsp));

  if (is_null_index(result)) {
    result = writer_il_enter_namespace(self, nsp);
  }  /* if */
  return result;
}  /* writer_il_find_or_enter_namespace */

static an_ifc_decl_index writer_il_enter_namespace(an_ifc_il_map *self, a_namespace_ptr nsp)
/*
For the given namespace enter the respective declaration into the IFC output
state.  Return the declaration index for the entered declaration.
*/
{
  an_ifc_decl_scope scope_decl = {0};
  an_ifc_decl_index
                result = writer_il_map_new_decl(self, nsp, &scope_decl);

  /* Set the name information. */
  an_ifc_name_index
                ifc_name_index = writer_il_entity_name_as_name_index(self, nsp);
  set_ifc_name(&scope_decl, ifc_name_index);

  /* Set the source location information. */
  an_ifc_source_location
                ifc_src_pos = writer_il_find_or_enter_entity_pos(self, nsp);
  set_ifc_locus(&scope_decl, ifc_src_pos);

  /* Set the IFC fundamental type to indicate this is a namespace scope. */
  an_ifc_type_index
                ifc_scope_type = writer_il_find_or_enter_namespace_scope_type(self);
  set_ifc_type(&scope_decl, ifc_scope_type);

  /* Namespaces do not have a base type, so use a null type. */
  an_ifc_type_index base_type = {0};
  set_ifc_base(&scope_decl, base_type);

  /* Set the scope information. */
  an_ifc_decl_index
                scope_decl_idx = writer_il_associate_entity_home_scope(self, nsp);
  set_ifc_home_scope(&scope_decl, scope_decl_idx);
  /* FIXME: Set alignment. */
  /* FIXME: Set pack_size. */
  /* FIXME: Set specifiers. */
  /* FIXME: Set traits. */

  /* Set the access specifier. */
  set_ifc_access(&scope_decl, ifc_as_none);
  /* FIXME: Set properties. */
  return result;
}  /* writer_il_enter_namespace */

static an_ifc_decl_index writer_il_find_or_enter_routine(an_ifc_il_map *self, a_routine_ptr rp)
/*
For the given routine find or enter the function declaration into the IFC
output state.  Return the declaration index for the routine.
*/
{
  an_ifc_decl_index result = writer_get_il_entry_to_decl(self, make_tagged_ptr(rp));

  if (is_null_index(result)) {
    result = writer_il_enter_routine(self, rp);
  }  /* if */
  return result;
}  /* writer_il_find_or_enter_routine */

static an_ifc_decl_index writer_il_enter_routine(an_ifc_il_map *self, a_routine_ptr rp)
/*
For the given routine enter the function declaration into the IFC output state.
Return the declaration index for the routine.
*/
{
  an_ifc_decl_index result = {0};

  switch (rp->special_kind) {
    case sfk_none:
      if (routine_type_is_nonstatic_member_function(rp->type)) {
        result = writer_il_enter_member_function(self, rp);
      } else {
        result = writer_il_enter_free_function(self, rp);
      }  /* if */
      break;
    case sfk_constructor:
      result = writer_il_enter_constructor(self, rp);
      break;
    case sfk_conversion:
      /* FIXME: Implement these. */
      break;
    case sfk_destructor:
      result = writer_il_enter_destructor(self, rp);
      break;
    case sfk_operator:
      /* FIXME: Implement these. */
      break;
#if BUILTIN_FUNCTIONS_ENABLED
    case sfk_builtin_operator_delete:
    case sfk_builtin_operator_new:
#endif /* BUILTIN_FUNCTIONS_ENABLED */
    case sfk_deduction_guide:
#if MICROSOFT_EXTENSIONS_ALLOWED
    case sfk_dispose_bool:
    case sfk_event_add:
    case sfk_event_raise:
    case sfk_event_remove:
    case sfk_finalizer:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
#if BUILTIN_FUNCTIONS_ENABLED
    case sfk_gnu_atomic_generic_function:
    case sfk_gnu_atomic_nongeneric_function:
    case sfk_gnu_sync_concrete_function:
#endif /* BUILTIN_FUNCTIONS_ENABLED */
#if MICROSOFT_EXTENSIONS_ALLOWED
    case sfk_idisposable_dispose:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
    case sfk_lambda_entry_point:
#if MICROSOFT_EXTENSIONS_ALLOWED
    case sfk_object_finalize:
    case sfk_property_get:
    case sfk_property_set:
    case sfk_static_constructor:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
    case sfk_udl_operator:
      ifc_write_catastrophe(ec_unsupported_il_to_ifc);
      break;
    case sfk_last:
      /* sfk_last should not appear in the IL. */
      unexpected_condition();
    default_is_unexpected();
  }  /* switch */
  return result;
}  /* writer_il_enter_routine */

static an_ifc_decl_index writer_il_find_or_enter_template(an_ifc_il_map *self, a_template_ptr templ)
/*
For the given template find or enter the template declaration into the IFC
output state.  Return the declaration index for the template.
*/
{
  a_tagged_pointer  tagged_templ = make_tagged_ptr(templ);
  an_ifc_decl_index result = writer_get_il_entry_to_decl(self, tagged_templ);

  if (is_null_index(result)) {
    result = writer_il_enter_template(self, templ);
  }  /* if */
  return result;
}  /* writer_il_find_or_enter_template */

static inline a_boolean is_alias_template(a_template_ptr templ)
/*
Return TRUE if the given template is an alias template; otherwise, return
FALSE.
*/
{
  a_symbol_ptr          template_sym = symbol_for(templ);
  a_template_symbol_supplement_ptr
                        tssp = template_sym->variant.template_info;

  return tssp->variant.class_template.is_alias_template;
}  /* is_alias_template */


static an_ifc_decl_index writer_il_enter_template(an_ifc_il_map *self, a_template_ptr templ)
/*
For the given template enter the template declaration into the IFC output
state.  Return the declaration index for the template.
*/
{
  an_ifc_decl_index result = {0};

  switch (templ->kind) {
    case templk_class:
      if (is_alias_template(templ)) {
        result = writer_il_enter_alias_template(self, templ);
      } else {
        result = writer_il_enter_class_template(self, templ);
      }  /* if */
      break;
    case templk_concept:
      ifc_write_catastrophe(ec_unsupported_il_to_ifc);
      break;
    case templk_function:
      result = writer_il_enter_function_template(self, templ);
      break;
    case templk_member_class:
    case templk_member_enum:
    case templk_member_function:
    case templk_none:
    case templk_static_data_member:
    case templk_template_template_param:
    case templk_variable:
      ifc_write_catastrophe(ec_unsupported_il_to_ifc);
      break;
    default_is_unexpected();
  }  /* switch */
  return result;
}  /* writer_il_enter_template */

static an_ifc_decl_index writer_il_find_or_enter_using_directive(an_ifc_il_map *self, 
                                                        a_using_decl_ptr udp,
                                                        a_scope_ptr      scope)
/*
For the given using directive find or enter the using directive in the given
scope into the IFC output state.  Return the declaration index for the using
directive.

Note this function explicitly requires a scope as using directives do not have
a source correspondence and thus must explicitly be provided a scope.
*/
{
  a_tagged_pointer  tagged_udp = make_tagged_ptr(udp);
  an_ifc_decl_index result = writer_get_il_entry_to_decl(self, tagged_udp);

  if (is_null_index(result)) {
    result = writer_il_enter_using_directive(self, udp, scope);
  }  /* if */
  return result;
}  /* writer_il_find_or_enter_using_directive */

static an_ifc_decl_index writer_il_enter_using_directive(an_ifc_il_map *self, a_using_decl_ptr udp,
                                                       a_scope_ptr      scope)
/*
For the given using directive enter the using directive in the given scope into
the IFC output state.  Return the declaration index for the using directive.

Note this function explicitly requires a scope as using directives do not have
a source correspondence and thus must explicitly be provided a scope.
*/
{
  an_ifc_decl_barren barren_decl = {0};
  an_ifc_decl_index
                result = writer_il_map_new_decl(self, udp, &barren_decl);
  /* Set up the directive. */
  an_ifc_dir_using using_dir = {0};
  an_ifc_dir_index
                using_dir_idx = writer_alloc_dir(self->output_state, &using_dir);
  /* Set the directive location. */
  an_ifc_source_location
                ifc_src_pos = writer_il_find_or_enter_pos(self, &(udp->position));

  set_ifc_locus(&using_dir, ifc_src_pos);
  /* FIXME: Implement the "nominated" (as spelled in source form). */

  /* Set the resolved scope. */
  /* FIXME: Check the entity type. */
  an_ifc_decl_index
                namespace_idx = writer_il_find_or_enter_namespace(self, 
                                             (a_namespace_ptr)udp->entity.ptr);
  /* It is expected that the namespace will have already been entered. */
  check_assertion(!is_null_index(namespace_idx));
  set_ifc_resolution(&using_dir, namespace_idx);
  /* Set the directive on the barren declaration. */
  set_ifc_directive(&barren_decl, using_dir_idx);
  /* FIXME: Set specifiers. */

  /* Set the access specifier. */
  an_ifc_access_sort
                access_sort = convert_access_specifier(udp->access);
  set_ifc_access(&barren_decl, access_sort);
  /* Associate the using-directive with its scope. */
  writer_associate_home_scope(self, make_tagged_ptr(udp), scope);
  return result;
}  /* writer_il_enter_using_directive */

static an_ifc_text_offset writer_il_string_as_text_offset_bytes(an_ifc_il_map *self, a_const_char *str,
                                                        size_t       str_len)
/*
Return the text offset representing the given string of the given length.  The
lifetime of the given string must be at least as long as the lifetime of the
IFC IL map.
*/
{
  size_t name_offset = 0;

  if (str != NULL && str[0] != '\0') {
    a_string_view str_handle = edg_string_view(str, str_len);

    name_offset = writer_get_string_table_map(self, str_handle);
    if (name_offset == 0) {
      /* The name does not yet exist in the string table.  Add it now. */
      name_offset = writer_add_string(self->output_state, str);
      writer_set_string_table_map(self, str_handle, name_offset);
    }  /* if */
  }  /* if */
  return (an_ifc_text_offset){self->output_state->output_file, name_offset};
}  /* writer_il_string_as_text_offset */

static an_ifc_text_offset writer_il_string_as_text_offset(an_ifc_il_map *self, a_const_char *str)
/*
Return the text offset representing the given string.  The lifetime of the
given string must be at least as long as the lifetime of the IFC IL map.
*/
{
  an_ifc_text_offset result = {0};

  if (str != NULL && str[0] != '\0') {
    result = writer_il_string_as_text_offset_bytes(self, str, strlen(str));
  }  /* if */
  return result;
}  /* writer_il_string_as_text_offset */

static an_ifc_name_index writer_il_string_as_name_index(an_ifc_il_map *self, a_const_char *str)
/*
Return the name index representing the given string.  The lifetime of the given
string must be at least as long as the lifetime of the IFC IL map.
*/
{
  an_ifc_text_offset text_offset = writer_il_string_as_text_offset(self, str);

  return (an_ifc_name_index){self->output_state->output_file, ifc_ns_text_offset,
                           (an_ifc_text_offset_storage)text_offset.value};
}  /* writer_il_string_as_name_index */

static an_ifc_name_index writer_il_find_or_enter_src_file(an_ifc_il_map *self, 
                                                       a_source_file_ptr file)
/*
For the given source file find or enter the file into the IFC output state.
Return the name index for the source file.
*/
{
  an_ifc_name_index result = writer_get_src_file_map(self, file);

  if (is_null_index(result)) {
    result = writer_il_enter_src_file(self, file);
  }  /* if */
  return result;
}  /* writer_il_find_or_enter_src_file */

static an_ifc_name_index writer_il_enter_src_file(an_ifc_il_map *self, a_source_file_ptr file)
/*
For the given source file enter the file into the IFC output state.  Return the
name index for the source file.
*/
{
  an_ifc_name_source_file src_file = {0};
  an_ifc_name_index
                result = writer_alloc_name(self->output_state, &src_file);
  size_t        path_offset = writer_add_string(self->output_state, 
                                                             file->file_name);
  an_ifc_text_offset ifc_path_offset = (an_ifc_text_offset){src_file.file, path_offset};

  set_ifc_path(&src_file, ifc_path_offset);
  writer_set_src_file_map(self, file, result);
  return result;
}  /* writer_il_enter_src_file */

static inline a_seq_number_key
make_seq_num_map_key(const a_source_position *pos)
/*
Given a source position, return the corresponding hash map key.
*/
{
  return (a_seq_number_key)(pos->seq + 1);
}  /* make_seq_num_map_key */


static an_ifc_source_location writer_il_find_or_enter_null_pos(an_ifc_il_map *self)
/*
Find or enter the NULL source position into the IFC output state.  Return the
corresponding IFC source location.
*/
{
  return writer_il_find_or_enter_pos(self, &(null_source_position));
}  /* writer_il_find_or_enter_null_pos */

static an_ifc_source_location writer_il_find_or_enter_pos(an_ifc_il_map *self, 
                                                 const a_source_position *pos)
/*
For the given source position find or enter the source position into the IFC
output state.  Return the corresponding IFC source location.
*/
{
  an_ifc_source_location result = {0};
  an_ifc_line_offset     ifc_line_offset = writer_get_seq_number_map(self, make_seq_num_map_key(pos));

  if (is_null_index(ifc_line_offset)) {
    result = writer_il_enter_pos(self, pos);
  } else {
    an_ifc_column ifc_column = (an_ifc_column){ifc_line_offset.file, pos->column};

    result = an_ifc_source_location_init(ifc_line_offset.file);
    set_ifc_line(&result, ifc_line_offset);
    set_ifc_column(&result, ifc_column);
  }  /* if */
  return result;
}  /* writer_il_find_or_enter_pos */

static an_ifc_source_location writer_il_enter_pos(an_ifc_il_map *self, const a_source_position *pos)
/*
For the given source position enter the source position into the IFC output
state.  Return the corresponding IFC source location.
*/
{
  a_seq_number_key   seq_key = make_seq_num_map_key(pos);

  check_assertion(is_null_index(writer_get_seq_number_map(self, seq_key)));

  an_ifc_line_offset ifc_line_offset = {0};
  if (pos->seq == 0) {
    /* A unknown source line is being referenced. */
    /* Allocate a source line node. */
    an_ifc_source_line line = {0};
    size_t      line_offset = writer_alloc_node(self->output_state, &line);
    /* Allocate a source file for the empty source file. */
    an_ifc_name_source_file src_file = {0};
    an_ifc_name_index
                src_file_idx = writer_alloc_name(self->output_state, &src_file);

    /* Associate the source file. */
    set_ifc_file(&line, src_file_idx);

    /* Associate an absent line number. */
    an_ifc_line_number ifc_line_number = (an_ifc_line_number){self->output_state->output_file, 0};
    set_ifc_line(&line, ifc_line_number);
    /* Use the created line offset. */
    ifc_line_offset = (an_ifc_line_offset){line.file, line_offset + 1};
  } else {
    /* A known source line is required. */
    an_ifc_source_line line = {0};
    size_t      line_offset = writer_alloc_node(self->output_state, &line);
      /* Gather the information for the source sequence. */
    a_line_number
                line_number;
    a_boolean   at_end_of_source;
    /* physical_line == FALSE means consider information from #line directives
       as well as true file information. */
    a_source_file_ptr
                file = source_file_for_seq(pos->seq, &line_number,
                                           &at_end_of_source,
                                           /*physical_line=*/FALSE);
    an_ifc_name_index
                file_name_idx = writer_il_find_or_enter_src_file(self, file);
    an_ifc_line_number ifc_line_number = (an_ifc_line_number){line.file, line_number};

    set_ifc_file(&line, file_name_idx);
    set_ifc_line(&line, ifc_line_number);
    /* Use the created line offset. */
    ifc_line_offset = (an_ifc_line_offset){line.file, line_offset + 1};
  }  /* if */
  writer_set_seq_number_map(self, seq_key, ifc_line_offset);

  /* Form a result IFC SourceLocation. */
  an_ifc_source_location result = an_ifc_source_location_init(ifc_line_offset.file);
  an_ifc_column ifc_column = (an_ifc_column){result.file, pos->column};
  set_ifc_line(&result, ifc_line_offset);
  set_ifc_column(&result, ifc_column);
  return result;
}  /* writer_il_enter_pos */

static an_ifc_type_index writer_il_enter_class_struct_union_type(an_ifc_il_map *self, a_type_ptr type)
/*
Enter the given class, struct, or union type into the IFC output state.  Return
the type index for the class type.
*/
{
  check_assertion(is_class_or_struct(type));
  an_ifc_type_index result = {0};
  a_template_arg_ptr
                arg_list = class_type_supp(type)->template_arg_list;
  unsigned      arg_count = edg_count_list_elements(arg_list, offsetof(a_template_arg, next));

  if (arg_count > 0) {
    /* This is a substituted type, enter it as such. */
    an_ifc_edg_type_substituted substituted_type = {0};

    result = writer_il_map_new_type(self, type, &substituted_type);

    a_template_ptr
                templ = class_type_supp(type)->assoc_template;
    an_ifc_decl_index
                ifc_templ_idx = writer_il_find_or_enter_template(self, templ);
    set_ifc_subject(&substituted_type, ifc_templ_idx);

    /* Preallocate all the arguments to ensure we get one contiguous block. */
    size_t      start = writer_alloc_node_block(self->output_state, get_ifc_partition_kind_an_ifc_edg_heap_template_argument(), arg_count);
    /* Associate the preallocated parameter nodes with the type. */
    an_ifc_module_file
                *file = self->output_state->output_file;
    an_ifc_edg_heap_template_argument_offset ifc_arg_start = (an_ifc_edg_heap_template_argument_offset){file, start};
    an_ifc_cardinality ifc_arg_count = (an_ifc_cardinality){file, arg_count};
    set_ifc_arguments(&substituted_type, ifc_arg_start);
    set_ifc_num_arguments(&substituted_type, ifc_arg_count);

    /* Complete the arguments. */
    a_template_arg_ptr curr_templ_arg = arg_list;
    for (size_t i = 0; i < arg_count; ++i) {
      size_t                            curr_arg_offset = start + i;
      an_ifc_edg_heap_template_argument curr_arg = {0};

      writer_fetch_node(self->output_state, &curr_arg, curr_arg_offset);

      an_ifc_edg_template_argument_index
                arg_index = writer_il_enter_template_argument(self, curr_templ_arg);
      set_ifc_index(&curr_arg, arg_index);
      /* Advance to the next argument. */
      curr_templ_arg = curr_templ_arg->next;
    }  /* for */
  } else {
    an_ifc_type_designated designated_type = {0};

    result = writer_il_map_new_type(self, type, &designated_type);

    an_ifc_decl_index
                decl_idx = writer_il_find_or_enter_class_struct_union(self, type);
    set_ifc_decl(&designated_type, decl_idx);
  }  /* if */
  return result;
}  /* writer_il_enter_class_struct_union_type */

static an_ifc_type_index writer_il_enter_constructor_type(an_ifc_il_map *self, a_type_ptr type)
/*
Enter the type for the given constructor type into the IFC output state.
Return the type index for the constructor type.
*/
{
  an_ifc_type_tor tor_type = {0};
  an_ifc_type_index
                result = writer_alloc_type(self->output_state, &tor_type);

  an_ifc_type_index
                ifc_param_types = writer_il_enter_routine_params_type(self, type);
  set_ifc_source(&tor_type, ifc_param_types);

  /* FIXME: Set eh_spec. */
  /* FIXME: Set convention. */
  return result;
}  /* writer_il_enter_constructor_type */

static an_ifc_type_index writer_il_enter_enum_type(an_ifc_il_map *self, a_type_ptr type)
/*
Enter the given enum type into the IFC output state.  Return the type index for
the enum type.
*/
{
  check_assertion(type->kind == tk_integer);
  an_ifc_type_designated designated_type = {0};
  an_ifc_type_index
                result = writer_il_map_new_type(self, type, &designated_type);
  an_ifc_decl_index
                decl_idx = writer_il_find_or_enter_enum(self, type);

  set_ifc_decl(&designated_type, decl_idx);
  return result;
}  /* writer_il_enter_enum_type */

static an_ifc_type_index writer_il_enter_float_type(an_ifc_il_map *self, a_type_ptr type)
/*
Enter the given float type into the IFC output state.  Return the type index
for the float type.
*/
{
  check_assertion(type->kind == tk_float);
  an_ifc_type_fundamental fund_type = {0};
  an_ifc_type_index
                result = writer_il_map_new_type(self, type, &fund_type);

  switch (type->variant.float_kind) {
    case fk_float:
      set_ifc_basis(&fund_type, ifc_tbs_float);
      set_ifc_precision(&fund_type, ifc_tps_default);
      break;
    case fk_double:
      set_ifc_basis(&fund_type, ifc_tbs_double);
      set_ifc_precision(&fund_type, ifc_tps_default);
      break;
    case fk_long_double:
      set_ifc_basis(&fund_type, ifc_tbs_double);
      set_ifc_precision(&fund_type, ifc_tps_long);
      break;
    case fk_float128:
      set_ifc_basis(&fund_type, ifc_tbs_float);
      set_ifc_precision(&fund_type, ifc_tps_bit128);
      break;
    case fk_float16:
    case fk_fp16:
    case fk_float32x:
    case fk_float64x:
    case fk_float80:
    case fk_std_bfloat16:
    case fk_std_float16:
    case fk_std_float32:
    case fk_std_float64:
    case fk_std_float128:
    case fk_last:
      ifc_write_catastrophe(ec_unsupported_il_to_ifc);
      break;
    default_is_unexpected();
  }  /* switch */
  set_ifc_sign(&fund_type, ifc_tss_signed);
  return result;
}  /* writer_il_enter_float_type */

static an_ifc_type_index writer_il_enter_free_function_type(an_ifc_il_map *self, a_type_ptr type)
/*
Enter the given free function (routine) type into the IFC output state.  Return
the type index for the function type.
*/
{
  check_assertion(type->kind == tk_routine);
  an_ifc_type_function func_type = {0};
  an_ifc_type_index
                result = writer_il_map_new_type(self, type, &func_type);
  a_type_ptr    return_type = type->variant.routine.return_type;
  an_ifc_type_index
                ifc_return_type = writer_il_find_or_enter_type(self, return_type);

  set_ifc_target(&func_type, ifc_return_type);

  an_ifc_type_index
                ifc_param_types = writer_il_enter_routine_params_type(self, type);
  set_ifc_source(&func_type, ifc_param_types);
  /* FIXME: Set eh_spec. */
  /* FIXME: Set convention. */
  /* FIXME: Set traits. */
  return result;
}  /* writer_il_enter_free_function_type */

static an_ifc_type_precision_sort type_size_to_precision(a_type_ptr type)
/*
Given a type, return the corresponding precision.
*/
{
  an_ifc_type_precision_sort result = {0};

  switch (size_of_type(type)) {
    case 1:
      result = ifc_tps_bit8;
      break;
    case 2:
      result = ifc_tps_bit16;
      break;
    case 4:
      result = ifc_tps_bit32;
      break;
    case 8:
      result = ifc_tps_bit64;
      break;
    case 16:
      result = ifc_tps_bit128;
      break;
    default:
      /* The conversion from type size to type precision is unknown. */
      unexpected_condition();
  }  /* switch */
  return result;
}  /* type_size_to_precision */


static an_ifc_type_index writer_il_enter_integer_type(an_ifc_il_map *self, a_type_ptr type)
/*
Enter the given integer type into the IFC output state.  Return the type index
for the integer type.
*/
{
  check_assertion(type->kind == tk_integer);
  an_ifc_type_fundamental fund_type = {0};
  an_ifc_type_index
                result = writer_il_map_new_type(self, type, &fund_type);

  /* Set the basis, precision, and sign. */
  if (type->variant.integer.bool_type) {
    /* Handle the bool type. */
    set_ifc_basis(&fund_type, ifc_tbs_bool);
    set_ifc_precision(&fund_type, ifc_tps_default);
    set_ifc_sign(&fund_type, ifc_tss_plain);
  } else if (type->variant.integer.wchar_t_type) {
    /* Handle the wchar_t type. */
    set_ifc_basis(&fund_type, ifc_tbs_wchar_t);
    set_ifc_precision(&fund_type, ifc_tps_default);
    set_ifc_sign(&fund_type, ifc_tss_plain);
  } else if (type->variant.integer.char8_t_type) {
    /* Handle the char8_t type. */
    set_ifc_basis(&fund_type, ifc_tbs_char);
    set_ifc_precision(&fund_type, ifc_tps_bit8);
    set_ifc_sign(&fund_type, ifc_tss_plain);
  } else if (type->variant.integer.char16_t_type) {
    /* Handle the char16_t type. */
    set_ifc_basis(&fund_type, ifc_tbs_char);
    set_ifc_precision(&fund_type, ifc_tps_bit16);
    set_ifc_sign(&fund_type, ifc_tss_plain);
  } else if (type->variant.integer.char32_t_type) {
    /* Handle the char32_t type. */
    set_ifc_basis(&fund_type, ifc_tbs_char);
    set_ifc_precision(&fund_type, ifc_tps_bit32);
    set_ifc_sign(&fund_type, ifc_tss_plain);
  } else {
    switch (type->variant.integer.int_kind) {
      case ik_char:
        /* Handle the char type. */
        set_ifc_basis(&fund_type, ifc_tbs_char);
        set_ifc_precision(&fund_type, ifc_tps_default);
        set_ifc_sign(&fund_type, ifc_tss_plain);
        break;
      case ik_signed_char:
        /* Handle the signed char type. */
        set_ifc_basis(&fund_type, ifc_tbs_char);
        set_ifc_precision(&fund_type, ifc_tps_default);
        set_ifc_sign(&fund_type, ifc_tss_signed);
        break;
      case ik_unsigned_char:
        /* Handle the unsigned char type. */
        set_ifc_basis(&fund_type, ifc_tbs_char);
        set_ifc_precision(&fund_type, ifc_tps_default);
        set_ifc_sign(&fund_type, ifc_tss_unsigned);
        break;
      case ik_short:
        /* Handle the (signed) short type. */
        set_ifc_basis(&fund_type, ifc_tbs_int);
        set_ifc_precision(&fund_type, ifc_tps_short);
        set_ifc_sign(&fund_type, ifc_tss_signed);
        break;
      case ik_unsigned_short:
        /* Handle the unsigned short type. */
        set_ifc_basis(&fund_type, ifc_tbs_int);
        set_ifc_precision(&fund_type, ifc_tps_short);
        set_ifc_sign(&fund_type, ifc_tss_unsigned);
        break;
      case ik_int:
        /* Handle the (signed) int type. */
        set_ifc_basis(&fund_type, ifc_tbs_int);
        set_ifc_precision(&fund_type, ifc_tps_default);
        set_ifc_sign(&fund_type, ifc_tss_signed);
        break;
      case ik_unsigned_int:
        /* Handle the unsigned int type. */
        set_ifc_basis(&fund_type, ifc_tbs_int);
        set_ifc_precision(&fund_type, ifc_tps_default);
        set_ifc_sign(&fund_type, ifc_tss_unsigned);
        break;
      case ik_long:
        /* Handle the (signed) long type. */
        set_ifc_basis(&fund_type, ifc_tbs_int);
        set_ifc_precision(&fund_type, ifc_tps_long);
        set_ifc_sign(&fund_type, ifc_tss_signed);
        break;
      case ik_unsigned_long:
        /* Handle the unsigned long type. */
        set_ifc_basis(&fund_type, ifc_tbs_int);
        set_ifc_precision(&fund_type, ifc_tps_long);
        set_ifc_sign(&fund_type, ifc_tss_unsigned);
        break;
      default:
        /* Handle miscellaneous fixed size integer types (e.g., uint32_t,
           int32_t, etc). */
        { an_ifc_type_precision_sort
                  precision_sort = type_size_to_precision(type);
          an_ifc_type_sign_sort
                  sign_sort = is_signed_integral_type(type) ? ifc_tss_signed
                                                            : ifc_tss_unsigned;
          set_ifc_basis(&fund_type, ifc_tbs_int);
          set_ifc_precision(&fund_type, precision_sort);
          set_ifc_sign(&fund_type, sign_sort);
        }
        break;
    }  /* switch */
  }  /* if */
  return result;
}  /* writer_il_enter_integer_type */

static an_ifc_type_index writer_il_enter_member_function_type(an_ifc_il_map *self, a_type_ptr type)
/*
Enter the given member function (routine) type into the IFC output state.
Return the type index for the function type.
*/
{
  check_assertion(type->kind == tk_routine);
  an_ifc_type_method func_type = {0};
  an_ifc_type_index
                result = writer_il_map_new_type(self, type, &func_type);
  a_type_ptr    return_type = type->variant.routine.return_type;
  an_ifc_type_index
                ifc_return_type = writer_il_find_or_enter_type(self, return_type);

  set_ifc_target(&func_type, ifc_return_type);

  an_ifc_type_index
                ifc_param_types = writer_il_enter_routine_params_type(self, type);
  set_ifc_source(&func_type, ifc_param_types);

  a_routine_type_supplement_ptr
                rtsp = type->variant.routine.extra_info;
  a_type_ptr    class_type = rtsp->this_class;
  an_ifc_type_index
                ifc_class_type  = writer_il_find_or_enter_type(self, class_type);
  set_ifc_scope(&func_type, ifc_class_type);
  /* FIXME: Set eh_spec. */
  /* FIXME: Set convention. */
  /* FIXME: Set traits. */
  return result;
}  /* writer_il_enter_member_function_type */

static an_ifc_type_index writer_il_enter_nullptr_type(an_ifc_il_map *self, a_type_ptr type)
/*
Enter the given nullptr type into the IFC output state.  Return the type index
for the nullptr type.
*/
{
  check_assertion(type->kind == tk_nullptr);
  an_ifc_type_fundamental fund_type = {0};
  an_ifc_type_index
                result = writer_il_map_new_type(self, type, &fund_type);

  set_ifc_basis(&fund_type, ifc_tbs_nullptr);
  set_ifc_precision(&fund_type, ifc_tps_default);
  set_ifc_sign(&fund_type, ifc_tss_plain);
  return result;
}  /* writer_il_enter_nullptr_type */

static an_ifc_type_index writer_il_enter_pointer_type(an_ifc_il_map *self, a_type_ptr type)
/*
Enter the given pointer type into the IFC output state.  Return the type index
for the pointer type.
*/
{
  check_assertion(type->kind == tk_pointer);
  an_ifc_type_index result = {0};
  a_type_ptr        pointee_type = type->variant.pointer.type;
  an_ifc_type_index ifc_pointee_type = writer_il_find_or_enter_type(self, pointee_type);

  if (type->variant.pointer.is_reference) {
    if (type->variant.pointer.is_rvalue_reference) {
      an_ifc_type_lvalue_reference ref_type = {0};

      result = writer_il_map_new_type(self, type, &ref_type);
      set_ifc_referee(&ref_type, ifc_pointee_type);
    } else {
      an_ifc_type_rvalue_reference ref_type = {0};

      result = writer_il_map_new_type(self, type, &ref_type);
      set_ifc_referee(&ref_type, ifc_pointee_type);
    }  /* if */
  } else {
    an_ifc_type_pointer ptr_type = {0};

    result = writer_il_map_new_type(self, type, &ptr_type);
    set_ifc_pointee(&ptr_type, ifc_pointee_type);
  }  /* if */
  return result;
}  /* writer_il_enter_pointer_type */

static an_ifc_type_index writer_il_enter_routine_params_type(an_ifc_il_map *self, a_type_ptr type)
/*
Given a routine type, find or enter the types for the routine's parameters.
Return the IFC type index of the parameter types.
*/
{
  check_assertion(type->kind == tk_routine);
  an_ifc_type_index result = {0};
  Edg_array param_types;
  an_ifc_type_index param_types_buffer[10];
  edg_array_init_buffer(&param_types, sizeof(an_ifc_type_index), EDG_ALLOC_GENERAL,
                       10, NULL, NULL, param_types_buffer, 10);
  a_param_type_ptr
                curr_param_type = function_type_params(type);

  /* Convert each parameter type and collect the converted types. */
  while (curr_param_type != NULL) {
    an_ifc_type_index ifc_param_type =
                              writer_il_find_or_enter_type(self, curr_param_type->type);

    edg_array_push(&param_types, &ifc_param_type);
    curr_param_type = curr_param_type->next;
  }  /* while */

  if (param_types.len == 0) {
    /* Do nothing, the result type is a null index as there are no
       parameters. */
  } else if (param_types.len == 1) {
    /* There is only one parameter type, use it directly. */
    result = *(an_ifc_type_index *)edg_array_at(&param_types, 0);
  } else {
    /* Add the parameter types to the type heap and track the type heap offset
       of the first type added to the heap. */
    a_boolean first = TRUE;
    size_t    heap_start_offset = 0;

    for (size_t param_i = 0; param_i < param_types.len; ++param_i) {
      an_ifc_type_index param_type = *(an_ifc_type_index *)edg_array_at(&param_types, param_i);
      an_ifc_heap_type heap_type = {0};
      size_t    heap_part_offset = writer_alloc_node(self->output_state, &heap_type);

      if (first) {
        /* Update the heap start offset. */
        first = FALSE;
        heap_start_offset = heap_part_offset;
      }  /* if */
      set_ifc_value(&heap_type, param_type);
    }  /* for */

    /* Create and return a type tuple type pointing to all the parameters for
       this type. */
    an_ifc_type_tuple param_tuple_type = {0};
    result = writer_alloc_type(self->output_state, &param_tuple_type);

    an_ifc_index ifc_type_heap_start = (an_ifc_index){param_tuple_type.file,
                                    heap_start_offset};
    set_ifc_start(&param_tuple_type, ifc_type_heap_start);

    an_ifc_cardinality ifc_param_type_count = (an_ifc_cardinality){
                            param_tuple_type.file,
                            (an_ifc_cardinality_storage)param_types.len};
    set_ifc_cardinality(&param_tuple_type, ifc_param_type_count);
  }  /* if */
  edg_array_destroy(&param_types);
  return result;
}  /* writer_il_enter_routine_params_type */

static an_ifc_type_index writer_il_enter_routine_type(an_ifc_il_map *self, a_type_ptr type)
/*
Enter the given routine type into the IFC output state.  Return the type index
for the function type.
*/
{
  check_assertion(type->kind == tk_routine);
  an_ifc_type_index result = {0};
  a_routine_type_supplement_ptr
                rtsp = type->variant.routine.extra_info;
  a_routine_ptr rp = rtsp->assoc_routine;

  if (rp == NULL) {
    /* Sometimes there's no associated routine (e.g., this a type corresponding
       to a function pointer). */
    if (routine_type_is_nonstatic_member_function(type)) {
      result = writer_il_enter_member_function_type(self, type);
    } else {
      result = writer_il_enter_free_function_type(self, type);
    }  /* if */
  } else {
    switch (rp->special_kind) {
      case sfk_none:
        if (routine_type_is_nonstatic_member_function(type)) {
          result = writer_il_enter_member_function_type(self, type);
        } else {
          result = writer_il_enter_free_function_type(self, type);
        }  /* if */
        break;
      case sfk_constructor:
        result = writer_il_enter_constructor_type(self, type);
        break;
      case sfk_conversion:
        /* FIXME: Implement these. */
        break;
      case sfk_destructor:
        /* Destructors do not have types in the IFC and should not appear
           here. */
        unexpected_condition();
      case sfk_operator:
        /* FIXME: Implement these. */
        break;
  #if BUILTIN_FUNCTIONS_ENABLED
      case sfk_builtin_operator_delete:
      case sfk_builtin_operator_new:
  #endif /* BUILTIN_FUNCTIONS_ENABLED */
      case sfk_deduction_guide:
  #if MICROSOFT_EXTENSIONS_ALLOWED
      case sfk_dispose_bool:
      case sfk_event_add:
      case sfk_event_raise:
      case sfk_event_remove:
      case sfk_finalizer:
  #endif /* MICROSOFT_EXTENSIONS_ALLOWED */
  #if BUILTIN_FUNCTIONS_ENABLED
      case sfk_gnu_atomic_generic_function:
      case sfk_gnu_atomic_nongeneric_function:
      case sfk_gnu_sync_concrete_function:
  #endif /* BUILTIN_FUNCTIONS_ENABLED */
  #if MICROSOFT_EXTENSIONS_ALLOWED
      case sfk_idisposable_dispose:
  #endif /* MICROSOFT_EXTENSIONS_ALLOWED */
      case sfk_lambda_entry_point:
  #if MICROSOFT_EXTENSIONS_ALLOWED
      case sfk_object_finalize:
      case sfk_property_get:
      case sfk_property_set:
      case sfk_static_constructor:
  #endif /* MICROSOFT_EXTENSIONS_ALLOWED */
      case sfk_udl_operator:
        ifc_write_catastrophe(ec_unsupported_il_to_ifc);
        break;
      case sfk_last:
        /* sfk_last should not appear in the IL. */
        unexpected_condition();
      default_is_unexpected();
    }  /* switch */
  }  /* if */
  return result;
}  /* writer_il_enter_routine_type */

static an_ifc_type_index writer_il_enter_template_type_param_type(an_ifc_il_map *self, 
                                              a_type_ptr        type,
                                              an_ifc_decl_index param_decl_idx)
/*
Enter the IFC template type parameter corresponding to the given type
represented by the given IFC declaration index.  Return the type index for
the template type parameter.
*/
{
  an_ifc_type_designated designated_type = {0};
  an_ifc_type_index
                result = writer_il_map_new_type(self, type, &designated_type);

  set_ifc_decl(&designated_type, param_decl_idx);
  return result;
}  /* writer_il_enter_template_type_param_type */

static an_ifc_type_index writer_il_enter_typedef_type(an_ifc_il_map *self, a_type_ptr type)
/*
Enter the given typedef type into the IFC output state.  Return the type index
for the typedef type.
*/
{
  check_assertion(type->kind == tk_typeref && typeref_is_typedef(type));
  an_ifc_type_designated designated_type = {0};
  an_ifc_type_index
                result = writer_il_map_new_type(self, type, &designated_type);
  an_ifc_decl_index
                typedef_decl = writer_il_enter_typedef(self, type);

  set_ifc_decl(&designated_type, typedef_decl);
  return result;
}  /* writer_il_enter_typedef_type */

static an_ifc_type_index writer_il_enter_void_type(an_ifc_il_map *self, a_type_ptr type)
/*
Enter the given void type into the IFC output state.  Return the type index
for the void type.
*/
{
  an_ifc_type_fundamental fund_type = {0};
  an_ifc_type_index
                result = writer_il_map_new_type(self, type, &fund_type);

  set_ifc_basis(&fund_type, ifc_tbs_void);
  return result;
}  /* writer_il_enter_void_type */

static an_ifc_output_token_cache writer_il_copy_template_body_to_cache(an_ifc_il_map *self, 
                                                          a_template_ptr templ)
/*
This is a utility function for creating an IFC output token cache representing
the body of the given template.  The created IFC output token cache is
returned.
*/
{
  an_ifc_output_token_cache result;
  writer_cache_init(&result);
  a_symbol_ptr  templ_sym = symbol_for(templ);
  a_template_symbol_supplement_ptr
                tssp = templ_sym->variant.template_info;

  if (!edg_reusable_cache_is_empty(&tssp->cache->tokens)) {
    writer_il_enter_token_cache(self, &result, edg_reusable_cache_ptr(&tssp->cache->tokens));
  }  /* if */
  return result;
}  /* writer_il_copy_template_body_to_cache */

static a_constant_ptr enumerator_constants_for_type(a_type_ptr type)
/*
Return the list of constants representing the enumerators of the given
enumeration type.
*/
{
  check_assertion(type->kind == tk_enum && type->variant.integer.enum_type);
  a_constant_ptr result = NULL;

  if (type->variant.integer.is_scoped_enum) {
    /* Scoped enumerators use a scope with a constant list rather than
       storing the constants directly on the integer type.  Check for
       said scope and then read the constants from it if it exists. */
    a_scope_ptr scope = type->variant.integer.enum_info.assoc_scope;

    if (scope != NULL) {
      result = scope->constants;
    }  /* if */
  } else {
    result = type->variant.integer.enum_info.constant_list;
  }  /* if */
  return result;
}  /* enumerator_constants_for_type */


static an_ifc_sequence writer_il_enter_enumerators(an_ifc_il_map *self, a_type_ptr type)
/*
Given a enumeration type, enter the type's enumerators.  Return the IFC
sequence representing the IFC DeclSort::Enumerators.
*/
{
  check_assertion(type->kind == tk_enum && type->variant.integer.enum_type);
  an_ifc_sequence result = an_ifc_sequence_init(self->output_state->output_file);
  a_constant_ptr  constants = enumerator_constants_for_type(type);
  size_t          num_constants = edg_count_list_elements(constants, offsetof(a_constant, next));

  if (num_constants > 0) {
    /* Preallocate all the enumerators to ensure we get one contiguous
       block. */
    size_t      start = writer_alloc_node_block(self->output_state, get_ifc_partition_kind_an_ifc_decl_enumerator(), num_constants);
    /* Associate the preallocated enumerator nodes with the sequence. */
    an_ifc_index ifc_start = (an_ifc_index){self->output_state->output_file, start};
    an_ifc_cardinality ifc_cardinality = (an_ifc_cardinality){self->output_state->output_file, num_constants};

    set_ifc_start(&result, ifc_start);
    set_ifc_cardinality(&result, ifc_cardinality);

    /* Find or enter the enumeration type. */
    an_ifc_type_index
                enumeration_type = writer_il_find_or_enter_type(self, type);
    /* Complete the enumerator declarations. */
    a_constant_ptr curr_constant = constants;
    for (size_t i = 0; i < num_constants; ++i) {
      size_t                 curr_enumerator_offset = start + i;
      an_ifc_decl_enumerator curr_enumerator = {0};

      writer_fetch_node(self->output_state, &curr_enumerator,
                                     curr_enumerator_offset);

      /* Set the enumerator name. */
      an_ifc_text_offset
                ifc_name_offset =
                               writer_il_entity_name_as_text_offset(self, curr_constant);
      set_ifc_name(&curr_enumerator, ifc_name_offset);

      /* Set the source location information. */
      an_ifc_source_location
                ifc_src_pos = writer_il_find_or_enter_entity_pos(self, curr_constant);
      set_ifc_locus(&curr_enumerator, ifc_src_pos);
      /* Associate the enumeration type with the enumerator. */
      set_ifc_type(&curr_enumerator, enumeration_type);

      /* Create a token cache representation of the initializing constant. */
      an_ifc_output_token_cache init_token_cache;
  writer_cache_init(&init_token_cache);
      /* FIXME: We probably eventually want to have a find_or_enter_constant
         to hash and deduplicate constants. */
      an_ifc_edg_constant_index
                ifc_constant = writer_il_enter_constant(self, curr_constant);
      an_ifc_edg_complex_token_index
                ifc_constant_token = writer_il_enter_constant_token(self, 
                                                         ifc_ects_int_constant,
                                                         ifc_constant);
      writer_cache_add_complex(&init_token_cache, ifc_constant_token);

      /* Set the initializing expression. */
      an_ifc_expr_index init_idx = writer_alloc_token_cache_expr(self->output_state, &init_token_cache);
writer_cache_destroy(&init_token_cache);
      set_ifc_initializer(&curr_enumerator, init_idx);
      /* FIXME: Set specifier. */

      /* Set the access specifier. */
      an_ifc_access_sort ifc_access = access_specifier_of(curr_constant);
      set_ifc_access(&curr_enumerator, ifc_access);

      /* Advance to the next parameter. */
      curr_constant = curr_constant->next;
    }  /* for */
  }  /* if */
  return result;
}  /* writer_il_enter_enumerators */

static an_ifc_chart_index writer_il_enter_routine_params(an_ifc_il_map *self, a_routine_ptr rp)
/*
Given a routine, enter the routine's parameters.  Return the IFC chart index of
the parameters.
*/
{
  an_ifc_chart_index result = {0};
  a_type_ptr    type = rp->type;
  a_param_type_ptr
                param_type_list = function_type_params(type);
  unsigned      param_count = edg_count_list_elements(param_type_list, offsetof(a_param_type, next));

  if (param_count > 0) {
    an_ifc_chart_unilevel param_chart = {0};

    result = writer_alloc_chart(self->output_state, &param_chart);

    /* Preallocate all the parameters to ensure we get one contiguous block. */
    size_t      start = writer_alloc_node_block(self->output_state, get_ifc_partition_kind_an_ifc_decl_parameter(), param_count);
    /* Associate the preallocated parameter nodes with the parameter chart. */
    an_ifc_module_file
                *file = self->output_state->output_file;
    an_ifc_index ifc_param_start = (an_ifc_index){file, start};
    an_ifc_cardinality ifc_param_count = (an_ifc_cardinality){file, param_count};
    set_ifc_start(&param_chart, ifc_param_start);
    set_ifc_cardinality(&param_chart, ifc_param_count);

    /* Complete the parameter declarations. */
    a_param_type_ptr curr_param_type = param_type_list;
    for (size_t i = 0; i < param_count; ++i) {
      size_t                curr_param_offset = start + i;
      an_ifc_decl_parameter curr_param = {0};

      writer_fetch_node(self->output_state, &curr_param, curr_param_offset);

      /* Set the parameter name. */
      a_const_char
                *name = curr_param_type->name;
      if (name != NULL) {
        size_t  name_offset = writer_add_string(self->output_state, name);
        an_ifc_text_offset ifc_name_offset = (an_ifc_text_offset){curr_param.file, name_offset};

        set_ifc_name(&curr_param, ifc_name_offset);
      } else {
        an_ifc_text_offset ifc_name_offset = {0};

        set_ifc_name(&curr_param, ifc_name_offset);
      }  /* if */

      /* Set the source location information. */
#if EXTRA_SOURCE_POSITIONS_IN_IL
      a_decl_position_supplement_ptr
                decl_pos_sup = curr_param_type->decl_pos_info;
      if (decl_pos_sup != NULL) {
        a_source_position
                src_pos = decl_pos_sup->identifier_range.start;
        an_ifc_source_location
                ifc_src_pos = writer_il_find_or_enter_pos(self, &(src_pos));

        set_ifc_locus(&curr_param, ifc_src_pos);
      } else
#endif /* EXTRA_SOURCE_POSITIONS_IN_IL */
      {
        an_ifc_source_location
                ifc_src_pos = writer_il_find_or_enter_null_pos(self);

        set_ifc_locus(&curr_param, ifc_src_pos);
      }  /* if */

      /* Set the type information. */
      an_ifc_type_index
                ifc_type_idx = writer_il_find_or_enter_type(self, curr_param_type->type);
      set_ifc_type(&curr_param, ifc_type_idx);
      /* FIXME: Set the constraint. */
      /* FIXME: Set the initializer. */
      /* FIXME: Set the level. */
      /* FIXME: Set the position. */
      set_ifc_sort(&curr_param, ifc_ps_object);
      /* FIXME: Set the reachable properties. */
      /* Advance to the next parameter. */
      curr_param_type = curr_param_type->next;
    }  /* for */
  }  /* if */
  return result;
}  /* writer_il_enter_routine_params */

static an_ifc_edg_template_argument_index writer_il_enter_template_argument(an_ifc_il_map *self, 
                                                  a_template_arg_ptr templ_arg)
/*
For the given template argument enter the template argument into the IFC output
state.  Return the EDG IFC template argument index for the template argument.
*/
{
  an_ifc_edg_template_argument_index result = {0};
  a_templ_arg_kind
                arg_kind = templ_arg->kind;

  switch (arg_kind) {
    case tak_nontype:
      { an_ifc_edg_template_argument_non_type non_type_templ_arg = {0};
        size_t  arg_offset =
                           writer_alloc_node(self->output_state, &non_type_templ_arg);
        an_ifc_output_token_cache init_token_cache;
  writer_cache_init(&init_token_cache);

        if (templ_arg->is_array_bound_of_unknown_type) {
          ifc_write_catastrophe(ec_unsupported_il_to_ifc);
        } else {
          an_ifc_edg_constant_index
                ifc_constant =
                             writer_il_enter_constant(self, templ_arg->variant.constant);
          an_ifc_edg_complex_token_index
                ifc_constant_token = writer_il_enter_constant_token(self, 
                                                         ifc_ects_int_constant,
                                                         ifc_constant);

          writer_cache_add_complex(&init_token_cache, ifc_constant_token);
        }  /* if */

        /* Set the value for the argument. */
        an_ifc_expr_index
                init_idx = writer_alloc_token_cache_expr(self->output_state, &init_token_cache);
writer_cache_destroy(&init_token_cache);
        set_ifc_value(&non_type_templ_arg, init_idx);
        result = (an_ifc_edg_template_argument_index){
                                       non_type_templ_arg.file,
                                       ifc_etas_edg_template_argument_non_type,
                                       arg_offset};
      }
      break;
    case tak_template:
      { an_ifc_edg_template_argument_template template_templ_arg = {0};
        size_t  arg_offset =
                           writer_alloc_node(self->output_state, &template_templ_arg);

        if (templ_arg->variant.templ.substituted_param_template != NULL) {
          /* FIXME: What if anything needs done here? */
          ifc_write_catastrophe(ec_unsupported_il_to_ifc);
        }  /* if */

        an_ifc_decl_index
                decl_idx =
                    writer_il_find_or_enter_template(self, templ_arg->variant.templ.ptr);
        set_ifc_value(&template_templ_arg, decl_idx);
        result = (an_ifc_edg_template_argument_index){
                                       template_templ_arg.file,
                                       ifc_etas_edg_template_argument_template,
                                       arg_offset};
      }
      break;
    case tak_type:
      { an_ifc_edg_template_argument_type template_templ_arg = {0};
        size_t  arg_offset =
                           writer_alloc_node(self->output_state, &template_templ_arg);
        an_ifc_type_index
                type_idx = writer_il_find_or_enter_type(self, templ_arg->variant.type);

        set_ifc_value(&template_templ_arg, type_idx);
        result = (an_ifc_edg_template_argument_index){
                                           template_templ_arg.file,
                                           ifc_etas_edg_template_argument_type,
                                           arg_offset};
      }
      break;
    case tak_start_of_pack_expansion:
      ifc_write_catastrophe(ec_unsupported_il_to_ifc);
      break;
    default_is_unexpected();
  }  /* switch */
  return result;
}  /* writer_il_enter_template_argument */

static an_ifc_chart_index writer_il_enter_template_params(an_ifc_il_map *self, a_template_ptr templ)
/*
Enter the chart representing the given template's template parameters.  Return
the index of the template parameter chart.
*/
{
  an_ifc_chart_index result = {0};
  a_template_decl_ptr
                templ_decl = templ->template_decl;
  a_template_parameter_ptr
                param_list = templ_decl->param_list;
  unsigned      param_count = edg_count_list_elements(param_list, offsetof(a_template_parameter, next));

  if (param_count > 0) {
    an_ifc_chart_unilevel param_chart = {0};

    result = writer_alloc_chart(self->output_state, &param_chart);

    /* Preallocate all the parameters to ensure we get one contiguous block. */
    size_t      start = writer_alloc_node_block(self->output_state, get_ifc_partition_kind_an_ifc_decl_parameter(), param_count);
    /* Associate the preallocated parameter nodes with the parameter chart. */
    an_ifc_module_file
                *file = self->output_state->output_file;
    an_ifc_index ifc_param_start = (an_ifc_index){file, start};
    an_ifc_cardinality ifc_param_count = (an_ifc_cardinality){file, param_count};
    set_ifc_start(&param_chart, ifc_param_start);
    set_ifc_cardinality(&param_chart, ifc_param_count);

    /* Complete the parameter declarations. */
    a_template_parameter_ptr curr_templ_param = param_list;
    for (size_t i = 0; i < param_count; ++i) {
      size_t                curr_param_offset = start + i;
      an_ifc_decl_parameter curr_param = {0};

      writer_fetch_node(self->output_state, &curr_param, curr_param_offset);

      an_ifc_decl_index curr_param_idx = (an_ifc_decl_index){curr_param.file, ifc_ds_decl_parameter,
                               curr_param_offset};
      a_template_parameter_kind
                param_kind = curr_templ_param->kind;
      switch (param_kind) {
        case tpk_nontype:
          writer_il_set_up_template_non_type_param(self, &curr_param, curr_templ_param);
          break;
        case tpk_template:
          writer_il_set_up_template_template_param(self, &curr_param, curr_param_idx,
                                               curr_templ_param);
          break;
        case tpk_type:
          writer_il_set_up_template_type_param(self, &curr_param, curr_param_idx,
                                           curr_templ_param);
          break;
        case tpk_error:
          /* An error template parameter kind should not make it to IFC
             writing. */
          unexpected_condition();
        default_is_unexpected();
      }  /* switch */
      /* Advance to the next parameter. */
      curr_templ_param = curr_templ_param->next;
    }  /* for */
  }  /* if */
  return result;
}  /* writer_il_enter_template_params */

static an_ifc_type_index writer_il_enter_template_template_param_type(an_ifc_il_map *self, 
                                                          a_template_ptr templ)
/*
Enter an IFC forall type representing the given template (representing a
template parameter).
*/
{
  an_ifc_type_forall forall_type = {0};
  an_ifc_type_index
                result = writer_alloc_type(self->output_state, &forall_type);
  an_ifc_chart_index
                param_chart = writer_il_enter_template_params(self, templ);

  set_ifc_chart(&forall_type, param_chart);
  set_ifc_subject(&forall_type, writer_il_find_or_enter_class_scope_type(self));
  return result;
}  /* writer_il_enter_template_template_param_type */

static void writer_il_set_template_param_coordinates(an_ifc_il_map *self, 
                                an_ifc_decl_parameter             *param_decl,
                                const a_template_param_coordinate *coordinates)
/*
Set the level (i.e., depth) and position information for the given IFC
parameter declaration based on the given front end template parameter
coordinate information.
*/
{
  /* Set the level information (i.e., depth). */
  an_ifc_parameter_level ifc_param_depth = (an_ifc_parameter_level){self->output_state->output_file, coordinates->depth};
  set_ifc_level(param_decl, ifc_param_depth);

  /* Set the position information. */
  an_ifc_parameter_position ifc_param_position = (an_ifc_parameter_position){self->output_state->output_file,
                                   coordinates->position};
  set_ifc_position(param_decl, ifc_param_position);
}  /* writer_il_set_template_param_coordinates */

static void writer_il_set_up_template_non_type_param(an_ifc_il_map *self, 
                                       an_ifc_decl_parameter    *param_decl,
                                       a_template_parameter_ptr templ_param)
/*
The given IFC parameter declaration node is freshly allocated.  Set its fields
to represent the given front end non-type template parameter.
*/
{
  check_assertion(templ_param->kind == tpk_nontype);
  /* Set the parameter name. */
  an_ifc_text_offset
                ifc_name_offset =
                            writer_il_entity_name_as_text_offset(self, templ_param);
  set_ifc_name(param_decl, ifc_name_offset);

  /* Set the source location information. */
  an_ifc_source_location
                ifc_src_pos = writer_il_find_or_enter_entity_pos(self, templ_param);
  set_ifc_locus(param_decl, ifc_src_pos);

  /* Set the type information. */
  a_constant_ptr
                param_constant = templ_param->variant.nontype.constant;
  /* Check some assumptions about the constant representation.  If these
     assertions are violated either the IFC writer has been fed bad data or
     the IFC writer needs to be updated to handle the new case. */
  check_assertion(param_constant->kind == ck_template_param);
  check_assertion(param_constant->variant.template_param.kind == tpck_param);
  a_type_ptr    param_type = param_constant->type;
  an_ifc_type_index
                ifc_param_type = writer_il_find_or_enter_type(self, param_type);
  set_ifc_type(param_decl, ifc_param_type);

  /* Set the level (i.e., depth) and position information. */
  a_template_param_coordinate
                coordinates =
                    param_constant->variant.template_param.variant.coordinates;
  writer_il_set_template_param_coordinates(self, param_decl, &coordinates);
  /* FIXME: Set the constraint. */
  /* FIXME: Set the initializer. */
  /* Set the parameter sort. */
  set_ifc_sort(param_decl, ifc_ps_non_type);
  /* FIXME: Set the reachable properties. */
}  /* writer_il_set_up_template_non_type_param */

static void writer_il_set_up_template_template_param(an_ifc_il_map *self, 
                                       an_ifc_decl_parameter    *param_decl,
                                       an_ifc_decl_index        param_decl_idx,
                                       a_template_parameter_ptr templ_param)
/*
The given IFC parameter declaration node (at the given declaration index) is
freshly allocated.  Set its fields to represent the given front end template
template parameter.
*/
{
  check_assertion(templ_param->kind == tpk_template);
  /* Link the template representing this parameter to the IFC
     DeclSort::Parameter now.  Following the mapping, this ensures that any
     references to the template properly resolve to the correct IFC
     parameter declaration. */
  a_template_ptr   param_template = templ_param->variant.templ.class_template;
  a_tagged_pointer tagged_templ = make_tagged_ptr(param_template);

  writer_set_il_entry_to_decl(self, tagged_templ, param_decl_idx);

  /* Set the parameter name. */
  an_ifc_text_offset
                ifc_name_offset =
                            writer_il_entity_name_as_text_offset(self, templ_param);
  set_ifc_name(param_decl, ifc_name_offset);

  /* Set the source location information. */
  an_ifc_source_location
                ifc_src_pos = writer_il_find_or_enter_entity_pos(self, templ_param);
  set_ifc_locus(param_decl, ifc_src_pos);

  /* Set the type information. */
  an_ifc_type_index
                type_idx =
                      writer_il_enter_template_template_param_type(self, param_template);
  set_ifc_type(param_decl, type_idx);

  /* Set the level (i.e., depth) and position information. */
  writer_il_set_template_param_coordinates(self, param_decl, &(param_template->coordinates));
  /* FIXME: Set the constraint. */
  /* FIXME: Set the initializer. */
  /* Set the parameter sort. */
  set_ifc_sort(param_decl, ifc_ps_template);
  /* FIXME: Set the reachable properties. */
}  /* writer_il_set_up_template_template_param */

static void writer_il_set_up_template_type_param(an_ifc_il_map *self, 
                                       an_ifc_decl_parameter    *param_decl,
                                       an_ifc_decl_index        param_decl_idx,
                                       a_template_parameter_ptr templ_param)
/*
The given IFC parameter declaration node (at the given declaration index) is
freshly allocated.  Set its fields to represent the given front end type
template parameter.
*/
{
  check_assertion(templ_param->kind == tpk_type);
  /* Link the type representing this parameter to the IFC DeclSort::Parameter
     now.  Following the mapping, this ensures that any references to the
     type properly resolve to the correct IFC parameter declaration. */
  a_type_ptr    param_type = templ_param->variant.type.ptr;
  (void)writer_il_enter_template_type_param_type(self, param_type, param_decl_idx);

  /* Set the parameter name. */
  an_ifc_text_offset
                ifc_name_offset =
                            writer_il_entity_name_as_text_offset(self, templ_param);
  set_ifc_name(param_decl, ifc_name_offset);

  /* Set the source location information. */
  an_ifc_source_location
                ifc_src_pos = writer_il_find_or_enter_entity_pos(self, templ_param);
  set_ifc_locus(param_decl, ifc_src_pos);

  /* Set the type information. */
  /* FIXME: What does Microsoft put here for a type template parameter? */
  set_ifc_type(param_decl, (an_ifc_type_index){0});

  /* Set the level (i.e., depth) and position information. */
  a_template_param_type_supplement_ptr
                tpts = param_type->variant.template_param.extra_info;
  writer_il_set_template_param_coordinates(self, param_decl, &(tpts->coordinates));
  /* FIXME: Set the constraint. */
  /* FIXME: Set the initializer. */
  /* Set the parameter sort. */
  set_ifc_sort(param_decl, ifc_ps_type);
  /* FIXME: Set the reachable properties. */
}  /* writer_il_set_up_template_type_param */

static an_ifc_edg_constant_index writer_il_enter_constant(an_ifc_il_map *self, const a_constant *cp)
/*
For the given constant enter the constant into the IFC output state.  Return
the expr index for the constant.
*/
{
  an_ifc_edg_constant_index result = {0};

  switch (cp->kind) {
    case ck_error:
    case ck_last:
      ifc_write_catastrophe(ec_unsupported_il_to_ifc);
      break;
    case ck_integer:
      { const an_integer_value *int_val = &cp->variant.integer_value;
        /* Convert the front end integer value into the necessary number of EDG
           IFC integer constant words.

           an_integer_value must be a multiple of 4 bytes; otherwise, the
           "constant words" will not be completely filled.  As an example:

             0x00 0x00 0x0f 0x00
             ^^^^ ^^^^ ^^^^

           may be written instead of:

             0x00 0x00 0x00 0x0f
             ^^^^ ^^^^ ^^^^ ^^^^
         */
        _Static_assert(sizeof(an_integer_value) % sizeof(uint32_t) == 0,
                      "an_integer_value must be a multiple of 4 bytes");
        const unsigned
                num_parts_expected =
                                 (sizeof(an_integer_value) / sizeof(uint32_t));
        Integer_translator output_int;
        integer_value_as_translator(&output_int, int_val);
        /* Allocate the integer constant word block and populate it with
           the computed values. */
        check_assertion(output_int.parts.len == num_parts_expected);
        size_t  start = writer_alloc_node_block(self->output_state, get_ifc_partition_kind_an_ifc_edg_constant_integer_word(), 
                                                           num_parts_expected);
        for (size_t i = 0; i < num_parts_expected; ++i) {
          an_ifc_edg_constant_integer_word int_word = {0};

          writer_fetch_node(self->output_state, &int_word, start + i);

          an_ifc_edg_constant_word_storage word_bytes;
          uint32_t word = integer_translator_get_u32(&output_int, i);
          memcpy(word_bytes, &word, /*num_bytes=*/4);

          an_ifc_edg_constant_word word_contents = an_ifc_edg_constant_word_from_storage(int_word.file, &(word_bytes));
          set_ifc_bytes(&int_word, word_contents);
        }  /* for */

        /* Create the integer constant itself. */
        an_ifc_edg_constant_integer int_constant = {0};
        result = writer_alloc_constant(self->output_state, &int_constant);

        /* Associate the constant's type. */
        a_type_ptr
                constant_type = cp->type;
        if (is_enum_type(constant_type)) {
          /* The IFC reading code expects an integer constant with a
             fundamental integral type.  The constant's enumeration type will
             be restored automatically as part of the reconstruction
             process. */
          an_integer_kind
                underlying_int_kind = constant_type->variant.integer.int_kind;

          constant_type = integer_type(underlying_int_kind);
        }  /* if */

        an_ifc_type_index
                ifc_constant_type = writer_il_find_or_enter_type(self, constant_type);
        set_ifc_type(&int_constant, ifc_constant_type);

        /* Reference the created words. */
        an_ifc_edg_constant_integer_word_offset ifc_start = (an_ifc_edg_constant_integer_word_offset){int_constant.file, start};
        an_ifc_cardinality ifc_cardinality = (an_ifc_cardinality){int_constant.file, num_parts_expected};
        set_ifc_start(&int_constant, ifc_start);
        set_ifc_cardinality(&int_constant, ifc_cardinality);
        integer_translator_destroy(&output_int);
      }
      break;
#if FIXED_POINT_ALLOWED
    case ck_fixed_point:
#endif /* FIXED_POINT_ALLOWED */
#if DO_IL_LOWERING && GENERATE_EH_TABLES && !DO_FULL_PORTABLE_EH_LOWERING
    case ck_stack_offset:
#endif /* DO_IL_LOWERING && ... */
    case ck_string:
    case ck_float:
#if C99_IL_EXTENSIONS_SUPPORTED
    case ck_complex:
    case ck_imaginary:
#endif /* C99_IL_EXTENSIONS_SUPPORTED */
    case ck_address:
    case ck_ptr_to_member:
#if GNU_EXTENSIONS_ALLOWED
    case ck_label_difference:
#endif /* GNU_EXTENSIONS_ALLOWED */
    case ck_dynamic_init:
    case ck_aggregate:
    case ck_init_repeat:
    case ck_template_param:
    case ck_designator:
#if UPC_EXTENSIONS_ALLOWED
    case ck_upc_threads:
    case ck_upc_mythread:
#endif /* UPC_EXTENSIONS_ALLOWED */
    case ck_void:
    case ck_reflection:
      ifc_write_catastrophe(ec_unsupported_il_to_ifc);
      break;
    default_is_unexpected();
  } /* switch */
  return result;
}  /* writer_il_enter_constant */

static inline an_ifc_edg_basic_token_sort token_to_basic_token_kind(
                                                            a_token_kind token)
/*
Given a front end token kind (for a token where extra_info_kind == teik_none)
return the corresponding EDG IFC BasicToken sort value.  For tokens that should
be handled textually, add a case to ifc_ebts_complex.  See the documentation of
a_token_kind for more information about IFC token serialization.
*/
{
  an_ifc_edg_basic_token_sort result = ifc_ebts_semicolon;

  switch (token) {
    case tok_alignas:
      result = ifc_ebts_alignas;
      break;
    case tok_alignof:
      result = ifc_ebts_alignof;
      break;
    case tok_ampersand:
      result = ifc_ebts_ampersand;
      break;
    case tok_and_and:
      result = ifc_ebts_and_and;
      break;
    case tok_and_assign:
      result = ifc_ebts_and_assign;
      break;
    case tok_arrow:
      result = ifc_ebts_arrow;
      break;
    case tok_arrow_star:
      result = ifc_ebts_arrow_star;
      break;
    case tok_asm:
      result = ifc_ebts_asm;
      break;
    case tok_assign:
      result = ifc_ebts_assign;
      break;
    case tok_auto:
      result = ifc_ebts_auto;
      break;
    case tok_bool:
      result = ifc_ebts_bool;
      break;
    case tok_break:
      result = ifc_ebts_break;
      break;
    case tok_c11_atomic:
      result = ifc_ebts_c11_atomic;
      break;
    case tok_c11_generic:
      result = ifc_ebts_c11_generic;
      break;
    case tok_c11_thread_local:
      result = ifc_ebts_c11_thread_local;
      break;
    case tok_c99_bool:
      result = ifc_ebts_c99_bool;
      break;
    case tok_c99_complex:
      result = ifc_ebts_c99_complex;
      break;
    case tok_c99_generic:
      result = ifc_ebts_c99_generic;
      break;
    case tok_c99_genericfx:
      result = ifc_ebts_c99_genericfx;
      break;
    case tok_c99_imaginary:
      result = ifc_ebts_c99_imaginary;
      break;
    case tok_case:
      result = ifc_ebts_case;
      break;
    case tok_catch:
      result = ifc_ebts_catch;
      break;
    case tok_char:
      result = ifc_ebts_char;
      break;
    case tok_char16_t:
      result = ifc_ebts_char16_t;
      break;
    case tok_char32_t:
      result = ifc_ebts_char32_t;
      break;
    case tok_char8_t:
      result = ifc_ebts_char8_t;
      break;
    case tok_class:
      result = ifc_ebts_class;
      break;
    case tok_colon:
      result = ifc_ebts_colon;
      break;
    case tok_colon_colon:
      result = ifc_ebts_colon_colon;
      break;
    case tok_comma:
      result = ifc_ebts_comma;
      break;
    case tok_compl:
      result = ifc_ebts_compl;
      break;
    case tok_concept:
      result = ifc_ebts_concept;
      break;
    case tok_const:
      result = ifc_ebts_const;
      break;
    case tok_const_cast:
      result = ifc_ebts_const_cast;
      break;
    case tok_consteval:
      result = ifc_ebts_consteval;
      break;
    case tok_constexpr:
      result = ifc_ebts_constexpr;
      break;
    case tok_constinit:
      result = ifc_ebts_constinit;
      break;
    case tok_continue:
      result = ifc_ebts_continue;
      break;
    case tok_coroutine_await:
      result = ifc_ebts_coroutine_await;
      break;
    case tok_coroutine_return:
      result = ifc_ebts_coroutine_return;
      break;
    case tok_coroutine_yield:
      result = ifc_ebts_coroutine_yield;
      break;
    case tok_cpp98_export:
      result = ifc_ebts_cpp98_export;
      break;
    case tok_decltype:
      result = ifc_ebts_decltype;
      break;
    case tok_default:
      result = ifc_ebts_default;
      break;
    case tok_delete:
      result = ifc_ebts_delete;
      break;
    case tok_divide:
      result = ifc_ebts_divide;
      break;
    case tok_divide_assign:
      result = ifc_ebts_divide_assign;
      break;
    case tok_do:
      result = ifc_ebts_do;
      break;
    case tok_double:
      result = ifc_ebts_double;
      break;
    case tok_dynamic_cast:
      result = ifc_ebts_dynamic_cast;
      break;
    case tok_ellipsis:
      result = ifc_ebts_ellipsis;
      break;
    case tok_else:
      result = ifc_ebts_else;
      break;
    case tok_enum:
      result = ifc_ebts_enum;
      break;
    case tok_eq:
      result = ifc_ebts_eq;
      break;
    case tok_excl_or:
      result = ifc_ebts_excl_or;
      break;
    case tok_excl_or_assign:
      result = ifc_ebts_excl_or_assign;
      break;
    case tok_explicit:
      result = ifc_ebts_explicit;
      break;
    case tok_export:
      result = ifc_ebts_export;
      break;
    case tok_export_keyword:
      result = ifc_ebts_export_keyword;
      break;
    case tok_extern:
      result = ifc_ebts_extern;
      break;
    case tok_false:
      result = ifc_ebts_false;
      break;
    case tok_final:
      result = ifc_ebts_final;
      break;
    case tok_float:
      result = ifc_ebts_float;
      break;
    case tok_float128:
      result = ifc_ebts_float128;
      break;
    case tok_float32:
      result = ifc_ebts_float32;
      break;
    case tok_float32x:
      result = ifc_ebts_float32x;
      break;
    case tok_float64:
      result = ifc_ebts_float64;
      break;
    case tok_float64x:
      result = ifc_ebts_float64x;
      break;
    case tok_for:
      result = ifc_ebts_for;
      break;
    case tok_friend:
      result = ifc_ebts_friend;
      break;
    case tok_ge:
      result = ifc_ebts_ge;
      break;
    case tok_goto:
      result = ifc_ebts_goto;
      break;
    case tok_gt:
      result = ifc_ebts_gt;
      break;
    case tok_if:
      result = ifc_ebts_if;
      break;
    case tok_import:
      result = ifc_ebts_import;
      break;
    case tok_infinity:
      result = ifc_ebts_infinity;
      break;
    case tok_inline:
      result = ifc_ebts_inline;
      break;
    case tok_int:
      result = ifc_ebts_int;
      break;
    case tok_lbrace:
      result = ifc_ebts_lbrace;
      break;
    case tok_lbracket:
      result = ifc_ebts_lbracket;
      break;
    case tok_le:
      result = ifc_ebts_le;
      break;
    case tok_long:
      result = ifc_ebts_long;
      break;
    case tok_lparen:
      result = ifc_ebts_lparen;
      break;
    case tok_lsplice:
      result = ifc_ebts_lsplice;
      break;
    case tok_lt:
      result = ifc_ebts_lt;
      break;
    case tok_minus:
      result = ifc_ebts_minus;
      break;
    case tok_minus_assign:
      result = ifc_ebts_minus_assign;
      break;
    case tok_minus_minus:
      result = ifc_ebts_minus_minus;
      break;
    case tok_module:
      result = ifc_ebts_module;
      break;
    case tok_mutable:
      result = ifc_ebts_mutable;
      break;
    case tok_namespace:
      result = ifc_ebts_namespace;
      break;
    case tok_nan:
      result = ifc_ebts_nan;
      break;
    case tok_ne:
      result = ifc_ebts_ne;
      break;
    case tok_new:
      result = ifc_ebts_new;
      break;
    case tok_noexcept:
      result = ifc_ebts_noexcept;
      break;
    case tok_noreturn:
      result = ifc_ebts_noreturn;
      break;
    case tok_not:
      result = ifc_ebts_not;
      break;
    case tok_nullptr:
      result = ifc_ebts_nullptr;
      break;
    case tok_operator:
      result = ifc_ebts_operator;
      break;
    case tok_or:
      result = ifc_ebts_or;
      break;
    case tok_or_assign:
      result = ifc_ebts_or_assign;
      break;
    case tok_or_or:
      result = ifc_ebts_or_or;
      break;
    case tok_overload:
      result = ifc_ebts_overload;
      break;
    case tok_override:
      result = ifc_ebts_override;
      break;
    case tok_paste:
      result = ifc_ebts_paste;
      break;
    case tok_period:
      result = ifc_ebts_period;
      break;
    case tok_period_star:
      result = ifc_ebts_period_star;
      break;
    case tok_plus:
      result = ifc_ebts_plus;
      break;
    case tok_plus_assign:
      result = ifc_ebts_plus_assign;
      break;
    case tok_plus_plus:
      result = ifc_ebts_plus_plus;
      break;
    case tok_private:
      result = ifc_ebts_private;
      break;
    case tok_protected:
      result = ifc_ebts_protected;
      break;
    case tok_public:
      result = ifc_ebts_public;
      break;
    case tok_quest_mark:
      result = ifc_ebts_quest_mark;
      break;
    case tok_rbrace:
      result = ifc_ebts_rbrace;
      break;
    case tok_rbracket:
      result = ifc_ebts_rbracket;
      break;
    case tok_register:
      result = ifc_ebts_register;
      break;
    case tok_reinterpret_cast:
      result = ifc_ebts_reinterpret_cast;
      break;
    case tok_remainder:
      result = ifc_ebts_remainder;
      break;
    case tok_remainder_assign:
      result = ifc_ebts_remainder_assign;
      break;
    case tok_remove_all_extents:
      result = ifc_ebts_remove_all_extents;
      break;
    case tok_requires:
      result = ifc_ebts_requires;
      break;
    case tok_return:
      result = ifc_ebts_return;
      break;
    case tok_rparen:
      result = ifc_ebts_rparen;
      break;
    case tok_rsplice:
      result = ifc_ebts_rsplice;
      break;
    case tok_semicolon:
      result = ifc_ebts_semicolon;
      break;
    case tok_sharp:
      result = ifc_ebts_sharp;
      break;
    case tok_shift_left:
      result = ifc_ebts_shift_left;
      break;
    case tok_shift_left_assign:
      result = ifc_ebts_shift_left_assign;
      break;
    case tok_shift_right:
      result = ifc_ebts_shift_right;
      break;
    case tok_shift_right_assign:
      result = ifc_ebts_shift_right_assign;
      break;
    case tok_short:
      result = ifc_ebts_short;
      break;
    case tok_signed:
      result = ifc_ebts_signed;
      break;
    case tok_sizeof:
      result = ifc_ebts_sizeof;
      break;
    case tok_spaceship:
      result = ifc_ebts_spaceship;
      break;
    case tok_star:
      result = ifc_ebts_star;
      break;
    case tok_static:
      result = ifc_ebts_static;
      break;
    case tok_static_assert:
      result = ifc_ebts_static_assert;
      break;
    case tok_static_cast:
      result = ifc_ebts_static_cast;
      break;
    case tok_struct:
      result = ifc_ebts_struct;
      break;
    case tok_switch:
      result = ifc_ebts_switch;
      break;
    case tok_template:
      result = ifc_ebts_template;
      break;
    case tok_this:
      result = ifc_ebts_this;
      break;
    case tok_thread_local:
      result = ifc_ebts_thread_local;
      break;
    case tok_throw:
      result = ifc_ebts_throw;
      break;
    case tok_times_assign:
      result = ifc_ebts_times_assign;
      break;
    case tok_true:
      result = ifc_ebts_true;
      break;
    case tok_try:
      result = ifc_ebts_try;
      break;
    case tok_typedef:
      result = ifc_ebts_typedef;
      break;
    case tok_typeid:
      result = ifc_ebts_typeid;
      break;
    case tok_typename:
      result = ifc_ebts_typename;
      break;
    case tok_union:
      result = ifc_ebts_union;
      break;
    case tok_unsigned:
      result = ifc_ebts_unsigned;
      break;
    case tok_using:
      result = ifc_ebts_using;
      break;
    case tok_virtual:
      result = ifc_ebts_virtual;
      break;
    case tok_void:
      result = ifc_ebts_void;
      break;
    case tok_volatile:
      result = ifc_ebts_volatile;
      break;
    case tok_wchar_t:
      result = ifc_ebts_wchar_t;
      break;
    case tok_while:
      result = ifc_ebts_while;
      break;
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_abstract:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
    case tok_accum:
    case tok_add_lvalue_reference:
    case tok_add_pointer:
    case tok_add_rvalue_reference:
    case tok_array_extent:
    case tok_array_rank:
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_assume:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
    case tok_attribute:
    case tok_auto_type:
    case tok_backslash:
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_based:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
#if GNU_EXTENSIONS_ALLOWED
    case tok_bases:
#endif /* GNU_EXTENSIONS_ALLOWED */
    case tok_builtin_addressof:
    case tok_builtin_bit_cast:
    case tok_builtin_complex:
#if GNU_VECTOR_TYPES_ALLOWED
    case tok_builtin_convertvector:
#endif /* GNU_VECTOR_TYPES_ALLOWED */
    case tok_builtin_has_attribute:
    case tok_builtin_is_corresponding_member:
    case tok_builtin_is_pointer_interconvertible_with_class:
    case tok_builtin_is_virtual_base_of:
    case tok_builtin_is_implicit_lifetime:
    case tok_builtin_lt_synthesizes_from_spaceship:
    case tok_builtin_gt_synthesizes_from_spaceship:
    case tok_builtin_le_synthesizes_from_spaceship:
    case tok_builtin_ge_synthesizes_from_spaceship:
    case tok_builtin_is_structural:
    case tok_builtin_offsetof:
#if GNU_VECTOR_TYPES_ALLOWED
    case tok_builtin_shuffle:
    case tok_builtin_shufflevector:
#endif /* GNU_VECTOR_TYPES_ALLOWED */
#if GNU_EXTENSIONS_ALLOWED
    case tok_builtin_types_compatible:
#endif /* GNU_EXTENSIONS_ALLOWED */
    case tok_caret_caret:
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_cdecl:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_charize:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
    case tok_clang_version:
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_clrcall:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
    case tok_decay:
    case tok_decorated_function_name:
#if GNU_EXTENSIONS_ALLOWED
    case tok_direct_bases:
#endif /* GNU_EXTENSIONS_ALLOWED */
    case tok_edg_bool_type:
    case tok_edg_internal_opnd:
    case tok_edg_internal_type:
    case tok_edg_is_deducible:
    case tok_edg_ptrdiff_type:
    case tok_edg_size_type:
    case tok_edg_throw:
    case tok_edg_vector_type:
    case tok_edg_wchar_type:
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_end_of_if_exists:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_enum_class:
    case tok_enum_struct:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_event:
    case tok_except:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
    case tok_ext_alignof:
    case tok_extension:
#if NEAR_AND_FAR_ALLOWED
    case tok_far:
#endif /* NEAR_AND_FAR_ALLOWED */
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_fastcall:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_finally:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_for_each:
    case tok_forceinline:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
    case tok_fract:
    case tok_func_name:
    case tok_function_name:
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_gcnew:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
#if SUN_EXTENSIONS_ALLOWED
    case tok_global_link_scope:
#endif /* SUN_EXTENSIONS_ALLOWED */
#if GNU_EXTENSIONS_ALLOWED
    case tok_gnu_imag:
#endif /* GNU_EXTENSIONS_ALLOWED */
    case tok_gnu_max:
    case tok_gnu_min:
#if GNU_EXTENSIONS_ALLOWED
    case tok_gnu_real:
#endif /* GNU_EXTENSIONS_ALLOWED */
    case tok_gnu_restrict:
    case tok_has_assign:
    case tok_has_copy:
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_has_finalizer:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
    case tok_has_nothrow_assign:
    case tok_has_nothrow_constructor:
    case tok_has_nothrow_copy:
    case tok_has_nothrow_move_assign:
    case tok_has_trivial_assign:
    case tok_has_trivial_constructor:
    case tok_has_trivial_copy:
    case tok_has_trivial_destructor:
    case tok_has_trivial_move_assign:
    case tok_has_trivial_move_constructor:
    case tok_has_unique_object_representations:
    case tok_has_user_destructor:
    case tok_has_virtual_destructor:
#if SUN_EXTENSIONS_ALLOWED
    case tok_hidden_link_scope:
#endif /* SUN_EXTENSIONS_ALLOWED */
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_if_exists:
    case tok_if_not_exists:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
    case tok_imaginary_unit:
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_implements:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_in:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
#if INT128_EXTENSIONS_ALLOWED
    case tok_int128:
#endif /* INT128_EXTENSIONS_ALLOWED */
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_int16:
    case tok_int32:
    case tok_int64:
    case tok_int8:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
    case tok_intaddr:
    case tok_integer_pack:
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_interface:
    case tok_interface_class:
    case tok_interface_struct:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
    case tok_internal_alias_decl:
    case tok_is_abstract:
    case tok_is_aggregate:
    case tok_is_arithmetic:
    case tok_is_array:
    case tok_is_assignable:
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_is_assignable_no_precondition_check:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
    case tok_is_base_of:
    case tok_is_bounded_array:
    case tok_is_class:
    case tok_is_complete_type:
    case tok_is_compound:
    case tok_is_const:
    case tok_is_constructible:
    case tok_is_convertible:
    case tok_is_convertible_to:
    case tok_is_corresponding_member:
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_is_delegate:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
    case tok_is_destructible:
    case tok_is_empty:
    case tok_is_enum:
    case tok_is_final:
    case tok_is_floating_point:
    case tok_is_function:
    case tok_is_fundamental:
    case tok_is_invocable:
    case tok_is_integral:
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_is_interface_class:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
    case tok_is_layout_compatible:
    case tok_is_literal_type:
    case tok_is_lvalue_reference:
    case tok_is_member_function_pointer:
    case tok_is_member_object_pointer:
    case tok_is_member_pointer:
    case tok_is_nothrow_assignable:
    case tok_is_nothrow_constructible:
    case tok_is_nothrow_convertible:
    case tok_is_nothrow_destructible:
    case tok_is_nothrow_invocable:
    case tok_is_object:
    case tok_is_pod:
    case tok_is_pointer:
    case tok_is_pointer_interconvertible_base_of:
    case tok_is_pointer_interconvertible_with_class:
    case tok_is_polymorphic:
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_is_ref_array:
    case tok_is_ref_class:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
    case tok_is_reference:
    case tok_is_referenceable:
    case tok_is_rvalue_reference:
    case tok_is_same:
    case tok_is_same_as:
    case tok_is_scalar:
    case tok_is_scoped_enum:
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_is_sealed:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
    case tok_is_signed:
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_is_simple_value_class:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
    case tok_is_standard_layout:
    case tok_is_trivial:
    case tok_is_trivially_assignable:
    case tok_is_trivially_constructible:
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_is_trivially_copy_assignable:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
    case tok_is_trivially_copyable:
    case tok_is_trivially_destructible:
    case tok_is_trivially_equality_comparable:
    case tok_is_trivially_relocatable:
    case tok_is_bitwise_cloneable:
    case tok_is_unbounded_array:
    case tok_is_union:
    case tok_is_unsigned:
    case tok_is_valid_winrt_type:
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_is_value_class:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
    case tok_is_void:
    case tok_is_volatile:
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_is_win_class:
    case tok_is_win_interface:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_leave:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
    case tok_make_signed:
    case tok_make_unsigned:
    case tok_microsoft_asm:
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_microsoft_identifier:
    case tok_microsoft_inline:
    case tok_microsoft_lprefix:
    case tok_microsoft_ptr32:
    case tok_microsoft_ptr64:
    case tok_microsoft_sptr:
    case tok_microsoft_try:
    case tok_microsoft_uprefix:
    case tok_microsoft_uptr:
    case tok_microsoft_w64:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_native_nullptr:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
#if NEAR_AND_FAR_ALLOWED
    case tok_near:
#endif /* NEAR_AND_FAR_ALLOWED */
    case tok_nonnull:
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_noop:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
    case tok_null:
    case tok_null_unspecified:
    case tok_nullable:
    case tok_nullptr_t:
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_partial_ref_class:
    case tok_partial_ref_struct:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_prefix_enum:
    case tok_prefix_for:
    case tok_prefix_interface:
    case tok_prefix_partial:
    case tok_prefix_ref:
    case tok_prefix_value:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
    case tok_pretty_function_name:
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_ref_class:
    case tok_ref_new:
    case tok_ref_struct:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
    case tok_reference_binds_to_temporary:
    case tok_reference_constructs_from_temporary:
    case tok_reference_converts_from_temporary:
    case tok_remove_const:
    case tok_remove_cv:
    case tok_remove_cvref:
    case tok_remove_extent:
    case tok_remove_pointer:
    case tok_remove_reference:
    case tok_remove_reference_t:
    case tok_remove_restrict:
    case tok_remove_volatile:
    case tok_restrict:
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_safe_cast:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
    case tok_sat:
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_sealed:
    case tok_stdcall:
    case tok_super:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
#if SUN_EXTENSIONS_ALLOWED
    case tok_symbolic_link_scope:
#endif /* SUN_EXTENSIONS_ALLOWED */
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_thiscall:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
    case tok_thread:
    case tok_typeof:
    case tok_typeof_unqual:
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_unaligned:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
    case tok_underlying_type:
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_unresolved_type:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
#if UPC_EXTENSIONS_ALLOWED
    case tok_upc_barrier:
    case tok_upc_blocksizeof:
    case tok_upc_elemsizeof:
    case tok_upc_fence:
    case tok_upc_forall:
    case tok_upc_localsizeof:
    case tok_upc_mythread:
    case tok_upc_notify:
    case tok_upc_relaxed:
    case tok_upc_shared:
    case tok_upc_strict:
    case tok_upc_threads:
    case tok_upc_wait:
#endif /* UPC_EXTENSIONS_ALLOWED */
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_uuid:
    case tok_uuidof:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
    case tok_va_copy:
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_value_class:
    case tok_value_struct:
    case tok_vectorcall:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
    case tok_bit_precise_int:
      result = ifc_ebts_complex;
      break;
    case tok_gen_constant:
    case tok_char_constant:
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_cli_typeid:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
    case tok_cpp_quote:
    case tok_datasizeof:
    case tok_declspec:
    case tok_decltype_construct:
    case tok_digit_sequence:
    case tok_edg_neon_polyvector_type:
    case tok_edg_neon_vector_type:
    case tok_edg_scalable_vector_type:
    case tok_end_of_source:
    case tok_error:
    case tok_fixed_point_constant:
    case tok_float_constant:
    case tok_header_name:
    case tok_identifier:
    case tok_ifc_decl:
    case tok_ifc_decl_ref:
    case tok_ifc_entity_ref:
    case tok_ifc_param_ref:
    case tok_ifc_type_ref:
    case tok_int_constant:
    case tok_last:
#if MICROSOFT_EXTENSIONS_ALLOWED
    case tok_microsoft_Lprefix:
    case tok_microsoft_Uprefix:
#endif /* MICROSOFT_EXTENSIONS_ALLOWED */
    case tok_newline:
    case tok_pending_ifc_expr:
    case tok_pp_number:
    case tok_ptr_to_member:
    case tok_removed_expr:
    case tok_removed_template_body:
    case tok_string_literal:
    case tok_ud_literal:
    case tok_unimplemented:
    case tok_unresolved_ud_literal:
    case tok_va_arg:
    case tok_va_end:
    case tok_va_start:
      /* An attempt was made to convert an unsupported or complex token to an
         EDG IFC basic token kind.  Either the token needs a case added above
         or the wrong conversion function has been called.  See the
         documentation of a_token_kind for more information. */
      unexpected_condition();
      break;
    default_is_unexpected();
  }  /* switch */
  return result;
}  /* token_to_basic_token_kind */


static inline an_ifc_edg_constant_token_sort token_to_constant_token_kind(
                                                            a_token_kind token)
/*
Given a token kind that has an associated constant, return the corresponding
EDG IFC constant token kind.
*/
{
  an_ifc_edg_constant_token_sort result = {0};

  switch (token) {
    case tok_char_constant:
      result = ifc_ects_char_constant;
      break;
    case tok_float_constant:
      result = ifc_ects_float_constant;
      break;
    case tok_gen_constant:
      result = ifc_ects_gen_constant;
      break;
    case tok_int_constant:
      result = ifc_ects_int_constant;
      break;
    case tok_string_literal:
      result = ifc_ects_string_literal;
      break;
    default:
      /* An unsupported token was cached as a basic token.  Either it needs to
         be added above as a basic token, the token needs its own complex token
         add method, or the caller called the wrong add function (i.e., there's
         a corresponding complex token add function that should have instead
         been called). */
      ifc_write_catastrophe(ec_unsupported_il_to_ifc);
  }  /* switch */
  return result;
}  /* token_to_constant_token_kind */


static an_ifc_edg_complex_token_index writer_il_find_or_enter_textual_token(an_ifc_il_map *self, 
                                                     a_shared_token tok)
/*
For the given cached token (representing a token with a singular textual
identity) find or enter the textual token representation into the IFC output
state.  Return the corresponding IFC EDG complex token index.
*/
{
  an_ifc_edg_complex_token_index result = {0};
  size_t        textual_token_offset =
                                  writer_get_textual_token_map(self, edg_token_get_kind(tok));

  if (textual_token_offset == 0) {
    result = writer_il_enter_textual_token(self, tok);
  } else {
    result = (an_ifc_edg_complex_token_index){self->output_state->output_file,
                                            ifc_ects_edg_token_textual,
                                            textual_token_offset - 1};
  }  /* if */
  return result;
}  /* writer_il_find_or_enter_textual_token */

static void writer_il_enter_basic_token_to_cache(an_ifc_il_map *self, 
                                         an_ifc_output_token_cache *ifc_cache,
                                         a_shared_token tok)
/*
Add the given cached token (representing a token representable as either a
basic token or a textual token) to the given token cache.
*/
{
  an_ifc_edg_basic_token_sort
                ifc_token = token_to_basic_token_kind(edg_token_get_kind(tok));

  if (ifc_token == ifc_ebts_complex) {
    writer_il_enter_textual_token_to_cache(self, ifc_cache, tok);
  } else {
    writer_cache_add_basic(ifc_cache, ifc_token);
  }  /* if */
}  /* writer_il_enter_basic_token_to_cache */

static an_ifc_edg_complex_token_index writer_il_enter_textual_token(an_ifc_il_map *self, 
                                                     a_shared_token tok)
/*
For the given cached token (representing a token with a singular textual
identity) enter the textual token representation into the IFC output state.
Return the corresponding IFC EDG complex token index.
*/
{
  /* If this assertion fails, this token should not be represented as a textual
     token because it's directly representable as a basic token. */
  check_assertion(token_to_basic_token_kind(edg_token_get_kind(tok)) ==
                  ifc_ebts_complex);
  /* If this assertion fails, this token should not be represented as a textual
     token because it has additional state that needs to be serialized. */
  check_assertion(edg_token_is_basic(tok));
  /* If this assertion fails, this textual token has already been entered. */
  check_assertion(writer_get_textual_token_map(self, edg_token_get_kind(tok)) == 0);
  an_ifc_edg_token_textual textual_token = {0};
  an_ifc_edg_complex_token_index
                result = writer_alloc_complex_token(self->output_state, 
                                                               &textual_token);

  writer_set_textual_token_map(self, edg_token_get_kind(tok), result.value + 1);

  size_t        token_name_offset = writer_add_string(self->output_state, 
                                           ifc_token_name_of(edg_token_get_kind(tok)));
  an_ifc_text_offset ifc_token_name_offset = (an_ifc_text_offset){self->output_state->output_file,
                                      token_name_offset};
  set_ifc_characters(&textual_token, ifc_token_name_offset);
  return result;
}  /* writer_il_enter_textual_token */

static void writer_il_enter_textual_token_to_cache(an_ifc_il_map *self, 
                                          an_ifc_output_token_cache *ifc_cache,
                                          a_shared_token tok)
/*
Add the given cached token (representing a token with a singular textual
identity) to the given token cache.
*/
{
  check_assertion(edg_token_is_basic(tok));
  an_ifc_edg_complex_token_index
                complex_token_idx = writer_il_find_or_enter_textual_token(self, tok);

  writer_cache_add_complex(ifc_cache, complex_token_idx);
}  /* writer_il_enter_textual_token_to_cache */

static an_ifc_edg_complex_token_index writer_il_enter_constant_token(an_ifc_il_map *self, 
                                   an_ifc_edg_constant_token_sort token_kind,
                                   an_ifc_edg_constant_index      constant_idx)

/*
For the given constant and constant token kind, enter a complex constant token
into the IFC output state.  Return the index of the token.
*/
{
  an_ifc_edg_token_constant constant_token = {0};
  an_ifc_edg_complex_token_index
                result = writer_alloc_complex_token(self->output_state, 
                                                              &constant_token);

  set_ifc_kind(&constant_token, token_kind);
  set_ifc_constant(&constant_token, constant_idx);
  return result;
}  /* writer_il_enter_constant_token */


static void writer_il_enter_constant_token_to_cache(an_ifc_il_map *self, 
                                          an_ifc_output_token_cache *ifc_cache,
                                          a_shared_token tok)
/*
Add the given cached token (representing an EDG IFC constant token) to the
given token cache.
*/
{
  const a_constant
                *constant = edg_token_get_constant(tok);
  an_ifc_edg_constant_index
                ifc_constant = writer_il_enter_constant(self, constant);
  an_ifc_edg_constant_token_sort
                ifc_token_kind = token_to_constant_token_kind(edg_token_get_kind(tok));
  an_ifc_edg_complex_token_index
                complex_token_idx = writer_il_enter_constant_token(self, ifc_token_kind,
                                                               ifc_constant);

  writer_cache_add_complex(ifc_cache, complex_token_idx);
}  /* writer_il_enter_constant_token_to_cache */

static void writer_il_enter_extracted_body_to_cache(an_ifc_il_map *self, 
                                          an_ifc_output_token_cache *ifc_cache,
                                          a_shared_token tok)
/*
Enter the token cache associated with the given extracted template token into
the given token cache.
*/
{
  const an_extracted_template_descr
                *extracted_template = edg_token_get_extracted_template_descr(tok);
  a_symbol_ptr  template_sym = extracted_template->symbol;

  switch (template_sym->kind) {
    case sk_member_function:
      { a_template_instance_ptr
                instance_ptr = template_sym->variant.routine.instance_ptr;
        a_template_symbol_supplement_ptr
                tssp = instance_ptr->template_info;

        writer_il_enter_token_cache(self, ifc_cache, edg_reusable_cache_ptr(&tssp->cache->tokens));
      }
      break;
    default:
      ifc_write_catastrophe(ec_unsupported_il_to_ifc);
      break;
  }  /* switch */
}  /* writer_il_enter_extracted_body_to_cache */

static void writer_il_enter_identifier_token_to_cache(an_ifc_il_map *self, 
                                          an_ifc_output_token_cache *ifc_cache,
                                          a_shared_token tok)
/*
Add the given cached token (representing an EDG IFC identifier token) to the
given token cache.
*/
{
  an_ifc_edg_token_identifier identifier_token = {0};
  an_ifc_edg_complex_token_index
                result = writer_alloc_complex_token(self->output_state, 
                                                            &identifier_token);
  a_symbol_header_ptr
                sym_header = edg_token_get_locator(tok)->symbol_header;
  an_ifc_text_offset
                ifc_identifier = writer_il_string_as_text_offset_bytes(self, 
                                                sym_header->identifier,
                                                sym_header->identifier_length);

  set_ifc_text(&identifier_token, ifc_identifier);
  writer_cache_add_complex(ifc_cache, result);
}  /* writer_il_enter_identifier_token_to_cache */

static a_boolean should_token_be_simplified(a_shared_token tok)
/*
Return TRUE if the given token should be simplified to just its corresponding
basic token kind.  This occurs for some token kinds (e.g., tok_true) where the
front end has an associated constant value but it should not be used.
Otherwise, return FALSE.
*/
{
  a_boolean result = FALSE;

  switch (edg_token_get_kind(tok)) {
    case tok_true:
    case tok_false:
      result = TRUE;
      break;
    default:
      break;
  }  /* switch */
  /* Textual tokens cannot currently be simplified tokens. */
  check_assertion(!result ||
               token_to_basic_token_kind(edg_token_get_kind(tok)) != ifc_ebts_complex);
  return result;
}  /* should_token_be_simplified */


static void writer_il_enter_token_cache(an_ifc_il_map *self, an_ifc_output_token_cache *ifc_cache,
                                      a_token_cache             *fe_cache)
/*
Add the tokens in the given front end token cache to the given EDG IFC token
cache.
*/
{
  a_token_cache_iterator it = edg_cache_begin(fe_cache);
  a_token_cache_iterator it_end = edg_cache_end(fe_cache);

  for (; !edg_cache_iter_equal(it, it_end); ++it.offset) {
    a_shared_token tok = *edg_cache_iter_get(it);

    if (should_token_be_simplified(tok)) {
      writer_il_enter_basic_token_to_cache(self, ifc_cache, tok);
      continue;
    }  /* if */

    switch (edg_token_get_extra_kind(tok)) {
      case teik_constant:
        writer_il_enter_constant_token_to_cache(self, ifc_cache, tok);
        break;
      case teik_extracted_body:
        writer_il_enter_extracted_body_to_cache(self, ifc_cache, tok);
        break;
      case teik_identifier:
        writer_il_enter_identifier_token_to_cache(self, ifc_cache, tok);
        break;
      case teik_none:
        { if (edg_token_is(tok, tok_end_of_source)) {
            /* An end of source token should never be followed by another
               token.  If this happens, something is wrong with the token cache
               and the authoring code should be corrected. */
            check_assertion(it.offset + 1 == it_end.offset);
            break;
          }  /* if */
          writer_il_enter_basic_token_to_cache(self, ifc_cache, tok);
        }
        break;
      case teik_asm_string:
      case teik_ifc_index:
      case teik_insert_string:
      case teik_pp_token:
      case teik_pragma:
      case teik_removed_expr:
      case teik_ud_lit:
        /* FIXME: Implement these. */
        ifc_write_catastrophe(ec_unsupported_il_to_ifc);
        break;
      case teik_unresolved_ud_lit:
        /* This token state should never appear in a token cache that is to be
           written to an IFC file; see the description of
           tok_unresolved_ud_literal. */
        unexpected_condition();
      default_is_unexpected();
    }  /* switch */
  }  /* for */
}  /* writer_il_enter_token_cache */

static an_ifc_type_index writer_il_find_or_enter_class_scope_type(an_ifc_il_map *self)
/*
Find or enter the fundamental type used by the IFC to indicate that a given IFC
DeclSort::Scope is a class.  Return the index for the fundamental type.
*/
{
  if (is_null_index(self->fund_class_type)) {
    an_ifc_type_fundamental fund_type = {0};

    self->fund_class_type = writer_alloc_type(self->output_state, &fund_type);
    set_ifc_basis(&fund_type, ifc_tbs_class);
    set_ifc_precision(&fund_type, ifc_tps_default);
    set_ifc_sign(&fund_type, ifc_tss_plain);
  }  /* if */
  return self->fund_class_type;
}  /* writer_il_find_or_enter_class_scope_type */

static an_ifc_type_index writer_il_find_or_enter_namespace_scope_type(an_ifc_il_map *self)
/*
Find or enter the fundamental type used by the IFC to indicate that a given IFC
DeclSort::Scope is a namespace.  Return the index for the fundamental type.
*/
{
  if (is_null_index(self->fund_namespace_type)) {
    an_ifc_type_fundamental fund_type = {0};

    self->fund_namespace_type = writer_alloc_type(self->output_state, &fund_type);
    set_ifc_basis(&fund_type, ifc_tbs_namespace);
    set_ifc_precision(&fund_type, ifc_tps_default);
    set_ifc_sign(&fund_type, ifc_tss_plain);
  }  /* if */
  return self->fund_namespace_type;
}  /* writer_il_find_or_enter_namespace_scope_type */

static an_ifc_type_index writer_il_find_or_enter_alias_typedef_type(an_ifc_il_map *self)
/*
Find or enter the fundamental type used by the IFC to indicate that a given IFC
DeclSort::Alias is a typedef.  Return the index for the fundamental type.
*/
{
  if (is_null_index(self->fund_alias_typedef_type)) {
    an_ifc_type_fundamental fund_type = {0};

    self->fund_alias_typedef_type = writer_alloc_type(self->output_state, &fund_type);
    set_ifc_basis(&fund_type, ifc_tbs_typename);
    set_ifc_precision(&fund_type, ifc_tps_default);
    set_ifc_sign(&fund_type, ifc_tss_plain);
  }  /* if */
  return self->fund_alias_typedef_type;
}  /* writer_il_find_or_enter_alias_typedef_type */

static an_ifc_type_index writer_il_find_or_enter_scoped_enum_type(an_ifc_il_map *self)
/*
Find or enter the fundamental type used by the IFC to indicate that a given IFC
DeclSort::Enumeration is a scoped enum type.  Return the index for the
fundamental type.
*/
{
  if (is_null_index(self->fund_scoped_enum_type)) {
    an_ifc_type_fundamental fund_type = {0};

    self->fund_scoped_enum_type = writer_alloc_type(self->output_state, &fund_type);
    set_ifc_basis(&fund_type, ifc_tbs_class);
    set_ifc_precision(&fund_type, ifc_tps_default);
    set_ifc_sign(&fund_type, ifc_tss_plain);
  }  /* if */
  return self->fund_scoped_enum_type;
}  /* writer_il_find_or_enter_scoped_enum_type */

static an_ifc_type_index writer_il_find_or_enter_struct_scope_type(an_ifc_il_map *self)
/*
Find or enter the fundamental type used by the IFC to indicate that a given IFC
DeclSort::Scope is a struct.  Return the index for the fundamental type.
*/
{
  if (is_null_index(self->fund_struct_type)) {
    an_ifc_type_fundamental fund_type = {0};

    self->fund_struct_type = writer_alloc_type(self->output_state, &fund_type);
    set_ifc_basis(&fund_type, ifc_tbs_struct);
    set_ifc_precision(&fund_type, ifc_tps_default);
    set_ifc_sign(&fund_type, ifc_tss_plain);
  }  /* if */
  return self->fund_struct_type;
}  /* writer_il_find_or_enter_struct_scope_type */

static an_ifc_type_index writer_il_find_or_enter_union_scope_type(an_ifc_il_map *self)
/*
Find or enter the fundamental type used by the IFC to indicate that a given IFC
DeclSort::Scope is a union.  Return the index for the fundamental type.
*/
{
  if (is_null_index(self->fund_union_type)) {
    an_ifc_type_fundamental fund_type = {0};

    self->fund_union_type = writer_alloc_type(self->output_state, &fund_type);
    set_ifc_basis(&fund_type, ifc_tbs_union);
    set_ifc_precision(&fund_type, ifc_tps_default);
    set_ifc_sign(&fund_type, ifc_tss_plain);
  }  /* if */
  return self->fund_union_type;
}  /* writer_il_find_or_enter_union_scope_type */

static an_ifc_type_index writer_il_find_or_enter_unscoped_enum_type(an_ifc_il_map *self)
/*
Find or enter the fundamental type used by the IFC to indicate that a given IFC
DeclSort::Enumeration is an unscoped enum type.  Return the index for the
fundamental type.
*/
{
  if (is_null_index(self->fund_unscoped_enum_type)) {
    an_ifc_type_fundamental fund_type = {0};

    self->fund_unscoped_enum_type = writer_alloc_type(self->output_state, &fund_type);
    set_ifc_basis(&fund_type, ifc_tbs_enum);
    set_ifc_precision(&fund_type, ifc_tps_default);
    set_ifc_sign(&fund_type, ifc_tss_plain);
  }  /* if */
  return self->fund_unscoped_enum_type;
}  /* writer_il_find_or_enter_unscoped_enum_type */

static an_ifc_decl_index writer_il_enter_alias_template(an_ifc_il_map *self, a_template_ptr templ)
/*
Enter the given alias template (templ) into the IFC output state.  Return the
declaration index of the alias template declaration.
*/
{
  an_ifc_decl_alias alias_templ = {0};
  an_ifc_decl_index
                result = writer_il_map_new_decl(self, templ, &alias_templ);

  /* Set the name information. */
  an_ifc_text_offset
                ifc_name_offset = writer_il_entity_name_as_text_offset(self, templ);
  set_ifc_name(&alias_templ, ifc_name_offset);

  /* Set the source location information. */
  an_ifc_source_location
                ifc_src_pos = writer_il_find_or_enter_entity_pos(self, templ);
  set_ifc_locus(&alias_templ, ifc_src_pos);

  /* Set the for-all type describing the parameters. */
  an_ifc_type_forall forall_type = {0};
  an_ifc_type_index
                type_idx = writer_alloc_type(self->output_state, &forall_type);
  an_ifc_chart_index
                param_chart = writer_il_enter_template_params(self, templ);
  set_ifc_chart(&forall_type, param_chart);
  set_ifc_subject(&forall_type, writer_il_find_or_enter_alias_typedef_type(self));
  set_ifc_type(&alias_templ, type_idx);

  /* Set the scope information. */
  an_ifc_decl_index
                scope_decl_idx = writer_il_associate_entity_home_scope(self, templ);
  set_ifc_home_scope(&alias_templ, scope_decl_idx);

  /* Set the aliasee information. */
  an_ifc_type_syntactic syntactic_type = {0};
  an_ifc_type_index
                subject_type_idx = writer_alloc_type(self->output_state, 
                                                              &syntactic_type);
  /* Create a token cache representation of the initializing constant. */
  an_ifc_output_token_cache
                init_token_cache = writer_il_copy_template_body_to_cache(self, templ);
  an_ifc_expr_index
                aliasee_expr_idx = writer_alloc_token_cache_expr(self->output_state, &init_token_cache);
writer_cache_destroy(&init_token_cache);
  an_ifc_type_forall aliasee_type = {0};
  an_ifc_type_index
                aliasee_type_idx = writer_alloc_type(self->output_state, 
                                                                &aliasee_type);
  set_ifc_expr(&syntactic_type, aliasee_expr_idx);
  set_ifc_chart(&aliasee_type, param_chart);
  set_ifc_subject(&aliasee_type, subject_type_idx);
  set_ifc_aliasee(&alias_templ, aliasee_type_idx);

  /* FIXME: Set specifiers. */

  /* Set the access specifier. */
  an_ifc_access_sort
                ifc_access = access_specifier_of(templ);
  set_ifc_access(&alias_templ, ifc_access);
  /* FIXME: Set properties. */
  return result;
}  /* writer_il_enter_alias_template */

static an_ifc_decl_index writer_il_enter_class_template(an_ifc_il_map *self, a_template_ptr templ)
/*
Enter the given class template (templ) into the IFC output state.  Return the
declaration index of the class template declaration.
*/
{
  an_ifc_decl_template class_templ = {0};
  an_ifc_decl_index
                result = writer_il_map_new_decl(self, templ, &class_templ);

  /* Set the name information. */
  an_ifc_name_index
                ifc_name_index = writer_il_entity_name_as_name_index(self, templ);
  set_ifc_name(&class_templ, ifc_name_index);

  /* Set the source location information. */
  an_ifc_source_location
                ifc_src_pos = writer_il_find_or_enter_entity_pos(self, templ);
  set_ifc_locus(&class_templ, ifc_src_pos);

  /* Set the scope information. */
  an_ifc_decl_index
                scope_decl_idx = writer_il_associate_entity_home_scope(self, templ);
  set_ifc_home_scope(&class_templ, scope_decl_idx);

  /* Set the template parameter chart. */
  an_ifc_chart_index
                param_chart = writer_il_enter_template_params(self, templ);
  set_ifc_chart(&class_templ, param_chart);

  /* Set the parameterized entity information. */
  an_ifc_parameterized_entity ifc_entity = an_ifc_parameterized_entity_init(self->output_state->output_file);
  a_type_ptr    prototype_decl = templ->prototype_instantiation.type;
  an_ifc_decl_index
                prototype_decl_idx =
                                writer_il_enter_class_struct_union(self, prototype_decl);
  set_ifc_decl(&ifc_entity, prototype_decl_idx);
  set_ifc_entity(&class_templ, ifc_entity);

  /* FIXME: Set type. */
  /* FIXME: Set specifiers. */

  /* Set the access specifier. */
  an_ifc_access_sort
                ifc_access = access_specifier_of(templ);
  set_ifc_access(&class_templ, ifc_access);
  /* FIXME: Set properties. */
  return result;
}  /* writer_il_enter_class_template */

static INLINE an_ifc_function_traits_bitfield build_function_traits(
                                                      an_ifc_module_file *file,
                                                      a_routine_ptr      rp)
/*
Construct and return a new IFC function traits bitfield value for the given
routine and module file.
*/
{
  an_ifc_function_traits_bitfield_query
          traits = (an_ifc_function_traits_bitfield_query)0;

  if (rp->is_inline) {
    traits = traits | ifc_ftb_inline;
  }  /* if */
  if (rp->is_constexpr) {
    traits = traits | ifc_ftb_constexpr;
  }  /* if */
  if (rp->is_consteval) {
    traits = traits | ifc_ftb_immediate;
  }  /* if */
  if (rp->is_explicit_constructor || rp->is_explicit_conversion_function) {
    traits = traits | ifc_ftb_explicit;
  }  /* if */
  if (rp->is_virtual) {
    traits = traits | ifc_ftb_virtual;
  }  /* if */
  if (rp->pure_virtual) {
    traits = traits | ifc_ftb_pure_virtual;
  }  /* if */
  if (rp->is_defaulted || rp->compiler_generated) {
    traits = traits | ifc_ftb_defaulted;
  }  /* if */
  if (rp->is_deleted) {
    traits = traits | ifc_ftb_deleted;
  }  /* if */
  /* FIXME: Handle: NoReturn, HiddenFriend, and Constrained? */

  an_ifc_function_traits_bitfield_storage
          ifc_raw_traits = to_bitmask_an_ifc_function_traits_bitfield_query(file, traits);
  return (an_ifc_function_traits_bitfield){file, ifc_raw_traits};
}  /* build_function_traits */


static an_ifc_decl_index writer_il_enter_constructor(an_ifc_il_map *self, a_routine_ptr rp)
/*
Enter the given constructor (routine) into the IFC output state.  Return the
declaration index of the constructor declaration.
*/
{
  an_ifc_decl_constructor func_decl = {0};
  an_ifc_decl_index
                result = writer_il_map_new_decl(self, rp, &func_decl);

  /* Set the name information. */
  an_ifc_text_offset
                ifc_name_offset = writer_il_entity_name_as_text_offset(self, rp);
  set_ifc_name(&func_decl, ifc_name_offset);

  /* Set the source location information. */
  an_ifc_source_location
                ifc_src_pos = writer_il_find_or_enter_entity_pos(self, rp);
  set_ifc_locus(&func_decl, ifc_src_pos);

  /* Set the type information. */
  a_type_ptr    type = rp->type;
  an_ifc_type_index
                type_idx = writer_il_find_or_enter_type(self, type);
  set_ifc_type(&func_decl, type_idx);

  /* Set the scope information. */
  an_ifc_decl_index
                scope_decl_idx = writer_il_associate_entity_home_scope(self, rp);
  set_ifc_home_scope(&func_decl, scope_decl_idx);

  /* Set the parameter information. */
  an_ifc_chart_index
                param_chart_idx = writer_il_enter_routine_params(self, rp);
  set_ifc_chart(&func_decl, param_chart_idx);

  /* Set the function traits. */
  an_ifc_function_traits_bitfield
                func_traits = build_function_traits(func_decl.file,
                                                    rp);
  set_ifc_traits(&func_decl, func_traits);
  /* FIXME: Set specifiers. */

  /* Set the access specifier. */
  an_ifc_access_sort
                ifc_access = access_specifier_of(rp);
  set_ifc_access(&func_decl, ifc_access);
  /* FIXME: Set properties. */
  return result;
}  /* writer_il_enter_constructor */

static an_ifc_decl_index writer_il_enter_destructor(an_ifc_il_map *self, a_routine_ptr rp)
/*
Enter the given destructor (routine) into the IFC output state.  Return the
declaration index of the destructor declaration.
*/
{
  an_ifc_decl_destructor func_decl = {0};
  an_ifc_decl_index
                result = writer_il_map_new_decl(self, rp, &func_decl);

  /* Set the name information. */
  an_ifc_text_offset
                ifc_name_offset = writer_il_entity_name_as_text_offset(self, rp);
  set_ifc_name(&func_decl, ifc_name_offset);

  /* Set the source location information. */
  an_ifc_source_location
                ifc_src_pos = writer_il_find_or_enter_entity_pos(self, rp);
  set_ifc_locus(&func_decl, ifc_src_pos);

  /* Set the scope information. */
  an_ifc_decl_index
                scope_decl_idx = writer_il_associate_entity_home_scope(self, rp);
  set_ifc_home_scope(&func_decl, scope_decl_idx);
  /* FIXME: Set eh_spec. */

  /* Set the function traits. */
  an_ifc_function_traits_bitfield
                func_traits = build_function_traits(func_decl.file,
                                                    rp);
  set_ifc_traits(&func_decl, func_traits);
  /* FIXME: Set specifiers. */

  /* Set the access specifier. */
  an_ifc_access_sort
                ifc_access = access_specifier_of(rp);
  set_ifc_access(&func_decl, ifc_access);
  /* FIXME: Set convention. */
  /* FIXME: Set properties. */
  return result;
}  /* writer_il_enter_destructor */

static an_ifc_decl_index writer_il_enter_field(an_ifc_il_map *self, a_field_ptr field)
/*
Enter the given field into the IFC output state.  Return the declaration index
of the field declaration.
*/
{
  an_ifc_decl_index result = {0};

  if (field->bit_size == 0) {
    an_ifc_decl_field field_decl = {0};

    result = writer_il_map_new_decl(self, field, &field_decl);

    /* Set the name information. */
    an_ifc_text_offset
                ifc_name_offset = writer_il_entity_name_as_text_offset(self, field);
    set_ifc_name(&field_decl, ifc_name_offset);

    /* Set the source location information. */
    an_ifc_source_location
                ifc_src_pos = writer_il_find_or_enter_entity_pos(self, field);
    set_ifc_locus(&field_decl, ifc_src_pos);

    /* Set the type information. */
    an_ifc_type_index
                type_idx = writer_il_find_or_enter_type(self, field->type);
    set_ifc_type(&field_decl, type_idx);

    /* Set the scope information. */
    an_ifc_decl_index
                scope_decl_idx = writer_il_associate_entity_home_scope(self, field);
    set_ifc_home_scope(&field_decl, scope_decl_idx);

    /* Set the initializer. */
    if (field->has_initializer) {
      a_shared_token_cache
                init_cache = get_field_initializer_for_module_write(field);
      an_ifc_output_token_cache init_token_cache;
  writer_cache_init(&init_token_cache);
      writer_il_enter_token_cache(self, &init_token_cache, init_cache);
    edg_shared_cache_release(init_cache);

      an_ifc_expr_index
                ifc_init_expr = writer_alloc_token_cache_expr(self->output_state, &init_token_cache);
writer_cache_destroy(&init_token_cache);
      set_ifc_initializer(&field_decl, ifc_init_expr);
    }  /* if */
    /* FIXME: Set alignment. */
    /* FIXME: Set traits. */
    /* FIXME: Set specifier. */

    /* Set the access specifier. */
    an_ifc_access_sort ifc_access = access_specifier_of(field);
    set_ifc_access(&field_decl, ifc_access);
    /* FIXME: Set properties. */
  } else {
    /* FIXME: Implement bitfields. */
    ifc_write_catastrophe(ec_unsupported_il_to_ifc);
  }  /* if */
  return result;
}  /* writer_il_enter_field */

static an_ifc_decl_index writer_il_enter_free_function(an_ifc_il_map *self, a_routine_ptr rp)
/*
Enter the given free function (routine) into the IFC output state.  Return the
declaration index of the free function declaration.
*/
{
  an_ifc_decl_function func_decl = {0};
  an_ifc_decl_index
                result = writer_il_map_new_decl(self, rp, &func_decl);

  /* Set the name information. */
  an_ifc_name_index
                ifc_name_index = writer_il_entity_name_as_name_index(self, rp);
  set_ifc_name(&func_decl, ifc_name_index);

  /* Set the source location information. */
  an_ifc_source_location
                ifc_src_pos = writer_il_find_or_enter_entity_pos(self, rp);
  set_ifc_locus(&func_decl, ifc_src_pos);

  /* Set the type information. */
  a_type_ptr    type = rp->type;
  an_ifc_type_index
                type_idx = writer_il_find_or_enter_type(self, type);
  set_ifc_type(&func_decl, type_idx);

  /* Set the scope information. */
  an_ifc_decl_index
                scope_decl_idx = writer_il_associate_entity_home_scope(self, rp);
  set_ifc_home_scope(&func_decl, scope_decl_idx);

  /* Set the parameter information. */
  an_ifc_chart_index
                param_chart_idx = writer_il_enter_routine_params(self, rp);
  set_ifc_chart(&func_decl, param_chart_idx);

  /* Set the function traits. */
  an_ifc_function_traits_bitfield
                func_traits = build_function_traits(func_decl.file,
                                                    rp);
  set_ifc_traits(&func_decl, func_traits);
  /* FIXME: Set specifiers. */

  /* Set the access specifier. */
  an_ifc_access_sort
                ifc_access = access_specifier_of(type);
  set_ifc_access(&func_decl, ifc_access);

  /* Apply the appropriate reachable property flags. */
  an_ifc_reachable_properties_bitfield_query
                properties = (an_ifc_reachable_properties_bitfield_query)0;
  /* Flag that an initializer is present if relevant. */
  if (rp->assoc_template != NULL) {
    an_ifc_edg_trait_function_definition def_trait = {0};

    writer_alloc_decl_trait(self->output_state, result, &def_trait);

    /* Create an IFC token cache corresponding to the front end template token
       cache. */
    a_template_ptr
                templ = rp->assoc_template;
    an_ifc_output_token_cache
                init_token_cache = writer_il_copy_template_body_to_cache(self, templ);
    an_ifc_edg_token_cache_offset
                token_cache_offset = writer_alloc_token_cache(self->output_state, &init_token_cache);
writer_cache_destroy(&init_token_cache);
    set_ifc_initializer(&def_trait, token_cache_offset);
    /* Apply the initializer flag to indicate the presence of this
       definition. */
    properties = properties | ifc_rpb_initializer;
  } else if (is_routine_definition_exported_inline(rp)) {
    an_ifc_edg_trait_function_definition def_trait = {0};

    writer_alloc_decl_trait(self->output_state, result, &def_trait);

    /* Create an IFC token cache corresponding to the saved front end
       definition token cache. */
    a_shared_token_cache
                definition_cache =
                                  get_function_definition_for_module_write(rp);
    an_ifc_output_token_cache init_token_cache;
  writer_cache_init(&init_token_cache);
    writer_il_enter_token_cache(self, &init_token_cache, definition_cache);
    edg_shared_cache_release(definition_cache);

    an_ifc_edg_token_cache_offset
                token_cache_offset = writer_alloc_token_cache(self->output_state, &init_token_cache);
writer_cache_destroy(&init_token_cache);
    set_ifc_initializer(&def_trait, token_cache_offset);
    /* Apply the initializer flag to indicate the presence of this
       definition. */
    properties = properties | ifc_rpb_initializer;
  }  /* if */

  an_ifc_reachable_properties_bitfield_storage
                ifc_raw_properties = to_bitmask_an_ifc_reachable_properties_bitfield_query(func_decl.file,
                                                properties);
  an_ifc_reachable_properties_bitfield ifc_properties = (an_ifc_reachable_properties_bitfield){func_decl.file, ifc_raw_properties};
  set_ifc_properties(&func_decl, ifc_properties);
  return result;
}  /* writer_il_enter_free_function */

static an_ifc_decl_index writer_il_enter_function_template(an_ifc_il_map *self, a_template_ptr templ)
/*
Enter the given function template declaration into the IFC output state.
Return the declaration index of the function template declaration.
*/
{
  an_ifc_decl_template func_templ = {0};
  an_ifc_decl_index
                result = writer_il_map_new_decl(self, templ, &func_templ);

  /* Set the name information. */
  an_ifc_name_index
                ifc_name_index = writer_il_entity_name_as_name_index(self, templ);
  set_ifc_name(&func_templ, ifc_name_index);

  /* Set the source location information. */
  an_ifc_source_location
                ifc_src_pos = writer_il_find_or_enter_entity_pos(self, templ);
  set_ifc_locus(&func_templ, ifc_src_pos);

  /* Set the scope information. */
  an_ifc_decl_index
                scope_decl_idx = writer_il_associate_entity_home_scope(self, templ);
  set_ifc_home_scope(&func_templ, scope_decl_idx);

  /* Set the template parameter chart. */
  an_ifc_chart_index
                param_chart = writer_il_enter_template_params(self, templ);
  set_ifc_chart(&func_templ, param_chart);

  /* Set the parameterized entity information. */
  an_ifc_parameterized_entity ifc_entity = an_ifc_parameterized_entity_init(self->output_state->output_file);
  a_routine_ptr prototype_decl = templ->prototype_instantiation.routine;
  an_ifc_decl_index
                prototype_decl_idx = writer_il_enter_routine(self, prototype_decl);
  set_ifc_decl(&ifc_entity, prototype_decl_idx);
  set_ifc_entity(&func_templ, ifc_entity);

  /* FIXME: Set type. */
  /* FIXME: Set specifiers. */

  /* Set the access specifier. */
  an_ifc_access_sort
                ifc_access = access_specifier_of(templ);
  set_ifc_access(&func_templ, ifc_access);
  /* FIXME: Set properties. */
  return result;
}  /* writer_il_enter_function_template */

static an_ifc_decl_index writer_il_enter_member_function(an_ifc_il_map *self, a_routine_ptr rp)
/*
Enter the given member function (routine) into the IFC output state.  Return
the declaration index of the member function declaration.
*/
{
  an_ifc_decl_method func_decl = {0};
  an_ifc_decl_index
                result = writer_il_map_new_decl(self, rp, &func_decl);

  /* Set the name information. */
  an_ifc_name_index
                ifc_name_index = writer_il_entity_name_as_name_index(self, rp);
  set_ifc_name(&func_decl, ifc_name_index);

  /* Set the source location information. */
  an_ifc_source_location
                ifc_src_pos = writer_il_find_or_enter_entity_pos(self, rp);
  set_ifc_locus(&func_decl, ifc_src_pos);

  /* Set the type information. */
  a_type_ptr    type = rp->type;
  an_ifc_type_index
                type_idx = writer_il_find_or_enter_type(self, type);
  set_ifc_type(&func_decl, type_idx);

  /* Set the scope information. */
  an_ifc_decl_index
                scope_decl_idx = writer_il_associate_entity_home_scope(self, rp);
  set_ifc_home_scope(&func_decl, scope_decl_idx);

  /* Set the parameter information. */
  an_ifc_chart_index
                param_chart_idx = writer_il_enter_routine_params(self, rp);
  set_ifc_chart(&func_decl, param_chart_idx);

  /* Set the function traits. */
  an_ifc_function_traits_bitfield
                func_traits = build_function_traits(func_decl.file,
                                                    rp);
  set_ifc_traits(&func_decl, func_traits);
  /* FIXME: Set specifiers. */

  /* Set the access specifier. */
  an_ifc_access_sort
                ifc_access = access_specifier_of(type);
  set_ifc_access(&func_decl, ifc_access);

  /* Apply the appropriate reachable property flags. */
  an_ifc_reachable_properties_bitfield_query
                properties = (an_ifc_reachable_properties_bitfield_query)0;
  /* Flag that an initializer is present if relevant. */
  if (is_routine_definition_exported_inline(rp)) {
    an_ifc_edg_trait_function_definition def_trait = {0};

    writer_alloc_decl_trait(self->output_state, result, &def_trait);

    /* Create an IFC token cache corresponding to the saved front end
       definition token cache. */
    a_shared_token_cache
                definition_cache =
                                  get_function_definition_for_module_write(rp);
    an_ifc_output_token_cache init_token_cache;
  writer_cache_init(&init_token_cache);
    writer_il_enter_token_cache(self, &init_token_cache, definition_cache);
    edg_shared_cache_release(definition_cache);

    an_ifc_edg_token_cache_offset
                token_cache_offset = writer_alloc_token_cache(self->output_state, &init_token_cache);
writer_cache_destroy(&init_token_cache);
    set_ifc_initializer(&def_trait, token_cache_offset);
    /* Apply the initializer flag to indicate the presence of this
       definition. */
    properties = properties | ifc_rpb_initializer;
  }  /* if */
  an_ifc_reachable_properties_bitfield_storage
                ifc_raw_properties = to_bitmask_an_ifc_reachable_properties_bitfield_query(func_decl.file,
                                                properties);
  an_ifc_reachable_properties_bitfield ifc_properties = (an_ifc_reachable_properties_bitfield){func_decl.file, ifc_raw_properties};
  set_ifc_properties(&func_decl, ifc_properties);
  return result;
}  /* writer_il_enter_member_function */

static an_ifc_decl_index writer_il_enter_namespace_scope(an_ifc_il_map *self, a_scope_ptr scope)
/*
Enter the namespace corresponding to the given scope into the IFC output state.
Return the declaration index of the scope declaration.
*/
{
  check_assertion(scope->kind == sck_namespace ||
                  scope->kind == sck_namespace_extension ||
                  scope->kind == sck_namespace_reactivation);
  a_namespace_ptr
                nsp = scope->variant.assoc_namespace;
  an_ifc_decl_index
                result = writer_il_find_or_enter_namespace(self, nsp);

  /* Resolve the scope to this DeclIndex to resolve the scope for find_
     functions that utilize the scope. */
  writer_set_il_entry_to_decl(self, make_tagged_ptr(scope), result);

  /* Fetch the scope declaration representing the namespace; there is now at
     least one element that needs to be associated with it. */
  an_ifc_decl_scope scope_decl = {0};
  writer_fetch_decl(self->output_state, &scope_decl, result);

  /* Associate the namespace scope decl with its contents. */
  an_ifc_scope_offset
                initializer = writer_il_find_or_enter_scope(self, scope);
  set_ifc_initializer(&scope_decl, initializer);
  return result;
}  /* writer_il_enter_namespace */

static an_ifc_decl_index writer_il_enter_typedef(an_ifc_il_map *self, a_type_ptr type)
/*
Enter the typedef corresponding to the given type into the IFC output state.
Return the declaration index of the IFC DeclSort::Alias (i.e., the IFC
representation of the typedef).
*/
{
  check_assertion(type->kind == tk_typeref && typeref_is_typedef(type));
  an_ifc_decl_alias alias_decl = {0};
  an_ifc_decl_index
                result = writer_il_map_new_decl(self, type, &alias_decl);
  /* Set the name information. */
  an_ifc_text_offset
                ifc_name_offset = writer_il_entity_name_as_text_offset(self, type);
  set_ifc_name(&alias_decl, ifc_name_offset);

  /* Set the source location information. */
  an_ifc_source_location
                ifc_src_pos = writer_il_find_or_enter_entity_pos(self, type);
  set_ifc_locus(&alias_decl, ifc_src_pos);

  /* Set the IFC fundamental type to indicate this is a typedef
     DeclSort::Alias. */
  an_ifc_type_index
                type_kind_type = writer_il_find_or_enter_alias_typedef_type(self);
  set_ifc_type(&alias_decl, type_kind_type);

  /* Set the scope information. */
  an_ifc_decl_index
                scope_decl_idx = writer_il_associate_entity_home_scope(self, type);
  set_ifc_home_scope(&alias_decl, scope_decl_idx);

  /* Set the aliasee type. */
  an_ifc_type_index
                aliasee = writer_il_find_or_enter_type(self, type->variant.typeref.type);
  set_ifc_aliasee(&alias_decl, aliasee);
  /* FIXME: Set specifiers. */

  /* Set the access specifier. */
  an_ifc_access_sort ifc_access = access_specifier_of(type);
  set_ifc_access(&alias_decl, ifc_access);
  return result;
}  /* writer_il_enter_typedef */

static an_ifc_decl_index writer_il_find_or_enter_home_scope(an_ifc_il_map *self, a_scope_ptr scope)
/*
For the given scope find or enter the associated scope information into the IFC
output state.  Return the declaration index for the scope.

Note that this function is typically not what should be used to set an entity's
home scope as it does not register the entity with the scope.  Instead, prefer
using associate_entity_home_scope.
*/
{
  a_tagged_pointer
                tagged_scope = make_tagged_ptr(scope);
  an_ifc_decl_index
                result = writer_get_il_entry_to_decl(self, tagged_scope);
  if (is_null_index(result)) {
    result = writer_il_enter_home_scope(self, scope);
  }  /* if */
  return result;
}  /* writer_il_find_or_enter_home_scope */

static an_ifc_decl_index writer_il_enter_home_scope(an_ifc_il_map *self, a_scope_ptr scope)
/*
For the given scope enter the associated scope information into the IFC output
state.  Return the declaration index for the scope.
*/
{
  check_assertion(is_null_index(
                          writer_get_il_entry_to_decl(self, make_tagged_ptr(scope))));
  an_ifc_decl_index result = {0};

  switch (scope->kind) {
    case sck_class_struct_union:
      result =
             writer_il_find_or_enter_class_struct_union(self, scope->variant.assoc_type);
      break;
    case sck_file:
      /* Use a null index to indicate the primary scope. */
      /* Additionally ensure this scope is entered in the scope table. */
      (void)writer_il_find_or_enter_scope(self, scope);
      break;
    case sck_namespace:
    case sck_namespace_extension:
    case sck_namespace_reactivation:
      result = writer_il_enter_namespace_scope(self, scope);
      break;
    case sck_func_prototype:
    case sck_block:
    case sck_class_reactivation:
    case sck_template_declaration:
    case sck_template_instantiation:
    case sck_instantiation_context:
    case sck_module_decl_import:
    case sck_module_isolated:
    case sck_pragma:
    case sck_function_access:
    case sck_condition:
    case sck_enum:
    case sck_function:
    case sck_none:
      break;
    default_is_unexpected();
  }  /* switch */
  return result;
}  /* writer_il_enter_home_scope */

static void writer_il_map_scope_member(an_ifc_il_map *self, a_scope_ptr       scope,
                                     an_ifc_decl_index decl)
/*
Map the given declaration index to the given scope's associated array of scope
members.
*/
{
  an_ifc_scope_offset offset = writer_il_find_or_enter_scope(self, scope);

  /* A null index should never be mapped as a member of a scope. */
  check_assertion(!is_null_index(decl));
  edg_array_push(edg_array_at(&self->scope_members, offset.value - 1), &decl);
}  /* writer_il_map_scope_member */

static void dump_scope_types(an_ifc_il_map *il_map,
                             a_scope_ptr   scope)
/*
Add all the types in the given scope to the given IL -> IFC mapping.
*/
{
  for (a_type_ptr type = scope->types; type != NULL; type = type->next) {
    if (is_template_class_type(type) || is_template_alias_type(type)) {
      /* Skip template types (these will be handled by
         dump_scope_templates). */
      continue;
    }  /* if */
    if (type->kind == tk_template_param && tptk_is(type, tptk_member)) {
      /* Skip member template parameters, since these are used for nonreal
         nested types (and nonreal nested types are not currently relevant to
         modules).  See create_nonreal_version_of_nested_type for more
         information. */
      continue;
    }  /* if */
    (void)writer_il_find_or_enter_type(il_map, type);
  }  /* for */
}  /* dump_scope_types */


static void dump_scope_templates(an_ifc_il_map *il_map,
                                 a_scope_ptr   scope)
/*
Add all the templates in the given scope to the given IL -> IFC mapping.
*/
{
  for (a_template_ptr tp = scope->templates; tp != NULL; tp = tp->next) {
    (void)writer_il_find_or_enter_template(il_map, tp);
  }  /* for */
}  /* dump_scope_templates */


static void dump_scope_routines(an_ifc_il_map *il_map,
                                a_scope_ptr   scope)
/*
Add all the routines in the given scope to the given IL -> IFC mapping.
*/
{
  for (a_routine_ptr rp = scope->routines; rp != NULL; rp = rp->next) {
    if (rp->is_template_function && !rp->is_specialized) {
      /* This is a function template which will be handled when
         dump_scope_templates is called. */
      continue;
    }  /* if */
    (void)writer_il_find_or_enter_routine(il_map, rp);
  }  /* for */
}  /* dump_scope_routines */


static void dump_scope_namespaces(an_ifc_il_map *il_map,
                                  a_scope_ptr   scope)
/*
Add all the namespaces in the given scope to the given IL -> IFC mapping.
*/
{
  for (a_namespace_ptr np = scope->namespaces; np != NULL; np = np->next) {
    if (np->is_namespace_alias) {
      ifc_write_catastrophe(ec_unsupported_il_to_ifc);
    } else {
      a_scope_ptr assoc_scope = np->variant.assoc_scope;

      dump_scope_recursively(il_map, assoc_scope);
    }  /* if */
  }  /* for */
}  /* dump_scope_namespaces */


static void dump_scope_using_directives(an_ifc_il_map *il_map,
                                        a_scope_ptr   scope)
/*
Add all the using-directives in the given scope to the given IL -> IFC mapping.
*/
{
  for (a_using_decl_ptr udp = scope->using_directives; udp != NULL;
       udp = udp->next) {
    (void)writer_il_find_or_enter_using_directive(il_map, udp, scope);
  }  /* for */
}  /* dump_scope_using_directives */


static void dump_scope_recursively(an_ifc_il_map *il_map,
                                   a_scope_ptr   scope)
/*
Add the contents of the given scope and all its child scopes to the given IL ->
IFC mapping.
*/
{
  dump_scope_types(il_map, scope);
  dump_scope_templates(il_map, scope);
  dump_scope_routines(il_map, scope);
  dump_scope_namespaces(il_map, scope);
  dump_scope_using_directives(il_map, scope);
}  /* dump_scope_recursively */


static void complete_scope_info(an_ifc_output_state *output_state,
                                an_ifc_il_map       *il_map)
/*
The IFC scope membership information is built up during the IL -> IFC mapping
process.  This function is called once the IL has been completely mapped to the
output state to create the scope membership information as a sort of
"post-processing" step.

This is done as IFC scopes are required to be contiguous blocks of declaration
indexes; thus, by collecting the scope members and deferring construction no
special traversal logic is required to account for all scope members.
*/
{
  size_t num_of_scopes = il_map->scope_members.len;
  size_t scope_start = 0;

  for (size_t i = 0; i < num_of_scopes; ++i) {
    an_ifc_scope_descriptor scope_descr = {0};

    writer_fetch_node(output_state, &scope_descr, i);

    const a_scope_member_array *members = ((a_scope_member_array *)edg_array_at(&il_map->scope_members, i));
    for (size_t member_i = 0; member_i < members->len; ++member_i) {
      an_ifc_decl_index decl_idx = *(an_ifc_decl_index *)edg_array_at(members, member_i);
      an_ifc_scope_member scope_mem = {0};

      (void)writer_alloc_node(output_state, &scope_mem);
      set_ifc_index(&scope_mem, decl_idx);
    }  /* for */

    an_ifc_module_file *file = scope_descr.file;
    an_ifc_index ifc_scope_start = (an_ifc_index){file, scope_start};
    an_ifc_cardinality ifc_scope_count = (an_ifc_cardinality){
                                file,
                                (an_ifc_cardinality_storage)members->len};
    set_ifc_start(&scope_descr, ifc_scope_start);
    set_ifc_cardinality(&scope_descr, ifc_scope_count);
    scope_start += members->len;
  }  /* for */
}  /* complete_scope_info */


static an_error_code module_to_error_code(a_module_ptr mod)
/*
Return the error code corresponding to the current output module kind.
*/
{
  an_error_code result;

  if (mod == NULL) {
    result = ec_edg_ifc_header_unit;
  } else if (mod->kind == mk_unit) {
    result = ec_edg_ifc_interface_unit;
  } else if (mod->kind == mk_unit_partition) {
    result = ec_edg_ifc_partition_unit;
  } else {
    /* This should not be possible as the module writing code should not have
       been called. */
    unexpected_condition();
  }  /* switch */
  return result;
}  /* module_to_error_code */



typedef struct writer_optional_module_file {
  a_boolean has_value;
  an_ifc_module_file value;
} writer_optional_module_file;

static writer_optional_module_file create_output_file(a_const_char *file_path,
                                                       a_module_ptr source_module)
{
  writer_optional_module_file result = {0};
  FILE *f_handle = open_output_file_with_error_handling(
                                          file_path,
                                          TRUE, FALSE, OFF_NO_OPTIONS,
                                          module_to_error_code(source_module));
  an_ifc_module_file_write_state *write_state;
  if (f_handle == NULL) return result;
  an_ifc_module_file_init(&result.value, mfk_edg_ifc, FALSE);
  result.has_value = TRUE;
  result.value.version_major = 0;
  result.value.version_minor = 44;
  result.value.f_module = f_handle;
  write_state = an_ifc_module_file_get_write_state(&result.value);
  write_state->source_module = source_module;
  write_state->source_file_name = primary_source_file_name;
  return result;
}

/* Heap-backed active contexts remain valid when MAKE_FRONT_END_CALLABLE
   terminates writing via longjmp. Teardown always runs in reverse order. */
STATIC_THREAD an_ifc_module_file *output_module_file;
STATIC_THREAD an_ifc_output_state *active_output_state;
STATIC_THREAD an_ifc_il_map *active_il_map;

static void writer_clear_active(void)
{
  if (active_il_map != NULL) {
    writer_il_map_destroy(active_il_map);
    free_general(active_il_map, sizeof(*active_il_map));
    active_il_map = NULL;
  }
  if (active_output_state != NULL) {
    writer_state_destroy(active_output_state);
    free_general(active_output_state, sizeof(*active_output_state));
    active_output_state = NULL;
  }
  if (output_module_file != NULL) {
    an_ifc_module_file_destroy(output_module_file);
    free_general(output_module_file, sizeof(*output_module_file));
    output_module_file = NULL;
  }
}

void ifc_modules_write_one_time_init(void)
{
  output_module_file = NULL;
  active_output_state = NULL;
  active_il_map = NULL;
}

void ifc_modules_write_out(void)
{
  writer_optional_module_file opt_module_file = create_output_file(
                             module_unit_output_file_name, trans_unit_module);
  if (opt_module_file.has_value) {
    a_scope_ptr scope = il_header.primary_scope;
    an_ifc_scope_offset ifc_global_scope;
    check_assertion(output_module_file == NULL);
    output_module_file = (an_ifc_module_file *)alloc_general(sizeof(*output_module_file));
    an_ifc_module_file_init(output_module_file, mfk_edg_ifc, FALSE);
    an_ifc_module_file_move(output_module_file, &opt_module_file.value);
    opt_module_file.has_value = FALSE;
    active_output_state = (an_ifc_output_state *)alloc_general(sizeof(*active_output_state));
    writer_state_init(active_output_state, output_module_file);
    active_il_map = (an_ifc_il_map *)alloc_general(sizeof(*active_il_map));
    writer_il_map_init(active_il_map, active_output_state);
    ifc_global_scope = writer_il_enter_scope(active_il_map, scope);
    active_output_state->global_scope = ifc_global_scope;
    dump_scope_recursively(active_il_map, scope);
    complete_scope_info(active_output_state, active_il_map);
    writer_sort_traits(active_output_state);
    writer_write(active_output_state);
    writer_clear_active();
  }
}

#if MAKE_FRONT_END_CALLABLE
void ifc_modules_write_cleanup(void)
{
  writer_clear_active();
}
#endif /* MAKE_FRONT_END_CALLABLE */

static an_ifc_decl_index writer_associate_home_scope(an_ifc_il_map *self,
                                        a_tagged_pointer entity, a_scope_ptr scope)
{
  if (!tagged_entity_is_nonreal(entity)) {
    an_ifc_decl_index index = writer_get_il_entry_to_decl(self, entity);
    writer_il_map_scope_member(self, scope, index);
  }
  return writer_il_find_or_enter_home_scope(self, scope);
}
