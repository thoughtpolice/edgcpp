/*
Part of the EDG Compiler Project, under the Apache License v2.0 with LLVM
Exceptions.
See https://edgcpp.org/LICENSE.txt for license information.
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
*/

/*

modules.c -- Classes and routines handling modules.

*/

/* Header files common to all files. */
#include "fe_common.h"
#include "ifc_modules.h"
#include "pch.h"
#include "util.h"

/* Other required header files. */
#include "templates.h"

#ifdef PCH_PRAGMA_GUARD
/* Mark the end of the sequence of headers subject to precompiled header
   processing. */
#pragma hdrstop
#endif /* ifdef PCH_PRAGMA_GUARD */

/* Conditionally open the "edg" namespace. */

static void *module_alloc(size_t size)
{ void *p=alloc_fe(size);memset(p,0,size);return p; }

void a_module_entity_init(a_module_entity *entity,a_module_ptr module)
{
  memset(entity,0,sizeof(*entity));
  entity->module_info=module;
  entity->entity.kind=iek_none;
  edg_array_init_buffer(&entity->locators,sizeof(a_module_entry_locator),
                        EDG_ALLOC_FE,1,NULL,NULL,entity->locator_buffer,1);
}
a_module_entity *a_module_entity_new(a_module_ptr module)
{ a_module_entity *p=alloc_fe_of_type(a_module_entity);a_module_entity_init(p,module);return p; }
void a_module_entity_destroy(a_module_entity *entity)
{ edg_array_destroy(&entity->locators); }
static void module_array_delete(Edg_array **array)
{ if (*array != NULL) { edg_array_destroy(*array);free_fe(*array,sizeof(**array));*array=NULL; } }
static void module_map_delete(Edg_map **map)
{ if (*map != NULL) { edg_map_destroy(*map);free_fe(*map,sizeof(**map));*map=NULL; } }
static void module_optional_string_destroy(Opt_a_string *value)
{ if (value->has_value) { edg_string_destroy(&value->value);value->has_value=FALSE; } }
static const char *module_map_path(const Edg_map *map,a_string_view key)
{ const char **value=edg_map_get(map,&key);return value==NULL?NULL:*value; }
static a_module_ptr module_map_module(const Edg_map *map,a_string_view key)
{ a_module_ptr *value=edg_map_get(map,&key);return value==NULL?NULL:*value; }

struct a_module_file_suffix {
  a_const_char  *suffix;
                        /* The suffix associated with the module file. */
  a_module_file_kind
                kind;
                        /* The kind of module this suffix implies. */
};  /* a_module_file_suffix */

static const struct a_module_file_suffix module_file_suffixes[] = {
  { "eifc", mfk_edg_ifc },
  { "ifc", mfk_ms_ifc }
};



STATIC_THREAD a_text_buffer_ptr
                module_search_buffer;
                        /* A buffer used during the search for the binary
                           module interface file associated with a given
                           module. */


static a_boolean module_has_interface_dependency(a_symbol_ptr module_sym,
                                                 a_symbol_ptr interface_sym)
/*
Return TRUE if the given module symbol depends on the given interface symbol;
otherwise, return FALSE.
*/
{
  check_assertion(module_sym->kind == sk_named_module &&
                  (interface_sym == NULL ||
                   interface_sym->kind == sk_named_module));
  a_boolean result = TRUE;

  if (interface_sym == NULL) {
    result = FALSE;
  } else if (!module_sym->variant.module_info.is_interface_unit &&
             (module_sym->variant.module_info.partition_name != NULL ||
              microsoft_mode)) {
    /* See decl_module in decls.c for more information about the above
       condition. */
    result = FALSE;
  } else if (module_sym->header != interface_sym->header) {
    result = FALSE;
  }  /* if */
  return result;
}  /* module_has_interface_dependency */


a_boolean check_module_has_interface_dependency(
                                           a_symbol_ptr          module_sym,
                                           a_symbol_ptr          interface_sym,
                                           a_source_position_ptr pos)
/*
Determine whether the given module has an interface dependency on the module
referred to by interface_sym.  pos is the position to use for diagnostics (if
any).  Return TRUE if there exists an interface dependency, FALSE otherwise.
*/
{
  a_boolean result = FALSE;

  /* The easy case of a module importing itself.  The only exception is an
     implementation unit which imports its own primary interface unit. */
  if (module_has_interface_dependency(module_sym, interface_sym)) {
    pos_error(ec_module_cannot_depend_on_itself, pos);
    result = TRUE;
  }
  /* FIXME: Recurse over interface_sym's interface dependencies. */
  return result;
}  /* check_module_has_interface_dependency */


typedef Edg_map a_module_name_map;

STATIC_THREAD a_module_name_map
                *known_modules;
                        /* A mapping of module names to their corresponding
                           module IL entry. */


static void init_named_module(a_module_ptr        mod,
                              a_symbol_header_ptr name_sym)
/*
Initialize a named module with the module name specified by the given symbol
header.
*/
{
  mod->variant.unit.name = name_sym->identifier;
}  /* init_named_module */


a_module_ptr find_or_create_module(a_symbol_ptr module_sym)
/*
Find or create an IL module entity corresponding to the given module symbol.
*/
{
  /* Ensure the modules system is fully loaded. */
  require_modules();
  /* Ensure a module symbol was given. */
  check_assertion(module_sym->kind == sk_named_module);
  a_string_view module_sym_id=edg_string_view(module_sym->header->identifier,
                              module_sym->header->identifier_length);
  a_module_ptr  result = module_map_module(known_modules,module_sym_id);

  if (result == NULL) {
    a_symbol_header_ptr unit_hdr =
                                  module_sym->variant.module_info.primary_name;
    a_symbol_header_ptr partition_hdr =
                                module_sym->variant.module_info.partition_name;
    a_module_kind       module_kind =
                                     partition_hdr == NULL ? mk_unit
                                                           : mk_unit_partition;

    /* Allocate the new module. */
    result = alloc_module(module_kind);
    edg_map_set(known_modules,&module_sym_id,&result);
    if (module_kind == mk_unit) {
      init_named_module(result, unit_hdr);
    } else {
      a_string_view unit_sym_id=edg_string_view(unit_hdr->identifier,
                                unit_hdr->identifier_length);
      a_module_ptr  unit_module = module_map_module(known_modules,unit_sym_id);

      if (unit_module == NULL) {
        unit_module = alloc_module(mk_unit);
        edg_map_set(known_modules,&unit_sym_id,&unit_module);
        init_named_module(unit_module, unit_hdr);
      }  /* if */
      result->variant.unit_partition.name = partition_hdr->identifier;
      result->variant.unit_partition.unit = unit_module;
    }  /* if */
  }  /* if */
  return result;
}  /* find_or_create_module */


static a_module_file_kind determine_module_file_kind(FILE *file)
/*
Given an already open file pointer, determine what kind of module file is open
(if any).  Return the kind of module file.
*/
{
  a_module_file_kind result = mfk_unknown;
  a_byte             magic[4];

  /* Ensure we're at the start of the file. */
  if (fseek(file, 0, SEEK_SET) != 0) {
    catastrophe(ec_module_read_error);
  }  /* if */
  if (fread(magic, (size_t)1, sizeof(magic), file) == sizeof(magic)) {
    if (magic_numbers_match(magic, edg_ifc_magic_numbers)) {
      result = mfk_edg_ifc;
    } else if (magic_numbers_match(magic, ms_ifc_magic_numbers)) {
      result = mfk_ms_ifc;
    }  /* if */
  }  /* if */
  return result;
}  /* determine_module_file_kind */


static a_boolean is_module_kind_available(a_module_file_kind kind)
/*
Given a supported module file kind, return TRUE if it's currently usable;
otherwise, return FALSE.
*/
{
  a_boolean result = FALSE;

  switch (kind) {
    case mfk_edg_ifc:
      /* Always enabled. */
      result = TRUE;
      break;
    case mfk_unknown:
      /* Always disabled. */
      break;
    case mfk_ms_ifc:
#if MICROSOFT_EXTENSIONS_ALLOWED
      result = microsoft_mode;
#endif /* #if MICROSOFT_EXTENSIONS_ALLOWED */
      break;
    default_is_unexpected();
  }  /* if */
  return result;
}  /* is_module_kind_available */


static a_boolean check_module_file(a_const_char       *module_file,
                                   a_module_file_kind *file_kind)
/*
Return TRUE if the provided module file exists and is the given kind, FALSE
otherwise.  Set *file_kind to the module file kind discovered.

This function may not return and instead issue a catastrophic error if the
module file exists but cannot be opened.
*/
{
  a_boolean           result = FALSE;
  FILE*               file;
  an_open_file_result open_result;

  file = fopen_with_result(module_file, FOPEN_MODE_FOR_BINARY_READ,
                           &open_result);
  *file_kind = mfk_unknown;
  if (file == NULL) {
    /* Open failed.  Most reasons are likely valid, but check for specific
       problem cases. */
    if (open_result.flags & OFR_CANNOT_OPEN) {
      /* Note that file_open_error does not return when called from here. */
      file_open_error(es_error, ec_module_file, module_file,
                      &open_result);
    }  /* if */

    a_diagnostic_ptr dp = pos_start_catastrophe(ec_file_for_module_not_found,
                                                &error_position,
                                                module_file);
    /* Note any module mappings pointing to this file. */
    size_t cursor=0;
    void *key,*value;
    while (edg_map_next(mod_map,&cursor,&key,&value)) {
      if (strcmp(*(a_const_char **)value,module_file)==0)
        add_diag_info(dp,ec_found_from_module_map,*(a_string_view *)key);
    }
    cursor=0;
    while (edg_map_next(header_unit_map,&cursor,&key,&value)) {
      if (strcmp(*(a_const_char **)value,module_file)==0)
        str_add_diag_info(dp,ec_found_from_header_unit_map,((a_path_handle *)key)->ptr);
    }
    /* Emit the diagnostic. */
    end_diagnostic(dp);
    /* This function should not continue following the end of the
       (catastrophic) diagnostic. */
  } else {
    /* We've found a file - determine what kind it is. */
    *file_kind = determine_module_file_kind(file);
    (void)fclose(file);
    if (!is_module_kind_available(*file_kind)) {
      str_catastrophe(ec_ms_ifc_unavailable, module_file);
      /* This function should not continue following the catastrophic
         diagnostic. */
    }  /* if */
    result = TRUE;
  }  /* if */
  return result;
}  /* check_module_file */


static a_module_file_kind get_module_kind(a_const_char *module_file)
/*
Given a module file, return the module's kind.
*/
{
  a_module_file_kind  result = mfk_unknown;
  FILE*               file;
  an_open_file_result open_result;

  file = fopen_with_result(module_file, FOPEN_MODE_FOR_BINARY_READ,
                           &open_result);
  if (file == NULL) {
    goto done;
  }  /* if */
  /* We've found a file - determine what kind it is. */
  result = determine_module_file_kind(file);
done:
  if (file != NULL) {
    (void)fclose(file);
  }  /* if */
  return result;
}  /* get_module_kind */


static Opt_a_string get_name_of_module(a_const_char *module_file)
/*
Given a module file, return the module's name.  If the name could not be
determined, return an empty optional.
*/
{
  Opt_a_string result={0};
  a_module_file_kind kind = get_module_kind(module_file);

  switch (kind) {
    case mfk_edg_ifc:
    case mfk_ms_ifc:
      result = get_name_of_ifc_module(module_file);
      break;
    case mfk_unknown:
      /* The name cannot be resolved; return an empty optional. */
      break;
    default_is_unexpected();
  }  /* switch */
  return result;
}  /* get_name_of_module */


static a_boolean module_file_matches(a_const_char *module_name,a_const_char *module_file)
{
  Opt_a_string name=get_name_of_module(module_file);
  a_boolean result=name.has_value && strcmp(edg_string_cstr(&name.value),module_name)==0;
  module_optional_string_destroy(&name);
  return result;
}  /* module_file_matches */


static void resolve_lazy_mod_map_element(void)
{
  check_assertion(lazy_mod_map_arr->len!=0);
  a_const_char *path=*(a_const_char **)edg_array_at(lazy_mod_map_arr,lazy_mod_map_arr->len-1);
  Opt_a_string name=get_name_of_module(path);
  if (name.has_value) {
    a_string_view temporary=edg_string_as_view(&name.value);
    if (edg_map_get(mod_map,&temporary)==NULL) {
      a_const_char *chars=edg_string_to_storage(&name.value,EDG_ALLOC_GENERAL);
      a_string_view key=edg_string_view(chars,name.value.len);
      edg_map_set(mod_map,&key,&path);
    } else {
      pos_catastrophe(ec_multiple_module_matches,&error_position,temporary);
    }
  }
  module_optional_string_destroy(&name);
  edg_array_remove(lazy_mod_map_arr,lazy_mod_map_arr->len-1,1);
}  /* resolve_lazy_mod_map_element */


static a_boolean find_module_file_in_map(a_module_ptr mod)
{
  a_boolean found=FALSE;
  a_string name=module_full_name_of(mod);
  a_string_view key=edg_string_as_view(&name);
  a_const_char *path=module_map_path(mod_map,key);
  if (path==NULL && lazy_mod_map_arr->len!=0) {
    while (lazy_mod_map_arr->len!=0) resolve_lazy_mod_map_element();
    path=module_map_path(mod_map,key);
  }
  if (path!=NULL && check_module_file(path,&mod->file_kind)) {
    mod->resolved_file=copy_string_to_region(file_scope_region_number,path);
    found=TRUE;
  }
  edg_string_destroy(&name);
  return found;
}  /* find_module_file_in_map */


static a_boolean find_header_unit_in_map(a_module_ptr mod)
/*
Find the module file associated with mod in the header unit map and update mod
with the path to the file.  Return TRUE if a module file was found, FALSE
otherwise.

This routine is a helper for find_module_file and assumes the module file has
not already been found - deferring diagnostics related to failing to find the
module file to the caller.
*/
{
  a_boolean     found = FALSE;
  a_const_char  *module_path;

  /* The module must be a header unit module to use this search function. */
  check_assertion(mod->kind == mk_header_unit);
  module_path = resolve_header_in_map(mod->variant.header_unit.name,
                                      mod->variant.header_unit.resolved_header,
                                      mod->variant.header_unit.is_sys_include);
  if (module_path != NULL) {
    if (check_module_file(module_path, &mod->file_kind)) {
      mod->resolved_file = copy_string_to_region(file_scope_region_number,
                                                 module_path);
      found = TRUE;
    }  /* if */
  }  /* if */
  return found;
}  /* find_header_unit_in_map */


static a_string module_file_name_of(a_module_ptr mod)
{
  check_assertion(mod->kind==mk_unit || mod->kind==mk_unit_partition);
  a_string result;
  edg_string_init(&result,EDG_ALLOC_GENERAL);
  edg_string_append_view(&result,module_name_of(mod));
  if (mod->kind==mk_unit_partition) {
    edg_string_append_cstr(&result,"-");
    edg_string_append_view(&result,module_partition_name_of(mod));
  }
  return result;
}  /* module_file_name_of */


static a_boolean find_module_file_in_dirs(a_module_ptr mod)
/*
Find the module file associated with mod in the module search paths and update
mod with the path to the file.  Return TRUE if a module file was found, FALSE
otherwise.

This routine is a helper for find_module_file and assumes the module file has
not already been found - deferring diagnostics related to failing to find the
module file to the caller.
*/
{
  a_boolean                  found = FALSE;
  a_directory_name_entry_ptr dir = module_search_path;
  a_string                   bare_file_name = module_file_name_of(mod);

  for (; !found && dir != NULL; dir = dir->next) {
    /* combine_dir_and_file_name clears the buffer for us. */
    (void)combine_dir_and_file_name(dir->dir_name,
                                    edg_string_cstr(&bare_file_name),
                                    module_search_buffer);
    /* Add an arbitrary extension to prevent replace_file_name_suffix from
       replacing part of the actual module name. */
    remove_null_terminator_from_text_buffer(module_search_buffer);
    add_to_text_buffer(module_search_buffer, ".ext", 5);
    for (size_t suffix_index=0;suffix_index<sizeof(module_file_suffixes)/sizeof(*module_file_suffixes);++suffix_index) {
      const struct a_module_file_suffix suffix=module_file_suffixes[suffix_index];
      replace_file_name_suffix(suffix.suffix, module_search_buffer);
      if (!file_exists(module_search_buffer->buffer)) {
        continue;
      }  /* if */
      if (check_module_file(module_search_buffer->buffer, &mod->file_kind)) {
        found = TRUE;
        mod->resolved_file = copy_string_to_region(
                                                 file_scope_region_number,
                                                 module_search_buffer->buffer);
        break;
      }  /* if */
    }  /* for */
  }  /* for */
  edg_string_destroy(&bare_file_name);
  return found;
}  /* find_module_file_in_dirs */


a_boolean find_module_file(a_module_ptr mod)
/*
Find the module file associated with mod and update mod with the path to the
file.  Return TRUE if a module file was found, FALSE otherwise.
*/
{
  a_boolean found = FALSE;

  if (mod->resolved_file != NULL && mod->file_kind != mfk_unknown) {
    /* Module file has already been found. */
    found = TRUE;
  }  /* if */
  if (found || skip_module_imports) {
    goto done;
  }  /* if */
  if (mod->kind == mk_header_unit) {
    a_boolean saved_windows_paths=windows_paths_allowed;
    a_boolean saved_backslash=backslash_is_also_dir_separator;
    windows_paths_allowed=TRUE;
    backslash_is_also_dir_separator=TRUE;
    a_const_char           *header_path;
    a_const_char           *header_name = mod->variant.header_unit.name;
    a_const_char           *name_for_search = header_name;

    if (ignore_absolute_paths_for_header_units &&
        is_absolute_file_name(name_for_search)) {
      name_for_search = get_base_name(name_for_search);
    }  /* if */
    header_path = resolve_header(name_for_search,
                                 mod->variant.header_unit.is_sys_include,
                                 /*is_include_next=*/FALSE, /*is_embed=*/FALSE,
                                 /*suppress_diagnostics=*/FALSE);
    if (header_path == NULL) {
      pos_catastrophe(ec_cannot_find_header_for_import, &error_position,
                      header_name);
    } else {
      mod->variant.header_unit.resolved_header = header_path;
      found = find_header_unit_in_map(mod);
    }  /* if */
    windows_paths_allowed=saved_windows_paths;
    backslash_is_also_dir_separator=saved_backslash;
  } else {
    found = find_module_file_in_map(mod);
    if (!found) {
      found = find_module_file_in_dirs(mod);
    }  /* if */

    a_string mod_name = module_full_name_of(mod);
    if (!found) {
      pos_catastrophe(ec_module_file_not_found, &error_position,
                      edg_string_cstr(&mod_name));
    } else if (!module_file_matches(edg_string_cstr(&mod_name),
                                    mod->resolved_file)) {
      pos_catastrophe(ec_module_file_mismatch, &error_position,
                      edg_string_cstr(&mod_name));
    }  /* if */
    edg_string_destroy(&mod_name);
  }  /* if */
done:
  return found;
}  /* find_module_file */


void import_module_file(a_module_import_decl_ptr midp)
/*
Import the module file specified in the module-import-declaration.
*/
{
  /* Ensure the modules system is fully loaded. */
  require_modules();
  check_assertion(midp->module_info->resolved_file != NULL);
  switch (midp->module_info->file_kind) {
    case mfk_unknown:
      /* This should never happen: the module file kind should be set if the
         resolved path is set. */
      unexpected_condition();
    case mfk_edg_ifc:
    case mfk_ms_ifc:
      /* Ensure the IFC modules system is fully loaded. */
      require_ifc_modules();
      /* Import the IFC module. */
      (void)import_ifc_module_file(midp);
      break;
    default_is_unexpected();
  }  /* switch */
}  /* import_module_file */


void define_names_from_scope(a_scope_ptr     scope,
                             a_symbol_header *sym_hdr)
/*
Called during name lookup when the name associated with sym_hdr has been
referred to in the indicated scope.  Scan through the list of deferred name
entries for this symbol header and process declarations for any that match the
scope.  The caller is responsible for ensuring there are deferred module
entities associated with the given symbol header.  Similarly, the caller is
responsible for making sure this function is not called after lexical
processing has ended.
*/
{
  a_deferred_module_entry_array
                *deferred_entries = sym_hdr->deferred_module_entries;

  /* This function should only be called if lexical processing is still
     enabled. */
  check_assertion(curr_lexical_state_stack_entry != NULL);
  /* This function should only be called if there are deferred module
     entities available. */
  check_assertion(deferred_entries != NULL);
#if DEBUG
  if (db_flag_is_set("ifc_symbols")) {
    a_string dbg_msg;
    edg_string_init(&dbg_msg,EDG_ALLOC_GENERAL);
    edg_string_append_cstr(&dbg_msg,"Symbol load started (depth = ");
    edg_string_append_u64(&dbg_msg,deferred_entries->num_active_scopes);
    edg_string_append_cstr(&dbg_msg,") for \"");
    edg_string_append_cstr(&dbg_msg,sym_hdr->identifier);
    edg_string_append_cstr(&dbg_msg,"\" in ");

    (void)fputs(edg_string_cstr(&dbg_msg),f_debug);
    edg_string_destroy(&dbg_msg);
    db_scope(scope);
    (void)fputs("\n", f_debug);
  }  /* if */
#endif /* DEBUG */
  ++deferred_entries->num_active_scopes;
  for (size_t i = 0; i < deferred_entries->entries.len; ++i) {
    size_t                  entry_idx = (deferred_entries->entries.len -
                                         (i + 1));
    a_deferred_module_entry *entry=edg_array_at(&deferred_entries->entries,entry_idx);
    if (entry->scope != scope) {
      continue;
    }  /* if */
#if DEBUG
    if (db_flag_is_set("ifc_symbols")) {
      (void)fprintf(f_debug, "Symbol load of \"%s\" in ", sym_hdr->identifier);
      db_scope(scope);
      (void)fprintf(f_debug, "\n");
    }  /* if */
#endif /* DEBUG */
    ++deferred_entries->num_processed;
    entry->scope = NULL;

    a_module_entity_ptr mep = locate_module_entity(entry->locator);
    if (mep->scope == NULL) {
      mep->scope = scope;
    }  /* if */
    (void)request_entity(mep);
  }  /* for */
#if DEBUG
  if (db_flag_is_set("ifc_symbols")) {
    a_string dbg_msg;
    edg_string_init(&dbg_msg,EDG_ALLOC_GENERAL);
    edg_string_append_cstr(&dbg_msg,"Symbol load finished (depth = ");
    edg_string_append_u64(&dbg_msg,deferred_entries->num_active_scopes);
    edg_string_append_cstr(&dbg_msg,") for \"");
    edg_string_append_cstr(&dbg_msg,sym_hdr->identifier);
    edg_string_append_cstr(&dbg_msg,"\" in ");

    (void)fputs(edg_string_cstr(&dbg_msg),f_debug);
    edg_string_destroy(&dbg_msg);
    db_scope(scope);
    (void)fputs("\n", f_debug);
  }  /* if */
#endif /* DEBUG */
  if (--deferred_entries->num_active_scopes == 0) {
    size_t total_num_entries = deferred_entries->entries.len;

    if (deferred_entries->num_processed == total_num_entries) {
      /* Free the entire list as it's now unused. */
#if DEBUG
      if (db_flag_is_set("ifc_symbols")) {
        a_string dbg_msg;
    edg_string_init(&dbg_msg,EDG_ALLOC_GENERAL);
    edg_string_append_cstr(&dbg_msg,"Symbol loading completed for \"");
    edg_string_append_cstr(&dbg_msg,sym_hdr->identifier);
    edg_string_append_cstr(&dbg_msg,"\"");

        (void)fprintf(f_debug,"%s\n",edg_string_cstr(&dbg_msg));
    edg_string_destroy(&dbg_msg);
      }  /* if */
#endif /* DEBUG */
      edg_array_destroy(&sym_hdr->deferred_module_entries->entries);
      free_fe(sym_hdr->deferred_module_entries,sizeof(*sym_hdr->deferred_module_entries));
      sym_hdr->deferred_module_entries=NULL;
    } else if (deferred_entries->num_processed > 0) {
      /* Clean up any entries that match the current scope. */
#if DEBUG
      if (db_flag_is_set("ifc_symbols")) {
        a_string dbg_msg;
    edg_string_init(&dbg_msg,EDG_ALLOC_GENERAL);
    edg_string_append_cstr(&dbg_msg,"Symbol loading partially completed for \"");
    edg_string_append_cstr(&dbg_msg,sym_hdr->identifier);
    edg_string_append_cstr(&dbg_msg,"\" (");
    edg_string_append_u64(&dbg_msg,deferred_entries->num_processed);
    edg_string_append_cstr(&dbg_msg," entries completed)");

        (void)fprintf(f_debug,"%s\n",edg_string_cstr(&dbg_msg));
    edg_string_destroy(&dbg_msg);
      }  /* if */
#endif /* DEBUG */
      for (size_t index=deferred_entries->entries.len;index>0;--index) {
        a_deferred_module_entry *entry=edg_array_at(&deferred_entries->entries,index-1);
        if (entry->scope==NULL) edg_array_remove(&deferred_entries->entries,index-1,1);
      }
      /* Since the counter num_active_scopes ensures this is the top level call
         to define_names_from_scope for this particular symbol header, it is
         safe to assume all entries that were considered processed have been
         removed. */
      check_assertion(((total_num_entries -
                        deferred_entries->entries.len) ==
                       deferred_entries->num_processed));
      deferred_entries->num_processed = 0;
    }  /* if */
  }  /* if */
}  /* define_names_from_scope */


/*
A lightweight representation of a scope used for module entity hashing.  All
instances of a module entity scope should be obtained through
get_module_entity_scope.  This class has external linkage but a hidden
definition to prevent misuse and (slightly) reduce the size of the modules.h
header.
*/
struct a_module_entity_scope {
  a_symbol_header_ptr
                name;   /* The symbol header that represents the name of this
                           module entity scope. */
  a_module_entity_scope
                *parent;
                        /* The parent scope of this scope. */



};  /* a_module_entity_scope */


static uintptr_t a_module_entity_scope_hash(a_module_entity_scope key)
/*
Return a hash value for the given module entity scope.
*/
{
  uintptr_t  result = edg_hash_pointer(&key.name);

  result = result*31 + edg_hash_pointer(&key.parent);
  return result;
}  /* hash_ptr */


static a_boolean a_module_entity_scope_equal(a_module_entity_scope a,a_module_entity_scope b)
/*
Return TRUE if the given module entity scopes are equal; otherwise, return
FALSE.
*/
{
  a_boolean result = TRUE;

  if (a.name != b.name) {
    result = FALSE;
  } else if (a.parent != b.parent) {
    result = FALSE;
  }  /* if */
  return result;
}  /* operator== */


static inline a_boolean a_module_entity_scope_not_equal(a_module_entity_scope a,a_module_entity_scope b)
/*
Return TRUE if the given module entity scopes are not equal; otherwise, return
FALSE.
*/
{
  return !a_module_entity_scope_equal(a,b);
}  /* operator!= */


void a_module_template_parameter_destroy(void *object)
{
  a_module_template_parameter *param=object;
  if (param->kind==mtpk_template) module_array_delete(&param->variant.params_list);
}
void a_module_template_parameter_list_init(a_module_template_parameter_list *list)
{ edg_array_init(list,sizeof(a_module_template_parameter),EDG_ALLOC_FE,0,NULL,a_module_template_parameter_destroy); }
void a_module_func_param_list_init(a_module_func_param_list *list)
{ edg_array_init(list,sizeof(a_type_ptr),EDG_ALLOC_FE,0,NULL,NULL); }

a_boolean a_module_template_parameter_equal(a_module_template_parameter a,a_module_template_parameter b)
/*
Return TRUE if the given module template parameters are equal; otherwise,
return FALSE.
*/
{
  a_boolean result = TRUE;

  if (a.kind != b.kind) {
    result = FALSE;
  } else {
    switch (a.kind) {
      case mtpk_non_type:
        { a_type_ptr type_a = a.variant.type;
          a_type_ptr type_b = b.variant.type;

          if (!il_identical_types(type_a, type_b)) {
            result = FALSE;
          }  /* if */
        }
        break;
      case mtpk_template:
        { a_module_template_parameter_list *params_a = a.variant.params_list;
          a_module_template_parameter_list *params_b = b.variant.params_list;

          if (params_a->len != params_b->len) {
            result = FALSE;
          } else {
            for (size_t i = 0; i < params_a->len; ++i) {
              a_module_template_parameter param_a=*(a_module_template_parameter *)edg_array_at(params_a,i);
              a_module_template_parameter param_b=*(a_module_template_parameter *)edg_array_at(params_b,i);

              if (!a_module_template_parameter_equal(param_a,param_b)) {
                result = FALSE;
              }  /* if */
            }  /* for */
          }  /* if */
        }
        break;
      case mtpk_type:
        /* Nothing to compare. */
        break;
      default_is_unexpected();
    }  /* switch */
  }  /* if */
  return result;
}  /* operator== */

/*
A key structure used for module entity hashing of functions.
*/
typedef struct a_module_entity_function_key a_module_entity_function_key;
struct a_module_entity_function_key {
  a_module_func_param_list *
                parameter_types;
                        /* The parameter types. */
  a_boolean     has_ellipsis;
                        /* TRUE if the given function key represents a function
                           with a C-style ellipsis argument. */
};  /* a_module_entity_function_key */

/*
A key structure used for module entity hashing of specializations.
*/
typedef struct a_module_entity_specialization_key a_module_entity_specialization_key;
struct a_module_entity_specialization_key {
  a_template_arg_ptr
                arguments;
                        /* The argument set this entity is specialized on. */
};  /* a_module_entity_specialization_key */

/*
A key structure used for module entity hashing of function templates.
*/
typedef struct a_module_entity_func_templ_key a_module_entity_func_templ_key;
struct a_module_entity_func_templ_key {
  a_module_entity_function_key
                function;
                        /* The function parameters for the function
                           template. */
  a_module_template_parameter_list *
                parameters;
                        /* The template parameters for the function
                           template. */
};  /* a_module_entity_func_templ_key */

/*
A key structure used for module entity hashing of function specializations.
*/
typedef struct a_module_entity_func_spec_key a_module_entity_func_spec_key;
struct a_module_entity_func_spec_key {
  a_module_entity_function_key
                function;
                        /* The function key portion of the function
                           specialization key. */
  a_module_entity_specialization_key
                specialization;
                        /* The specialization key portion of the function
                           specialization key. */
};  /* a_module_entity_func_spec_key */

/*
A key structure used for module entity hashing of deduction guides.
*/
typedef struct a_module_entity_deduct_guide_key a_module_entity_deduct_guide_key;
struct a_module_entity_deduct_guide_key {
  a_module_template_parameter_list *
                template_params;
                        /* The template parameters for the deduction guide. */
  a_module_deduct_guide_param_list *
                param_list;
                        /* The parameter types for the deduction guide. */
};  /* a_module_entity_deduct_guide_key */


typedef enum a_module_entity_extra_info_kind {
  meeik_none,           /* A basic module entity that doesn't need extra
                           information. */
  meeik_alias,          /* A module entity for an alias. */
  meeik_function,       /* A module entity for a function. */
  meeik_specialization, /* A module entity for a specialized entity. */
  meeik_func_templ,     /* A module entity for a function template. */
  meeik_func_spec,      /* A module entity for a function specialization. */
  meeik_deduct_guide    /* A module entity for a deduction guide. */
} a_module_entity_extra_info_kind;

/*
The primary key structure used for module entity hashing.  Non-common state is
dynamically allocated to reduce the size of the key structure (and by proxy,
the size of the module entity hash table -- see module_entity_hash_table).
*/
typedef struct a_module_entity_key a_module_entity_key;
struct a_module_entity_key {
  a_module      *mod;   /* The module this entity is owned by. */
  a_module_entity_scope
                *scope; /* The module entity scope this entity resides in. */
  a_symbol_header_ptr
                name;   /* The symbol header that represents the name of this
                           module entity. */
  a_module_entity_extra_info_kind
                kind;   /* The extra info kind. */
  union {
    /* When kind == meeik_none or meeik_alias, no variant fields. */
    /* When kind == meeik_function: */
    a_module_entity_function_key
                *function;
                        /* A pointer to extra information about the function's
                           identity. */
    /* When kind == meeik_specialization: */
    a_module_entity_specialization_key
                *specialization;
                        /* A pointer to extra information about the
                           specialization's identity. */
    /* When kind == meeik_func_templ: */
    a_module_entity_func_templ_key
                *func_templ;
                        /* A pointer to extra information about the function
                           template's identity. */
    /* When kind == meeik_func_spec: */
    a_module_entity_func_spec_key
                *func_spec;
                        /* A pointer to extra information about the function
                           specialization's identity. */
    /* When kind == meeik_deduct_guide: */
    a_module_entity_deduct_guide_key
                *deduct_guide;
                        /* A pointer to extra information about the deduction
                           guide's identity. */
  } variant;

};  /* a_module_entity_key */


static void module_key_destroy(void *object)
{
  a_module_entity_key *key=object;
  switch (key->kind) {
    case meeik_none: case meeik_alias: break;
    case meeik_function:
      module_array_delete(&key->variant.function->parameter_types);
      free_fe(key->variant.function,sizeof(*key->variant.function));break;
    case meeik_specialization:
      free_template_arg_list(key->variant.specialization->arguments);
      free_fe(key->variant.specialization,sizeof(*key->variant.specialization));break;
    case meeik_func_templ:
      module_array_delete(&key->variant.func_templ->function.parameter_types);
      module_array_delete(&key->variant.func_templ->parameters);
      free_fe(key->variant.func_templ,sizeof(*key->variant.func_templ));break;
    case meeik_func_spec:
      module_array_delete(&key->variant.func_spec->function.parameter_types);
      free_template_arg_list(key->variant.func_spec->specialization.arguments);
      free_fe(key->variant.func_spec,sizeof(*key->variant.func_spec));break;
    case meeik_deduct_guide:
      module_array_delete(&key->variant.deduct_guide->template_params);
      module_array_delete(&key->variant.deduct_guide->param_list);
      free_fe(key->variant.deduct_guide,sizeof(*key->variant.deduct_guide));break;
    default_is_unexpected();
  }
  key->kind=meeik_none;
}

static a_boolean a_module_entity_function_key_equal(a_module_entity_function_key a,a_module_entity_function_key b)
/*
Return TRUE if the given module entity function keys are equal; otherwise,
return FALSE.
*/
{
  a_boolean             result = TRUE;
  Edg_array *a_params = a.parameter_types;
  Edg_array *b_params = b.parameter_types;

  if (a_params->len != b_params->len) {
    result = FALSE;
  } else if (a.has_ellipsis != b.has_ellipsis) {
    result = FALSE;
  } else {
    for (size_t i = 0; i < a_params->len; ++i) {
      a_type_ptr a_param = *(a_type_ptr *)edg_array_at(a_params,i);
      a_type_ptr b_param = *(a_type_ptr *)edg_array_at(b_params,i);

      /* Compare on object identity as all equal module entity pointers
         have the same address. */
      if (!routine_types_are_redecl_compatible(a_param, b_param,
                                               TCF_NO_FLAGS)) {
        result = FALSE;
        break;
      }  /* if */
    }  /* for */
  }  /* if */
  return result;
}  /* operator== */


static a_boolean a_module_entity_specialization_key_equal(a_module_entity_specialization_key a,a_module_entity_specialization_key b)
/*
Return TRUE if the given module entity specialization keys are equal;
otherwise, return FALSE.
*/
{
  a_boolean          result = TRUE;
  a_template_arg_ptr a_args = a.arguments;
  a_template_arg_ptr b_args = b.arguments;
  size_t             a_n_args = edg_count_list_elements(a_args,offsetof(a_template_arg,next));
  size_t             b_n_args = edg_count_list_elements(b_args,offsetof(a_template_arg,next));

  if (a_n_args != b_n_args) {
    result = FALSE;
  } else if (a_n_args == 0) {
    /* This is an empty list compared to an empty list, this is equal. */
  } else {
    /* FIXME: Do we need to have eta_options_for_template (or some
       equivalent) factored in here? */
    an_equiv_templ_arg_options_set
                     eta_options = ETA_EXACT_DECLTYPE_EXPR_MATCH_REQUIRED;

    if (!equiv_template_arg_lists(a_args, b_args, eta_options)) {
      result = FALSE;
    }  /* if */
  }
  return result;
}  /* operator== */


static a_boolean a_module_entity_func_templ_key_equal(a_module_entity_func_templ_key a,a_module_entity_func_templ_key b)
/*
Return TRUE if the given module entity function template keys are equal;
otherwise, return FALSE.
*/
{
  a_boolean result = TRUE;

  if (!a_module_entity_function_key_equal(a.function,b.function)) {
    result = FALSE;
  } else {
    a_module_template_parameter_list *a_params = a.parameters;
    a_module_template_parameter_list *b_params = b.parameters;

    if (a_params->len != b_params->len) {
      result = FALSE;
    } else {
      for (size_t i = 0; i < a_params->len; ++i) {
        a_module_template_parameter a_param = *(a_module_template_parameter *)edg_array_at(a_params,i);
        a_module_template_parameter b_param = *(a_module_template_parameter *)edg_array_at(b_params,i);

        if (!a_module_template_parameter_equal(a_param,b_param)) {
          result = FALSE;
          break;
        }  /* if */
      }  /* for */
    }  /* if */
  }  /* if */
  return result;
}  /* operator== */


static a_boolean a_module_entity_func_spec_key_equal(a_module_entity_func_spec_key a,a_module_entity_func_spec_key b)
/*
Return TRUE if the given module entity function specialization keys are equal;
otherwise, return FALSE.
*/
{
  a_boolean result = TRUE;

  if (!a_module_entity_function_key_equal(a.function,b.function)) {
    result = FALSE;
  } else if (!a_module_entity_specialization_key_equal(a.specialization,b.specialization)) {
    result = FALSE;
  }  /* if */
  return result;
}  /* operator== */


static a_boolean a_module_entity_deduct_guide_key_equal(a_module_entity_deduct_guide_key a,a_module_entity_deduct_guide_key b)
/*
Return TRUE if the given module entity deduction guide keys are equal;
otherwise, return FALSE.
*/
{
  a_boolean result = TRUE;

  {
    a_module_template_parameter_list *a_params = a.template_params;
    a_module_template_parameter_list *b_params = b.template_params;

    if (a_params->len != b_params->len) {
      result = FALSE;
      goto done;
    } else {
      for (size_t i = 0; i < a_params->len; ++i) {
        a_module_template_parameter a_param = *(a_module_template_parameter *)edg_array_at(a_params,i);
        a_module_template_parameter b_param = *(a_module_template_parameter *)edg_array_at(b_params,i);

        if (!a_module_template_parameter_equal(a_param,b_param)) {
          result = FALSE;
          goto done;
        }  /* if */
      }  /* for */
    }  /* if */
  }
  {
    Edg_array *a_params = a.param_list;
    Edg_array *b_params = b.param_list;

    if (a_params->len != b_params->len) {
      result = FALSE;
      goto done;
    } else {
      for (size_t i = 0; i < a_params->len; ++i) {
        a_type_ptr a_param = *(a_type_ptr *)edg_array_at(a_params,i);
        a_type_ptr b_param = *(a_type_ptr *)edg_array_at(b_params,i);

        /* Compare on object identity as all equal module entity pointers
           have the same address. */
        if (!routine_types_are_redecl_compatible(a_param, b_param,
                                                 TCF_NO_FLAGS)) {
          result = FALSE;
          goto done;
        }  /* if */
      }  /* for */
    }  /* if */
  }
done:
  return result;
}  /* operator== */


static uintptr_t a_module_entity_key_hash(a_module_entity_key key)
/*
Return a hash value for the given module entity key.
*/
{
  uintptr_t  result = 0;

  if (key.mod != NULL) {
    a_module_ptr true_module = skip_module_partitions(key.mod);

    if (true_module->kind == mk_unit) {
      result = edg_hash_pointer(&true_module);
    }  /* if */
  }  /* if */
  result = result*31 + edg_hash_pointer(&key.scope);
  result = result*31 + edg_hash_pointer(&key.name);
  result += key.kind;
  switch (key.kind) {
    case meeik_none:
    case meeik_alias:
      /* No additional information to hash. */
      break;
    case meeik_function:
      /* FIXME: Implement this. */
      break;
    case meeik_specialization:
      check_assertion(key.variant.specialization != NULL);
      /* FIXME: This hash isn't sufficiently stable, the "same" template
         argument list hashes differently. */
      /* result += hash_template_arg_list(
                                     key.variant.specialization->arguments); */
      break;
    case meeik_func_templ:
      /* FIXME: Implement this. */
      break;
    case meeik_func_spec:
      /* FIXME: Implement this. */
      break;
    case meeik_deduct_guide:
      /* FIXME: Implement this. */
      break;
    default_is_unexpected();
  }  /* switch */
  return result;
}  /* hash_ptr */


static inline a_boolean is_same_module_or_global_module(
                                                  a_module_entity_key a,
                                                  a_module_entity_key b)
/*
Return TRUE if both of the given module entity keys reside in the same module
or both reside in the global module; otherwise, return FALSE.
*/
{
  /* The module here should already be stripped of any module partitions. */
  check_assertion(a.mod == skip_module_partitions(a.mod) &&
                  b.mod == skip_module_partitions(b.mod));
  /* If the same IL module is being used (or both are using no IL module and
     are thus part of the global module), these are definitely the same
     module. */
  return a.mod == b.mod;
}  /* is_same_module_or_global_module */


static a_boolean a_module_entity_key_equal(a_module_entity_key a,a_module_entity_key b)
/*
Return TRUE if the given module entity keys are equal; otherwise, return FALSE.
*/
{
  a_boolean result = TRUE;

  if (!is_same_module_or_global_module(a, b)) {
    result = FALSE;
  } else if (a.scope != b.scope) {
    result = FALSE;
  } else if (a.name != b.name) {
    result = FALSE;
  } else if (a.kind != b.kind) {
    result = FALSE;
  } else {
    switch (a.kind) {
      case meeik_none:
      case meeik_alias:
        /* No additional information to compare. */
        break;
      case meeik_function:
        if (!a_module_entity_function_key_equal(*a.variant.function,*b.variant.function)) {
          result = FALSE;
        }  /* if */
        break;
      case meeik_specialization:
        if (!a_module_entity_specialization_key_equal(*a.variant.specialization,*b.variant.specialization)) {
          result = FALSE;
        }  /* if */
        break;
      case meeik_func_templ:
        if (!a_module_entity_func_templ_key_equal(*a.variant.func_templ,*b.variant.func_templ)) {
          result = FALSE;
        }  /* if */
        break;
      case meeik_func_spec:
        if (!a_module_entity_func_spec_key_equal(*a.variant.func_spec,*b.variant.func_spec)) {
          result = FALSE;
        }  /* if */
        break;
      case meeik_deduct_guide:
        if (!a_module_entity_deduct_guide_key_equal(*a.variant.deduct_guide,*b.variant.deduct_guide)) {
          result = FALSE;
        }  /* if */
        break;
      default_is_unexpected();
    }  /* switch */
  }  /* if */
  return result;
}  /* operator== */


static inline a_boolean a_module_entity_key_not_equal(a_module_entity_key a,a_module_entity_key b)
/*
Return TRUE if the given module entity keys are not equal; otherwise, return
FALSE.
*/
{
  return !a_module_entity_key_equal(a,b);
}  /* operator!= */



typedef Edg_map a_module_entity_scope_hash_table;
                        /* The type used for the module entity scope hash
                           table. */

STATIC_THREAD a_module_entity_scope_hash_table
                *module_entity_scope_hash_table;
                        /* A hash table to find module entities, mapping module
                           entity scopes to a single (equivalent) instance.
                           The (module entity scope) values are not directly
                           represented to ensure they have a stable address
                           that does not change during reallocation of the
                           hash table. */

STATIC_THREAD a_module_entity_scope
                *trans_unit_module_entity_scope;
                        /* The translation unit module entity scope. */


a_module_entity_scope *get_module_entity_scope(a_symbol_header_ptr name,a_module_entity_scope *parent)
{
  if (name==NULL && parent==NULL) {
    if (trans_unit_module_entity_scope==NULL)
      trans_unit_module_entity_scope=module_alloc(sizeof(*trans_unit_module_entity_scope));
    return trans_unit_module_entity_scope;
  }
  a_module_entity_scope key={name,parent};
  uintptr_t hash=a_module_entity_scope_hash(key);
  a_module_entity_scope **slot=edg_map_get_with_hash(module_entity_scope_hash_table,&key,hash);
  if (slot!=NULL) return *slot;
  a_module_entity_scope *result=alloc_fe_of_type(a_module_entity_scope);
  *result=key;
  edg_map_set_with_hash(module_entity_scope_hash_table,&key,&result,hash);
  return result;
}  /* get_module_entity_scope */


typedef Edg_map a_module_entity_hash_table;
                        /* The type used for the module entity hash table. */

STATIC_THREAD a_module_entity_hash_table
                *module_entity_hash_table;
                        /* A hash table to find module entities, mapping module
                           entity keys to module entity pointer values.  The
                           (module entity) values are not directly represented
                           to ensure they have a stable address that does not
                           change during reallocation of the hash table. */


static a_module_entity_ptr get_module_entity_from_key(a_module_entity_key *key)
{
  uintptr_t hash=a_module_entity_key_hash(*key);
  a_module_entity_ptr *slot=edg_map_get_with_hash(module_entity_hash_table,key,hash);
  a_module_entity_ptr entity=slot==NULL?NULL:*slot;
  if (entity==NULL) {
    entity=a_module_entity_new(key->mod);
    entity->sym_header=key->name;
    edg_map_set_with_hash(module_entity_hash_table,key,&entity,hash);
    key->kind=meeik_none; /* The map now owns the extra key information. */
  }
  module_key_destroy(key);
  return entity;
}  /* get_module_entity_from_key */


a_module_entity_ptr get_module_entity(a_module_ptr          mod,
                                      a_module_entity_scope *scope,
                                      a_symbol_header_ptr   name)
/*
Return a pointer to the module entity for the entity in the given module, with
the given scope and name.
*/
{
  a_module_entity_key key={0};
  key.mod = mod;
  key.scope = scope;
  key.name = name;
  key.kind = meeik_none;

  a_module_entity_ptr result = get_module_entity_from_key(&key);
  return result;
}  /* get_module_entity */


a_module_entity_ptr get_alias_module_entity(a_module_ptr          mod,
                                            a_module_entity_scope *scope,
                                            a_symbol_header_ptr   name)
/*
Return a pointer to the module entity for the alias in the given module, with
the given scope and name.
*/
{
  a_module_entity_key key={0};
  key.mod = mod;
  key.scope = scope;
  key.name = name;
  key.kind = meeik_alias;

  a_module_entity_ptr result = get_module_entity_from_key(&key);
  return result;
}  /* get_alias_module_entity */


a_module_entity_ptr get_function_module_entity(
                            a_module_ptr                         mod,
                            a_module_entity_scope                *scope,
                            a_symbol_header_ptr                  name,
                            a_module_func_param_list *func_params,
                            a_boolean                            has_ellipsis)
/*
Return a pointer to the module entity for the entity in the given module, with
the given scope, name, and function parameters.  has_ellipsis should be TRUE if
the given module entity is a C-style variable argument function.
*/
{
  a_module_entity_key key={0};
  key.mod = mod;
  key.scope = scope;
  key.name = name;
  key.kind = meeik_function;
  key.variant.function = (a_module_entity_function_key *)module_alloc(sizeof(a_module_entity_function_key));
  key.variant.function->parameter_types = func_params;
  key.variant.function->has_ellipsis = has_ellipsis;

  a_module_entity_ptr result = get_module_entity_from_key(&key);
  return result;
}  /* get_function_module_entity */


a_module_entity_ptr get_specialized_module_entity(
                                    a_module_ptr               mod,
                                    a_module_entity_scope      *scope,
                                    a_symbol_header_ptr        name,
                                    a_template_arg_ptr template_args)
/*
Return a pointer to the module entity for the entity in the given module, with
the given scope, name, and template arguments.
*/
{
  a_module_entity_key key={0};
  key.mod = mod;
  key.scope = scope;
  key.name = name;
  key.kind = meeik_specialization;
  key.variant.specialization = (a_module_entity_specialization_key *)module_alloc(sizeof(a_module_entity_specialization_key));
  key.variant.specialization->arguments = template_args;

  a_module_entity_ptr result = get_module_entity_from_key(&key);
  return result;
}  /* get_specialized_module_entity */


a_module_entity_ptr get_function_template_module_entity(
                a_module_ptr                                 mod,
                a_module_entity_scope                        *scope,
                a_symbol_header_ptr                          name,
                a_module_template_parameter_list *template_params,
                a_module_func_param_list *func_params,
                a_boolean                                    has_ellipsis)
/*
Return a pointer to the module entity for the entity in the given module, with
the given scope, name, template parameters, and function parameters.
has_ellipsis should be TRUE if the given module entity is a C-style variable
argument function.
*/
{
  a_module_entity_key key={0};
  key.mod = mod;
  key.scope = scope;
  key.name = name;
  key.kind = meeik_func_templ;
  key.variant.func_templ = (a_module_entity_func_templ_key *)module_alloc(sizeof(a_module_entity_func_templ_key));
  key.variant.func_templ->function.parameter_types = func_params;
  key.variant.func_templ->function.has_ellipsis = has_ellipsis;
  key.variant.func_templ->parameters = template_params;

  a_module_entity_ptr result = get_module_entity_from_key(&key);
  return result;
}  /* get_function_template_module_entity */


a_module_entity_ptr get_specialized_function_module_entity(
                          a_module_ptr                         mod,
                          a_module_entity_scope                *scope,
                          a_symbol_header_ptr                  name,
                          a_template_arg_ptr           template_args,
                          a_module_func_param_list *func_params,
                          a_boolean                            has_ellipsis)
/*
Return a pointer to the module entity for the entity in the given module, with
the given scope, name, template arguments, and function parameters.
has_ellipsis should be TRUE if the given module entity is a C-style variable
argument function.
*/
{
  a_module_entity_key key={0};
  key.mod = mod;
  key.scope = scope;
  key.name = name;
  key.kind = meeik_func_spec;
  key.variant.func_spec = (a_module_entity_func_spec_key *)module_alloc(sizeof(a_module_entity_func_spec_key));
  key.variant.func_spec->function.parameter_types = func_params;
  key.variant.func_spec->function.has_ellipsis = has_ellipsis;
  key.variant.func_spec->specialization.arguments = template_args;

  a_module_entity_ptr result = get_module_entity_from_key(&key);
  return result;
}  /* get_specialized_function_module_entity */


a_module_entity_ptr get_deduction_guide_module_entity(
                a_module_ptr                                 mod,
                a_module_entity_scope                        *scope,
                a_symbol_header_ptr                          name,
                a_module_template_parameter_list *template_params,
                a_module_deduct_guide_param_list *param_list)
/*
Return a pointer to the deduction guide module entity for the entity in the
given module, with the given scope, name, template parameters, and deduction
guide parameter list.
*/
{
  a_module_entity_key key={0};
  key.mod = mod;
  key.scope = scope;
  key.name = name;
  key.kind = meeik_deduct_guide;
  key.variant.deduct_guide = (a_module_entity_deduct_guide_key *)module_alloc(sizeof(a_module_entity_deduct_guide_key));
  key.variant.deduct_guide->template_params = template_params;
  key.variant.deduct_guide->param_list = param_list;

  a_module_entity_ptr result = get_module_entity_from_key(&key);
  return result;
}  /* get_deduction_guide_module_entity */


void push_module_entity_state(a_module_entity_ptr mep)
/*
Push a new module entity to the module entity stack.  If mep is NULL, this
represents a return to the translation unit module or the global module.
*/
{
  a_module_entity_stack_entry mese={0};

  mese.mep = mep;
  mese.saved_error_position = error_position;
  edg_array_push(module_entity_stack,&mese);
  if (mep != NULL) {
    Opt_a_source_position opt_pos_info = source_position_of(mep);

    if (opt_pos_info.has_value) {
      error_position = opt_pos_info.value;
    } else {
      error_position = null_source_position;
    }  /* if */
  }  /* if */
}  /* push_module_entity_state */


void pop_module_entity_state()
/*
Pop the current module entity from the module entity stack.
*/
{
 a_module_entity_stack_entry *mese=edg_array_at(module_entity_stack,module_entity_stack->len-1);

 error_position=mese->saved_error_position;
 edg_array_remove(module_entity_stack,module_entity_stack->len-1,1);
}  /* pop_module_entity_state */


static a_boolean check_module_already_imported(a_module_import_decl_ptr midp)
/*
Check if the module import declaration imports a module that has already been
imported.  If so, return TRUE and update *midp to refer to the original import
declaration.  Otherwise, return FALSE and leave *midp unmodified.
*/
{
  a_boolean    already_imported = FALSE;
  a_module_ptr mod = midp->module_info;

  for (a_module_import_decl_ptr ptr = il_header.imported_modules;
       ptr != NULL; ptr = ptr->next) {
    a_module_ptr other_mod = ptr->module_info;

    if (other_mod->kind != mod->kind) {
      continue;
    }  /* if */

    /* For header units (which could possibly reference the same module file
       from multiple paths) ensure the same file was not found.  For named
       modules, simply ensure the same named module isn't resolved (as there is
       only one module object per module). */
    if (mod->kind == mk_header_unit) {
      a_boolean same_file = (mod->resolved_file != NULL &&
                             (strcmp(ptr->module_info->resolved_file,
                                     mod->resolved_file) == 0));
      if (!same_file) {
        continue;
      }  /* if */
    } else if (mod != other_mod) {
      continue;
    }  /* if */
    pos_remark(ec_module_already_imported, &midp->module_name_position, mod);
    *midp = *ptr;
    already_imported = TRUE;
    break;
  }  /* for */
  return already_imported;
}  /* check_module_already_imported */


void import_header_module(a_module_import_decl_ptr midp)
/*
Import the given header unit.
*/
{
  /* See if there's a known module file for the imported header and import
     that file if so. */
  if (find_module_file(midp->module_info)) {
    if (!check_module_already_imported(midp)) {
      import_module_file(midp);
      /* Add this to the list of imported modules regardless of whether the
         import was successful - future attempts to import the same module
         aren't likely to succeed if this one failed. */
      midp->next = il_header.imported_modules;
      il_header.imported_modules = midp;
    }  /* if */
  } else if (skip_module_imports) {
    /* For testing purposes, this import was skipped.  Issue a warning. */
    pos_warning(ec_import_skipped, &midp->module_name_position,
                midp->module_info->variant.header_unit.name);
  } else {
    /* The set of importable headers is implementation-defined.  Currently no
       headers are importable. */
    pos_catastrophe(ec_header_not_importable, &midp->module_name_position,
                    midp->module_info->variant.header_unit.name);
  }  /* if */
}  /* import_header_module */


void import_module_unchecked(a_module_import_decl_ptr midp)
/*
Import the given module without performing any checks that might inhibit the
import.  In most cases, import_module should be preferred.
*/
{
  if (find_module_file(midp->module_info)) {
    import_module_file(midp);
  }  /* if */
  /* Add this to the list of imported modules regardless of whether the
     import was successful - future attempts to import the same module
     aren't likely to succeed if this one failed. */
  midp->next = il_header.imported_modules;
  il_header.imported_modules = midp;
}  /* import_module_unchecked */


void import_curr_module()
/*
Import the module referred to by the current module unit.  This is called
immediately after parsing a module declaration where the module interface needs
to be implicitly imported.
*/
{
  a_module_import_decl_ptr midp;

  check_assertion(in_module_implementation_unit());
  midp = alloc_module_import_decl();
  midp->position = curr_module_sym->decl_position;
  midp->module_name_position = curr_module_sym->decl_position;
  midp->module_info = trans_unit_module;
  midp->impl_unit_importing_self = TRUE;
  import_module_unchecked(midp);
}  /* import_curr_module */


void import_module(a_module_import_decl_ptr midp,
                   a_symbol_ptr             assoc_sym)
/*
Import the given module.  assoc_sym is the associated symbol for the module.
*/
{
  a_boolean already_imported = check_module_already_imported(midp);

  /* See if this module has already been imported.  If so, ignore it. */
  if (!already_imported &&
      !check_module_has_interface_dependency(assoc_sym, curr_module_sym,
                                             &midp->module_name_position)) {
    import_module_unchecked(midp);
  }  /* if */
}  /* import_module */


a_boolean has_variable_initializer_from_module(a_variable_ptr vp)
/*
If the given variable has an initializer available in an imported module,
return TRUE.  Otherwise, return FALSE.
*/
{
  return (lazy_symbols_may_be_visible &&
          has_variable_initializer_from_ifc_module(vp));
}  /* has_variable_initializer_from_module */


a_boolean load_variable_initializer_from_module(a_variable_ptr vp)
/*
The given variable has an initializer available in an imported module (i.e.,
has_variable_initializer_from_module(vp) has returned TRUE); load and process
that initializer now and return TRUE. If an error occurs, return FALSE.
*/
{
  return load_variable_initializer_from_ifc_module(vp);
}  /* load_variable_initializer_from_module */


a_boolean has_routine_definition_from_module(a_routine_ptr rp)
/*
If the given routine has a definition available in an imported module, return
TRUE.  Otherwise, return FALSE.
*/
{
  return (lazy_symbols_may_be_visible &&
          has_routine_definition_from_ifc_module(rp));
}  /* has_routine_definition_from_module */


a_boolean load_routine_definition_from_module(a_routine_ptr rp)
/*
The given routine has a definition available in an imported module (i.e.,
has_routine_definition_from_module(rp) has returned TRUE); load and process
that definition now and return TRUE. If an error occurs, return FALSE.
*/
{
  return load_routine_definition_from_ifc_module(rp);
}  /* load_routine_definition_from_module */


a_boolean has_template_definition_from_module(a_template_ptr templ)
/*
If the given template has a definition available in an imported module, return
TRUE.  Otherwise, return FALSE.
*/
{
  return (lazy_symbols_may_be_visible &&
          has_template_definition_from_ifc_module(templ));
}  /* has_template_definition_from_module */


a_boolean has_pending_template_definition_from_module(a_template_ptr templ)
/*
If the given template is not already defined and has a definition available in
an imported module, return TRUE.  Otherwise, return FALSE.  Note that templ
must refer to the canonical template.
*/
{
  check_assertion(templ != NULL);
  return (!symbol_for(templ)->defined &&
          has_template_definition_from_module(templ));
}  /* has_pending_template_definition_from_module */


a_boolean load_template_definition_from_module(a_template_ptr templ)
/*
The given template has a definition available in an imported module (i.e.,
has_template_definition_from_module(templ) has returned TRUE); load and process
that definition now and return TRUE. If an error occurs, return FALSE.
*/
{
  return load_template_definition_from_ifc_module(templ);
}  /* load_template_definition_from_module */


a_boolean has_pending_template_specializations_from_module(
                                                          a_template_ptr templ)
/*
If the given template has specializations available in an imported module,
return TRUE.  Otherwise, return FALSE.  Note that templ must refer to the
canonical template.
*/
{
  check_assertion(templ != NULL);
  return (lazy_symbols_may_be_visible &&
          has_template_specializations_from_ifc_module(templ));
}  /* has_pending_template_specializations_from_module */


a_boolean load_template_specializations_from_module(a_template_ptr templ)
/*
The given template has one or more specializations available in an imported
module (i.e., has_pending_template_specializations_from_module(templ) has
returned TRUE); load and process those specializations now and return TRUE. If
an error occurs, return FALSE.
*/
{
  return load_template_specializations_from_ifc_module(templ);
}  /* load_template_specializations_from_module */


a_boolean has_type_definition_from_module(a_type_ptr ty)
/*
If the given type has a definition available in an imported module, return
TRUE.  Otherwise, return FALSE.
*/
{
  return (lazy_symbols_may_be_visible &&
          has_type_definition_from_ifc_module(ty));
}  /* has_type_definition_from_module */


a_boolean load_type_definition_from_module(a_type_ptr ty)
/*
The given type has a definition available in an imported module (i.e.,
has_type_definition_from_module(ty) has returned TRUE); load and process that
definition now and return TRUE. If an error occurs, return FALSE.
*/
{
  a_boolean  result = FALSE;

  if (!ty->definition_pending) {
    ty->definition_pending = TRUE;
    result = load_type_definition_from_ifc_module(ty);
    /* Once the class is defined, there is no need for this information. */
    ty->definition_pending = FALSE;
  }  /* if */
  return result;
}  /* load_type_definition_from_module */


a_module_entity_ptr locate_module_entity(a_module_entry_locator loc)
/*
Return the module entity referenced by the given module entry locator.
*/
{
  a_module_entity_ptr result = NULL;

  switch (loc.kind) {
    case melk_none:
      /* An unknown locator should never be passed to this function. */
      unexpected_condition();
    case melk_ifc:
      result = locate_ifc_module_entity(loc);
      break;
    default_is_unexpected();
  }  /* switch */
  /* The module entity should never be NULL.  Instead, an error module entity
     locator should be returned. */
  check_assertion(result != NULL);
  return result;
}  /* locate_module_entity */


a_boolean request_entity(a_module_entity_ptr mep)
/*
Request that the given module entity be processed (if not already being
processed).  If the entity's processing is complete, return TRUE; otherwise,
return FALSE.

This function should be preferred when immediate processing is not required
(i.e., the exact entity doesn't need to be known).  process_ifc_declaration
should be preferred if the entity should be processed immediately.
*/
{
  if (!is_entity_resolved(mep) && !is_entity_imminent(mep)) {
    switch (((a_module_entry_locator *)edg_array_at(&mep->locators,mep->primary_locator_idx))->kind) {
      case melk_none:
        /* This should never happen as a module entity with an uninitialized
           locator should not exist. */
        unexpected_condition();
        break;
      case melk_ifc:
        process_ifc_declaration(mep);
        break;
      default_is_unexpected();
    }  /* switch */
  }  /* if */
  return is_entity_resolved(mep);
}  /* request_entity */


void load_namespace_elements_from_locator(a_module_entity_ptr    mep,
                                          a_module_entry_locator loc)
/*
Load the namespace elements specified by the given module entry locator.
*/
{
  switch (loc.kind) {
    case melk_none:
      /* An unknown locator should never be passed to this function. */
      unexpected_condition();
    case melk_ifc:
      load_namespace_elements_from_ifc_locator(mep, loc);
      break;
    default_is_unexpected();
  }  /* switch */
}  /* load_namespace_elements_from_locator */


void mark_locator_as_primary(a_module_entity_ptr    mep,
                             a_module_entry_locator loc)
/*
Mark the given locator as the primary locator for the given module entity.
*/
{
  /* In most cases the locator being marked primary is the most recently
     added locator so traverse in reverse. */
  for (uint16_t k = (uint16_t)(mep->locators.len); k > 0; --k) {
    uint16_t               idx = k - 1;
    a_module_entry_locator idx_loc=*(a_module_entry_locator *)edg_array_at(&mep->locators,idx);

    if (module_entry_locator_equal(idx_loc,loc)) {
      mep->primary_locator_idx = idx;
      goto done;
    }  /* if */
  }  /* for */
  /* If this condition is reached, the locator was not found in the module
     entity's locators.  The locator should have been added by
     update_entity_from_new_locator. */
  unexpected_condition();
done:;
}  /* mark_locator_as_primary */


void update_entity_from_new_locator(a_module_entity_ptr    mep,
                                    a_module_entry_locator new_loc)
/*
A new locator has been discovered for the given module entity.  Add the locator
to the list of known locators and (if necessary) inform the corresponding
module implementation about the new information.
*/
{
  edg_array_push(&mep->locators,&new_loc);
#if DEBUG
  if (db_flag_is_set("module_entity_locator") && mep->locators.len > 1) {
    a_string dbg_msg;
    edg_string_init(&dbg_msg,EDG_ALLOC_GENERAL);
    edg_string_append_cstr(&dbg_msg,"Entity identified by multiple locators: ");
    a_boolean first = TRUE;

    for (size_t i=0;i<mep->locators.len;++i) {
      a_module_entry_locator loc=*(a_module_entry_locator *)edg_array_at(&mep->locators,i);
      if (!first) {
        edg_string_append_cstr(&dbg_msg,", ");
      }  /* if */
      first = FALSE;
      { a_string part=s_db_module_entry_locator(loc);edg_string_append_view(&dbg_msg,edg_string_as_view(&part));edg_string_destroy(&part); }
    }  /* for */
    (void)fprintf(f_debug,"%s\n",edg_string_cstr(&dbg_msg));
    edg_string_destroy(&dbg_msg);
  }  /* if */
#endif /* DEBUG */
  if (mep->entity.ptr != NULL) {
    /* Notify the appropriate module implementation. */
    switch (new_loc.kind) {
      case melk_none:
        /* An unknown locator should never be passed to this function. */
        unexpected_condition();
      case melk_ifc:
        update_entity_from_new_ifc_locator(mep, new_loc);
        break;
      default_is_unexpected();
    }  /* switch */
    if (mep->entity.kind == iek_namespace) {
      /* Load any new namespace elements. */
      load_namespace_elements_from_locator(mep, new_loc);
    }  /* if */
  }  /* if */
}  /* update_entity_from_new_locator */


Opt_a_source_position source_position_of(a_module_entity_ptr mep)
/*
Return the source position of the given module entity pointer if available;
otherwise, return an empty optional.
*/
{
  Opt_a_source_position result={0};
  a_module_entry_locator primary=*(a_module_entry_locator *)edg_array_at(&mep->locators,mep->primary_locator_idx);

  switch (primary.kind) {
    case melk_ifc:
      result = source_position_from_ifc_of(mep);
      break;
    case melk_none:
      unexpected_condition();
      break;
    default_is_unexpected();
  }  /* switch */
  return result;
}  /* source_position_of */


typedef Edg_map an_entity_to_token_cache_map;
                        /* The type for a map of tagged entities to an
                           associated token cache. */

STATIC_THREAD an_entity_to_token_cache_map
                *entity_to_definition_tokens;
                        /* A map of an entity to the token cache representing
                           its definition. */


STATIC_THREAD an_entity_to_token_cache_map
                *entity_to_initializer_tokens;
                        /* A map of an entity to the token cache representing
                           its initializer. */


void save_function_definition_for_module_write(
                                             a_routine_ptr              rp,
                                             a_shared_token_cache token)
/*
Save the tokens composing the definition of the given function for use writing
a module output file.
*/
{
  a_tagged_pointer key=make_tagged_ptr(rp);
  edg_map_set(entity_to_definition_tokens,&key,&token);
}  /* save_function_definition_for_module_write */


a_shared_token_cache get_function_definition_for_module_write(a_routine_ptr rp)
/*
Get the tokens composing the definition of the given function for use writing a
module output file.
*/
{
  a_tagged_pointer key=make_tagged_ptr(rp);
  a_shared_token_cache *slot=edg_map_get(entity_to_definition_tokens,&key);
  a_shared_token_cache cache=slot==NULL?NULL:edg_shared_cache_retain(*slot);

  /* This function should only be called for functions that are expected to
     have a definition during module write out. */
  check_assertion(create_module_unit &&
                  is_routine_definition_exported_inline(rp) &&
                  cache != NULL);
  return cache;
}  /* get_function_definition_for_module_write */


void save_field_initializer_for_module_write(a_field_ptr                fp,
                                             a_shared_token_cache token)
/*
Save the tokens composing the initializer of the given field for use writing a
module output file.
*/
{
  a_tagged_pointer key=make_tagged_ptr(fp);
  edg_map_set(entity_to_initializer_tokens,&key,&token);
}  /* save_field_initializer_for_module_write */


a_shared_token_cache get_field_initializer_for_module_write(a_field_ptr fp)
/*
Get the tokens composing the initializer of the given field for use writing a
module output file.
*/
{
  a_tagged_pointer key=make_tagged_ptr(fp);
  a_shared_token_cache *slot=edg_map_get(entity_to_initializer_tokens,&key);
  a_shared_token_cache cache=slot==NULL?NULL:edg_shared_cache_retain(*slot);

  /* This function should only be called for fields that are expected to
     have an initializer during module write out. */
  check_assertion(create_module_unit && cache != NULL);
  return cache;
}  /* get_field_initializer_for_module_write */


#if DEBUG

void db_mep_stack()
/*
Print information about the module entity stack.
*/
{
  for (size_t i = module_entity_stack->len; i > 0; i--) {
    a_module_entity_stack_entry *mese=edg_array_at(module_entity_stack,i-1);

    db_mep(mese->mep);
  }  /* for */
}  /* db_mep_stack */


void db_module_tokens(a_module_token_cache_ptr cache)
/*
This function proxies calls to the common db_tokens function when using
a module token cache pointer.
*/
{
  db_tokens(a_module_token_cache_as_canonical(cache));
}  /* db_tokens */


a_string s_db_module(a_module_ptr mod)
/*
Return a string containing debug information about the given module.
*/
{
  a_string result;
  edg_string_init(&result,EDG_ALLOC_GENERAL);
  edg_string_append_cstr(&result,"kind: ");
  a_module_kind m_kind = mk_none;

  if (mod != NULL) {
    m_kind = mod->kind;
  }  /* if */
  switch (m_kind) {
    case mk_header_unit:
      { a_boolean     is_sys_include = mod->variant.header_unit.is_sys_include;
        a_string_view header_name = header_unit_name_of(mod);

        edg_string_append_cstr(&result,"Header Unit, id: ");
        if (is_sys_include) {
          edg_string_append_cstr(&result,"<");
        } else {
          edg_string_append_cstr(&result,"\"");
        }  /* if */
        edg_string_append_view(&result,header_name);
        if (is_sys_include) {
          edg_string_append_cstr(&result,">");
        } else {
          edg_string_append_cstr(&result,"\"");
        }  /* if */
      }
      break;
    case mk_unit:
      { a_string mod_name = module_full_name_of(mod);

        edg_string_append_cstr(&result,"Module Unit, id: \""); edg_string_append_view(&result,edg_string_as_view(&mod_name)); edg_string_append_cstr(&result,"\"");
        edg_string_destroy(&mod_name);
      }
      break;
    case mk_unit_partition:
      { a_string mod_name = module_full_name_of(mod);

        edg_string_append_cstr(&result,"Module Unit (Partition), id: \""); edg_string_append_view(&result,edg_string_as_view(&mod_name)); edg_string_append_cstr(&result,"\"");
        edg_string_destroy(&mod_name);
      }
      break;
    default:
      edg_string_append_cstr(&result,"UNKNOWN");
      break;
  }  /* switch */
  edg_string_append_cstr(&result,", file: ");
  if (mod != NULL && mod->resolved_file != NULL) {
    edg_string_append_cstr(&result,mod->resolved_file);
    edg_string_append_cstr(&result,", file kind: ");
    switch (mod->file_kind) {
      case mfk_edg_ifc:
        edg_string_append_cstr(&result,"IFC (EDG)");
        break;
      case mfk_ms_ifc:
        edg_string_append_cstr(&result,"IFC (Microsoft)");
        break;
      default:
        edg_string_append_cstr(&result,"UNKNOWN");
        break;
    }  /* switch */
  } else {
    edg_string_append_cstr(&result,"<NULL>");
  }  /* if */
  return result;
}  /* s_db_module */


void db_module(a_module_ptr mod)
/*
Display debug information about the specified module.
*/
{
  a_string text=s_db_module(mod);
  (void)fprintf(f_debug,"%s\n",edg_string_cstr(&text));
  edg_string_destroy(&text);
}  /* db_module */


a_string s_db_module_entry_locator(a_module_entry_locator loc)
/*
Return a string containing a string representation of the given module entry
locator.
*/
{
  a_string result;
  edg_string_init(&result,EDG_ALLOC_GENERAL);

  switch (loc.kind) {
    case melk_none:
      edg_string_append_cstr(&result,"NONE");
      break;
    case melk_ifc:
      result = s_db_ifc_locator(loc);
      break;
    default_is_unexpected();
  }  /* switch */
  return result;
}  /* s_db_module_entry_locator */


void db_module_entry_locator(a_module_entry_locator loc)
/*
Display debug information about the specified module entry locator.
*/
{
  a_string text=s_db_module_entry_locator(loc);
  (void)fprintf(f_debug,"%s\n",edg_string_cstr(&text));
  edg_string_destroy(&text);
}  /* db_module_entry_locator */


a_string s_basic_db_mep(a_module_entity_ptr mep)
/*
Return a string containing debug information about the given module entity.
This string does not contain extensive information about the module.
*/
{
  a_string      result = s_db_module(mep->module_info);
  a_module_entry_locator_kind
                locator_kind = ((a_module_entry_locator *)edg_array_at(&mep->locators,mep->primary_locator_idx))->kind;

  edg_string_append_cstr(&result,", entity id: ");
  switch (locator_kind) {
    case melk_ifc:
      { a_string part=s_db_id_of_ifc_mep(mep);edg_string_append_view(&result,edg_string_as_view(&part));edg_string_destroy(&part); }
      break;
    default:
      edg_string_append_cstr(&result,"UNKNOWN");
      break;
  }  /* switch */
  edg_string_append_cstr(&result,", valid: "); edg_string_append_cstr(&result,(mep->invalid ? "FALSE" : "TRUE"));
  return result;
}  /* s_basic_db_mep */


void db_mep(a_module_entity_ptr mep)
/*
Display debug information about a module entity.
*/
{
  a_string text=s_basic_db_mep(mep);
  (void)fprintf(f_debug,"%s\n",edg_string_cstr(&text));
  edg_string_destroy(&text);
  if (mep->entity.kind != iek_none) {
    (void)fputs("source location: ", f_debug);
    db_scp((a_source_correspondence*)mep->entity.ptr);
  }
  if (mep->scope != NULL) {
    (void)fputs("scope: ", f_debug);
    db_scope(mep->scope);
    (void)fputs("\n", f_debug);
  }  /* if */
}  /* db_mep */

#endif /* DEBUG */

void modules_pch_reset()
/*
Called when a PCH file has just been read to re-open any module files that
had been opened at the time the PCH file was created.
*/
{
  ifc_modules_pch_read_reset();
}  /* modules_pch_reset */


void modules_check_for_suppressed_errors()
/*
Called after the translation unit has otherwise been processed to check
for suppressed errors while processing the module.
*/
{
  ifc_modules_report_suppressed_diagnostics();
}  /* modules_check_for_suppressed_errors */


void modules_early_init()
/*
One time initialization that must take place early on in the front end.
This is done before command line processing.
*/
{
#if DEBUG
  /* These are initialized per-translation unit.  However, in case of a command
     line error these must be reinitialized to prevent -d-module_report from
     reporting "declarations processed" by the previous front end
     invocation. */
  num_module_decls_attempted = 0;
  num_module_decls_failed = 0;
#endif /* DEBUG */
}  /* modules_early_init */


STATIC_THREAD a_boolean
        modules_initialized_for_curr_tu;
                /* TRUE if modules related variables have been fully
                   initialized for the current translation unit. */


void modules_one_time_init()
/*
Do one-time initialization of static variables defined in this file.
*/
{
  /* Save variables from ifc_modules.h and ifc_modules.c that are needed for
     precompiled headers */
  if (precompiled_header_processing_required) {
    STATIC_THREAD a_pch_saved_variable saved_vars[] = {
      pch_saved_var_array_elem(curr_module_sym),
      pch_saved_var_array_elem(lazy_symbols_may_be_visible),
      pch_saved_var_array_elem(module_entity_scope_hash_table),
      pch_saved_var_array_elem(trans_unit_module_entity_scope),
      pch_saved_var_array_elem(module_entity_hash_table),
      pch_saved_var_array_elem(entity_to_definition_tokens),
      pch_saved_var_array_elem(entity_to_initializer_tokens),
      pch_saved_var_array_elem(module_entity_stack),
      pch_saved_var_array_elem(known_modules),
      pch_saved_var_array_elem(modules_initialized_for_curr_tu),
      pch_saved_var_array_terminating_elem()
    };
    register_pch_saved_variables(saved_vars);
  }  /* if */
  module_search_buffer = alloc_text_buffer(256);
  ifc_modules_one_time_init();
  /* Register variables that have distinct copies for distinct translation
     units. */
#if DEBUG
  register_trans_unit_variable(num_module_decls_attempted);
  register_trans_unit_variable(num_module_decls_failed);
#endif /* DEBUG */
  register_trans_unit_variable(curr_module_sym);
  register_trans_unit_variable(lazy_symbols_may_be_visible);
  register_trans_unit_variable(module_entity_scope_hash_table);
  register_trans_unit_variable(trans_unit_module_entity_scope);
  register_trans_unit_variable(module_entity_hash_table);
  register_trans_unit_variable(entity_to_definition_tokens);
  register_trans_unit_variable(entity_to_initializer_tokens);
  register_trans_unit_variable(module_entity_stack);
  register_trans_unit_variable(known_modules);
  register_trans_unit_variable(modules_initialized_for_curr_tu);
}  /* modules_one_time_init */


static size_t module_scope_hash_callback(const void *key)
{ return a_module_entity_scope_hash(*(const a_module_entity_scope *)key); }
static a_boolean module_scope_equal_callback(const void *a,const void *b)
{ return a_module_entity_scope_equal(*(const a_module_entity_scope *)a,*(const a_module_entity_scope *)b); }
static size_t module_key_hash_callback(const void *key)
{ return a_module_entity_key_hash(*(const a_module_entity_key *)key); }
static a_boolean module_key_equal_callback(const void *a,const void *b)
{ return a_module_entity_key_equal(*(const a_module_entity_key *)a,*(const a_module_entity_key *)b); }
static size_t module_tag_hash_callback(const void *key)
{ const a_tagged_pointer *tag=key;return edg_hash_pointer(&tag->ptr)*31+tag->kind; }
static a_boolean module_tag_equal_callback(const void *a,const void *b)
{ const a_tagged_pointer *x=a,*y=b;return x->kind==y->kind && x->ptr==y->ptr; }
static void module_shared_cache_copy(void *dst,const void *src)
{ *(a_shared_token_cache *)dst=edg_shared_cache_retain(*(const a_shared_token_cache *)src); }
static void module_shared_cache_destroy(void *obj)
{ edg_shared_cache_release(*(a_shared_token_cache *)obj); }
static Edg_map *module_token_map_new(void)
{
  Edg_map *map=alloc_fe_of_type(Edg_map);
  edg_map_init(map,sizeof(a_tagged_pointer),sizeof(a_shared_token_cache),EDG_ALLOC_FE,10,
               module_tag_hash_callback,module_tag_equal_callback,NULL,NULL,
               module_shared_cache_copy,module_shared_cache_destroy);
  return map;
}

static void modules_trans_unit_delayed_init(void)
{
  module_entity_scope_hash_table=alloc_fe_of_type(a_module_entity_scope_hash_table);
  edg_map_init(module_entity_scope_hash_table,sizeof(a_module_entity_scope),sizeof(a_module_entity_scope *),EDG_ALLOC_FE,10,module_scope_hash_callback,module_scope_equal_callback,NULL,NULL,NULL,NULL);
  module_entity_hash_table=alloc_fe_of_type(a_module_entity_hash_table);
  edg_map_init(module_entity_hash_table,sizeof(a_module_entity_key),sizeof(a_module_entity_ptr),EDG_ALLOC_FE,10,module_key_hash_callback,module_key_equal_callback,NULL,module_key_destroy,NULL,NULL);
  known_modules=alloc_fe_of_type(a_module_name_map);
  edg_map_init(known_modules,sizeof(a_string_view),sizeof(a_module_ptr),EDG_ALLOC_FE,10,edg_string_view_hash,edg_string_view_equal,NULL,NULL,NULL,NULL);
}  /* modules_trans_unit_delayed_init */


void require_modules()
/*
Module use has been detected in the current translation unit.  Initialize the
corresponding front end structures (if not already initialized).
*/
{
  if (!modules_initialized_for_curr_tu) {
    modules_trans_unit_delayed_init();
    modules_initialized_for_curr_tu = TRUE;
  }  /* if */
}  /* require_modules */


void modules_trans_unit_init()
/*
Initialization of things related to modules that must be repeated for every
translation unit.
*/
{
#if DEBUG
  num_module_decls_attempted = 0;
  num_module_decls_failed = 0;
#endif /* DEBUG */
  curr_module_sym = NULL;
  lazy_symbols_may_be_visible = FALSE;
  module_entity_scope_hash_table = NULL;
  trans_unit_module_entity_scope = NULL;
  module_entity_hash_table = NULL;
  module_entity_stack=alloc_fe_of_type(a_module_entity_stack);
  edg_array_init(module_entity_stack,sizeof(a_module_entity_stack_entry),EDG_ALLOC_FE,0,NULL,NULL);
  known_modules = NULL;
  modules_initialized_for_curr_tu = FALSE;
  if (create_module_unit) {
    entity_to_definition_tokens = module_token_map_new();
    entity_to_initializer_tokens = module_token_map_new();
  } else {
    entity_to_definition_tokens = NULL;
    entity_to_initializer_tokens = NULL;
  }  /* if */
  ifc_modules_trans_unit_init();
}  /* modules_trans_unit_init */


void modules_trans_unit_wrapup_part_1()
/*
Perform the initial modules-related wrapup operations needed for the
translation unit.  This is called after all processing for the translation unit
(including template instantiations, etc.) has been done but before scopes have
been wrapped up.

This phase is primarily used to shutdown the lazy loading system and ensure no
additional entities are added to the IL.
*/
{
  lazy_symbols_may_be_visible = FALSE;
}  /* modules_trans_unit_wrapup_part_1 */


void modules_trans_unit_wrapup_part_2()
/*
Perform final modules-related wrapup operations needed for the translation
unit.  This is called after all processing for the translation unit (including
template instantiations, etc.) and scope wrapup have been done.

This phase is primarily used to deallocate objects allocated for modules
support in the current translation unit.
*/
{
  ifc_modules_trans_unit_wrapup();
  modules_container_cleanup();
}  /* modules_trans_unit_wrapup_part_2 */

#if MAKE_FRONT_END_CALLABLE

void modules_cleanup()
/*
This routine is called at the end of compilation, or if compilation is
terminated prematurely for some reason.  It performs any cleanup operations
required.  In particular, it closes any files that may have been open at
the point at which the compilation was terminated and makes sure any required
destructors are invoked.
*/
{
  ifc_modules_cleanup();
  modules_container_cleanup();
}  /* modules_cleanup */

#endif /* MAKE_FRONT_END_CALLABLE */

void modules_write_out()
/*
Write out the module files for the current translation unit.  This is called
after all processing for the translation unit (including template
instantiations, etc.) has been done.
*/
{
  a_boolean saved_in_front_end=in_front_end;
  in_front_end=FALSE;

  if (!is_at_least_one_error()) {
    /* Only emit the module declaration missing diagnostic if no other errors
       were emitted. */
    if (!created_module_unit_is_header && trans_unit_module == NULL) {
      error(ec_cannot_write_module_none_declared);
    }  /* if */
  }  /* if */
  if (!is_at_least_one_error()) {
    /* Only attempt module writing when there aren't any errors. */
    ifc_modules_write_out();
  }  /* if */
  in_front_end=saved_in_front_end;
}  /* modules_write_out */


/* Conditionally close the "edg" namespace. */

void a_module_token_cache_init(a_module_token_cache *cache,
                               const a_source_position *hint)
{
  edg_cache_init(&cache->underlying_cache,TRUE,0);
  cache->valid=TRUE;
  cache->position_hint=hint;
}

void a_module_token_cache_destroy(a_module_token_cache *cache)
{
#if DEBUG
  if (!cache->valid && db_flag_is_set("invalid_token_cache")) {
    (void)fprintf(f_debug,"Discarded invalid module cache:\n");
    db_tokens(&cache->underlying_cache);
  }
#endif
  edg_cache_destroy(&cache->underlying_cache);
}

a_boolean a_module_token_cache_is_empty(const a_module_token_cache *cache)
{
  return edg_cache_is_empty(&cache->underlying_cache);
}

void a_module_token_cache_append_cached_token(a_module_token_cache *cache,
                                             const a_cached_token *token)
{
  edg_cache_append_token_copy(&cache->underlying_cache,token);
}

void a_module_token_cache_append_token(a_module_token_cache *cache,
                                      a_shared_token token)
{
  edg_cache_append_token(&cache->underlying_cache,token);
}

a_shared_token a_module_token_cache_get_first_token(a_module_token_cache *cache)
{
  return edg_cache_get_first_token(&cache->underlying_cache);
}

a_shared_token a_module_token_cache_get_last_token(a_module_token_cache *cache)
{
  return edg_cache_get_last_token(&cache->underlying_cache);
}

a_token_cache_iterator a_module_token_cache_get_last_token_iter(
                                                    a_module_token_cache *cache)
{
  return edg_cache_get_last_token_iter(&cache->underlying_cache);
}

a_token_cache_iterator a_module_token_cache_begin(a_module_token_cache *cache)
{
  return edg_cache_begin(&cache->underlying_cache);
}

a_token_cache_iterator a_module_token_cache_end(a_module_token_cache *cache)
{
  return edg_cache_end(&cache->underlying_cache);
}

void a_module_token_cache_suggest_source_position(a_module_token_cache *cache,
                                                 a_source_position *hint)
{
  check_assertion(hint != NULL);
  if (a_module_token_cache_is_empty(cache)) {
    cache->position_hint=hint;
  } else {
    const a_source_position *last=edg_token_get_source_position(
                                      a_module_token_cache_get_last_token(cache));
    if (last->seq<hint->seq ||
        (last->seq==hint->seq && last->column<hint->column)) {
      cache->position_hint=hint;
    }
  }
}

const a_source_position *infer_next_source_position(a_module_token_cache *cache,
                                                    const a_source_position *pos)
{
  if (pos==NULL) pos=cache->position_hint;
  if (pos==NULL) {
    pos=a_module_token_cache_is_empty(cache)?&null_source_position:
         edg_token_get_source_position(a_module_token_cache_get_last_token(cache));
  }
  cache->position_hint=NULL;
  return pos;
}

a_token_sequence_number enter_module_token_rescan(a_module_token_cache *cache)
{
#if CHECKING
  check_assertion_str(!a_module_token_cache_is_empty(cache),
                       "the cache cannot be empty");
  check_assertion_str(!edg_token_is(a_module_token_cache_get_last_token(cache),
                                    tok_end_of_source),
                       "the cache cannot be pre-terminated.");
#endif
  terminate_token_cache(&cache->underlying_cache);
  a_shared_token_cache tokens=edg_shared_cache_move(&cache->underlying_cache);
  a_token_sequence_number end_tsn=edg_token_get_starting_seq_number(
                                            edg_cache_get_last_token(tokens));
  push_lexical_state_stack();
  push_stop_token_stack();
  rescan_shared_reusable_cache(tokens);
  edg_shared_cache_release(tokens);
  increment_dependent_scans_for_reusable_cache();
  return end_tsn;
}

void exit_module_token_rescan(a_const_char *original_start,
                              a_const_char *original_end,
                              ARG_UNUSED a_token_sequence_number end_tsn,
                              ARG_UNUSED a_token_kind final_token)
{
#if CHECKING
  if (final_token!=tok_error && curr_token!=final_token) expect_error();
#endif
  clear_stop_tokens();
  flush_to_end_of_source(TRUE);
  decrement_dependent_scans_for_reusable_cache();
  pop_stop_token_stack();
  pop_lexical_state_stack();
  check_assertion(curr_token==tok_end_of_source);
  check_assertion(curr_token_sequence_number==end_tsn ||
                  curr_token_sequence_number==NO_TOKEN_SEQUENCE_NUMBER);
  (void)get_token();
  start_of_curr_token=original_start;
  end_of_curr_token=original_end;
}

void a_module_entity_rescan_init(a_module_entity_rescan *state,
                                 a_module_token_cache *cache,
                                 a_token_kind *final_token)
{
  state->valid=cache->valid;
  state->final_token_ptr=final_token;
#if CHECKING
  state->expected_curr_source_line=curr_source_line;
#endif
  state->save_start_of_curr_token=start_of_curr_token;
  state->save_end_of_curr_token=end_of_curr_token;
  if (state->valid) {
#if CHECKING
    state->expected_end_tsn=
#endif
      enter_module_token_rescan(cache);
  } else {
#if CHECKING
    state->expected_end_tsn=NO_TOKEN_SEQUENCE_NUMBER;
#endif
  }
}

void a_module_entity_rescan_destroy(a_module_entity_rescan *state)
{
  if (state->valid) {
    a_token_sequence_number end_tsn=NO_TOKEN_SEQUENCE_NUMBER;
    a_token_kind final_token=state->final_token_ptr==NULL?tok_error:
                                                      *state->final_token_ptr;
#if CHECKING
    end_tsn=state->expected_end_tsn;
    check_assertion(state->expected_curr_source_line==curr_source_line);
#endif
    exit_module_token_rescan(state->save_start_of_curr_token,
                             state->save_end_of_curr_token,end_tsn,final_token);
    state->valid=FALSE;
  }
}

void modules_container_cleanup(void)
{
  module_map_delete(&entity_to_definition_tokens);
  module_map_delete(&entity_to_initializer_tokens);
  module_map_delete(&known_modules);
  module_array_delete(&module_entity_stack);
  module_map_delete(&module_entity_hash_table);
  if (trans_unit_module_entity_scope != NULL) {
    free_fe(trans_unit_module_entity_scope,sizeof(*trans_unit_module_entity_scope));
    trans_unit_module_entity_scope=NULL;
  }
  module_map_delete(&module_entity_scope_hash_table);
}
