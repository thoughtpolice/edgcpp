/*
Part of the EDG Compiler Project, under the Apache License v2.0 with LLVM
Exceptions.
See https://edgcpp.org/LICENSE.txt for license information.
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
*/

/*

modules.h -- Declarations related to module handling.

*/

/* Avoid including these declarations more than once: */
#ifndef MODULES_H
#define MODULES_H 1

/* Conditionally open the "edg" namespace. */

#if !STANDALONE_UTILITY_PROGRAM

/*
The following structure is associated with every entity in a module file that
has been loaded into the IL (or, if invalid is TRUE, failed to load into the
IL).

The module entity serves as a central point to accumulate module related
information about the module entity across different BMIs.
*/
typedef struct a_module_entity a_module_entity;
struct a_module_entity {
  a_module_ptr  module_info;
                        /* The module corresponding to the module-unit this
                           entity is in the purview of; if NULL, the entity is
                           in the purview of the global module. */
  a_symbol_header_ptr
                sym_header;
                        /* The symbol header this entity is named by (if
                           any); otherwise, NULL. */
  a_scope_ptr   scope;  /* The scope in which this entity is defined. */
  a_tagged_pointer
                entity; /* The IL entity that corresponds to the module
                           entity. */
  Edg_array
                locators;
                        /* The locations within BMIs where details about this
                           module entity can be recovered.  There is always at
                           least one locator for a module entity.  New locators
                           should be added via
                           update_entity_from_new_locator. */
  uint16_t      primary_locator_idx;
                        /* The index of the primary locator (i.e., the locator
                           with the most detailed information).  In most cases,
                           this corresponds to the locator that contains the
                           definition of the entity (if any). */
  a_bit_field   imminent:1;
                        /* This flag is typically set to TRUE when processing
                           to form or find the associated IL entity starts.
                           This is used to avoid unbounded recursion.

                           The flag is sometimes unset if an entity might need
                           to be reexamined (e.g., this is a namespace that
                           might be expanded by additional module imports).  In
                           that case, the num_locators_processed value is
                           typically used to track what work still needs to be
                           completed. */
  a_bit_field   def_imminent:1;
                        /* This flag is set when the definition of the
                           associated entity has started being loaded. */
  a_bit_field   uses_bound_token:1;
                        /* This flag is set when the IL entity is being
                           declared or defined via a reparse of a broader
                           cache.  This indicates less strict requirements may
                           be placed on recursive definition processing, as the
                           entity could have a severely "broken" token
                           representation or the entity may be invalid but not
                           yet marked as such (as the parse is still
                           ongoing). */
  a_bit_field   invalid:1;
                        /* TRUE if the associated entity cannot be constructed
                           from the module for any reason. */
  a_bit_field   global_module:1;
                        /* TRUE if this is an entity owned by the "global
                           module". */
  a_bit_field   non_exported:1;
                        /* TRUE if this is an entity that's reachable but not
                           visible (and thus was not exported from the imported
                           module). */
  a_module_entry_locator locator_buffer[1];
};  /* a_module_entity */
extern void a_module_entity_init(a_module_entity *,a_module_ptr);
extern a_module_entity *a_module_entity_new(a_module_ptr);
extern void a_module_entity_destroy(a_module_entity *);

#if DEBUG

EXTERN_THREAD unsigned long
                num_module_decls_attempted;
                        /* The number of declarations that process_ifc_decl
                           attempted to process. */

EXTERN_THREAD unsigned long
                num_module_decls_failed;
                        /* The number of declarations that process_ifc_decl
                           attempted to process, but marked invalid. */

#endif /* DEBUG */

static inline a_boolean is_module_entity_globally_visible(a_module_entity_ptr mep)
/*
Return TRUE if the given module entity is globally visible to lookup.  For the
symbol to be globally visible to lookup, it must either be part of the global
module or it must have been exported from a module (that has been imported).
Otherwise, return FALSE.

Note that this function does not account for class membership (as class members
do not currently have individual module entities).  To account for class
membership, prefer using is_symbol_globally_visible with the appropriate class
member symbol.
*/
{
  a_boolean result = FALSE;

  if (mep == NULL || mep->global_module) {
    /* This declaration belongs to the global module, so it's definitely
       lookup visible. */
    result = TRUE;
  } else if (!mep->non_exported) {
    /* This declaration was exported from an imported module. */
    result = TRUE;
  }  /* if */
  return result;
}  /* is_module_entity_globally_visible */

#endif /* !STANDALONE_UTILITY_PROGRAM */

static inline a_module_ptr skip_module_partitions(a_module_ptr mod)
/*
Return the module unit skipping over any module partition unit.
*/
{
  a_module_ptr result = mod;

  while (result != NULL && result->kind == mk_unit_partition) {
    result = result->variant.unit_partition.unit;
  }  /* while */
  return result;
}  /* skip_module_partitions */

#if !STANDALONE_UTILITY_PROGRAM

static inline a_module *lookup_module_for_mep(a_module_entity_ptr mep)
/*
Return a pointer to the module used for lookup of the given module entity
pointer.  Return NULL if there isn't one.
*/
{
  a_module_ptr result;

  if (is_module_entity_globally_visible(mep)) {
    result = NULL;
  } else {
    result = mep->module_info;
  }  /* if */
  return result;
}  /* lookup_module_for_mep */


EXTERN_THREAD a_symbol_ptr
                curr_module_sym;
                        /* If in a module unit, the current module symbol. */

/*
Entry used to maintain a stack of module contexts.  The top of the stack
is the "current" module for declarations.
*/
typedef struct a_module_entity_stack_entry *a_module_entity_stack_entry_ptr;
struct a_module_entity_stack_entry {

  a_module_entity_ptr
                mep;    /* The current module entity pointer.  If mep is NULL,
                           this represents a return to the translation unit
                           module or the global module. */
  a_source_position
                saved_error_position;
                        /* The value of error_position prior to this module
                           entity being pushed to the stack. */
};  /* a_module_entity_stack_entry */

typedef Edg_array a_module_entity_stack;
typedef struct a_module_entity_stack_entry a_module_entity_stack_entry;
static inline void a_module_entity_stack_entry_invalidate(a_module_entity_stack_entry *entry)
{ check_assertion(entry->mep != NULL);entry->mep->invalid=TRUE; }

EXTERN_THREAD a_module_entity_stack
                *module_entity_stack;
                        /* A dynamic array of the active module entities. */

typedef ptrdiff_t a_module_entity_depth;
                        /* The type used to represent the module entity scope
                           depth. */

static inline a_module_entity_depth module_entity_stack_depth()
/*
Return the depth of the module entity stack.
*/
{
  a_module_entity_depth result = NO_SCOPE_DEPTH;

  if (module_entity_stack != NULL && module_entity_stack->len != 0) {
    result = (a_module_entity_depth)(module_entity_stack->len - 1);
  }  /* if */
  return result;
}  /* module_entity_stack_depth */


extern void push_module_entity_state(a_module_entity_ptr mep);

extern void pop_module_entity_state();


/*
A class used to represent an element on the module entity state stack.  This is
an RAII object that automatically manages the calls to
push/pop_module_entity_state for the module entity given during construction.
*/
typedef struct a_module_entity_stack_state { a_boolean active; } a_module_entity_stack_state;
static inline void a_module_entity_stack_state_init(a_module_entity_stack_state *state,a_module_entity_ptr mep)
{ push_module_entity_state(mep);state->active=TRUE; }
static inline void a_module_entity_stack_state_destroy(a_module_entity_stack_state *state)
{ if (state->active) { pop_module_entity_state();state->active=FALSE; } }



static inline a_module_entity_ptr curr_module_entity()
/*
Return a pointer to the module entity at the top of the module entity stack, or
NULL if there is no current module entity.
*/
{
  a_module_entity_ptr mep = NULL;

  if (module_entity_stack != NULL && module_entity_stack->len > 0) {
    a_module_entity_stack_entry_ptr mesep = (a_module_entity_stack_entry *)edg_array_at(module_entity_stack,module_entity_stack->len-1);

    mep = mesep->mep;
  }  /* if */
  return mep;
}  /* curr_module_entity */


static inline a_module_ptr curr_lookup_module()
/*
Return the module that should be used for additional lookup (outside of
the global module and any exported entities from imported modules).
*/
{
  a_module_ptr        result = NULL;
  a_module_entity_ptr mep = curr_module_entity();

  if (mep != NULL && mep->module_info != NULL &&
      mep->module_info->kind != mk_header_unit) {
    result = mep->module_info;
  } else if (trans_unit_module != NULL) {
    result = trans_unit_module;
  }   /* if */
  return result;
}  /* curr_lookup_module */


typedef struct a_module_lookup_array { a_module_ptr data[2];size_t len; } a_module_lookup_array;
                        /* An array used to represent the array of modules
                           to be used for lookup. */


static inline a_module_lookup_array curr_lookup_modules()
/*
Return all modules that can be pulled from during lookup.  A NULL value
represents the global module and any exported entities from imported modules.
*/
{
  a_module_lookup_array result={{NULL,NULL},0};
  a_module_ptr          curr_module = curr_lookup_module();

  result.data[result.len++]=NULL;
  if (curr_module != NULL) {
    result.data[result.len++]=curr_module;
  }  /* if */
  return result;
}  /* curr_lookup_modules */


EXTERN_THREAD a_boolean
                lazy_symbols_may_be_visible;
                        /* TRUE if symbols (and their definitions) may be
                           "lazily loaded" (i.e., because at least one module
                           has been imported). */

static inline a_boolean magic_numbers_match(const a_byte magic[4],
                                     const a_byte expected[4])
/*
Return true if the provided magic numbers match their expected magic numbers.
*/
{
  return magic[0] == expected[0] &&
         magic[1] == expected[1] &&
         magic[2] == expected[2] &&
         magic[3] == expected[3];
}  /* magic_numbers_match */

extern a_boolean check_module_has_interface_dependency(
                                           a_symbol_ptr          module_sym,
                                           a_symbol_ptr          interface_sym,
                                           a_source_position_ptr module_pos);

extern a_module_ptr find_or_create_module(a_symbol_ptr module_sym);

extern a_boolean find_module_file(a_module_ptr mod);

extern void import_module_file(a_module_import_decl_ptr midp);

extern void define_names_from_scope(a_scope_ptr     scope,
                                    a_symbol_header *sym_hdr);

typedef struct a_module_entity_scope a_module_entity_scope;
struct a_module_template_parameter;

typedef enum a_module_template_parameter_kind {
  mtpk_non_type, /* A non-type template parameter. */
  mtpk_template, /* A template template parameter. */
  mtpk_type      /* A type template parameter. */
} a_module_template_parameter_kind;

typedef Edg_array a_module_template_parameter_list;
                        /* A list of template parameters for use by module
                           entity hashing. */

/*
A lightweight representation of a template parameter used for module entity
hashing.
*/
typedef struct a_module_template_parameter a_module_template_parameter;
struct a_module_template_parameter {
  a_module_template_parameter_kind
                kind;   /* The kind of template parameter being represented. */
  union {
    /* When kind == mtpk_type, no variant fields. */
    /* When kind == mtpk_non_type: */
    a_type_ptr  type;   /* The type of the non-type template parameter. */
    /* When kind == mtpk_template: */
    a_module_template_parameter_list
                *params_list;
                        /* The inner template parameter list of the template
                           template parameter. */
  } variant;

};  /* a_module_template_parameter */

extern a_boolean a_module_template_parameter_equal(a_module_template_parameter a,a_module_template_parameter b);
extern void a_module_template_parameter_destroy(void *);
extern void a_module_template_parameter_list_init(a_module_template_parameter_list *);

typedef Edg_array a_module_func_param_list;
extern void a_module_func_param_list_init(a_module_func_param_list *);
                        /* A list of function parameter types for use by module
                           entity hashing. */

typedef Edg_array a_module_deduct_guide_param_list;
                        /* A list of deduction guide parameter types for use by
                           module entity hashing. */

extern a_module_entity_scope* get_module_entity_scope(
                                                a_symbol_header_ptr   name,
                                                a_module_entity_scope *parent);

extern a_module_entity_ptr get_module_entity(a_module_ptr          mod,
                                             a_module_entity_scope *scope,
                                             a_symbol_header_ptr   name);

extern a_module_entity_ptr get_alias_module_entity(
                                                  a_module_ptr          mod,
                                                  a_module_entity_scope *scope,
                                                  a_symbol_header_ptr   name);

extern a_module_entity_ptr get_function_module_entity(
                            a_module_ptr                         mod,
                            a_module_entity_scope                *scope,
                            a_symbol_header_ptr                  name,
                            a_module_func_param_list *func_params,
                            a_boolean                            has_ellipsis);

extern a_module_entity_ptr get_specialized_module_entity(
                                   a_module_ptr               mod,
                                   a_module_entity_scope      *scope,
                                   a_symbol_header_ptr        name,
                                   a_template_arg_ptr template_args);

extern a_module_entity_ptr get_function_template_module_entity(
                a_module_ptr                                 mod,
                a_module_entity_scope                        *scope,
                a_symbol_header_ptr                          name,
                a_module_template_parameter_list *template_params,
                a_module_func_param_list *func_params,
                a_boolean                                    has_ellipsis);

extern a_module_entity_ptr get_specialized_function_module_entity(
                          a_module_ptr                         mod,
                          a_module_entity_scope                *scope,
                          a_symbol_header_ptr                  name,
                          a_template_arg_ptr template_args,
                          a_module_func_param_list *func_params,
                          a_boolean                            has_ellipsis);

extern a_module_entity_ptr get_deduction_guide_module_entity(
                a_module_ptr                                 mod,
                a_module_entity_scope                        *scope,
                a_symbol_header_ptr                          name,
                a_module_template_parameter_list *template_params,
                a_module_deduct_guide_param_list *param_list);

extern void import_header_module(a_module_import_decl_ptr midp);

extern void import_curr_module();

extern void import_module(a_module_import_decl_ptr midp,
                          a_symbol_ptr             assoc_sym);

extern a_boolean has_variable_initializer_from_module(a_variable_ptr  vp);

extern a_boolean load_variable_initializer_from_module(a_variable_ptr  vp);

extern a_boolean has_routine_definition_from_module(a_routine_ptr  rp);

extern a_boolean load_routine_definition_from_module(a_routine_ptr  rp);

extern a_boolean has_template_definition_from_module(a_template_ptr  templ);

extern a_boolean has_pending_template_definition_from_module(
                                                        a_template_ptr  templ);

extern a_boolean load_template_definition_from_module(a_template_ptr  templ);

extern a_boolean has_pending_template_specializations_from_module(
                                                        a_template_ptr  templ);

extern a_boolean load_template_specializations_from_module(
                                                        a_template_ptr  templ);

extern a_boolean has_type_definition_from_module(a_type_ptr  ty);

extern a_boolean load_type_definition_from_module(a_type_ptr ty);


static inline a_boolean is_entity_resolved(a_module_entity_ptr mep)
/*
Given a module entity pointer, return TRUE if the entity is in a resolved state
(i.e., the entity is either resolved to an IL entity or marked invalid);
otherwise, return FALSE.
*/
{
  return mep->imminent && (mep->entity.ptr != NULL || mep->invalid);
}  /* is_entity_resolved */


static inline a_boolean is_entity_imminent(a_module_entity_ptr mep)
/*
Given a module entity pointer, return TRUE if the entity is an unresolved
imminent state (i.e., the entity is not yet resolved to an IL entity or marked
invalid); otherwise, return FALSE.
*/
{
  return mep->imminent && !mep->invalid && mep->entity.ptr == NULL;
}  /* is_entity_imminent */


extern a_module_entity_ptr locate_module_entity(a_module_entry_locator loc);

extern a_boolean request_entity(a_module_entity_ptr mep);

extern void load_namespace_elements_from_locator(a_module_entity_ptr    mep,
                                                 a_module_entry_locator loc);

extern void mark_locator_as_primary(a_module_entity_ptr    mep,
                                    a_module_entry_locator loc);

extern void update_entity_from_new_locator(a_module_entity_ptr    mep,
                                           a_module_entry_locator new_loc);

#ifndef Opt_a_source_position_IFC_DECLARED
#define Opt_a_source_position_IFC_DECLARED 1
typedef struct Opt_a_source_position { a_boolean has_value;a_source_position value; } Opt_a_source_position;
#endif
extern Opt_a_source_position source_position_of(a_module_entity_ptr mep);

/*
An internal token cache wrapper structure that represents additional state for
modules.
*/
typedef struct a_module_token_cache {
  a_token_cache underlying_cache;
  a_boolean valid;
  const a_source_position *position_hint;
} a_module_token_cache;
typedef a_module_token_cache *a_module_token_cache_ptr;
extern void a_module_token_cache_init(a_module_token_cache *,const a_source_position *);
extern void a_module_token_cache_destroy(a_module_token_cache *);
static inline a_boolean a_module_token_cache_is_valid(const a_module_token_cache *cache) { return cache->valid; }
static inline void a_module_token_cache_invalidate(a_module_token_cache *cache) { cache->valid=FALSE; }
static inline a_token_cache *a_module_token_cache_as_canonical(a_module_token_cache *cache) { return &cache->underlying_cache; }
static inline const a_source_position *a_module_token_cache_get_position_hint(const a_module_token_cache *cache) { return cache->position_hint; }
static inline void a_module_token_cache_set_position_hint(a_module_token_cache *cache,const a_source_position *pos) { cache->position_hint=pos; }
extern a_boolean a_module_token_cache_is_empty(const a_module_token_cache *);
extern void a_module_token_cache_append_cached_token(a_module_token_cache *,const a_cached_token *);
extern void a_module_token_cache_append_token(a_module_token_cache *,a_shared_token);
extern a_shared_token a_module_token_cache_get_first_token(a_module_token_cache *);
extern a_shared_token a_module_token_cache_get_last_token(a_module_token_cache *);
extern a_token_cache_iterator a_module_token_cache_get_last_token_iter(a_module_token_cache *);
extern a_token_cache_iterator a_module_token_cache_begin(a_module_token_cache *);
extern a_token_cache_iterator a_module_token_cache_end(a_module_token_cache *);
extern void a_module_token_cache_suggest_source_position(a_module_token_cache *,a_source_position *);
extern const a_source_position *infer_next_source_position(a_module_token_cache *,const a_source_position *);
extern a_token_sequence_number enter_module_token_rescan(a_module_token_cache *);
extern void exit_module_token_rescan(a_const_char *,a_const_char *,a_token_sequence_number,a_token_kind);
typedef struct a_module_entity_rescan {
  a_boolean valid;
  a_token_kind *final_token_ptr;
#if CHECKING
  a_const_char *expected_curr_source_line;
  a_token_sequence_number expected_end_tsn;
#endif
  a_const_char *save_start_of_curr_token,*save_end_of_curr_token;
} a_module_entity_rescan;
extern void a_module_entity_rescan_init(a_module_entity_rescan *,a_module_token_cache *,a_token_kind *);
extern void a_module_entity_rescan_destroy(a_module_entity_rescan *);

extern void save_function_definition_for_module_write(
                                            a_routine_ptr              rp,
                                            a_shared_token_cache token);

extern a_shared_token_cache get_function_definition_for_module_write(
                                                             a_routine_ptr rp);

extern void save_field_initializer_for_module_write(
                                            a_field_ptr                fp,
                                            a_shared_token_cache token);

extern a_shared_token_cache get_field_initializer_for_module_write(
                                                               a_field_ptr fp);

#if DEBUG

extern void db_module_tokens(a_module_token_cache_ptr cache);

extern a_string s_db_module(a_module_ptr mod);

extern void db_module(a_module_ptr mod);

extern a_string s_db_module_entry_locator(a_module_entry_locator loc);

extern void db_module_entry_locator(a_module_entry_locator loc);

extern a_string s_basic_db_mep(a_module_entity_ptr mep);

extern void db_mep(a_module_entity_ptr mep);

extern void push_module_context(a_module_ptr mod_ptr);

extern void pop_module_context(void);

extern void db_module_entity(a_module_entity_ptr mep);

extern void db_module_stack();

extern void db_mep_stack();

#endif /* DEBUG */

extern void modules_pch_reset();

extern void modules_check_for_suppressed_errors();

extern void modules_early_init();

extern void modules_one_time_init();

extern void require_modules();

extern void modules_trans_unit_init();

extern void modules_trans_unit_wrapup_part_1();

extern void modules_trans_unit_wrapup_part_2();

extern void modules_container_cleanup(void);

#if MAKE_FRONT_END_CALLABLE
extern void modules_cleanup();
#endif /* MAKE_FRONT_END_CALLABLE */

extern void modules_write_out();

#endif /* !STANDALONE_UTILITY_PROGRAM */

/* Conditionally close the "edg" namespace. */

#endif /* ifndef MODULES_H */

