/*
Part of the EDG Compiler Project, under the Apache License v2.0 with LLVM
Exceptions.
See https://edgcpp.org/LICENSE.txt for license information.
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
*/

/*

ifc_modules.h -- Declarations relating to IFC modules made available to the
                 rest of the front end.  For declarations related only to the
                 implementation of IFC modules itself see
                 ifc_modules_internal.h.

*/

/* Avoid including these declarations more than once: */
#ifndef IFC_MODULES_H
#define IFC_MODULES_H 1

#if !STANDALONE_UTILITY_PROGRAM

#include "ifc_map.h"

#include "util.h"

/* Conditionally open the "edg" namespace. */

typedef struct a_tmpl_decl_state *a_tmpl_decl_state_ptr;

/* FIXME: Temporarily disable "not referenced" warnings until completed. */
/*lint -save -e755 -e758 -e768 -e769*/

typedef uint64_t a_module_ref_key;

/*
Magic numbers that identify the beginning of a Microsoft IFC file.
*/
static const a_byte ms_ifc_magic_numbers[] = { 0x54, 0x51, 0x45, 0x1A };

/*
Magic numbers that identify the beginning of an EDG IFC file.
*/
static const a_byte edg_ifc_magic_numbers[] = { 0x54, 0x51, 0x45, 0x2C };

#ifndef Opt_a_string_IFC_DECLARED
#define Opt_a_string_IFC_DECLARED 1
typedef struct Opt_a_string { a_boolean has_value;a_string value; } Opt_a_string;
#endif
#ifndef Opt_a_source_position_IFC_DECLARED
#define Opt_a_source_position_IFC_DECLARED 1
typedef struct Opt_a_source_position { a_boolean has_value;a_source_position value; } Opt_a_source_position;
#endif
extern an_ifc_decl_index from_lexical_index_an_ifc_decl_index(a_lexical_ifc_index_reference);
extern a_lexical_ifc_index_reference to_lexical_index_an_ifc_decl_index(an_ifc_decl_index);
extern an_ifc_expr_index from_lexical_index_an_ifc_expr_index(a_lexical_ifc_index_reference);
extern a_lexical_ifc_index_reference to_lexical_index_an_ifc_expr_index(an_ifc_expr_index);
extern an_ifc_type_index from_lexical_index_an_ifc_type_index(a_lexical_ifc_index_reference);
extern a_lexical_ifc_index_reference to_lexical_index_an_ifc_type_index(an_ifc_type_index);
#define to_lexical_index(idx) _Generic((idx), an_ifc_decl_index:to_lexical_index_an_ifc_decl_index, an_ifc_expr_index:to_lexical_index_an_ifc_expr_index, an_ifc_type_index:to_lexical_index_an_ifc_type_index)(idx)

extern Opt_a_string get_name_of_ifc_module(a_const_char *file_name);

extern void process_ifc_declaration(a_module_entity_ptr mep);

extern a_boolean has_variable_initializer_from_ifc_module(a_variable_ptr  vp);

extern a_boolean load_variable_initializer_from_ifc_module(a_variable_ptr  vp);

extern a_boolean has_routine_definition_from_ifc_module(a_routine_ptr  rp);

extern a_boolean load_routine_definition_from_ifc_module(a_routine_ptr  rp);

extern a_boolean has_template_definition_from_ifc_module(a_template_ptr templ);

extern
a_boolean load_template_definition_from_ifc_module(a_template_ptr  templ);

extern a_boolean has_template_specializations_from_ifc_module(
                                                        a_template_ptr  templ);

extern
a_boolean load_template_specializations_from_ifc_module(a_template_ptr  templ);

extern a_boolean has_type_definition_from_ifc_module(a_type_ptr  ty);

extern a_boolean load_type_definition_from_ifc_module(a_type_ptr  ty);

extern a_module_entity_ptr locate_ifc_module_entity(
                                                   a_module_entry_locator loc);

extern void load_namespace_elements_from_ifc_locator(
                                                   a_module_entity_ptr    mep,
                                                   a_module_entry_locator loc);

extern void update_entity_from_new_ifc_locator(a_module_entity_ptr    mep,
                                               a_module_entry_locator new_loc);

extern Opt_a_source_position source_position_from_ifc_of(
                                                      a_module_entity_ptr mep);

extern a_dynamic_init_ptr load_variable_init_from_ifc_module(
                                                a_type_ptr        tp,
                                                an_ifc_expr_index init_expr);

extern a_boolean extract_tokens_for_ifc_module_expr(
                              a_lexical_ifc_index_reference *index,
                              a_token_sequence_number       *expected_end_tsn);

extern void record_symbol_for_ifc_decl(a_symbol_ptr  sym);

extern a_symbol_ptr load_tok_ifc_entity_ref();

extern a_symbol_ptr load_tok_ifc_decl_ref();

extern a_type_ptr load_tok_ifc_type_ref();

extern void scan_ifc_param_ref_expr(an_operand *result);

extern a_boolean import_ifc_module_file(a_module_import_decl_ptr midp);

extern void ifc_modules_report_suppressed_diagnostics();

extern void ifc_modules_pch_read_reset();

extern void ifc_modules_one_time_init();

extern void require_ifc_modules();

extern void ifc_modules_trans_unit_init();

extern void ifc_modules_trans_unit_wrapup();

#if MAKE_FRONT_END_CALLABLE
extern void ifc_modules_cleanup();
#endif /* MAKE_FRONT_END_CALLABLE */

extern void ifc_modules_write_out();

#if DEBUG

extern a_string s_db_ifc_locator(a_module_entry_locator loc);

extern a_string s_db_id_of_ifc_mep(a_module_entity_ptr mep);

extern a_string s_db_lexical_ifc_index(a_lexical_ifc_index_reference idx);

extern void db_mep_stack();

extern void db_node_at_tsn(a_token_cache_ptr        cache,
                           a_token_sequence_number  tsn);

extern void db_module_node_at_tsn(a_module_token_cache_ptr cache,
                           a_token_sequence_number  tsn);

extern void db_locus(an_ifc_source_location locus);

#endif /* DEBUG */

/*lint -restore*/ /* FIXME: temporary. */

/* Conditionally close the "edg" namespace. */

#endif /* !STANDALONE_UTILITY_PROGRAM */

#endif /* ifndef IFC_MODULES_H */

