/*
Part of the EDG Compiler Project, under the Apache License v2.0 with LLVM
Exceptions.
See https://edgcpp.org/LICENSE.txt for license information.
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
*/

/*

ifc_map_functions_dbg.c -- Function implementations for IFC-based module
                           debug functions.

** NOTICE: This file is produced by an external script. **

While EDG staff should update the generation script rather than manually
editing this file, customers are welcome to modify this file and create patches
as they see fit.

Please contact EDG Support if you would be interested in using, or learning
more about, the tool that generated this file.
*/

#include "basic_hdrs.h"
#include "checking.h"
#include "header_util.h"
#include "ifc_map.h"
#include "ifc_map_functions.h"
#include "ifc_modules_internal.h"


#if !STANDALONE_UTILITY_PROGRAM

#if DEBUG

/* Conditionally open the "edg" namespace. */
BEGIN_EDG_NAMESPACE

/* Disable spurious GCC warnings in generated code. */
BEGIN_DISABLE_GCC_WARNING_MAYBE_UNITIALIZED


void db_node_an_ifc_keyword_syntax_indent(const an_ifc_keyword_syntax universal, unsigned indent)
/*
Given the universal representation of KeywordSyntax, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_keyword_syntax(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_keyword_syntax(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_value_an_ifc_keyword_syntax(universal)) {
    an_ifc_keyword_sort field = get_ifc_value_an_ifc_keyword_syntax(universal);

    db_print_indent(indent);
    fprintf(f_debug, "value: %s\n", str_for_an_ifc_keyword_sort(field));
  }  /* if */
}  /* db_node */


void db_node_an_ifc_keyword_syntax(const an_ifc_keyword_syntax universal)
/*
Given the universal representation of KeywordSyntax, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "KeywordSyntax ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_keyword_syntax_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_module_reference_indent(const an_ifc_module_reference universal, unsigned indent)
/*
Given the universal representation of ModuleReference, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_owner_an_ifc_module_reference(universal)) {
    an_ifc_text_offset field = get_ifc_owner_an_ifc_module_reference(universal);

    db_print_indent(indent);
    fprintf(f_debug, "owner: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_partition_an_ifc_module_reference(universal)) {
    an_ifc_text_offset field = get_ifc_partition_an_ifc_module_reference(universal);

    db_print_indent(indent);
    fprintf(f_debug, "partition: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_module_reference(const an_ifc_module_reference universal)
/*
Given the universal representation of ModuleReference, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "=============================== ");
  fprintf(f_debug, "ModuleReference ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_module_reference_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_nestable_word_indent(const an_ifc_nestable_word universal, unsigned indent)
/*
Given the universal representation of NestableWord, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_category_an_ifc_nestable_word(universal)) {
    an_ifc_word_category field = get_ifc_category_an_ifc_nestable_word(universal);

    db_print_indent(indent);
    fprintf(f_debug, "category:\n");
    db_print_indent(indent);
    fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_word_sort(field.sort));
    ++indent;
    switch (field.sort) {
      case ifc_ws_source_directive:
        { an_ifc_source_directive_sort sd_ref =
                                                field.variant.source_directive;

          db_print_indent(indent);
          fprintf(f_debug, "source_directive: %s\n", str_for_an_ifc_source_directive_sort(sd_ref));
        }
        break;
      case ifc_ws_source_identifier:
        { an_ifc_source_identifier_category si_ref =
                                               field.variant.source_identifier;

          db_print_indent(indent);
          fprintf(f_debug, "source_identifier:\n");
          db_print_indent(indent);
          fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_source_identifier_sort(si_ref.sort));
          ++indent;
          switch (si_ref.sort) {
            case ifc_sis_msvc:
              { an_ifc_source_unknown_identifier m_ref = si_ref.variant.msvc;

                db_print_indent(indent);
                fprintf(f_debug, "msvc: %llu\n",
                        (unsigned long long)m_ref.value);
              }
              break;
            case ifc_sis_msvc_builtin_huge_val:
              { an_ifc_source_unknown_identifier mbhv_ref =
                                          si_ref.variant.msvc_builtin_huge_val;

                db_print_indent(indent);
                fprintf(f_debug, "msvc_builtin_huge_val: %llu\n",
                        (unsigned long long)mbhv_ref.value);
              }
              break;
            case ifc_sis_msvc_builtin_huge_valf:
              { an_ifc_source_unknown_identifier mbhv_ref =
                                         si_ref.variant.msvc_builtin_huge_valf;

                db_print_indent(indent);
                fprintf(f_debug, "msvc_builtin_huge_valf: %llu\n",
                        (unsigned long long)mbhv_ref.value);
              }
              break;
            case ifc_sis_msvc_builtin_nan:
              { an_ifc_source_unknown_identifier mbn_ref =
                                               si_ref.variant.msvc_builtin_nan;

                db_print_indent(indent);
                fprintf(f_debug, "msvc_builtin_nan: %llu\n",
                        (unsigned long long)mbn_ref.value);
              }
              break;
            case ifc_sis_msvc_builtin_nanf:
              { an_ifc_source_unknown_identifier mbn_ref =
                                              si_ref.variant.msvc_builtin_nanf;

                db_print_indent(indent);
                fprintf(f_debug, "msvc_builtin_nanf: %llu\n",
                        (unsigned long long)mbn_ref.value);
              }
              break;
            case ifc_sis_msvc_builtin_nans:
              { an_ifc_source_unknown_identifier mbn_ref =
                                              si_ref.variant.msvc_builtin_nans;

                db_print_indent(indent);
                fprintf(f_debug, "msvc_builtin_nans: %llu\n",
                        (unsigned long long)mbn_ref.value);
              }
              break;
            case ifc_sis_msvc_builtin_nansf:
              { an_ifc_source_unknown_identifier mbn_ref =
                                             si_ref.variant.msvc_builtin_nansf;

                db_print_indent(indent);
                fprintf(f_debug, "msvc_builtin_nansf: %llu\n",
                        (unsigned long long)mbn_ref.value);
              }
              break;
            case ifc_sis_plain:
              { an_ifc_text_offset p_ref = si_ref.variant.plain;

                db_print_indent(indent);
                fprintf(f_debug, "plain: %llu\n",
                        (unsigned long long)p_ref.value);
              }
              break;
            default_is_unexpected();
          }  /* switch */
          --indent;
        }
        break;
      case ifc_ws_source_keyword:
        { an_ifc_source_keyword_sort sk_ref = field.variant.source_keyword;

          db_print_indent(indent);
          fprintf(f_debug, "source_keyword: %s\n", str_for_an_ifc_source_keyword_sort(sk_ref));
        }
        break;
      case ifc_ws_source_literal:
        { an_ifc_source_literal_category sl_ref =
                                                  field.variant.source_literal;

          db_print_indent(indent);
          fprintf(f_debug, "source_literal:\n");
          db_print_indent(indent);
          fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_source_literal_sort(sl_ref.sort));
          ++indent;
          switch (sl_ref.sort) {
            case ifc_sls_defined_string:
              { an_ifc_string_index ds_ref = sl_ref.variant.defined_string;

                db_print_indent(indent);
                fprintf(f_debug, "defined_string:");
                if (is_null_index_an_ifc_string_index(ds_ref)) {
                  fprintf(f_debug, " NULL\n");
                } else {
                  fprintf(f_debug, "\n");
                  db_print_indent(indent);
                  fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_string_sort(ds_ref.sort));
                  db_print_indent(indent);
                  fprintf(f_debug, "  value: %llu\n",
                          (unsigned long long)ds_ref.value);
                }  /* if */
              }
              break;
            case ifc_sls_msvc:
              { an_ifc_source_unknown_literal m_ref = sl_ref.variant.msvc;

                db_print_indent(indent);
                fprintf(f_debug, "msvc: %llu\n",
                        (unsigned long long)m_ref.value);
              }
              break;
            case ifc_sls_msvc_binding:
              { an_ifc_expr_index mb_ref = sl_ref.variant.msvc_binding;

                db_print_indent(indent);
                fprintf(f_debug, "msvc_binding:");
                if (is_null_index_an_ifc_expr_index(mb_ref)) {
                  fprintf(f_debug, " NULL\n");
                } else {
                  fprintf(f_debug, "\n");
                  db_print_indent(indent);
                  fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(mb_ref.sort));
                  db_print_indent(indent);
                  fprintf(f_debug, "  value: %llu\n",
                          (unsigned long long)mb_ref.value);
                }  /* if */
              }
              break;
            case ifc_sls_msvc_cast_target_type:
              { an_ifc_type_index mctt_ref =
                                          sl_ref.variant.msvc_cast_target_type;

                db_print_indent(indent);
                fprintf(f_debug, "msvc_cast_target_type:");
                if (is_null_index_an_ifc_type_index(mctt_ref)) {
                  fprintf(f_debug, " NULL\n");
                } else {
                  fprintf(f_debug, "\n");
                  db_print_indent(indent);
                  fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(mctt_ref.sort));
                  db_print_indent(indent);
                  fprintf(f_debug, "  value: %llu\n",
                          (unsigned long long)mctt_ref.value);
                }  /* if */
              }
              break;
            case ifc_sls_msvc_defined_constant:
              { an_ifc_expr_index mdc_ref =
                                          sl_ref.variant.msvc_defined_constant;

                db_print_indent(indent);
                fprintf(f_debug, "msvc_defined_constant:");
                if (is_null_index_an_ifc_expr_index(mdc_ref)) {
                  fprintf(f_debug, " NULL\n");
                } else {
                  fprintf(f_debug, "\n");
                  db_print_indent(indent);
                  fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(mdc_ref.sort));
                  db_print_indent(indent);
                  fprintf(f_debug, "  value: %llu\n",
                          (unsigned long long)mdc_ref.value);
                }  /* if */
              }
              break;
            case ifc_sls_msvc_function_name_macro:
              { an_ifc_text_offset mfnm_ref =
                                       sl_ref.variant.msvc_function_name_macro;

                db_print_indent(indent);
                fprintf(f_debug, "msvc_function_name_macro: %llu\n",
                        (unsigned long long)mfnm_ref.value);
              }
              break;
            case ifc_sls_msvc_resolved_type:
              { an_ifc_type_index mrt_ref = sl_ref.variant.msvc_resolved_type;

                db_print_indent(indent);
                fprintf(f_debug, "msvc_resolved_type:");
                if (is_null_index_an_ifc_type_index(mrt_ref)) {
                  fprintf(f_debug, " NULL\n");
                } else {
                  fprintf(f_debug, "\n");
                  db_print_indent(indent);
                  fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(mrt_ref.sort));
                  db_print_indent(indent);
                  fprintf(f_debug, "  value: %llu\n",
                          (unsigned long long)mrt_ref.value);
                }  /* if */
              }
              break;
            case ifc_sls_msvc_string_prefix_macro:
              { an_ifc_text_offset mspm_ref =
                                       sl_ref.variant.msvc_string_prefix_macro;

                db_print_indent(indent);
                fprintf(f_debug, "msvc_string_prefix_macro: %llu\n",
                        (unsigned long long)mspm_ref.value);
              }
              break;
            case ifc_sls_scalar:
              { an_ifc_expr_index s_ref = sl_ref.variant.scalar;

                db_print_indent(indent);
                fprintf(f_debug, "scalar:");
                if (is_null_index_an_ifc_expr_index(s_ref)) {
                  fprintf(f_debug, " NULL\n");
                } else {
                  fprintf(f_debug, "\n");
                  db_print_indent(indent);
                  fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(s_ref.sort));
                  db_print_indent(indent);
                  fprintf(f_debug, "  value: %llu\n",
                          (unsigned long long)s_ref.value);
                }  /* if */
              }
              break;
            case ifc_sls_string:
              { an_ifc_string_index s_ref = sl_ref.variant.string;

                db_print_indent(indent);
                fprintf(f_debug, "string:");
                if (is_null_index_an_ifc_string_index(s_ref)) {
                  fprintf(f_debug, " NULL\n");
                } else {
                  fprintf(f_debug, "\n");
                  db_print_indent(indent);
                  fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_string_sort(s_ref.sort));
                  db_print_indent(indent);
                  fprintf(f_debug, "  value: %llu\n",
                          (unsigned long long)s_ref.value);
                }  /* if */
              }
              break;
            case ifc_sls_unknown:
              { an_ifc_source_unknown_literal u_ref = sl_ref.variant.unknown;

                db_print_indent(indent);
                fprintf(f_debug, "unknown: %llu\n",
                        (unsigned long long)u_ref.value);
              }
              break;
            default_is_unexpected();
          }  /* switch */
          --indent;
        }
        break;
      case ifc_ws_source_operator:
        { an_ifc_source_operator_sort so_ref = field.variant.source_operator;

          db_print_indent(indent);
          fprintf(f_debug, "source_operator: %s\n", str_for_an_ifc_source_operator_sort(so_ref));
        }
        break;
      case ifc_ws_source_punctuator:
        { an_ifc_source_punctuator_sort sp_ref =
                                               field.variant.source_punctuator;

          db_print_indent(indent);
          fprintf(f_debug, "source_punctuator: %s\n", str_for_an_ifc_source_punctuator_sort(sp_ref));
        }
        break;
      case ifc_ws_unknown:
        { an_ifc_source_unknown_word u_ref = field.variant.unknown;

          db_print_indent(indent);
          fprintf(f_debug, "unknown: %llu\n",
                  (unsigned long long)u_ref.value);
        }
        break;
      default_is_unexpected();
    }  /* switch */
    --indent;
  }  /* if */
  if (has_ifc_index_an_ifc_nestable_word(universal)) {
    an_ifc_index field = get_ifc_index_an_ifc_nestable_word(universal);

    db_print_indent(indent);
    fprintf(f_debug, "index: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_locus_an_ifc_nestable_word(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_nestable_word(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_sort_an_ifc_nestable_word(universal)) {
    an_ifc_word_sort field = get_ifc_sort_an_ifc_nestable_word(universal);

    db_print_indent(indent);
    fprintf(f_debug, "sort: %s\n", str_for_an_ifc_word_sort(field));
  }  /* if */
  if (has_ifc_value_an_ifc_nestable_word(universal)) {
    an_ifc_u16 field = get_ifc_value_an_ifc_nestable_word(universal);

    db_print_indent(indent);
    fprintf(f_debug, "value: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_nestable_word(const an_ifc_nestable_word universal)
/*
Given the universal representation of NestableWord, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "NestableWord ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_nestable_word_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_noexcept_specification_indent(const an_ifc_noexcept_specification universal, unsigned indent)
/*
Given the universal representation of NoexceptSpecification, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_expr_an_ifc_noexcept_specification(universal)) {
    an_ifc_expr_index field = get_ifc_expr_an_ifc_noexcept_specification(universal);

    db_print_indent(indent);
    fprintf(f_debug, "expr:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_sort_an_ifc_noexcept_specification(universal)) {
    an_ifc_noexcept_sort field = get_ifc_sort_an_ifc_noexcept_specification(universal);

    db_print_indent(indent);
    fprintf(f_debug, "sort: %s\n", str_for_an_ifc_noexcept_sort(field));
  }  /* if */
  if (has_ifc_words_an_ifc_noexcept_specification(universal)) {
    an_ifc_sentence_index field = get_ifc_words_an_ifc_noexcept_specification(universal);

    db_print_indent(indent);
    fprintf(f_debug, "words: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_noexcept_specification(const an_ifc_noexcept_specification universal)
/*
Given the universal representation of NoexceptSpecification, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================ ");
  fprintf(f_debug, "NoexceptSpecification ");
  fprintf(f_debug, "=============================\n");
  db_node_an_ifc_noexcept_specification_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_parameterized_entity_indent(const an_ifc_parameterized_entity universal, unsigned indent)
/*
Given the universal representation of ParameterizedEntity, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_attributes_an_ifc_parameterized_entity(universal)) {
    an_ifc_sentence_index field = get_ifc_attributes_an_ifc_parameterized_entity(universal);

    db_print_indent(indent);
    fprintf(f_debug, "attributes: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_body_an_ifc_parameterized_entity(universal)) {
    an_ifc_sentence_index field = get_ifc_body_an_ifc_parameterized_entity(universal);

    db_print_indent(indent);
    fprintf(f_debug, "body: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_decl_an_ifc_parameterized_entity(universal)) {
    an_ifc_decl_index field = get_ifc_decl_an_ifc_parameterized_entity(universal);

    db_print_indent(indent);
    fprintf(f_debug, "decl:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_head_an_ifc_parameterized_entity(universal)) {
    an_ifc_sentence_index field = get_ifc_head_an_ifc_parameterized_entity(universal);

    db_print_indent(indent);
    fprintf(f_debug, "head: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_parameterized_entity(const an_ifc_parameterized_entity universal)
/*
Given the universal representation of ParameterizedEntity, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================= ");
  fprintf(f_debug, "ParameterizedEntity ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_parameterized_entity_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_sequence_indent(const an_ifc_sequence universal, unsigned indent)
/*
Given the universal representation of Sequence, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_cardinality_an_ifc_sequence(universal)) {
    an_ifc_cardinality field = get_ifc_cardinality_an_ifc_sequence(universal);

    db_print_indent(indent);
    fprintf(f_debug, "cardinality: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_start_an_ifc_sequence(universal)) {
    an_ifc_index field = get_ifc_start_an_ifc_sequence(universal);

    db_print_indent(indent);
    fprintf(f_debug, "start: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_sequence(const an_ifc_sequence universal)
/*
Given the universal representation of Sequence, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "=================================== ");
  fprintf(f_debug, "Sequence ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_sequence_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_source_location_indent(const an_ifc_source_location universal, unsigned indent)
/*
Given the universal representation of SourceLocation, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_column_an_ifc_source_location(universal)) {
    an_ifc_column field = get_ifc_column_an_ifc_source_location(universal);

    db_print_indent(indent);
    fprintf(f_debug, "column: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_line_an_ifc_source_location(universal)) {
    an_ifc_line_offset field = get_ifc_line_an_ifc_source_location(universal);

    db_print_indent(indent);
    fprintf(f_debug, "line: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_source_location(const an_ifc_source_location universal)
/*
Given the universal representation of SourceLocation, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "SourceLocation ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_source_location_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_type_placeholder_basis_wrapper_indent(const an_ifc_type_placeholder_basis_wrapper universal,
             unsigned                                    indent)
/*
Given the universal representation of TypePlaceholderBasisWrapper, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_value_an_ifc_type_placeholder_basis_wrapper(universal)) {
    an_ifc_type_basis_sort field = get_ifc_value_an_ifc_type_placeholder_basis_wrapper(universal);

    db_print_indent(indent);
    fprintf(f_debug, "value: %s\n", str_for_an_ifc_type_basis_sort(field));
  }  /* if */
}  /* db_node */


void db_node_an_ifc_type_placeholder_basis_wrapper(const an_ifc_type_placeholder_basis_wrapper universal)
/*
Given the universal representation of TypePlaceholderBasisWrapper, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================= ");
  fprintf(f_debug, "TypePlaceholderBasisWrapper ");
  fprintf(f_debug, "==========================\n");
  db_node_an_ifc_type_placeholder_basis_wrapper_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_file_header_indent(const an_ifc_file_header universal, unsigned indent)
/*
Given the universal representation of FileHeader, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_abi_an_ifc_file_header(universal)) {
    an_ifc_abi field = get_ifc_abi_an_ifc_file_header(universal);

    db_print_indent(indent);
    fprintf(f_debug, "abi: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_arch_an_ifc_file_header(universal)) {
    an_ifc_architecture_sort field = get_ifc_arch_an_ifc_file_header(universal);

    db_print_indent(indent);
    fprintf(f_debug, "arch: %s\n", str_for_an_ifc_architecture_sort(field));
  }  /* if */
  if (has_ifc_checksum_an_ifc_file_header(universal)) {
    db_print_indent(indent);
    fprintf(f_debug, "checksum: UNIMPLEMENTED\n");
  }  /* if */
  if (has_ifc_dialect_an_ifc_file_header(universal)) {
    an_ifc_language_version field = get_ifc_dialect_an_ifc_file_header(universal);

    db_print_indent(indent);
    fprintf(f_debug, "dialect: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_global_scope_an_ifc_file_header(universal)) {
    an_ifc_scope_offset field = get_ifc_global_scope_an_ifc_file_header(universal);

    db_print_indent(indent);
    fprintf(f_debug, "global_scope: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_internal_an_ifc_file_header(universal)) {
    an_ifc_bool field = get_ifc_internal_an_ifc_file_header(universal);

    db_print_indent(indent);
    fprintf(f_debug, "internal: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_major_version_an_ifc_file_header(universal)) {
    an_ifc_version field = get_ifc_major_version_an_ifc_file_header(universal);

    db_print_indent(indent);
    fprintf(f_debug, "major_version: %llu\n",
            (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_minor_version_an_ifc_file_header(universal)) {
    an_ifc_version field = get_ifc_minor_version_an_ifc_file_header(universal);

    db_print_indent(indent);
    fprintf(f_debug, "minor_version: %llu\n",
            (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_partition_count_an_ifc_file_header(universal)) {
    an_ifc_cardinality field = get_ifc_partition_count_an_ifc_file_header(universal);

    db_print_indent(indent);
    fprintf(f_debug, "partition_count: %llu\n",
            (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_src_path_an_ifc_file_header(universal)) {
    an_ifc_text_offset field = get_ifc_src_path_an_ifc_file_header(universal);

    db_print_indent(indent);
    fprintf(f_debug, "src_path: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_string_table_bytes_an_ifc_file_header(universal)) {
    an_ifc_byte_offset field = get_ifc_string_table_bytes_an_ifc_file_header(universal);

    db_print_indent(indent);
    fprintf(f_debug, "string_table_bytes: %llu\n",
            (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_string_table_size_an_ifc_file_header(universal)) {
    an_ifc_cardinality field = get_ifc_string_table_size_an_ifc_file_header(universal);

    db_print_indent(indent);
    fprintf(f_debug, "string_table_size: %llu\n",
            (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_toc_an_ifc_file_header(universal)) {
    an_ifc_byte_offset field = get_ifc_toc_an_ifc_file_header(universal);

    db_print_indent(indent);
    fprintf(f_debug, "toc: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_unit_an_ifc_file_header(universal)) {
    an_ifc_unit_index field = get_ifc_unit_an_ifc_file_header(universal);

    db_print_indent(indent);
    fprintf(f_debug, "unit:");
    if (is_null_index_an_ifc_unit_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_unit_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_file_header(const an_ifc_file_header universal)
/*
Given the universal representation of FileHeader, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "FileHeader ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_file_header_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_partition_indent(const an_ifc_partition universal, unsigned indent)
/*
Given the universal representation of Partition, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_cardinality_an_ifc_partition(universal)) {
    an_ifc_cardinality field = get_ifc_cardinality_an_ifc_partition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "cardinality: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_entry_size_an_ifc_partition(universal)) {
    an_ifc_entity_size field = get_ifc_entry_size_an_ifc_partition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "entry_size: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_name_an_ifc_partition(universal)) {
    an_ifc_text_offset field = get_ifc_name_an_ifc_partition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_offset_an_ifc_partition(universal)) {
    an_ifc_byte_offset field = get_ifc_offset_an_ifc_partition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "offset: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_partition(const an_ifc_partition universal)
/*
Given the universal representation of Partition, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "Partition ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_partition_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_attr_basic_indent(const an_ifc_attr_basic universal, unsigned indent)
/*
Given the universal representation of AttrBasic, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_word_an_ifc_attr_basic(universal)) {
    an_ifc_nestable_word field = get_ifc_word_an_ifc_attr_basic(universal);

    db_print_indent(indent);
    fprintf(f_debug, "word:\n");
    db_node_an_ifc_nestable_word_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_attr_basic(const an_ifc_attr_basic universal)
/*
Given the universal representation of AttrBasic, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "AttrBasic ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_attr_basic_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_attr_called_indent(const an_ifc_attr_called universal, unsigned indent)
/*
Given the universal representation of AttrCalled, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_arguments_an_ifc_attr_called(universal)) {
    an_ifc_attr_index field = get_ifc_arguments_an_ifc_attr_called(universal);

    db_print_indent(indent);
    fprintf(f_debug, "arguments:");
    if (is_null_index_an_ifc_attr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_attr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_function_an_ifc_attr_called(universal)) {
    an_ifc_attr_index field = get_ifc_function_an_ifc_attr_called(universal);

    db_print_indent(indent);
    fprintf(f_debug, "function:");
    if (is_null_index_an_ifc_attr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_attr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_attr_called(const an_ifc_attr_called universal)
/*
Given the universal representation of AttrCalled, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "AttrCalled ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_attr_called_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_attr_elaborated_indent(const an_ifc_attr_elaborated universal, unsigned indent)
/*
Given the universal representation of AttrElaborated, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_expression_an_ifc_attr_elaborated(universal)) {
    an_ifc_expr_index field = get_ifc_expression_an_ifc_attr_elaborated(universal);

    db_print_indent(indent);
    fprintf(f_debug, "expression:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_attr_elaborated(const an_ifc_attr_elaborated universal)
/*
Given the universal representation of AttrElaborated, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "AttrElaborated ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_attr_elaborated_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_attr_expanded_indent(const an_ifc_attr_expanded universal, unsigned indent)
/*
Given the universal representation of AttrExpanded, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_operand_an_ifc_attr_expanded(universal)) {
    an_ifc_attr_index field = get_ifc_operand_an_ifc_attr_expanded(universal);

    db_print_indent(indent);
    fprintf(f_debug, "operand:");
    if (is_null_index_an_ifc_attr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_attr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_attr_expanded(const an_ifc_attr_expanded universal)
/*
Given the universal representation of AttrExpanded, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "AttrExpanded ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_attr_expanded_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_attr_factored_indent(const an_ifc_attr_factored universal, unsigned indent)
/*
Given the universal representation of AttrFactored, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_factor_an_ifc_attr_factored(universal)) {
    an_ifc_nestable_word field = get_ifc_factor_an_ifc_attr_factored(universal);

    db_print_indent(indent);
    fprintf(f_debug, "factor:\n");
    db_node_an_ifc_nestable_word_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_terms_an_ifc_attr_factored(universal)) {
    an_ifc_attr_index field = get_ifc_terms_an_ifc_attr_factored(universal);

    db_print_indent(indent);
    fprintf(f_debug, "terms:");
    if (is_null_index_an_ifc_attr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_attr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_attr_factored(const an_ifc_attr_factored universal)
/*
Given the universal representation of AttrFactored, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "AttrFactored ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_attr_factored_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_attr_labeled_indent(const an_ifc_attr_labeled universal, unsigned indent)
/*
Given the universal representation of AttrLabeled, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_attribute_an_ifc_attr_labeled(universal)) {
    an_ifc_attr_index field = get_ifc_attribute_an_ifc_attr_labeled(universal);

    db_print_indent(indent);
    fprintf(f_debug, "attribute:");
    if (is_null_index_an_ifc_attr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_attr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_label_an_ifc_attr_labeled(universal)) {
    an_ifc_nestable_word field = get_ifc_label_an_ifc_attr_labeled(universal);

    db_print_indent(indent);
    fprintf(f_debug, "label:\n");
    db_node_an_ifc_nestable_word_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_attr_labeled(const an_ifc_attr_labeled universal)
/*
Given the universal representation of AttrLabeled, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "AttrLabeled ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_attr_labeled_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_attr_scoped_indent(const an_ifc_attr_scoped universal, unsigned indent)
/*
Given the universal representation of AttrScoped, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_member_an_ifc_attr_scoped(universal)) {
    an_ifc_nestable_word field = get_ifc_member_an_ifc_attr_scoped(universal);

    db_print_indent(indent);
    fprintf(f_debug, "member:\n");
    db_node_an_ifc_nestable_word_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_scope_an_ifc_attr_scoped(universal)) {
    an_ifc_nestable_word field = get_ifc_scope_an_ifc_attr_scoped(universal);

    db_print_indent(indent);
    fprintf(f_debug, "scope:\n");
    db_node_an_ifc_nestable_word_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_attr_scoped(const an_ifc_attr_scoped universal)
/*
Given the universal representation of AttrScoped, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "AttrScoped ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_attr_scoped_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_attr_tuple_indent(const an_ifc_attr_tuple universal, unsigned indent)
/*
Given the universal representation of AttrTuple, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_cardinality_an_ifc_attr_tuple(universal)) {
    an_ifc_cardinality field = get_ifc_cardinality_an_ifc_attr_tuple(universal);

    db_print_indent(indent);
    fprintf(f_debug, "cardinality: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_start_an_ifc_attr_tuple(universal)) {
    an_ifc_index field = get_ifc_start_an_ifc_attr_tuple(universal);

    db_print_indent(indent);
    fprintf(f_debug, "start: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_attr_tuple(const an_ifc_attr_tuple universal)
/*
Given the universal representation of AttrTuple, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "AttrTuple ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_attr_tuple_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_chart_multilevel_indent(const an_ifc_chart_multilevel universal, unsigned indent)
/*
Given the universal representation of ChartMultilevel, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_cardinality_an_ifc_chart_multilevel(universal)) {
    an_ifc_cardinality field = get_ifc_cardinality_an_ifc_chart_multilevel(universal);

    db_print_indent(indent);
    fprintf(f_debug, "cardinality: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_start_an_ifc_chart_multilevel(universal)) {
    an_ifc_index field = get_ifc_start_an_ifc_chart_multilevel(universal);

    db_print_indent(indent);
    fprintf(f_debug, "start: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_chart_multilevel(const an_ifc_chart_multilevel universal)
/*
Given the universal representation of ChartMultilevel, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "=============================== ");
  fprintf(f_debug, "ChartMultilevel ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_chart_multilevel_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_chart_unilevel_indent(const an_ifc_chart_unilevel universal, unsigned indent)
/*
Given the universal representation of ChartUnilevel, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_cardinality_an_ifc_chart_unilevel(universal)) {
    an_ifc_cardinality field = get_ifc_cardinality_an_ifc_chart_unilevel(universal);

    db_print_indent(indent);
    fprintf(f_debug, "cardinality: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_constraint_an_ifc_chart_unilevel(universal)) {
    an_ifc_expr_index field = get_ifc_constraint_an_ifc_chart_unilevel(universal);

    db_print_indent(indent);
    fprintf(f_debug, "constraint:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_start_an_ifc_chart_unilevel(universal)) {
    an_ifc_index field = get_ifc_start_an_ifc_chart_unilevel(universal);

    db_print_indent(indent);
    fprintf(f_debug, "start: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_chart_unilevel(const an_ifc_chart_unilevel universal)
/*
Given the universal representation of ChartUnilevel, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "ChartUnilevel ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_chart_unilevel_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_const_f64_indent(const an_ifc_const_f64 universal, unsigned indent)
/*
Given the universal representation of ConstF64, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_value_an_ifc_const_f64(universal)) {
    db_print_indent(indent);
    fprintf(f_debug, "value: UNIMPLEMENTED\n");
  }  /* if */
}  /* db_node */


void db_node_an_ifc_const_f64(const an_ifc_const_f64 universal)
/*
Given the universal representation of ConstF64, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "=================================== ");
  fprintf(f_debug, "ConstF64 ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_const_f64_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_const_i64_indent(const an_ifc_const_i64 universal, unsigned indent)
/*
Given the universal representation of ConstI64, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_value_an_ifc_const_i64(universal)) {
    an_ifc_u64 field = get_ifc_value_an_ifc_const_i64(universal);

    db_print_indent(indent);
    fprintf(f_debug, "value: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_const_i64(const an_ifc_const_i64 universal)
/*
Given the universal representation of ConstI64, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "=================================== ");
  fprintf(f_debug, "ConstI64 ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_const_i64_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_const_str_indent(const an_ifc_const_str universal, unsigned indent)
/*
Given the universal representation of ConstStr, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_length_an_ifc_const_str(universal)) {
    an_ifc_cardinality field = get_ifc_length_an_ifc_const_str(universal);

    db_print_indent(indent);
    fprintf(f_debug, "length: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_start_an_ifc_const_str(universal)) {
    an_ifc_text_offset field = get_ifc_start_an_ifc_const_str(universal);

    db_print_indent(indent);
    fprintf(f_debug, "start: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_suffix_an_ifc_const_str(universal)) {
    an_ifc_text_offset field = get_ifc_suffix_an_ifc_const_str(universal);

    db_print_indent(indent);
    fprintf(f_debug, "suffix: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_const_str(const an_ifc_const_str universal)
/*
Given the universal representation of ConstStr, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "=================================== ");
  fprintf(f_debug, "ConstStr ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_const_str_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_decl_alias_indent(const an_ifc_decl_alias universal, unsigned indent)
/*
Given the universal representation of DeclAlias, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_access_an_ifc_decl_alias(universal)) {
    an_ifc_access_sort field = get_ifc_access_an_ifc_decl_alias(universal);

    db_print_indent(indent);
    fprintf(f_debug, "access: %s\n", str_for_an_ifc_access_sort(field));
  }  /* if */
  if (has_ifc_aliasee_an_ifc_decl_alias(universal)) {
    an_ifc_type_index field = get_ifc_aliasee_an_ifc_decl_alias(universal);

    db_print_indent(indent);
    fprintf(f_debug, "aliasee:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_home_scope_an_ifc_decl_alias(universal)) {
    an_ifc_decl_index field = get_ifc_home_scope_an_ifc_decl_alias(universal);

    db_print_indent(indent);
    fprintf(f_debug, "home_scope:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_decl_alias(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_decl_alias(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_decl_alias(universal)) {
    an_ifc_text_offset field = get_ifc_name_an_ifc_decl_alias(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_specifiers_an_ifc_decl_alias(universal)) {
    an_ifc_basic_specifiers_bitfield field = get_ifc_specifiers_an_ifc_decl_alias(universal);

    fprintf(f_debug, "specifiers:\n");
    ++indent;
    if (test_bitmask_ifc_bsb_c(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- C\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_cxx(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Cxx\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_deprecated(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Deprecated\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_external(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- External\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_initialized_in_class(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- InitializedInClass\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_internal(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Internal\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_is_member_of_global_module(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- IsMemberOfGlobalModule\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_non_exported(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- NonExported\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_vague(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Vague\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_type_an_ifc_decl_alias(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_decl_alias(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_decl_alias(const an_ifc_decl_alias universal)
/*
Given the universal representation of DeclAlias, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "DeclAlias ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_decl_alias_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_decl_barren_indent(const an_ifc_decl_barren universal, unsigned indent)
/*
Given the universal representation of DeclBarren, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_access_an_ifc_decl_barren(universal)) {
    an_ifc_access_sort field = get_ifc_access_an_ifc_decl_barren(universal);

    db_print_indent(indent);
    fprintf(f_debug, "access: %s\n", str_for_an_ifc_access_sort(field));
  }  /* if */
  if (has_ifc_directive_an_ifc_decl_barren(universal)) {
    an_ifc_dir_index field = get_ifc_directive_an_ifc_decl_barren(universal);

    db_print_indent(indent);
    fprintf(f_debug, "directive:");
    if (is_null_index_an_ifc_dir_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_dir_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_specifiers_an_ifc_decl_barren(universal)) {
    an_ifc_basic_specifiers_bitfield field = get_ifc_specifiers_an_ifc_decl_barren(universal);

    fprintf(f_debug, "specifiers:\n");
    ++indent;
    if (test_bitmask_ifc_bsb_c(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- C\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_cxx(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Cxx\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_deprecated(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Deprecated\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_external(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- External\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_initialized_in_class(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- InitializedInClass\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_internal(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Internal\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_is_member_of_global_module(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- IsMemberOfGlobalModule\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_non_exported(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- NonExported\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_vague(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Vague\n");
    }  /* if */
    --indent;
  }  /* if */
}  /* db_node */


void db_node_an_ifc_decl_barren(const an_ifc_decl_barren universal)
/*
Given the universal representation of DeclBarren, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "DeclBarren ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_decl_barren_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_decl_bitfield_indent(const an_ifc_decl_bitfield universal, unsigned indent)
/*
Given the universal representation of DeclBitfield, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_access_an_ifc_decl_bitfield(universal)) {
    an_ifc_access_sort field = get_ifc_access_an_ifc_decl_bitfield(universal);

    db_print_indent(indent);
    fprintf(f_debug, "access: %s\n", str_for_an_ifc_access_sort(field));
  }  /* if */
  if (has_ifc_home_scope_an_ifc_decl_bitfield(universal)) {
    an_ifc_decl_index field = get_ifc_home_scope_an_ifc_decl_bitfield(universal);

    db_print_indent(indent);
    fprintf(f_debug, "home_scope:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_initializer_an_ifc_decl_bitfield(universal)) {
    an_ifc_expr_index field = get_ifc_initializer_an_ifc_decl_bitfield(universal);

    db_print_indent(indent);
    fprintf(f_debug, "initializer:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_decl_bitfield(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_decl_bitfield(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_decl_bitfield(universal)) {
    an_ifc_text_offset field = get_ifc_name_an_ifc_decl_bitfield(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_properties_an_ifc_decl_bitfield(universal)) {
    an_ifc_reachable_properties_bitfield field = get_ifc_properties_an_ifc_decl_bitfield(universal);

    fprintf(f_debug, "properties:\n");
    ++indent;
    if (test_bitmask_ifc_rpb_all(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- All\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_attributes(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Attributes\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_default_arguments(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- DefaultArguments\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_initializer(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Initializer\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_none(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- None\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_specifiers_an_ifc_decl_bitfield(universal)) {
    an_ifc_basic_specifiers_bitfield field = get_ifc_specifiers_an_ifc_decl_bitfield(universal);

    fprintf(f_debug, "specifiers:\n");
    ++indent;
    if (test_bitmask_ifc_bsb_c(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- C\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_cxx(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Cxx\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_deprecated(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Deprecated\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_external(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- External\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_initialized_in_class(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- InitializedInClass\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_internal(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Internal\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_is_member_of_global_module(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- IsMemberOfGlobalModule\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_non_exported(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- NonExported\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_vague(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Vague\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_traits_an_ifc_decl_bitfield(universal)) {
    an_ifc_object_traits_bitfield field = get_ifc_traits_an_ifc_decl_bitfield(universal);

    fprintf(f_debug, "traits:\n");
    ++indent;
    if (test_bitmask_ifc_otb_constexpr(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Constexpr\n");
    }  /* if */
    if (test_bitmask_ifc_otb_initializer_exported(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- InitializerExported\n");
    }  /* if */
    if (test_bitmask_ifc_otb_inline(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Inline\n");
    }  /* if */
    if (test_bitmask_ifc_otb_mutable(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Mutable\n");
    }  /* if */
    if (test_bitmask_ifc_otb_none(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- None\n");
    }  /* if */
    if (test_bitmask_ifc_otb_thread_local(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- ThreadLocal\n");
    }  /* if */
    if (test_bitmask_ifc_otb_vendor(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Vendor\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_type_an_ifc_decl_bitfield(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_decl_bitfield(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_width_an_ifc_decl_bitfield(universal)) {
    an_ifc_expr_index field = get_ifc_width_an_ifc_decl_bitfield(universal);

    db_print_indent(indent);
    fprintf(f_debug, "width:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_decl_bitfield(const an_ifc_decl_bitfield universal)
/*
Given the universal representation of DeclBitfield, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "DeclBitfield ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_decl_bitfield_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_decl_concept_indent(const an_ifc_decl_concept universal, unsigned indent)
/*
Given the universal representation of DeclConcept, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_access_an_ifc_decl_concept(universal)) {
    an_ifc_access_sort field = get_ifc_access_an_ifc_decl_concept(universal);

    db_print_indent(indent);
    fprintf(f_debug, "access: %s\n", str_for_an_ifc_access_sort(field));
  }  /* if */
  if (has_ifc_body_an_ifc_decl_concept(universal)) {
    an_ifc_sentence_index field = get_ifc_body_an_ifc_decl_concept(universal);

    db_print_indent(indent);
    fprintf(f_debug, "body: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_chart_an_ifc_decl_concept(universal)) {
    an_ifc_chart_index field = get_ifc_chart_an_ifc_decl_concept(universal);

    db_print_indent(indent);
    fprintf(f_debug, "chart:");
    if (is_null_index_an_ifc_chart_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_chart_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_constraint_an_ifc_decl_concept(universal)) {
    an_ifc_expr_index field = get_ifc_constraint_an_ifc_decl_concept(universal);

    db_print_indent(indent);
    fprintf(f_debug, "constraint:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_head_an_ifc_decl_concept(universal)) {
    an_ifc_sentence_index field = get_ifc_head_an_ifc_decl_concept(universal);

    db_print_indent(indent);
    fprintf(f_debug, "head: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_home_scope_an_ifc_decl_concept(universal)) {
    an_ifc_decl_index field = get_ifc_home_scope_an_ifc_decl_concept(universal);

    db_print_indent(indent);
    fprintf(f_debug, "home_scope:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_decl_concept(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_decl_concept(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_decl_concept(universal)) {
    an_ifc_text_offset field = get_ifc_name_an_ifc_decl_concept(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_specifiers_an_ifc_decl_concept(universal)) {
    an_ifc_basic_specifiers_bitfield field = get_ifc_specifiers_an_ifc_decl_concept(universal);

    fprintf(f_debug, "specifiers:\n");
    ++indent;
    if (test_bitmask_ifc_bsb_c(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- C\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_cxx(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Cxx\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_deprecated(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Deprecated\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_external(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- External\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_initialized_in_class(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- InitializedInClass\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_internal(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Internal\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_is_member_of_global_module(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- IsMemberOfGlobalModule\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_non_exported(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- NonExported\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_vague(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Vague\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_type_an_ifc_decl_concept(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_decl_concept(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_unknown_an_ifc_decl_concept(universal)) {
    an_ifc_u16 field = get_ifc_unknown_an_ifc_decl_concept(universal);

    db_print_indent(indent);
    fprintf(f_debug, "unknown: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_decl_concept(const an_ifc_decl_concept universal)
/*
Given the universal representation of DeclConcept, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "DeclConcept ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_decl_concept_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_decl_constructor_indent(const an_ifc_decl_constructor universal, unsigned indent)
/*
Given the universal representation of DeclConstructor, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_access_an_ifc_decl_constructor(universal)) {
    an_ifc_access_sort field = get_ifc_access_an_ifc_decl_constructor(universal);

    db_print_indent(indent);
    fprintf(f_debug, "access: %s\n", str_for_an_ifc_access_sort(field));
  }  /* if */
  if (has_ifc_chart_an_ifc_decl_constructor(universal)) {
    an_ifc_chart_index field = get_ifc_chart_an_ifc_decl_constructor(universal);

    db_print_indent(indent);
    fprintf(f_debug, "chart:");
    if (is_null_index_an_ifc_chart_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_chart_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_home_scope_an_ifc_decl_constructor(universal)) {
    an_ifc_decl_index field = get_ifc_home_scope_an_ifc_decl_constructor(universal);

    db_print_indent(indent);
    fprintf(f_debug, "home_scope:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_decl_constructor(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_decl_constructor(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_decl_constructor(universal)) {
    an_ifc_name_index field = get_ifc_name_an_ifc_decl_constructor(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name:");
    if (is_null_index_an_ifc_name_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_name_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_properties_an_ifc_decl_constructor(universal)) {
    an_ifc_reachable_properties_bitfield field = get_ifc_properties_an_ifc_decl_constructor(universal);

    fprintf(f_debug, "properties:\n");
    ++indent;
    if (test_bitmask_ifc_rpb_all(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- All\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_attributes(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Attributes\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_default_arguments(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- DefaultArguments\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_initializer(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Initializer\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_none(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- None\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_specifiers_an_ifc_decl_constructor(universal)) {
    an_ifc_basic_specifiers_bitfield field = get_ifc_specifiers_an_ifc_decl_constructor(universal);

    fprintf(f_debug, "specifiers:\n");
    ++indent;
    if (test_bitmask_ifc_bsb_c(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- C\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_cxx(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Cxx\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_deprecated(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Deprecated\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_external(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- External\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_initialized_in_class(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- InitializedInClass\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_internal(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Internal\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_is_member_of_global_module(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- IsMemberOfGlobalModule\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_non_exported(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- NonExported\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_vague(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Vague\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_traits_an_ifc_decl_constructor(universal)) {
    an_ifc_function_traits_bitfield field = get_ifc_traits_an_ifc_decl_constructor(universal);

    fprintf(f_debug, "traits:\n");
    ++indent;
    if (test_bitmask_ifc_ftb_constexpr(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Constexpr\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_constrained(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Constrained\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_defaulted(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Defaulted\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_deleted(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Deleted\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_explicit(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Explicit\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_hidden_friend(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- HiddenFriend\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_immediate(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Immediate\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_inline(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Inline\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_no_return(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- NoReturn\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_none(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- None\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_pure_virtual(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- PureVirtual\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_virtual(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Virtual\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_type_an_ifc_decl_constructor(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_decl_constructor(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_decl_constructor(const an_ifc_decl_constructor universal)
/*
Given the universal representation of DeclConstructor, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "=============================== ");
  fprintf(f_debug, "DeclConstructor ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_decl_constructor_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_decl_deduction_guide_indent(const an_ifc_decl_deduction_guide universal, unsigned indent)
/*
Given the universal representation of DeclDeductionGuide, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_home_scope_an_ifc_decl_deduction_guide(universal)) {
    an_ifc_decl_index field = get_ifc_home_scope_an_ifc_decl_deduction_guide(universal);

    db_print_indent(indent);
    fprintf(f_debug, "home_scope:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_decl_deduction_guide(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_decl_deduction_guide(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_decl_deduction_guide(universal)) {
    an_ifc_text_offset field = get_ifc_name_an_ifc_decl_deduction_guide(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_source_an_ifc_decl_deduction_guide(universal)) {
    an_ifc_chart_index field = get_ifc_source_an_ifc_decl_deduction_guide(universal);

    db_print_indent(indent);
    fprintf(f_debug, "source:");
    if (is_null_index_an_ifc_chart_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_chart_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_specifiers_an_ifc_decl_deduction_guide(universal)) {
    an_ifc_basic_specifiers_bitfield field = get_ifc_specifiers_an_ifc_decl_deduction_guide(universal);

    fprintf(f_debug, "specifiers:\n");
    ++indent;
    if (test_bitmask_ifc_bsb_c(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- C\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_cxx(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Cxx\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_deprecated(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Deprecated\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_external(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- External\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_initialized_in_class(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- InitializedInClass\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_internal(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Internal\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_is_member_of_global_module(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- IsMemberOfGlobalModule\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_non_exported(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- NonExported\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_vague(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Vague\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_target_an_ifc_decl_deduction_guide(universal)) {
    an_ifc_expr_index field = get_ifc_target_an_ifc_decl_deduction_guide(universal);

    db_print_indent(indent);
    fprintf(f_debug, "target:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_traits_an_ifc_decl_deduction_guide(universal)) {
    an_ifc_guide_traits_bitfield field = get_ifc_traits_an_ifc_decl_deduction_guide(universal);

    db_print_indent(indent);
    fprintf(f_debug, "traits: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_decl_deduction_guide(const an_ifc_decl_deduction_guide universal)
/*
Given the universal representation of DeclDeductionGuide, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================== ");
  fprintf(f_debug, "DeclDeductionGuide ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_decl_deduction_guide_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_decl_default_argument_indent(const an_ifc_decl_default_argument universal, unsigned indent)
/*
Given the universal representation of DeclDefaultArgument, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_access_an_ifc_decl_default_argument(universal)) {
    an_ifc_access_sort field = get_ifc_access_an_ifc_decl_default_argument(universal);

    db_print_indent(indent);
    fprintf(f_debug, "access: %s\n", str_for_an_ifc_access_sort(field));
  }  /* if */
  if (has_ifc_home_scope_an_ifc_decl_default_argument(universal)) {
    an_ifc_decl_index field = get_ifc_home_scope_an_ifc_decl_default_argument(universal);

    db_print_indent(indent);
    fprintf(f_debug, "home_scope:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_initializer_an_ifc_decl_default_argument(universal)) {
    an_ifc_expr_index field = get_ifc_initializer_an_ifc_decl_default_argument(universal);

    db_print_indent(indent);
    fprintf(f_debug, "initializer:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_decl_default_argument(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_decl_default_argument(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_properties_an_ifc_decl_default_argument(universal)) {
    an_ifc_reachable_properties_bitfield field = get_ifc_properties_an_ifc_decl_default_argument(universal);

    fprintf(f_debug, "properties:\n");
    ++indent;
    if (test_bitmask_ifc_rpb_all(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- All\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_attributes(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Attributes\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_default_arguments(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- DefaultArguments\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_initializer(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Initializer\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_none(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- None\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_specifiers_an_ifc_decl_default_argument(universal)) {
    an_ifc_basic_specifiers_bitfield field = get_ifc_specifiers_an_ifc_decl_default_argument(universal);

    fprintf(f_debug, "specifiers:\n");
    ++indent;
    if (test_bitmask_ifc_bsb_c(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- C\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_cxx(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Cxx\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_deprecated(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Deprecated\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_external(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- External\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_initialized_in_class(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- InitializedInClass\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_internal(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Internal\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_is_member_of_global_module(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- IsMemberOfGlobalModule\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_non_exported(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- NonExported\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_vague(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Vague\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_type_an_ifc_decl_default_argument(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_decl_default_argument(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_decl_default_argument(const an_ifc_decl_default_argument universal)
/*
Given the universal representation of DeclDefaultArgument, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================= ");
  fprintf(f_debug, "DeclDefaultArgument ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_decl_default_argument_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_decl_destructor_indent(const an_ifc_decl_destructor universal, unsigned indent)
/*
Given the universal representation of DeclDestructor, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_access_an_ifc_decl_destructor(universal)) {
    an_ifc_access_sort field = get_ifc_access_an_ifc_decl_destructor(universal);

    db_print_indent(indent);
    fprintf(f_debug, "access: %s\n", str_for_an_ifc_access_sort(field));
  }  /* if */
  if (has_ifc_convention_an_ifc_decl_destructor(universal)) {
    an_ifc_calling_convention_sort field = get_ifc_convention_an_ifc_decl_destructor(universal);

    db_print_indent(indent);
    fprintf(f_debug, "convention: %s\n", str_for_an_ifc_calling_convention_sort(field));
  }  /* if */
  if (has_ifc_eh_spec_an_ifc_decl_destructor(universal)) {
    an_ifc_noexcept_specification field = get_ifc_eh_spec_an_ifc_decl_destructor(universal);

    db_print_indent(indent);
    fprintf(f_debug, "eh_spec:\n");
    db_node_an_ifc_noexcept_specification_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_home_scope_an_ifc_decl_destructor(universal)) {
    an_ifc_decl_index field = get_ifc_home_scope_an_ifc_decl_destructor(universal);

    db_print_indent(indent);
    fprintf(f_debug, "home_scope:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_decl_destructor(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_decl_destructor(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_decl_destructor(universal)) {
    an_ifc_name_index field = get_ifc_name_an_ifc_decl_destructor(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name:");
    if (is_null_index_an_ifc_name_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_name_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_properties_an_ifc_decl_destructor(universal)) {
    an_ifc_reachable_properties_bitfield field = get_ifc_properties_an_ifc_decl_destructor(universal);

    fprintf(f_debug, "properties:\n");
    ++indent;
    if (test_bitmask_ifc_rpb_all(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- All\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_attributes(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Attributes\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_default_arguments(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- DefaultArguments\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_initializer(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Initializer\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_none(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- None\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_specifiers_an_ifc_decl_destructor(universal)) {
    an_ifc_basic_specifiers_bitfield field = get_ifc_specifiers_an_ifc_decl_destructor(universal);

    fprintf(f_debug, "specifiers:\n");
    ++indent;
    if (test_bitmask_ifc_bsb_c(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- C\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_cxx(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Cxx\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_deprecated(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Deprecated\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_external(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- External\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_initialized_in_class(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- InitializedInClass\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_internal(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Internal\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_is_member_of_global_module(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- IsMemberOfGlobalModule\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_non_exported(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- NonExported\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_vague(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Vague\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_traits_an_ifc_decl_destructor(universal)) {
    an_ifc_function_traits_bitfield field = get_ifc_traits_an_ifc_decl_destructor(universal);

    fprintf(f_debug, "traits:\n");
    ++indent;
    if (test_bitmask_ifc_ftb_constexpr(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Constexpr\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_constrained(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Constrained\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_defaulted(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Defaulted\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_deleted(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Deleted\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_explicit(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Explicit\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_hidden_friend(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- HiddenFriend\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_immediate(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Immediate\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_inline(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Inline\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_no_return(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- NoReturn\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_none(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- None\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_pure_virtual(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- PureVirtual\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_virtual(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Virtual\n");
    }  /* if */
    --indent;
  }  /* if */
}  /* db_node */


void db_node_an_ifc_decl_destructor(const an_ifc_decl_destructor universal)
/*
Given the universal representation of DeclDestructor, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "DeclDestructor ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_decl_destructor_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_decl_enumeration_indent(const an_ifc_decl_enumeration universal, unsigned indent)
/*
Given the universal representation of DeclEnumeration, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_access_an_ifc_decl_enumeration(universal)) {
    an_ifc_access_sort field = get_ifc_access_an_ifc_decl_enumeration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "access: %s\n", str_for_an_ifc_access_sort(field));
  }  /* if */
  if (has_ifc_alignment_an_ifc_decl_enumeration(universal)) {
    an_ifc_expr_index field = get_ifc_alignment_an_ifc_decl_enumeration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "alignment:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_base_an_ifc_decl_enumeration(universal)) {
    an_ifc_type_index field = get_ifc_base_an_ifc_decl_enumeration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "base:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_home_scope_an_ifc_decl_enumeration(universal)) {
    an_ifc_decl_index field = get_ifc_home_scope_an_ifc_decl_enumeration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "home_scope:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_initializer_an_ifc_decl_enumeration(universal)) {
    an_ifc_sequence field = get_ifc_initializer_an_ifc_decl_enumeration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "initializer:\n");
    db_node_an_ifc_sequence_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_locus_an_ifc_decl_enumeration(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_decl_enumeration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_decl_enumeration(universal)) {
    an_ifc_text_offset field = get_ifc_name_an_ifc_decl_enumeration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_properties_an_ifc_decl_enumeration(universal)) {
    an_ifc_reachable_properties_bitfield field = get_ifc_properties_an_ifc_decl_enumeration(universal);

    fprintf(f_debug, "properties:\n");
    ++indent;
    if (test_bitmask_ifc_rpb_all(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- All\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_attributes(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Attributes\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_default_arguments(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- DefaultArguments\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_initializer(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Initializer\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_none(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- None\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_specifiers_an_ifc_decl_enumeration(universal)) {
    an_ifc_basic_specifiers_bitfield field = get_ifc_specifiers_an_ifc_decl_enumeration(universal);

    fprintf(f_debug, "specifiers:\n");
    ++indent;
    if (test_bitmask_ifc_bsb_c(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- C\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_cxx(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Cxx\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_deprecated(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Deprecated\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_external(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- External\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_initialized_in_class(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- InitializedInClass\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_internal(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Internal\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_is_member_of_global_module(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- IsMemberOfGlobalModule\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_non_exported(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- NonExported\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_vague(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Vague\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_type_an_ifc_decl_enumeration(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_decl_enumeration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_decl_enumeration(const an_ifc_decl_enumeration universal)
/*
Given the universal representation of DeclEnumeration, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "=============================== ");
  fprintf(f_debug, "DeclEnumeration ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_decl_enumeration_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_decl_enumerator_indent(const an_ifc_decl_enumerator universal, unsigned indent)
/*
Given the universal representation of DeclEnumerator, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_access_an_ifc_decl_enumerator(universal)) {
    an_ifc_access_sort field = get_ifc_access_an_ifc_decl_enumerator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "access: %s\n", str_for_an_ifc_access_sort(field));
  }  /* if */
  if (has_ifc_home_scope_an_ifc_decl_enumerator(universal)) {
    an_ifc_decl_index field = get_ifc_home_scope_an_ifc_decl_enumerator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "home_scope:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_initializer_an_ifc_decl_enumerator(universal)) {
    an_ifc_expr_index field = get_ifc_initializer_an_ifc_decl_enumerator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "initializer:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_decl_enumerator(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_decl_enumerator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_decl_enumerator(universal)) {
    an_ifc_text_offset field = get_ifc_name_an_ifc_decl_enumerator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_specifiers_an_ifc_decl_enumerator(universal)) {
    an_ifc_basic_specifiers_bitfield field = get_ifc_specifiers_an_ifc_decl_enumerator(universal);

    fprintf(f_debug, "specifiers:\n");
    ++indent;
    if (test_bitmask_ifc_bsb_c(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- C\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_cxx(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Cxx\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_deprecated(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Deprecated\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_external(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- External\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_initialized_in_class(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- InitializedInClass\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_internal(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Internal\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_is_member_of_global_module(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- IsMemberOfGlobalModule\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_non_exported(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- NonExported\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_vague(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Vague\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_type_an_ifc_decl_enumerator(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_decl_enumerator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_decl_enumerator(const an_ifc_decl_enumerator universal)
/*
Given the universal representation of DeclEnumerator, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "DeclEnumerator ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_decl_enumerator_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_decl_expansion_indent(const an_ifc_decl_expansion universal, unsigned indent)
/*
Given the universal representation of DeclExpansion, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_decl_expansion(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_decl_expansion(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_operand_an_ifc_decl_expansion(universal)) {
    an_ifc_decl_index field = get_ifc_operand_an_ifc_decl_expansion(universal);

    db_print_indent(indent);
    fprintf(f_debug, "operand:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_decl_expansion(const an_ifc_decl_expansion universal)
/*
Given the universal representation of DeclExpansion, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "DeclExpansion ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_decl_expansion_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_decl_explicit_instantiation_indent(const an_ifc_decl_explicit_instantiation universal,
             unsigned                                 indent)
/*
Given the universal representation of DeclExplicitInstantiation, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_decl_an_ifc_decl_explicit_instantiation(universal)) {
    an_ifc_decl_index field = get_ifc_decl_an_ifc_decl_explicit_instantiation(universal);

    db_print_indent(indent);
    fprintf(f_debug, "decl:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_form_an_ifc_decl_explicit_instantiation(universal)) {
    an_ifc_form_spec_offset field = get_ifc_form_an_ifc_decl_explicit_instantiation(universal);

    db_print_indent(indent);
    fprintf(f_debug, "form: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_decl_explicit_instantiation(const an_ifc_decl_explicit_instantiation universal)
/*
Given the universal representation of DeclExplicitInstantiation, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================== ");
  fprintf(f_debug, "DeclExplicitInstantiation ");
  fprintf(f_debug, "===========================\n");
  db_node_an_ifc_decl_explicit_instantiation_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_decl_explicit_specialization_indent(const an_ifc_decl_explicit_specialization universal,
             unsigned                                  indent)
/*
Given the universal representation of DeclExplicitSpecialization, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_decl_an_ifc_decl_explicit_specialization(universal)) {
    an_ifc_decl_index field = get_ifc_decl_an_ifc_decl_explicit_specialization(universal);

    db_print_indent(indent);
    fprintf(f_debug, "decl:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_form_an_ifc_decl_explicit_specialization(universal)) {
    an_ifc_form_spec_offset field = get_ifc_form_an_ifc_decl_explicit_specialization(universal);

    db_print_indent(indent);
    fprintf(f_debug, "form: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_decl_explicit_specialization(const an_ifc_decl_explicit_specialization universal)
/*
Given the universal representation of DeclExplicitSpecialization, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================== ");
  fprintf(f_debug, "DeclExplicitSpecialization ");
  fprintf(f_debug, "==========================\n");
  db_node_an_ifc_decl_explicit_specialization_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_decl_field_indent(const an_ifc_decl_field universal, unsigned indent)
/*
Given the universal representation of DeclField, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_access_an_ifc_decl_field(universal)) {
    an_ifc_access_sort field = get_ifc_access_an_ifc_decl_field(universal);

    db_print_indent(indent);
    fprintf(f_debug, "access: %s\n", str_for_an_ifc_access_sort(field));
  }  /* if */
  if (has_ifc_alignment_an_ifc_decl_field(universal)) {
    an_ifc_expr_index field = get_ifc_alignment_an_ifc_decl_field(universal);

    db_print_indent(indent);
    fprintf(f_debug, "alignment:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_home_scope_an_ifc_decl_field(universal)) {
    an_ifc_decl_index field = get_ifc_home_scope_an_ifc_decl_field(universal);

    db_print_indent(indent);
    fprintf(f_debug, "home_scope:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_initializer_an_ifc_decl_field(universal)) {
    an_ifc_expr_index field = get_ifc_initializer_an_ifc_decl_field(universal);

    db_print_indent(indent);
    fprintf(f_debug, "initializer:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_decl_field(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_decl_field(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_decl_field(universal)) {
    an_ifc_text_offset field = get_ifc_name_an_ifc_decl_field(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_properties_an_ifc_decl_field(universal)) {
    an_ifc_reachable_properties_bitfield field = get_ifc_properties_an_ifc_decl_field(universal);

    fprintf(f_debug, "properties:\n");
    ++indent;
    if (test_bitmask_ifc_rpb_all(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- All\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_attributes(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Attributes\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_default_arguments(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- DefaultArguments\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_initializer(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Initializer\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_none(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- None\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_specifiers_an_ifc_decl_field(universal)) {
    an_ifc_basic_specifiers_bitfield field = get_ifc_specifiers_an_ifc_decl_field(universal);

    fprintf(f_debug, "specifiers:\n");
    ++indent;
    if (test_bitmask_ifc_bsb_c(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- C\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_cxx(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Cxx\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_deprecated(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Deprecated\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_external(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- External\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_initialized_in_class(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- InitializedInClass\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_internal(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Internal\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_is_member_of_global_module(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- IsMemberOfGlobalModule\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_non_exported(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- NonExported\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_vague(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Vague\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_traits_an_ifc_decl_field(universal)) {
    an_ifc_object_traits_bitfield field = get_ifc_traits_an_ifc_decl_field(universal);

    fprintf(f_debug, "traits:\n");
    ++indent;
    if (test_bitmask_ifc_otb_constexpr(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Constexpr\n");
    }  /* if */
    if (test_bitmask_ifc_otb_initializer_exported(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- InitializerExported\n");
    }  /* if */
    if (test_bitmask_ifc_otb_inline(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Inline\n");
    }  /* if */
    if (test_bitmask_ifc_otb_mutable(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Mutable\n");
    }  /* if */
    if (test_bitmask_ifc_otb_none(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- None\n");
    }  /* if */
    if (test_bitmask_ifc_otb_thread_local(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- ThreadLocal\n");
    }  /* if */
    if (test_bitmask_ifc_otb_vendor(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Vendor\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_type_an_ifc_decl_field(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_decl_field(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_decl_field(const an_ifc_decl_field universal)
/*
Given the universal representation of DeclField, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "DeclField ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_decl_field_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_decl_friend_indent(const an_ifc_decl_friend universal, unsigned indent)
/*
Given the universal representation of DeclFriend, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_entity_an_ifc_decl_friend(universal)) {
    an_ifc_expr_index field = get_ifc_entity_an_ifc_decl_friend(universal);

    db_print_indent(indent);
    fprintf(f_debug, "entity:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_decl_friend(const an_ifc_decl_friend universal)
/*
Given the universal representation of DeclFriend, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "DeclFriend ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_decl_friend_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_decl_function_indent(const an_ifc_decl_function universal, unsigned indent)
/*
Given the universal representation of DeclFunction, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_access_an_ifc_decl_function(universal)) {
    an_ifc_access_sort field = get_ifc_access_an_ifc_decl_function(universal);

    db_print_indent(indent);
    fprintf(f_debug, "access: %s\n", str_for_an_ifc_access_sort(field));
  }  /* if */
  if (has_ifc_chart_an_ifc_decl_function(universal)) {
    an_ifc_chart_index field = get_ifc_chart_an_ifc_decl_function(universal);

    db_print_indent(indent);
    fprintf(f_debug, "chart:");
    if (is_null_index_an_ifc_chart_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_chart_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_home_scope_an_ifc_decl_function(universal)) {
    an_ifc_decl_index field = get_ifc_home_scope_an_ifc_decl_function(universal);

    db_print_indent(indent);
    fprintf(f_debug, "home_scope:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_decl_function(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_decl_function(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_decl_function(universal)) {
    an_ifc_name_index field = get_ifc_name_an_ifc_decl_function(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name:");
    if (is_null_index_an_ifc_name_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_name_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_properties_an_ifc_decl_function(universal)) {
    an_ifc_reachable_properties_bitfield field = get_ifc_properties_an_ifc_decl_function(universal);

    fprintf(f_debug, "properties:\n");
    ++indent;
    if (test_bitmask_ifc_rpb_all(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- All\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_attributes(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Attributes\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_default_arguments(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- DefaultArguments\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_initializer(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Initializer\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_none(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- None\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_specifiers_an_ifc_decl_function(universal)) {
    an_ifc_basic_specifiers_bitfield field = get_ifc_specifiers_an_ifc_decl_function(universal);

    fprintf(f_debug, "specifiers:\n");
    ++indent;
    if (test_bitmask_ifc_bsb_c(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- C\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_cxx(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Cxx\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_deprecated(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Deprecated\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_external(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- External\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_initialized_in_class(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- InitializedInClass\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_internal(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Internal\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_is_member_of_global_module(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- IsMemberOfGlobalModule\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_non_exported(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- NonExported\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_vague(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Vague\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_traits_an_ifc_decl_function(universal)) {
    an_ifc_function_traits_bitfield field = get_ifc_traits_an_ifc_decl_function(universal);

    fprintf(f_debug, "traits:\n");
    ++indent;
    if (test_bitmask_ifc_ftb_constexpr(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Constexpr\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_constrained(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Constrained\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_defaulted(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Defaulted\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_deleted(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Deleted\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_explicit(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Explicit\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_hidden_friend(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- HiddenFriend\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_immediate(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Immediate\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_inline(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Inline\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_no_return(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- NoReturn\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_none(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- None\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_pure_virtual(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- PureVirtual\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_virtual(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Virtual\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_type_an_ifc_decl_function(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_decl_function(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_decl_function(const an_ifc_decl_function universal)
/*
Given the universal representation of DeclFunction, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "DeclFunction ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_decl_function_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_decl_inherited_constructor_indent(const an_ifc_decl_inherited_constructor universal,
             unsigned                                indent)
/*
Given the universal representation of DeclInheritedConstructor, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_access_an_ifc_decl_inherited_constructor(universal)) {
    an_ifc_access_sort field = get_ifc_access_an_ifc_decl_inherited_constructor(universal);

    db_print_indent(indent);
    fprintf(f_debug, "access: %s\n", str_for_an_ifc_access_sort(field));
  }  /* if */
  if (has_ifc_base_ctor_an_ifc_decl_inherited_constructor(universal)) {
    an_ifc_decl_index field = get_ifc_base_ctor_an_ifc_decl_inherited_constructor(universal);

    db_print_indent(indent);
    fprintf(f_debug, "base_ctor:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_chart_an_ifc_decl_inherited_constructor(universal)) {
    an_ifc_chart_index field = get_ifc_chart_an_ifc_decl_inherited_constructor(universal);

    db_print_indent(indent);
    fprintf(f_debug, "chart:");
    if (is_null_index_an_ifc_chart_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_chart_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_home_scope_an_ifc_decl_inherited_constructor(universal)) {
    an_ifc_decl_index field = get_ifc_home_scope_an_ifc_decl_inherited_constructor(universal);

    db_print_indent(indent);
    fprintf(f_debug, "home_scope:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_decl_inherited_constructor(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_decl_inherited_constructor(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_decl_inherited_constructor(universal)) {
    an_ifc_text_offset field = get_ifc_name_an_ifc_decl_inherited_constructor(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_specifiers_an_ifc_decl_inherited_constructor(universal)) {
    an_ifc_basic_specifiers_bitfield field = get_ifc_specifiers_an_ifc_decl_inherited_constructor(universal);

    fprintf(f_debug, "specifiers:\n");
    ++indent;
    if (test_bitmask_ifc_bsb_c(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- C\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_cxx(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Cxx\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_deprecated(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Deprecated\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_external(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- External\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_initialized_in_class(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- InitializedInClass\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_internal(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Internal\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_is_member_of_global_module(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- IsMemberOfGlobalModule\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_non_exported(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- NonExported\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_vague(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Vague\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_traits_an_ifc_decl_inherited_constructor(universal)) {
    an_ifc_function_traits_bitfield field = get_ifc_traits_an_ifc_decl_inherited_constructor(universal);

    fprintf(f_debug, "traits:\n");
    ++indent;
    if (test_bitmask_ifc_ftb_constexpr(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Constexpr\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_constrained(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Constrained\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_defaulted(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Defaulted\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_deleted(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Deleted\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_explicit(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Explicit\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_hidden_friend(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- HiddenFriend\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_immediate(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Immediate\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_inline(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Inline\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_no_return(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- NoReturn\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_none(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- None\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_pure_virtual(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- PureVirtual\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_virtual(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Virtual\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_type_an_ifc_decl_inherited_constructor(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_decl_inherited_constructor(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_decl_inherited_constructor(const an_ifc_decl_inherited_constructor universal)
/*
Given the universal representation of DeclInheritedConstructor, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "=========================== ");
  fprintf(f_debug, "DeclInheritedConstructor ");
  fprintf(f_debug, "===========================\n");
  db_node_an_ifc_decl_inherited_constructor_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_decl_intrinsic_indent(const an_ifc_decl_intrinsic universal, unsigned indent)
/*
Given the universal representation of DeclIntrinsic, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_access_an_ifc_decl_intrinsic(universal)) {
    an_ifc_access_sort field = get_ifc_access_an_ifc_decl_intrinsic(universal);

    db_print_indent(indent);
    fprintf(f_debug, "access: %s\n", str_for_an_ifc_access_sort(field));
  }  /* if */
  if (has_ifc_home_scope_an_ifc_decl_intrinsic(universal)) {
    an_ifc_decl_index field = get_ifc_home_scope_an_ifc_decl_intrinsic(universal);

    db_print_indent(indent);
    fprintf(f_debug, "home_scope:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_decl_intrinsic(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_decl_intrinsic(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_decl_intrinsic(universal)) {
    an_ifc_text_offset field = get_ifc_name_an_ifc_decl_intrinsic(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_specifiers_an_ifc_decl_intrinsic(universal)) {
    an_ifc_basic_specifiers_bitfield field = get_ifc_specifiers_an_ifc_decl_intrinsic(universal);

    fprintf(f_debug, "specifiers:\n");
    ++indent;
    if (test_bitmask_ifc_bsb_c(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- C\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_cxx(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Cxx\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_deprecated(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Deprecated\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_external(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- External\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_initialized_in_class(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- InitializedInClass\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_internal(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Internal\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_is_member_of_global_module(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- IsMemberOfGlobalModule\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_non_exported(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- NonExported\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_vague(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Vague\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_type_an_ifc_decl_intrinsic(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_decl_intrinsic(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_decl_intrinsic(const an_ifc_decl_intrinsic universal)
/*
Given the universal representation of DeclIntrinsic, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "DeclIntrinsic ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_decl_intrinsic_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_decl_method_indent(const an_ifc_decl_method universal, unsigned indent)
/*
Given the universal representation of DeclMethod, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_access_an_ifc_decl_method(universal)) {
    an_ifc_access_sort field = get_ifc_access_an_ifc_decl_method(universal);

    db_print_indent(indent);
    fprintf(f_debug, "access: %s\n", str_for_an_ifc_access_sort(field));
  }  /* if */
  if (has_ifc_chart_an_ifc_decl_method(universal)) {
    an_ifc_chart_index field = get_ifc_chart_an_ifc_decl_method(universal);

    db_print_indent(indent);
    fprintf(f_debug, "chart:");
    if (is_null_index_an_ifc_chart_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_chart_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_home_scope_an_ifc_decl_method(universal)) {
    an_ifc_decl_index field = get_ifc_home_scope_an_ifc_decl_method(universal);

    db_print_indent(indent);
    fprintf(f_debug, "home_scope:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_decl_method(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_decl_method(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_decl_method(universal)) {
    an_ifc_name_index field = get_ifc_name_an_ifc_decl_method(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name:");
    if (is_null_index_an_ifc_name_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_name_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_properties_an_ifc_decl_method(universal)) {
    an_ifc_reachable_properties_bitfield field = get_ifc_properties_an_ifc_decl_method(universal);

    fprintf(f_debug, "properties:\n");
    ++indent;
    if (test_bitmask_ifc_rpb_all(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- All\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_attributes(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Attributes\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_default_arguments(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- DefaultArguments\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_initializer(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Initializer\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_none(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- None\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_specifiers_an_ifc_decl_method(universal)) {
    an_ifc_basic_specifiers_bitfield field = get_ifc_specifiers_an_ifc_decl_method(universal);

    fprintf(f_debug, "specifiers:\n");
    ++indent;
    if (test_bitmask_ifc_bsb_c(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- C\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_cxx(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Cxx\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_deprecated(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Deprecated\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_external(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- External\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_initialized_in_class(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- InitializedInClass\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_internal(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Internal\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_is_member_of_global_module(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- IsMemberOfGlobalModule\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_non_exported(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- NonExported\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_vague(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Vague\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_traits_an_ifc_decl_method(universal)) {
    an_ifc_function_traits_bitfield field = get_ifc_traits_an_ifc_decl_method(universal);

    fprintf(f_debug, "traits:\n");
    ++indent;
    if (test_bitmask_ifc_ftb_constexpr(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Constexpr\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_constrained(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Constrained\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_defaulted(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Defaulted\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_deleted(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Deleted\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_explicit(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Explicit\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_hidden_friend(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- HiddenFriend\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_immediate(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Immediate\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_inline(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Inline\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_no_return(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- NoReturn\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_none(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- None\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_pure_virtual(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- PureVirtual\n");
    }  /* if */
    if (test_bitmask_ifc_ftb_virtual(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Virtual\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_type_an_ifc_decl_method(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_decl_method(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_decl_method(const an_ifc_decl_method universal)
/*
Given the universal representation of DeclMethod, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "DeclMethod ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_decl_method_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_decl_output_segment_indent(const an_ifc_decl_output_segment universal, unsigned indent)
/*
Given the universal representation of DeclOutputSegment, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_ID_an_ifc_decl_output_segment(universal)) {
    an_ifc_text_offset field = get_ifc_ID_an_ifc_decl_output_segment(universal);

    db_print_indent(indent);
    fprintf(f_debug, "ID: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_name_an_ifc_decl_output_segment(universal)) {
    an_ifc_text_offset field = get_ifc_name_an_ifc_decl_output_segment(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_traits_an_ifc_decl_output_segment(universal)) {
    an_ifc_segment_traits field = get_ifc_traits_an_ifc_decl_output_segment(universal);

    db_print_indent(indent);
    fprintf(f_debug, "traits: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_type_an_ifc_decl_output_segment(universal)) {
    an_ifc_segment_type field = get_ifc_type_an_ifc_decl_output_segment(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_decl_output_segment(const an_ifc_decl_output_segment universal)
/*
Given the universal representation of DeclOutputSegment, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================== ");
  fprintf(f_debug, "DeclOutputSegment ");
  fprintf(f_debug, "===============================\n");
  db_node_an_ifc_decl_output_segment_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_decl_parameter_indent(const an_ifc_decl_parameter universal, unsigned indent)
/*
Given the universal representation of DeclParameter, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_constraint_an_ifc_decl_parameter(universal)) {
    an_ifc_expr_index field = get_ifc_constraint_an_ifc_decl_parameter(universal);

    db_print_indent(indent);
    fprintf(f_debug, "constraint:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_init_decl_an_ifc_decl_parameter(universal)) {
    an_ifc_expr_named_decl_offset field = get_ifc_init_decl_an_ifc_decl_parameter(universal);

    db_print_indent(indent);
    fprintf(f_debug, "init_decl: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_initializer_an_ifc_decl_parameter(universal)) {
    an_ifc_expr_index field = get_ifc_initializer_an_ifc_decl_parameter(universal);

    db_print_indent(indent);
    fprintf(f_debug, "initializer:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_level_an_ifc_decl_parameter(universal)) {
    an_ifc_parameter_level field = get_ifc_level_an_ifc_decl_parameter(universal);

    db_print_indent(indent);
    fprintf(f_debug, "level: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_locus_an_ifc_decl_parameter(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_decl_parameter(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_decl_parameter(universal)) {
    an_ifc_text_offset field = get_ifc_name_an_ifc_decl_parameter(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_pack_an_ifc_decl_parameter(universal)) {
    an_ifc_bool field = get_ifc_pack_an_ifc_decl_parameter(universal);

    db_print_indent(indent);
    fprintf(f_debug, "pack: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_position_an_ifc_decl_parameter(universal)) {
    an_ifc_parameter_position field = get_ifc_position_an_ifc_decl_parameter(universal);

    db_print_indent(indent);
    fprintf(f_debug, "position: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_properties_an_ifc_decl_parameter(universal)) {
    an_ifc_reachable_properties_bitfield field = get_ifc_properties_an_ifc_decl_parameter(universal);

    fprintf(f_debug, "properties:\n");
    ++indent;
    if (test_bitmask_ifc_rpb_all(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- All\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_attributes(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Attributes\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_default_arguments(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- DefaultArguments\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_initializer(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Initializer\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_none(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- None\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_sort_an_ifc_decl_parameter(universal)) {
    an_ifc_parameter_sort field = get_ifc_sort_an_ifc_decl_parameter(universal);

    db_print_indent(indent);
    fprintf(f_debug, "sort: %s\n", str_for_an_ifc_parameter_sort(field));
  }  /* if */
  if (has_ifc_type_an_ifc_decl_parameter(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_decl_parameter(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_decl_parameter(const an_ifc_decl_parameter universal)
/*
Given the universal representation of DeclParameter, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "DeclParameter ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_decl_parameter_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_decl_partial_specialization_indent(const an_ifc_decl_partial_specialization universal,
             unsigned                                 indent)
/*
Given the universal representation of DeclPartialSpecialization, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_access_an_ifc_decl_partial_specialization(universal)) {
    an_ifc_access_sort field = get_ifc_access_an_ifc_decl_partial_specialization(universal);

    db_print_indent(indent);
    fprintf(f_debug, "access: %s\n", str_for_an_ifc_access_sort(field));
  }  /* if */
  if (has_ifc_chart_an_ifc_decl_partial_specialization(universal)) {
    an_ifc_chart_index field = get_ifc_chart_an_ifc_decl_partial_specialization(universal);

    db_print_indent(indent);
    fprintf(f_debug, "chart:");
    if (is_null_index_an_ifc_chart_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_chart_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_entity_an_ifc_decl_partial_specialization(universal)) {
    an_ifc_parameterized_entity field = get_ifc_entity_an_ifc_decl_partial_specialization(universal);

    db_print_indent(indent);
    fprintf(f_debug, "entity:\n");
    db_node_an_ifc_parameterized_entity_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_form_an_ifc_decl_partial_specialization(universal)) {
    an_ifc_form_spec_offset field = get_ifc_form_an_ifc_decl_partial_specialization(universal);

    db_print_indent(indent);
    fprintf(f_debug, "form: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_home_scope_an_ifc_decl_partial_specialization(universal)) {
    an_ifc_decl_index field = get_ifc_home_scope_an_ifc_decl_partial_specialization(universal);

    db_print_indent(indent);
    fprintf(f_debug, "home_scope:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_decl_partial_specialization(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_decl_partial_specialization(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_decl_partial_specialization(universal)) {
    an_ifc_name_index field = get_ifc_name_an_ifc_decl_partial_specialization(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name:");
    if (is_null_index_an_ifc_name_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_name_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_primary_template_an_ifc_decl_partial_specialization(universal)) {
    an_ifc_decl_index field = get_ifc_primary_template_an_ifc_decl_partial_specialization(universal);

    db_print_indent(indent);
    fprintf(f_debug, "primary_template:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_properties_an_ifc_decl_partial_specialization(universal)) {
    an_ifc_reachable_properties_bitfield field = get_ifc_properties_an_ifc_decl_partial_specialization(universal);

    fprintf(f_debug, "properties:\n");
    ++indent;
    if (test_bitmask_ifc_rpb_all(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- All\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_attributes(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Attributes\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_default_arguments(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- DefaultArguments\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_initializer(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Initializer\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_none(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- None\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_specifiers_an_ifc_decl_partial_specialization(universal)) {
    an_ifc_basic_specifiers_bitfield field = get_ifc_specifiers_an_ifc_decl_partial_specialization(universal);

    fprintf(f_debug, "specifiers:\n");
    ++indent;
    if (test_bitmask_ifc_bsb_c(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- C\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_cxx(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Cxx\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_deprecated(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Deprecated\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_external(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- External\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_initialized_in_class(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- InitializedInClass\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_internal(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Internal\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_is_member_of_global_module(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- IsMemberOfGlobalModule\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_non_exported(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- NonExported\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_vague(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Vague\n");
    }  /* if */
    --indent;
  }  /* if */
}  /* db_node */


void db_node_an_ifc_decl_partial_specialization(const an_ifc_decl_partial_specialization universal)
/*
Given the universal representation of DeclPartialSpecialization, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================== ");
  fprintf(f_debug, "DeclPartialSpecialization ");
  fprintf(f_debug, "===========================\n");
  db_node_an_ifc_decl_partial_specialization_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_decl_prolongation_indent(const an_ifc_decl_prolongation universal, unsigned indent)
/*
Given the universal representation of DeclProlongation, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_enclosing_scope_an_ifc_decl_prolongation(universal)) {
    an_ifc_decl_index field = get_ifc_enclosing_scope_an_ifc_decl_prolongation(universal);

    db_print_indent(indent);
    fprintf(f_debug, "enclosing_scope:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_home_scope_an_ifc_decl_prolongation(universal)) {
    an_ifc_decl_index field = get_ifc_home_scope_an_ifc_decl_prolongation(universal);

    db_print_indent(indent);
    fprintf(f_debug, "home_scope:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_decl_prolongation(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_decl_prolongation(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_decl_prolongation(universal)) {
    an_ifc_name_index field = get_ifc_name_an_ifc_decl_prolongation(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name:");
    if (is_null_index_an_ifc_name_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_name_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_original_decl_an_ifc_decl_prolongation(universal)) {
    an_ifc_decl_index field = get_ifc_original_decl_an_ifc_decl_prolongation(universal);

    db_print_indent(indent);
    fprintf(f_debug, "original_decl:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_decl_prolongation(const an_ifc_decl_prolongation universal)
/*
Given the universal representation of DeclProlongation, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "=============================== ");
  fprintf(f_debug, "DeclProlongation ");
  fprintf(f_debug, "===============================\n");
  db_node_an_ifc_decl_prolongation_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_decl_property_indent(const an_ifc_decl_property universal, unsigned indent)
/*
Given the universal representation of DeclProperty, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_getter_an_ifc_decl_property(universal)) {
    an_ifc_text_offset field = get_ifc_getter_an_ifc_decl_property(universal);

    db_print_indent(indent);
    fprintf(f_debug, "getter: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_home_scope_an_ifc_decl_property(universal)) {
    an_ifc_decl_index field = get_ifc_home_scope_an_ifc_decl_property(universal);

    db_print_indent(indent);
    fprintf(f_debug, "home_scope:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_member_an_ifc_decl_property(universal)) {
    an_ifc_decl_index field = get_ifc_member_an_ifc_decl_property(universal);

    db_print_indent(indent);
    fprintf(f_debug, "member:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_setter_an_ifc_decl_property(universal)) {
    an_ifc_text_offset field = get_ifc_setter_an_ifc_decl_property(universal);

    db_print_indent(indent);
    fprintf(f_debug, "setter: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_decl_property(const an_ifc_decl_property universal)
/*
Given the universal representation of DeclProperty, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "DeclProperty ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_decl_property_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_decl_reference_indent(const an_ifc_decl_reference universal, unsigned indent)
/*
Given the universal representation of DeclReference, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_index_an_ifc_decl_reference(universal)) {
    an_ifc_decl_index field = get_ifc_index_an_ifc_decl_reference(universal);

    db_print_indent(indent);
    fprintf(f_debug, "index:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_local_index_an_ifc_decl_reference(universal)) {
    db_print_indent(indent);
    fprintf(f_debug, "local_index: UNIMPLEMENTED\n");
  }  /* if */
  if (has_ifc_unit_an_ifc_decl_reference(universal)) {
    an_ifc_module_reference field = get_ifc_unit_an_ifc_decl_reference(universal);

    db_print_indent(indent);
    fprintf(f_debug, "unit:\n");
    db_node_an_ifc_module_reference_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_decl_reference(const an_ifc_decl_reference universal)
/*
Given the universal representation of DeclReference, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "DeclReference ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_decl_reference_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_decl_scope_indent(const an_ifc_decl_scope universal, unsigned indent)
/*
Given the universal representation of DeclScope, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_access_an_ifc_decl_scope(universal)) {
    an_ifc_access_sort field = get_ifc_access_an_ifc_decl_scope(universal);

    db_print_indent(indent);
    fprintf(f_debug, "access: %s\n", str_for_an_ifc_access_sort(field));
  }  /* if */
  if (has_ifc_alignment_an_ifc_decl_scope(universal)) {
    an_ifc_expr_index field = get_ifc_alignment_an_ifc_decl_scope(universal);

    db_print_indent(indent);
    fprintf(f_debug, "alignment:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_base_an_ifc_decl_scope(universal)) {
    an_ifc_type_index field = get_ifc_base_an_ifc_decl_scope(universal);

    db_print_indent(indent);
    fprintf(f_debug, "base:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_home_scope_an_ifc_decl_scope(universal)) {
    an_ifc_decl_index field = get_ifc_home_scope_an_ifc_decl_scope(universal);

    db_print_indent(indent);
    fprintf(f_debug, "home_scope:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_initializer_an_ifc_decl_scope(universal)) {
    an_ifc_scope_offset field = get_ifc_initializer_an_ifc_decl_scope(universal);

    db_print_indent(indent);
    fprintf(f_debug, "initializer: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_locus_an_ifc_decl_scope(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_decl_scope(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_decl_scope(universal)) {
    an_ifc_name_index field = get_ifc_name_an_ifc_decl_scope(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name:");
    if (is_null_index_an_ifc_name_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_name_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_pack_size_an_ifc_decl_scope(universal)) {
    an_ifc_pack_size field = get_ifc_pack_size_an_ifc_decl_scope(universal);

    db_print_indent(indent);
    fprintf(f_debug, "pack_size: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_properties_an_ifc_decl_scope(universal)) {
    an_ifc_reachable_properties_bitfield field = get_ifc_properties_an_ifc_decl_scope(universal);

    fprintf(f_debug, "properties:\n");
    ++indent;
    if (test_bitmask_ifc_rpb_all(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- All\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_attributes(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Attributes\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_default_arguments(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- DefaultArguments\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_initializer(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Initializer\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_none(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- None\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_specifiers_an_ifc_decl_scope(universal)) {
    an_ifc_basic_specifiers_bitfield field = get_ifc_specifiers_an_ifc_decl_scope(universal);

    fprintf(f_debug, "specifiers:\n");
    ++indent;
    if (test_bitmask_ifc_bsb_c(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- C\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_cxx(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Cxx\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_deprecated(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Deprecated\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_external(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- External\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_initialized_in_class(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- InitializedInClass\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_internal(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Internal\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_is_member_of_global_module(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- IsMemberOfGlobalModule\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_non_exported(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- NonExported\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_vague(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Vague\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_traits_an_ifc_decl_scope(universal)) {
    an_ifc_scope_traits_bitfield field = get_ifc_traits_an_ifc_decl_scope(universal);

    fprintf(f_debug, "traits:\n");
    ++indent;
    if (test_bitmask_ifc_stb_closure_type(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- ClosureType\n");
    }  /* if */
    if (test_bitmask_ifc_stb_final(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Final\n");
    }  /* if */
    if (test_bitmask_ifc_stb_initializer_exported(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- InitializerExported\n");
    }  /* if */
    if (test_bitmask_ifc_stb_inline(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Inline\n");
    }  /* if */
    if (test_bitmask_ifc_stb_none(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- None\n");
    }  /* if */
    if (test_bitmask_ifc_stb_unnamed(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Unnamed\n");
    }  /* if */
    if (test_bitmask_ifc_stb_vendor(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Vendor\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_type_an_ifc_decl_scope(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_decl_scope(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_decl_scope(const an_ifc_decl_scope universal)
/*
Given the universal representation of DeclScope, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "DeclScope ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_decl_scope_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_decl_specialization_indent(const an_ifc_decl_specialization universal, unsigned indent)
/*
Given the universal representation of DeclSpecialization, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_decl_an_ifc_decl_specialization(universal)) {
    an_ifc_decl_index field = get_ifc_decl_an_ifc_decl_specialization(universal);

    db_print_indent(indent);
    fprintf(f_debug, "decl:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_form_an_ifc_decl_specialization(universal)) {
    an_ifc_form_spec_offset field = get_ifc_form_an_ifc_decl_specialization(universal);

    db_print_indent(indent);
    fprintf(f_debug, "form: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_home_scope_an_ifc_decl_specialization(universal)) {
    an_ifc_decl_index field = get_ifc_home_scope_an_ifc_decl_specialization(universal);

    db_print_indent(indent);
    fprintf(f_debug, "home_scope:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_decl_specialization(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_decl_specialization(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_decl_specialization(universal)) {
    an_ifc_name_index field = get_ifc_name_an_ifc_decl_specialization(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name:");
    if (is_null_index_an_ifc_name_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_name_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_primary_template_an_ifc_decl_specialization(universal)) {
    an_ifc_decl_index field = get_ifc_primary_template_an_ifc_decl_specialization(universal);

    db_print_indent(indent);
    fprintf(f_debug, "primary_template:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_sort_an_ifc_decl_specialization(universal)) {
    an_ifc_specialization_sort field = get_ifc_sort_an_ifc_decl_specialization(universal);

    db_print_indent(indent);
    fprintf(f_debug, "sort: %s\n", str_for_an_ifc_specialization_sort(field));
  }  /* if */
}  /* db_node */


void db_node_an_ifc_decl_specialization(const an_ifc_decl_specialization universal)
/*
Given the universal representation of DeclSpecialization, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================== ");
  fprintf(f_debug, "DeclSpecialization ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_decl_specialization_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_decl_syntax_tree_indent(const an_ifc_decl_syntax_tree universal, unsigned indent)
/*
Given the universal representation of DeclSyntaxTree, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_syntax_an_ifc_decl_syntax_tree(universal)) {
    an_ifc_syntax_index field = get_ifc_syntax_an_ifc_decl_syntax_tree(universal);

    db_print_indent(indent);
    fprintf(f_debug, "syntax:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_decl_syntax_tree(const an_ifc_decl_syntax_tree universal)
/*
Given the universal representation of DeclSyntaxTree, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "DeclSyntaxTree ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_decl_syntax_tree_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_decl_template_indent(const an_ifc_decl_template universal, unsigned indent)
/*
Given the universal representation of DeclTemplate, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_access_an_ifc_decl_template(universal)) {
    an_ifc_access_sort field = get_ifc_access_an_ifc_decl_template(universal);

    db_print_indent(indent);
    fprintf(f_debug, "access: %s\n", str_for_an_ifc_access_sort(field));
  }  /* if */
  if (has_ifc_chart_an_ifc_decl_template(universal)) {
    an_ifc_chart_index field = get_ifc_chart_an_ifc_decl_template(universal);

    db_print_indent(indent);
    fprintf(f_debug, "chart:");
    if (is_null_index_an_ifc_chart_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_chart_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_entity_an_ifc_decl_template(universal)) {
    an_ifc_parameterized_entity field = get_ifc_entity_an_ifc_decl_template(universal);

    db_print_indent(indent);
    fprintf(f_debug, "entity:\n");
    db_node_an_ifc_parameterized_entity_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_home_scope_an_ifc_decl_template(universal)) {
    an_ifc_decl_index field = get_ifc_home_scope_an_ifc_decl_template(universal);

    db_print_indent(indent);
    fprintf(f_debug, "home_scope:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_decl_template(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_decl_template(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_decl_template(universal)) {
    an_ifc_name_index field = get_ifc_name_an_ifc_decl_template(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name:");
    if (is_null_index_an_ifc_name_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_name_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_properties_an_ifc_decl_template(universal)) {
    an_ifc_reachable_properties_bitfield field = get_ifc_properties_an_ifc_decl_template(universal);

    fprintf(f_debug, "properties:\n");
    ++indent;
    if (test_bitmask_ifc_rpb_all(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- All\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_attributes(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Attributes\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_default_arguments(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- DefaultArguments\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_initializer(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Initializer\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_none(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- None\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_specifiers_an_ifc_decl_template(universal)) {
    an_ifc_basic_specifiers_bitfield field = get_ifc_specifiers_an_ifc_decl_template(universal);

    fprintf(f_debug, "specifiers:\n");
    ++indent;
    if (test_bitmask_ifc_bsb_c(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- C\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_cxx(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Cxx\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_deprecated(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Deprecated\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_external(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- External\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_initialized_in_class(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- InitializedInClass\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_internal(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Internal\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_is_member_of_global_module(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- IsMemberOfGlobalModule\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_non_exported(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- NonExported\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_vague(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Vague\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_type_an_ifc_decl_template(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_decl_template(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_decl_template(const an_ifc_decl_template universal)
/*
Given the universal representation of DeclTemplate, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "DeclTemplate ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_decl_template_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_decl_temploid_indent(const an_ifc_decl_temploid universal, unsigned indent)
/*
Given the universal representation of DeclTemploid, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_chart_an_ifc_decl_temploid(universal)) {
    an_ifc_chart_index field = get_ifc_chart_an_ifc_decl_temploid(universal);

    db_print_indent(indent);
    fprintf(f_debug, "chart:");
    if (is_null_index_an_ifc_chart_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_chart_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_entity_an_ifc_decl_temploid(universal)) {
    an_ifc_parameterized_entity field = get_ifc_entity_an_ifc_decl_temploid(universal);

    db_print_indent(indent);
    fprintf(f_debug, "entity:\n");
    db_node_an_ifc_parameterized_entity_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_properties_an_ifc_decl_temploid(universal)) {
    an_ifc_reachable_properties_bitfield field = get_ifc_properties_an_ifc_decl_temploid(universal);

    fprintf(f_debug, "properties:\n");
    ++indent;
    if (test_bitmask_ifc_rpb_all(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- All\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_attributes(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Attributes\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_default_arguments(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- DefaultArguments\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_initializer(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Initializer\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_none(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- None\n");
    }  /* if */
    --indent;
  }  /* if */
}  /* db_node */


void db_node_an_ifc_decl_temploid(const an_ifc_decl_temploid universal)
/*
Given the universal representation of DeclTemploid, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "DeclTemploid ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_decl_temploid_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_decl_tuple_indent(const an_ifc_decl_tuple universal, unsigned indent)
/*
Given the universal representation of DeclTuple, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_cardinality_an_ifc_decl_tuple(universal)) {
    an_ifc_cardinality field = get_ifc_cardinality_an_ifc_decl_tuple(universal);

    db_print_indent(indent);
    fprintf(f_debug, "cardinality: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_start_an_ifc_decl_tuple(universal)) {
    an_ifc_index field = get_ifc_start_an_ifc_decl_tuple(universal);

    db_print_indent(indent);
    fprintf(f_debug, "start: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_decl_tuple(const an_ifc_decl_tuple universal)
/*
Given the universal representation of DeclTuple, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "DeclTuple ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_decl_tuple_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_decl_using_declaration_indent(const an_ifc_decl_using_declaration universal, unsigned indent)
/*
Given the universal representation of DeclUsingDeclaration, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_access_an_ifc_decl_using_declaration(universal)) {
    an_ifc_access_sort field = get_ifc_access_an_ifc_decl_using_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "access: %s\n", str_for_an_ifc_access_sort(field));
  }  /* if */
  if (has_ifc_hidden_an_ifc_decl_using_declaration(universal)) {
    an_ifc_bool field = get_ifc_hidden_an_ifc_decl_using_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "hidden: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_home_scope_an_ifc_decl_using_declaration(universal)) {
    an_ifc_decl_index field = get_ifc_home_scope_an_ifc_decl_using_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "home_scope:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_decl_using_declaration(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_decl_using_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_decl_using_declaration(universal)) {
    an_ifc_text_offset field = get_ifc_name_an_ifc_decl_using_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_name2_an_ifc_decl_using_declaration(universal)) {
    an_ifc_text_offset field = get_ifc_name2_an_ifc_decl_using_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name2: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_parent_an_ifc_decl_using_declaration(universal)) {
    an_ifc_expr_index field = get_ifc_parent_an_ifc_decl_using_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "parent:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_resolution_an_ifc_decl_using_declaration(universal)) {
    an_ifc_decl_index field = get_ifc_resolution_an_ifc_decl_using_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "resolution:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_specifiers_an_ifc_decl_using_declaration(universal)) {
    an_ifc_basic_specifiers_bitfield field = get_ifc_specifiers_an_ifc_decl_using_declaration(universal);

    fprintf(f_debug, "specifiers:\n");
    ++indent;
    if (test_bitmask_ifc_bsb_c(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- C\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_cxx(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Cxx\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_deprecated(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Deprecated\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_external(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- External\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_initialized_in_class(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- InitializedInClass\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_internal(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Internal\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_is_member_of_global_module(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- IsMemberOfGlobalModule\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_non_exported(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- NonExported\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_vague(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Vague\n");
    }  /* if */
    --indent;
  }  /* if */
}  /* db_node */


void db_node_an_ifc_decl_using_declaration(const an_ifc_decl_using_declaration universal)
/*
Given the universal representation of DeclUsingDeclaration, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================= ");
  fprintf(f_debug, "DeclUsingDeclaration ");
  fprintf(f_debug, "=============================\n");
  db_node_an_ifc_decl_using_declaration_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_decl_variable_indent(const an_ifc_decl_variable universal, unsigned indent)
/*
Given the universal representation of DeclVariable, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_access_an_ifc_decl_variable(universal)) {
    an_ifc_access_sort field = get_ifc_access_an_ifc_decl_variable(universal);

    db_print_indent(indent);
    fprintf(f_debug, "access: %s\n", str_for_an_ifc_access_sort(field));
  }  /* if */
  if (has_ifc_alignment_an_ifc_decl_variable(universal)) {
    an_ifc_expr_index field = get_ifc_alignment_an_ifc_decl_variable(universal);

    db_print_indent(indent);
    fprintf(f_debug, "alignment:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_home_scope_an_ifc_decl_variable(universal)) {
    an_ifc_decl_index field = get_ifc_home_scope_an_ifc_decl_variable(universal);

    db_print_indent(indent);
    fprintf(f_debug, "home_scope:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_initializer_an_ifc_decl_variable(universal)) {
    an_ifc_expr_index field = get_ifc_initializer_an_ifc_decl_variable(universal);

    db_print_indent(indent);
    fprintf(f_debug, "initializer:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_decl_variable(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_decl_variable(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_decl_variable(universal)) {
    an_ifc_name_index field = get_ifc_name_an_ifc_decl_variable(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name:");
    if (is_null_index_an_ifc_name_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_name_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_properties_an_ifc_decl_variable(universal)) {
    an_ifc_reachable_properties_bitfield field = get_ifc_properties_an_ifc_decl_variable(universal);

    fprintf(f_debug, "properties:\n");
    ++indent;
    if (test_bitmask_ifc_rpb_all(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- All\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_attributes(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Attributes\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_default_arguments(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- DefaultArguments\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_initializer(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Initializer\n");
    }  /* if */
    if (test_bitmask_ifc_rpb_none(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- None\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_specifiers_an_ifc_decl_variable(universal)) {
    an_ifc_basic_specifiers_bitfield field = get_ifc_specifiers_an_ifc_decl_variable(universal);

    fprintf(f_debug, "specifiers:\n");
    ++indent;
    if (test_bitmask_ifc_bsb_c(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- C\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_cxx(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Cxx\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_deprecated(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Deprecated\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_external(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- External\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_initialized_in_class(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- InitializedInClass\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_internal(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Internal\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_is_member_of_global_module(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- IsMemberOfGlobalModule\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_non_exported(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- NonExported\n");
    }  /* if */
    if (test_bitmask_ifc_bsb_vague(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Vague\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_traits_an_ifc_decl_variable(universal)) {
    an_ifc_object_traits_bitfield field = get_ifc_traits_an_ifc_decl_variable(universal);

    fprintf(f_debug, "traits:\n");
    ++indent;
    if (test_bitmask_ifc_otb_constexpr(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Constexpr\n");
    }  /* if */
    if (test_bitmask_ifc_otb_initializer_exported(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- InitializerExported\n");
    }  /* if */
    if (test_bitmask_ifc_otb_inline(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Inline\n");
    }  /* if */
    if (test_bitmask_ifc_otb_mutable(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Mutable\n");
    }  /* if */
    if (test_bitmask_ifc_otb_none(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- None\n");
    }  /* if */
    if (test_bitmask_ifc_otb_thread_local(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- ThreadLocal\n");
    }  /* if */
    if (test_bitmask_ifc_otb_vendor(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Vendor\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_type_an_ifc_decl_variable(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_decl_variable(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_decl_variable(const an_ifc_decl_variable universal)
/*
Given the universal representation of DeclVariable, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "DeclVariable ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_decl_variable_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_dir_attribute_indent(const an_ifc_dir_attribute universal, unsigned indent)
/*
Given the universal representation of DirAttribute, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_attr_an_ifc_dir_attribute(universal)) {
    an_ifc_attr_index field = get_ifc_attr_an_ifc_dir_attribute(universal);

    db_print_indent(indent);
    fprintf(f_debug, "attr:");
    if (is_null_index_an_ifc_attr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_attr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_dir_attribute(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_dir_attribute(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_dir_attribute(const an_ifc_dir_attribute universal)
/*
Given the universal representation of DirAttribute, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "DirAttribute ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_dir_attribute_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_dir_decl_use_indent(const an_ifc_dir_decl_use universal, unsigned indent)
/*
Given the universal representation of DirDeclUse, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_dir_decl_use(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_dir_decl_use(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_path_an_ifc_dir_decl_use(universal)) {
    an_ifc_expr_index field = get_ifc_path_an_ifc_dir_decl_use(universal);

    db_print_indent(indent);
    fprintf(f_debug, "path:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_result_an_ifc_dir_decl_use(universal)) {
    an_ifc_decl_index field = get_ifc_result_an_ifc_dir_decl_use(universal);

    db_print_indent(indent);
    fprintf(f_debug, "result:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_dir_decl_use(const an_ifc_dir_decl_use universal)
/*
Given the universal representation of DirDeclUse, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "DirDeclUse ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_dir_decl_use_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_dir_empty_indent(const an_ifc_dir_empty universal, unsigned indent)
/*
Given the universal representation of DirEmpty, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_dir_empty(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_dir_empty(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_dir_empty(const an_ifc_dir_empty universal)
/*
Given the universal representation of DirEmpty, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "=================================== ");
  fprintf(f_debug, "DirEmpty ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_dir_empty_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_dir_expr_indent(const an_ifc_dir_expr universal, unsigned indent)
/*
Given the universal representation of DirExpr, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_expr_an_ifc_dir_expr(universal)) {
    an_ifc_expr_index field = get_ifc_expr_an_ifc_dir_expr(universal);

    db_print_indent(indent);
    fprintf(f_debug, "expr:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_dir_expr(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_dir_expr(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_phases_an_ifc_dir_expr(universal)) {
    an_ifc_phases_bitfield field = get_ifc_phases_an_ifc_dir_expr(universal);

    fprintf(f_debug, "phases:\n");
    ++indent;
    if (test_bitmask_ifc_pb_analysis(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Analysis\n");
    }  /* if */
    if (test_bitmask_ifc_pb_code_generation(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- CodeGeneration\n");
    }  /* if */
    if (test_bitmask_ifc_pb_evaluation(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Evaluation\n");
    }  /* if */
    if (test_bitmask_ifc_pb_execution(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Execution\n");
    }  /* if */
    if (test_bitmask_ifc_pb_importing(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Importing\n");
    }  /* if */
    if (test_bitmask_ifc_pb_instantiation(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Instantiation\n");
    }  /* if */
    if (test_bitmask_ifc_pb_lexing(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Lexing\n");
    }  /* if */
    if (test_bitmask_ifc_pb_linking(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Linking\n");
    }  /* if */
    if (test_bitmask_ifc_pb_loading(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Loading\n");
    }  /* if */
    if (test_bitmask_ifc_pb_name_resolution(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- NameResolution\n");
    }  /* if */
    if (test_bitmask_ifc_pb_parsing(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Parsing\n");
    }  /* if */
    if (test_bitmask_ifc_pb_preprocessing(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Preprocessing\n");
    }  /* if */
    if (test_bitmask_ifc_pb_reading(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Reading\n");
    }  /* if */
    if (test_bitmask_ifc_pb_typing(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Typing\n");
    }  /* if */
    if (test_bitmask_ifc_pb_unknown(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Unknown\n");
    }  /* if */
    --indent;
  }  /* if */
}  /* db_node */


void db_node_an_ifc_dir_expr(const an_ifc_dir_expr universal)
/*
Given the universal representation of DirExpr, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "=================================== ");
  fprintf(f_debug, "DirExpr ");
  fprintf(f_debug, "====================================\n");
  db_node_an_ifc_dir_expr_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_dir_pragma_indent(const an_ifc_dir_pragma universal, unsigned indent)
/*
Given the universal representation of DirPragma, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_dir_pragma(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_dir_pragma(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_words_an_ifc_dir_pragma(universal)) {
    an_ifc_sentence_index field = get_ifc_words_an_ifc_dir_pragma(universal);

    db_print_indent(indent);
    fprintf(f_debug, "words: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_dir_pragma(const an_ifc_dir_pragma universal)
/*
Given the universal representation of DirPragma, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "DirPragma ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_dir_pragma_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_dir_tuple_indent(const an_ifc_dir_tuple universal, unsigned indent)
/*
Given the universal representation of DirTuple, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_cardinality_an_ifc_dir_tuple(universal)) {
    an_ifc_cardinality field = get_ifc_cardinality_an_ifc_dir_tuple(universal);

    db_print_indent(indent);
    fprintf(f_debug, "cardinality: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_start_an_ifc_dir_tuple(universal)) {
    an_ifc_index field = get_ifc_start_an_ifc_dir_tuple(universal);

    db_print_indent(indent);
    fprintf(f_debug, "start: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_dir_tuple(const an_ifc_dir_tuple universal)
/*
Given the universal representation of DirTuple, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "=================================== ");
  fprintf(f_debug, "DirTuple ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_dir_tuple_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_dir_using_indent(const an_ifc_dir_using universal, unsigned indent)
/*
Given the universal representation of DirUsing, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_dir_using(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_dir_using(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_nominated_an_ifc_dir_using(universal)) {
    an_ifc_expr_index field = get_ifc_nominated_an_ifc_dir_using(universal);

    db_print_indent(indent);
    fprintf(f_debug, "nominated:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_resolution_an_ifc_dir_using(universal)) {
    an_ifc_decl_index field = get_ifc_resolution_an_ifc_dir_using(universal);

    db_print_indent(indent);
    fprintf(f_debug, "resolution:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_dir_using(const an_ifc_dir_using universal)
/*
Given the universal representation of DirUsing, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "=================================== ");
  fprintf(f_debug, "DirUsing ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_dir_using_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_edg_constant_integer_indent(const an_ifc_edg_constant_integer universal, unsigned indent)
/*
Given the universal representation of EdgConstantInteger, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_cardinality_an_ifc_edg_constant_integer(universal)) {
    an_ifc_cardinality field = get_ifc_cardinality_an_ifc_edg_constant_integer(universal);

    db_print_indent(indent);
    fprintf(f_debug, "cardinality: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_start_an_ifc_edg_constant_integer(universal)) {
    an_ifc_edg_constant_integer_word_offset field = get_ifc_start_an_ifc_edg_constant_integer(universal);

    db_print_indent(indent);
    fprintf(f_debug, "start: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_type_an_ifc_edg_constant_integer(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_edg_constant_integer(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_edg_constant_integer(const an_ifc_edg_constant_integer universal)
/*
Given the universal representation of EdgConstantInteger, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================== ");
  fprintf(f_debug, "EdgConstantInteger ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_edg_constant_integer_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_edg_constant_integer_word_indent(const an_ifc_edg_constant_integer_word universal,
             unsigned                               indent)
/*
Given the universal representation of EdgConstantIntegerWord, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_bytes_an_ifc_edg_constant_integer_word(universal)) {
    db_print_indent(indent);
    fprintf(f_debug, "bytes: UNIMPLEMENTED\n");
  }  /* if */
}  /* db_node */


void db_node_an_ifc_edg_constant_integer_word(const an_ifc_edg_constant_integer_word universal)
/*
Given the universal representation of EdgConstantIntegerWord, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "============================ ");
  fprintf(f_debug, "EdgConstantIntegerWord ");
  fprintf(f_debug, "============================\n");
  db_node_an_ifc_edg_constant_integer_word_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_edg_expr_template_argument_indent(const an_ifc_edg_expr_template_argument universal,
             unsigned                                indent)
/*
Given the universal representation of EdgExprTemplateArgument, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_index_an_ifc_edg_expr_template_argument(universal)) {
    an_ifc_edg_template_argument_index field = get_ifc_index_an_ifc_edg_expr_template_argument(universal);

    db_print_indent(indent);
    fprintf(f_debug, "index:");
    if (is_null_index_an_ifc_edg_template_argument_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_edg_template_argument_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_edg_expr_template_argument(const an_ifc_edg_expr_template_argument universal)
/*
Given the universal representation of EdgExprTemplateArgument, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "=========================== ");
  fprintf(f_debug, "EdgExprTemplateArgument ");
  fprintf(f_debug, "============================\n");
  db_node_an_ifc_edg_expr_template_argument_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_edg_extension_expr_indent(const an_ifc_edg_extension_expr universal, unsigned indent)
/*
Given the universal representation of EdgExtensionExpr, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_value_an_ifc_edg_extension_expr(universal)) {
    an_ifc_edg_expr_index field = get_ifc_value_an_ifc_edg_extension_expr(universal);

    db_print_indent(indent);
    fprintf(f_debug, "value:");
    if (is_null_index_an_ifc_edg_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_edg_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_edg_extension_expr(const an_ifc_edg_extension_expr universal)
/*
Given the universal representation of EdgExtensionExpr, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "=============================== ");
  fprintf(f_debug, "EdgExtensionExpr ");
  fprintf(f_debug, "===============================\n");
  db_node_an_ifc_edg_extension_expr_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_edg_extension_type_indent(const an_ifc_edg_extension_type universal, unsigned indent)
/*
Given the universal representation of EdgExtensionType, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_value_an_ifc_edg_extension_type(universal)) {
    an_ifc_edg_type_index field = get_ifc_value_an_ifc_edg_extension_type(universal);

    db_print_indent(indent);
    fprintf(f_debug, "value:");
    if (is_null_index_an_ifc_edg_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_edg_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_edg_extension_type(const an_ifc_edg_extension_type universal)
/*
Given the universal representation of EdgExtensionType, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "=============================== ");
  fprintf(f_debug, "EdgExtensionType ");
  fprintf(f_debug, "===============================\n");
  db_node_an_ifc_edg_extension_type_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_edg_heap_complex_token_indent(const an_ifc_edg_heap_complex_token universal, unsigned indent)
/*
Given the universal representation of EdgHeapComplexToken, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_index_an_ifc_edg_heap_complex_token(universal)) {
    an_ifc_edg_complex_token_index field = get_ifc_index_an_ifc_edg_heap_complex_token(universal);

    db_print_indent(indent);
    fprintf(f_debug, "index:");
    if (is_null_index_an_ifc_edg_complex_token_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_edg_complex_token_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_edg_heap_complex_token(const an_ifc_edg_heap_complex_token universal)
/*
Given the universal representation of EdgHeapComplexToken, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================= ");
  fprintf(f_debug, "EdgHeapComplexToken ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_edg_heap_complex_token_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_edg_heap_template_argument_indent(const an_ifc_edg_heap_template_argument universal,
             unsigned                                indent)
/*
Given the universal representation of EdgHeapTemplateArgument, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_index_an_ifc_edg_heap_template_argument(universal)) {
    an_ifc_edg_template_argument_index field = get_ifc_index_an_ifc_edg_heap_template_argument(universal);

    db_print_indent(indent);
    fprintf(f_debug, "index:");
    if (is_null_index_an_ifc_edg_template_argument_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_edg_template_argument_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_edg_heap_template_argument(const an_ifc_edg_heap_template_argument universal)
/*
Given the universal representation of EdgHeapTemplateArgument, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "=========================== ");
  fprintf(f_debug, "EdgHeapTemplateArgument ");
  fprintf(f_debug, "============================\n");
  db_node_an_ifc_edg_heap_template_argument_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_edg_template_argument_constant_indent(const an_ifc_edg_template_argument_constant universal,
             unsigned                                    indent)
/*
Given the universal representation of EdgTemplateArgumentConstant, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_value_an_ifc_edg_template_argument_constant(universal)) {
    an_ifc_edg_constant_index field = get_ifc_value_an_ifc_edg_template_argument_constant(universal);

    db_print_indent(indent);
    fprintf(f_debug, "value:");
    if (is_null_index_an_ifc_edg_constant_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_edg_constant_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_edg_template_argument_constant(const an_ifc_edg_template_argument_constant universal)
/*
Given the universal representation of EdgTemplateArgumentConstant, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================= ");
  fprintf(f_debug, "EdgTemplateArgumentConstant ");
  fprintf(f_debug, "==========================\n");
  db_node_an_ifc_edg_template_argument_constant_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_edg_template_argument_non_type_indent(const an_ifc_edg_template_argument_non_type universal,
             unsigned                                    indent)
/*
Given the universal representation of EdgTemplateArgumentNonType, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_value_an_ifc_edg_template_argument_non_type(universal)) {
    an_ifc_expr_index field = get_ifc_value_an_ifc_edg_template_argument_non_type(universal);

    db_print_indent(indent);
    fprintf(f_debug, "value:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_edg_template_argument_non_type(const an_ifc_edg_template_argument_non_type universal)
/*
Given the universal representation of EdgTemplateArgumentNonType, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================== ");
  fprintf(f_debug, "EdgTemplateArgumentNonType ");
  fprintf(f_debug, "==========================\n");
  db_node_an_ifc_edg_template_argument_non_type_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_edg_template_argument_template_indent(const an_ifc_edg_template_argument_template universal,
             unsigned                                    indent)
/*
Given the universal representation of EdgTemplateArgumentTemplate, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_value_an_ifc_edg_template_argument_template(universal)) {
    an_ifc_decl_index field = get_ifc_value_an_ifc_edg_template_argument_template(universal);

    db_print_indent(indent);
    fprintf(f_debug, "value:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_edg_template_argument_template(const an_ifc_edg_template_argument_template universal)
/*
Given the universal representation of EdgTemplateArgumentTemplate, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================= ");
  fprintf(f_debug, "EdgTemplateArgumentTemplate ");
  fprintf(f_debug, "==========================\n");
  db_node_an_ifc_edg_template_argument_template_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_edg_template_argument_type_indent(const an_ifc_edg_template_argument_type universal,
             unsigned                                indent)
/*
Given the universal representation of EdgTemplateArgumentType, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_value_an_ifc_edg_template_argument_type(universal)) {
    an_ifc_type_index field = get_ifc_value_an_ifc_edg_template_argument_type(universal);

    db_print_indent(indent);
    fprintf(f_debug, "value:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_edg_template_argument_type(const an_ifc_edg_template_argument_type universal)
/*
Given the universal representation of EdgTemplateArgumentType, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "=========================== ");
  fprintf(f_debug, "EdgTemplateArgumentType ");
  fprintf(f_debug, "============================\n");
  db_node_an_ifc_edg_template_argument_type_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_edg_token_basic_indent(const an_ifc_edg_token_basic universal, unsigned indent)
/*
Given the universal representation of EdgTokenBasic, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_kind_an_ifc_edg_token_basic(universal)) {
    an_ifc_edg_basic_token_sort field = get_ifc_kind_an_ifc_edg_token_basic(universal);

    db_print_indent(indent);
    fprintf(f_debug, "kind: %s\n", str_for_an_ifc_edg_basic_token_sort(field));
  }  /* if */
}  /* db_node */


void db_node_an_ifc_edg_token_basic(const an_ifc_edg_token_basic universal)
/*
Given the universal representation of EdgTokenBasic, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "EdgTokenBasic ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_edg_token_basic_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_edg_token_cache_indent(const an_ifc_edg_token_cache universal, unsigned indent)
/*
Given the universal representation of EdgTokenCache, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_complex_tokens_an_ifc_edg_token_cache(universal)) {
    an_ifc_edg_heap_complex_token_offset field =
                                             get_ifc_complex_tokens_an_ifc_edg_token_cache(universal);

    db_print_indent(indent);
    fprintf(f_debug, "complex_tokens: %llu\n",
            (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_num_complex_tokens_an_ifc_edg_token_cache(universal)) {
    an_ifc_cardinality field = get_ifc_num_complex_tokens_an_ifc_edg_token_cache(universal);

    db_print_indent(indent);
    fprintf(f_debug, "num_complex_tokens: %llu\n",
            (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_num_tokens_an_ifc_edg_token_cache(universal)) {
    an_ifc_cardinality field = get_ifc_num_tokens_an_ifc_edg_token_cache(universal);

    db_print_indent(indent);
    fprintf(f_debug, "num_tokens: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_tokens_an_ifc_edg_token_cache(universal)) {
    an_ifc_edg_token_basic_offset field = get_ifc_tokens_an_ifc_edg_token_cache(universal);

    db_print_indent(indent);
    fprintf(f_debug, "tokens: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_edg_token_cache(const an_ifc_edg_token_cache universal)
/*
Given the universal representation of EdgTokenCache, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "EdgTokenCache ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_edg_token_cache_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_edg_token_constant_indent(const an_ifc_edg_token_constant universal, unsigned indent)
/*
Given the universal representation of EdgTokenConstant, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_constant_an_ifc_edg_token_constant(universal)) {
    an_ifc_edg_constant_index field = get_ifc_constant_an_ifc_edg_token_constant(universal);

    db_print_indent(indent);
    fprintf(f_debug, "constant:");
    if (is_null_index_an_ifc_edg_constant_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_edg_constant_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_kind_an_ifc_edg_token_constant(universal)) {
    an_ifc_edg_constant_token_sort field = get_ifc_kind_an_ifc_edg_token_constant(universal);

    db_print_indent(indent);
    fprintf(f_debug, "kind: %s\n", str_for_an_ifc_edg_constant_token_sort(field));
  }  /* if */
}  /* db_node */


void db_node_an_ifc_edg_token_constant(const an_ifc_edg_token_constant universal)
/*
Given the universal representation of EdgTokenConstant, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "=============================== ");
  fprintf(f_debug, "EdgTokenConstant ");
  fprintf(f_debug, "===============================\n");
  db_node_an_ifc_edg_token_constant_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_edg_token_identifier_indent(const an_ifc_edg_token_identifier universal, unsigned indent)
/*
Given the universal representation of EdgTokenIdentifier, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_text_an_ifc_edg_token_identifier(universal)) {
    an_ifc_text_offset field = get_ifc_text_an_ifc_edg_token_identifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "text: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_edg_token_identifier(const an_ifc_edg_token_identifier universal)
/*
Given the universal representation of EdgTokenIdentifier, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================== ");
  fprintf(f_debug, "EdgTokenIdentifier ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_edg_token_identifier_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_edg_token_textual_indent(const an_ifc_edg_token_textual universal, unsigned indent)
/*
Given the universal representation of EdgTokenTextual, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_characters_an_ifc_edg_token_textual(universal)) {
    an_ifc_text_offset field = get_ifc_characters_an_ifc_edg_token_textual(universal);

    db_print_indent(indent);
    fprintf(f_debug, "characters: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_edg_token_textual(const an_ifc_edg_token_textual universal)
/*
Given the universal representation of EdgTokenTextual, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "=============================== ");
  fprintf(f_debug, "EdgTokenTextual ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_edg_token_textual_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_edg_trait_class_template_definition_indent(const an_ifc_edg_trait_class_template_definition universal,
             unsigned                                         indent)
/*
Given the universal representation of EdgTraitClassTemplateDefinition, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_decl_an_ifc_edg_trait_class_template_definition(universal)) {
    an_ifc_decl_index field = get_ifc_decl_an_ifc_edg_trait_class_template_definition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "decl:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_encoded_decl_an_ifc_edg_trait_class_template_definition(universal)) {
    an_ifc_encoded_decl_index field = get_ifc_encoded_decl_an_ifc_edg_trait_class_template_definition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "encoded_decl: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_initializer_an_ifc_edg_trait_class_template_definition(universal)) {
    an_ifc_edg_token_cache_offset field = get_ifc_initializer_an_ifc_edg_trait_class_template_definition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "initializer: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_edg_trait_class_template_definition(const an_ifc_edg_trait_class_template_definition universal)
/*
Given the universal representation of EdgTraitClassTemplateDefinition, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "======================= ");
  fprintf(f_debug, "EdgTraitClassTemplateDefinition ");
  fprintf(f_debug, "========================\n");
  db_node_an_ifc_edg_trait_class_template_definition_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_edg_trait_function_definition_indent(const an_ifc_edg_trait_function_definition universal,
             unsigned                                   indent)
/*
Given the universal representation of EdgTraitFunctionDefinition, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_decl_an_ifc_edg_trait_function_definition(universal)) {
    an_ifc_decl_index field = get_ifc_decl_an_ifc_edg_trait_function_definition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "decl:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_encoded_decl_an_ifc_edg_trait_function_definition(universal)) {
    an_ifc_encoded_decl_index field = get_ifc_encoded_decl_an_ifc_edg_trait_function_definition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "encoded_decl: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_initializer_an_ifc_edg_trait_function_definition(universal)) {
    an_ifc_edg_token_cache_offset field = get_ifc_initializer_an_ifc_edg_trait_function_definition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "initializer: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_edg_trait_function_definition(const an_ifc_edg_trait_function_definition universal)
/*
Given the universal representation of EdgTraitFunctionDefinition, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================== ");
  fprintf(f_debug, "EdgTraitFunctionDefinition ");
  fprintf(f_debug, "==========================\n");
  db_node_an_ifc_edg_trait_function_definition_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_edg_type_substituted_indent(const an_ifc_edg_type_substituted universal, unsigned indent)
/*
Given the universal representation of EdgTypeSubstituted, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_arguments_an_ifc_edg_type_substituted(universal)) {
    an_ifc_edg_heap_template_argument_offset field =
                                                  get_ifc_arguments_an_ifc_edg_type_substituted(universal);

    db_print_indent(indent);
    fprintf(f_debug, "arguments: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_num_arguments_an_ifc_edg_type_substituted(universal)) {
    an_ifc_cardinality field = get_ifc_num_arguments_an_ifc_edg_type_substituted(universal);

    db_print_indent(indent);
    fprintf(f_debug, "num_arguments: %llu\n",
            (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_subject_an_ifc_edg_type_substituted(universal)) {
    an_ifc_decl_index field = get_ifc_subject_an_ifc_edg_type_substituted(universal);

    db_print_indent(indent);
    fprintf(f_debug, "subject:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_edg_type_substituted(const an_ifc_edg_type_substituted universal)
/*
Given the universal representation of EdgTypeSubstituted, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================== ");
  fprintf(f_debug, "EdgTypeSubstituted ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_edg_type_substituted_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_alignof_indent(const an_ifc_expr_alignof universal, unsigned indent)
/*
Given the universal representation of ExprAlignof, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_expr_alignof(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_alignof(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_operand_an_ifc_expr_alignof(universal)) {
    an_ifc_syntax_index field = get_ifc_operand_an_ifc_expr_alignof(universal);

    db_print_indent(indent);
    fprintf(f_debug, "operand:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_type_an_ifc_expr_alignof(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_alignof(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_alignof(const an_ifc_expr_alignof universal)
/*
Given the universal representation of ExprAlignof, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "ExprAlignof ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_expr_alignof_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_array_value_indent(const an_ifc_expr_array_value universal, unsigned indent)
/*
Given the universal representation of ExprArrayValue, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_element_type_an_ifc_expr_array_value(universal)) {
    an_ifc_type_index field = get_ifc_element_type_an_ifc_expr_array_value(universal);

    db_print_indent(indent);
    fprintf(f_debug, "element_type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_elements_an_ifc_expr_array_value(universal)) {
    an_ifc_expr_index field = get_ifc_elements_an_ifc_expr_array_value(universal);

    db_print_indent(indent);
    fprintf(f_debug, "elements:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_expr_array_value(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_array_value(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_type_an_ifc_expr_array_value(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_array_value(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_array_value(const an_ifc_expr_array_value universal)
/*
Given the universal representation of ExprArrayValue, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "ExprArrayValue ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_expr_array_value_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_assign_initializer_indent(const an_ifc_expr_assign_initializer universal, unsigned indent)
/*
Given the universal representation of ExprAssignInitializer, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_equal_an_ifc_expr_assign_initializer(universal)) {
    an_ifc_source_location field = get_ifc_equal_an_ifc_expr_assign_initializer(universal);

    db_print_indent(indent);
    fprintf(f_debug, "equal:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_initializer_an_ifc_expr_assign_initializer(universal)) {
    an_ifc_expr_index field = get_ifc_initializer_an_ifc_expr_assign_initializer(universal);

    db_print_indent(indent);
    fprintf(f_debug, "initializer:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_assign_initializer(const an_ifc_expr_assign_initializer universal)
/*
Given the universal representation of ExprAssignInitializer, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================ ");
  fprintf(f_debug, "ExprAssignInitializer ");
  fprintf(f_debug, "=============================\n");
  db_node_an_ifc_expr_assign_initializer_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_binary_fold_indent(const an_ifc_expr_binary_fold universal, unsigned indent)
/*
Given the universal representation of ExprBinaryFold, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_associativity_an_ifc_expr_binary_fold(universal)) {
    an_ifc_associativity field = get_ifc_associativity_an_ifc_expr_binary_fold(universal);

    db_print_indent(indent);
    fprintf(f_debug, "associativity: %llu\n",
            (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_left_an_ifc_expr_binary_fold(universal)) {
    an_ifc_expr_index field = get_ifc_left_an_ifc_expr_binary_fold(universal);

    db_print_indent(indent);
    fprintf(f_debug, "left:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_expr_binary_fold(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_binary_fold(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_operation_an_ifc_expr_binary_fold(universal)) {
    an_ifc_dyadic_operator_sort field = get_ifc_operation_an_ifc_expr_binary_fold(universal);

    db_print_indent(indent);
    fprintf(f_debug, "operation: %s\n", str_for_an_ifc_dyadic_operator_sort(field));
  }  /* if */
  if (has_ifc_right_an_ifc_expr_binary_fold(universal)) {
    an_ifc_expr_index field = get_ifc_right_an_ifc_expr_binary_fold(universal);

    db_print_indent(indent);
    fprintf(f_debug, "right:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_type_an_ifc_expr_binary_fold(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_binary_fold(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_binary_fold(const an_ifc_expr_binary_fold universal)
/*
Given the universal representation of ExprBinaryFold, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "ExprBinaryFold ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_expr_binary_fold_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_call_indent(const an_ifc_expr_call universal, unsigned indent)
/*
Given the universal representation of ExprCall, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_arguments_an_ifc_expr_call(universal)) {
    an_ifc_expr_index field = get_ifc_arguments_an_ifc_expr_call(universal);

    db_print_indent(indent);
    fprintf(f_debug, "arguments:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_expr_call(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_call(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_operation_an_ifc_expr_call(universal)) {
    an_ifc_expr_index field = get_ifc_operation_an_ifc_expr_call(universal);

    db_print_indent(indent);
    fprintf(f_debug, "operation:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_type_an_ifc_expr_call(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_call(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_call(const an_ifc_expr_call universal)
/*
Given the universal representation of ExprCall, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "=================================== ");
  fprintf(f_debug, "ExprCall ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_expr_call_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_cast_indent(const an_ifc_expr_cast universal, unsigned indent)
/*
Given the universal representation of ExprCast, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_expr_cast(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_cast(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_op_an_ifc_expr_cast(universal)) {
    an_ifc_dyadic_operator_sort field = get_ifc_op_an_ifc_expr_cast(universal);

    db_print_indent(indent);
    fprintf(f_debug, "op: %s\n", str_for_an_ifc_dyadic_operator_sort(field));
  }  /* if */
  if (has_ifc_source_an_ifc_expr_cast(universal)) {
    an_ifc_expr_index field = get_ifc_source_an_ifc_expr_cast(universal);

    db_print_indent(indent);
    fprintf(f_debug, "source:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_target_an_ifc_expr_cast(universal)) {
    an_ifc_type_index field = get_ifc_target_an_ifc_expr_cast(universal);

    db_print_indent(indent);
    fprintf(f_debug, "target:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_type_an_ifc_expr_cast(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_cast(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_cast(const an_ifc_expr_cast universal)
/*
Given the universal representation of ExprCast, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "=================================== ");
  fprintf(f_debug, "ExprCast ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_expr_cast_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_compound_string_indent(const an_ifc_expr_compound_string universal, unsigned indent)
/*
Given the universal representation of ExprCompoundString, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_expr_compound_string(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_compound_string(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_prefix_an_ifc_expr_compound_string(universal)) {
    an_ifc_text_offset field = get_ifc_prefix_an_ifc_expr_compound_string(universal);

    db_print_indent(indent);
    fprintf(f_debug, "prefix: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_string_an_ifc_expr_compound_string(universal)) {
    an_ifc_expr_index field = get_ifc_string_an_ifc_expr_compound_string(universal);

    db_print_indent(indent);
    fprintf(f_debug, "string:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_type_an_ifc_expr_compound_string(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_compound_string(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_compound_string(const an_ifc_expr_compound_string universal)
/*
Given the universal representation of ExprCompoundString, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================== ");
  fprintf(f_debug, "ExprCompoundString ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_expr_compound_string_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_condition_indent(const an_ifc_expr_condition universal, unsigned indent)
/*
Given the universal representation of ExprCondition, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_expr_an_ifc_expr_condition(universal)) {
    an_ifc_expr_index field = get_ifc_expr_an_ifc_expr_condition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "expr:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_expr_condition(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_condition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_type_an_ifc_expr_condition(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_condition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_condition(const an_ifc_expr_condition universal)
/*
Given the universal representation of ExprCondition, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "ExprCondition ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_expr_condition_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_designated_initializer_indent(const an_ifc_expr_designated_initializer universal,
             unsigned                                 indent)
/*
Given the universal representation of ExprDesignatedInitializer, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_initializer_an_ifc_expr_designated_initializer(universal)) {
    an_ifc_expr_index field = get_ifc_initializer_an_ifc_expr_designated_initializer(universal);

    db_print_indent(indent);
    fprintf(f_debug, "initializer:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_expr_designated_initializer(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_designated_initializer(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_member_an_ifc_expr_designated_initializer(universal)) {
    an_ifc_text_offset field = get_ifc_member_an_ifc_expr_designated_initializer(universal);

    db_print_indent(indent);
    fprintf(f_debug, "member: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_type_an_ifc_expr_designated_initializer(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_designated_initializer(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_designated_initializer(const an_ifc_expr_designated_initializer universal)
/*
Given the universal representation of ExprDesignatedInitializer, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================== ");
  fprintf(f_debug, "ExprDesignatedInitializer ");
  fprintf(f_debug, "===========================\n");
  db_node_an_ifc_expr_designated_initializer_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_destructor_call_indent(const an_ifc_expr_destructor_call universal, unsigned indent)
/*
Given the universal representation of ExprDestructorCall, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_cleanup_an_ifc_expr_destructor_call(universal)) {
    an_ifc_destructor_sort field = get_ifc_cleanup_an_ifc_expr_destructor_call(universal);

    db_print_indent(indent);
    fprintf(f_debug, "cleanup: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_decltype_specifier_an_ifc_expr_destructor_call(universal)) {
    an_ifc_syntax_index field = get_ifc_decltype_specifier_an_ifc_expr_destructor_call(universal);

    db_print_indent(indent);
    fprintf(f_debug, "decltype_specifier:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_expr_destructor_call(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_destructor_call(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_expr_destructor_call(universal)) {
    an_ifc_expr_index field = get_ifc_name_an_ifc_expr_destructor_call(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_type_an_ifc_expr_destructor_call(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_destructor_call(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_destructor_call(const an_ifc_expr_destructor_call universal)
/*
Given the universal representation of ExprDestructorCall, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================== ");
  fprintf(f_debug, "ExprDestructorCall ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_expr_destructor_call_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_dyad_indent(const an_ifc_expr_dyad universal, unsigned indent)
/*
Given the universal representation of ExprDyad, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_argument_0_an_ifc_expr_dyad(universal)) {
    an_ifc_expr_index field = get_ifc_argument_0_an_ifc_expr_dyad(universal);

    db_print_indent(indent);
    fprintf(f_debug, "argument_0:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_argument_1_an_ifc_expr_dyad(universal)) {
    an_ifc_expr_index field = get_ifc_argument_1_an_ifc_expr_dyad(universal);

    db_print_indent(indent);
    fprintf(f_debug, "argument_1:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_assoc_an_ifc_expr_dyad(universal)) {
    an_ifc_dyadic_operator_sort field = get_ifc_assoc_an_ifc_expr_dyad(universal);

    db_print_indent(indent);
    fprintf(f_debug, "assoc: %s\n", str_for_an_ifc_dyadic_operator_sort(field));
  }  /* if */
  if (has_ifc_impl_an_ifc_expr_dyad(universal)) {
    an_ifc_decl_index field = get_ifc_impl_an_ifc_expr_dyad(universal);

    db_print_indent(indent);
    fprintf(f_debug, "impl:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_expr_dyad(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_dyad(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_type_an_ifc_expr_dyad(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_dyad(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_dyad(const an_ifc_expr_dyad universal)
/*
Given the universal representation of ExprDyad, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "=================================== ");
  fprintf(f_debug, "ExprDyad ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_expr_dyad_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_dynamic_dispatch_indent(const an_ifc_expr_dynamic_dispatch universal, unsigned indent)
/*
Given the universal representation of ExprDynamicDispatch, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_expr_dynamic_dispatch(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_dynamic_dispatch(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_pivot_an_ifc_expr_dynamic_dispatch(universal)) {
    an_ifc_expr_index field = get_ifc_pivot_an_ifc_expr_dynamic_dispatch(universal);

    db_print_indent(indent);
    fprintf(f_debug, "pivot:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_type_an_ifc_expr_dynamic_dispatch(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_dynamic_dispatch(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_dynamic_dispatch(const an_ifc_expr_dynamic_dispatch universal)
/*
Given the universal representation of ExprDynamicDispatch, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================= ");
  fprintf(f_debug, "ExprDynamicDispatch ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_expr_dynamic_dispatch_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_empty_indent(const an_ifc_expr_empty universal, unsigned indent)
/*
Given the universal representation of ExprEmpty, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_expr_empty(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_empty(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_type_an_ifc_expr_empty(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_empty(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_empty(const an_ifc_expr_empty universal)
/*
Given the universal representation of ExprEmpty, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "ExprEmpty ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_expr_empty_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_expansion_indent(const an_ifc_expr_expansion universal, unsigned indent)
/*
Given the universal representation of ExprExpansion, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_expr_expansion(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_expansion(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_operand_an_ifc_expr_expansion(universal)) {
    an_ifc_expr_index field = get_ifc_operand_an_ifc_expr_expansion(universal);

    db_print_indent(indent);
    fprintf(f_debug, "operand:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_type_an_ifc_expr_expansion(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_expansion(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_expansion(const an_ifc_expr_expansion universal)
/*
Given the universal representation of ExprExpansion, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "ExprExpansion ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_expr_expansion_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_expression_list_indent(const an_ifc_expr_expression_list universal, unsigned indent)
/*
Given the universal representation of ExprExpressionList, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_contents_an_ifc_expr_expression_list(universal)) {
    an_ifc_expr_index field = get_ifc_contents_an_ifc_expr_expression_list(universal);

    db_print_indent(indent);
    fprintf(f_debug, "contents:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_delimiter_an_ifc_expr_expression_list(universal)) {
    an_ifc_delimiter_sort field = get_ifc_delimiter_an_ifc_expr_expression_list(universal);

    db_print_indent(indent);
    fprintf(f_debug, "delimiter: %s\n", str_for_an_ifc_delimiter_sort(field));
  }  /* if */
  if (has_ifc_left_an_ifc_expr_expression_list(universal)) {
    an_ifc_source_location field = get_ifc_left_an_ifc_expr_expression_list(universal);

    db_print_indent(indent);
    fprintf(f_debug, "left:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_right_an_ifc_expr_expression_list(universal)) {
    an_ifc_source_location field = get_ifc_right_an_ifc_expr_expression_list(universal);

    db_print_indent(indent);
    fprintf(f_debug, "right:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_expression_list(const an_ifc_expr_expression_list universal)
/*
Given the universal representation of ExprExpressionList, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================== ");
  fprintf(f_debug, "ExprExpressionList ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_expr_expression_list_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_function_string_indent(const an_ifc_expr_function_string universal, unsigned indent)
/*
Given the universal representation of ExprFunctionString, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_expr_function_string(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_function_string(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_macro_an_ifc_expr_function_string(universal)) {
    an_ifc_text_offset field = get_ifc_macro_an_ifc_expr_function_string(universal);

    db_print_indent(indent);
    fprintf(f_debug, "macro: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_type_an_ifc_expr_function_string(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_function_string(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_function_string(const an_ifc_expr_function_string universal)
/*
Given the universal representation of ExprFunctionString, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================== ");
  fprintf(f_debug, "ExprFunctionString ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_expr_function_string_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_hierarchy_conversion_indent(const an_ifc_expr_hierarchy_conversion universal,
             unsigned                               indent)
/*
Given the universal representation of ExprHierarchyConversion, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_inheritance_an_ifc_expr_hierarchy_conversion(universal)) {
    an_ifc_expr_index field = get_ifc_inheritance_an_ifc_expr_hierarchy_conversion(universal);

    db_print_indent(indent);
    fprintf(f_debug, "inheritance:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_expr_hierarchy_conversion(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_hierarchy_conversion(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_op_an_ifc_expr_hierarchy_conversion(universal)) {
    an_ifc_dyadic_operator_sort field = get_ifc_op_an_ifc_expr_hierarchy_conversion(universal);

    db_print_indent(indent);
    fprintf(f_debug, "op: %s\n", str_for_an_ifc_dyadic_operator_sort(field));
  }  /* if */
  if (has_ifc_override_an_ifc_expr_hierarchy_conversion(universal)) {
    an_ifc_expr_index field = get_ifc_override_an_ifc_expr_hierarchy_conversion(universal);

    db_print_indent(indent);
    fprintf(f_debug, "override:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_source_an_ifc_expr_hierarchy_conversion(universal)) {
    an_ifc_expr_index field = get_ifc_source_an_ifc_expr_hierarchy_conversion(universal);

    db_print_indent(indent);
    fprintf(f_debug, "source:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_target_an_ifc_expr_hierarchy_conversion(universal)) {
    an_ifc_type_index field = get_ifc_target_an_ifc_expr_hierarchy_conversion(universal);

    db_print_indent(indent);
    fprintf(f_debug, "target:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_type_an_ifc_expr_hierarchy_conversion(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_hierarchy_conversion(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_hierarchy_conversion(const an_ifc_expr_hierarchy_conversion universal)
/*
Given the universal representation of ExprHierarchyConversion, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "=========================== ");
  fprintf(f_debug, "ExprHierarchyConversion ");
  fprintf(f_debug, "============================\n");
  db_node_an_ifc_expr_hierarchy_conversion_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_inheritance_path_indent(const an_ifc_expr_inheritance_path universal, unsigned indent)
/*
Given the universal representation of ExprInheritancePath, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_expr_inheritance_path(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_inheritance_path(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_path_an_ifc_expr_inheritance_path(universal)) {
    an_ifc_expr_index field = get_ifc_path_an_ifc_expr_inheritance_path(universal);

    db_print_indent(indent);
    fprintf(f_debug, "path:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_type_an_ifc_expr_inheritance_path(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_inheritance_path(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_inheritance_path(const an_ifc_expr_inheritance_path universal)
/*
Given the universal representation of ExprInheritancePath, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================= ");
  fprintf(f_debug, "ExprInheritancePath ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_expr_inheritance_path_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_initializer_indent(const an_ifc_expr_initializer universal, unsigned indent)
/*
Given the universal representation of ExprInitializer, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_expr_an_ifc_expr_initializer(universal)) {
    an_ifc_expr_index field = get_ifc_expr_an_ifc_expr_initializer(universal);

    db_print_indent(indent);
    fprintf(f_debug, "expr:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_expr_initializer(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_initializer(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_sort_an_ifc_expr_initializer(universal)) {
    an_ifc_initializer_sort field = get_ifc_sort_an_ifc_expr_initializer(universal);

    db_print_indent(indent);
    fprintf(f_debug, "sort: %s\n", str_for_an_ifc_initializer_sort(field));
  }  /* if */
  if (has_ifc_type_an_ifc_expr_initializer(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_initializer(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_initializer(const an_ifc_expr_initializer universal)
/*
Given the universal representation of ExprInitializer, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "=============================== ");
  fprintf(f_debug, "ExprInitializer ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_expr_initializer_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_initializer_list_indent(const an_ifc_expr_initializer_list universal, unsigned indent)
/*
Given the universal representation of ExprInitializerList, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_elements_an_ifc_expr_initializer_list(universal)) {
    an_ifc_expr_index field = get_ifc_elements_an_ifc_expr_initializer_list(universal);

    db_print_indent(indent);
    fprintf(f_debug, "elements:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_expr_initializer_list(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_initializer_list(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_type_an_ifc_expr_initializer_list(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_initializer_list(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_initializer_list(const an_ifc_expr_initializer_list universal)
/*
Given the universal representation of ExprInitializerList, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================= ");
  fprintf(f_debug, "ExprInitializerList ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_expr_initializer_list_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_label_indent(const an_ifc_expr_label universal, unsigned indent)
/*
Given the universal representation of ExprLabel, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_designator_an_ifc_expr_label(universal)) {
    an_ifc_expr_index field = get_ifc_designator_an_ifc_expr_label(universal);

    db_print_indent(indent);
    fprintf(f_debug, "designator:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_expr_label(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_label(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_type_an_ifc_expr_label(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_label(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_label(const an_ifc_expr_label universal)
/*
Given the universal representation of ExprLabel, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "ExprLabel ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_expr_label_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_lambda_indent(const an_ifc_expr_lambda universal, unsigned indent)
/*
Given the universal representation of ExprLambda, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_body_an_ifc_expr_lambda(universal)) {
    an_ifc_syntax_index field = get_ifc_body_an_ifc_expr_lambda(universal);

    db_print_indent(indent);
    fprintf(f_debug, "body:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_constraint_an_ifc_expr_lambda(universal)) {
    an_ifc_syntax_index field = get_ifc_constraint_an_ifc_expr_lambda(universal);

    db_print_indent(indent);
    fprintf(f_debug, "constraint:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_declarator_an_ifc_expr_lambda(universal)) {
    an_ifc_syntax_index field = get_ifc_declarator_an_ifc_expr_lambda(universal);

    db_print_indent(indent);
    fprintf(f_debug, "declarator:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_introducer_an_ifc_expr_lambda(universal)) {
    an_ifc_syntax_index field = get_ifc_introducer_an_ifc_expr_lambda(universal);

    db_print_indent(indent);
    fprintf(f_debug, "introducer:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_template_parameters_an_ifc_expr_lambda(universal)) {
    an_ifc_syntax_index field = get_ifc_template_parameters_an_ifc_expr_lambda(universal);

    db_print_indent(indent);
    fprintf(f_debug, "template_parameters:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_lambda(const an_ifc_expr_lambda universal)
/*
Given the universal representation of ExprLambda, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "ExprLambda ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_expr_lambda_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_literal_indent(const an_ifc_expr_literal universal, unsigned indent)
/*
Given the universal representation of ExprLiteral, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_expr_literal(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_literal(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_type_an_ifc_expr_literal(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_literal(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_value_an_ifc_expr_literal(universal)) {
    an_ifc_lit_index field = get_ifc_value_an_ifc_expr_literal(universal);

    db_print_indent(indent);
    fprintf(f_debug, "value:");
    if (is_null_index_an_ifc_lit_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_lit_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_literal(const an_ifc_expr_literal universal)
/*
Given the universal representation of ExprLiteral, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "ExprLiteral ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_expr_literal_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_member_access_indent(const an_ifc_expr_member_access universal, unsigned indent)
/*
Given the universal representation of ExprMemberAccess, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_enclosing_an_ifc_expr_member_access(universal)) {
    an_ifc_type_index field = get_ifc_enclosing_an_ifc_expr_member_access(universal);

    db_print_indent(indent);
    fprintf(f_debug, "enclosing:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_expr_member_access(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_member_access(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_expr_member_access(universal)) {
    an_ifc_text_offset field = get_ifc_name_an_ifc_expr_member_access(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_offset_an_ifc_expr_member_access(universal)) {
    an_ifc_expr_index field = get_ifc_offset_an_ifc_expr_member_access(universal);

    db_print_indent(indent);
    fprintf(f_debug, "offset:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_type_an_ifc_expr_member_access(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_member_access(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_member_access(const an_ifc_expr_member_access universal)
/*
Given the universal representation of ExprMemberAccess, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "=============================== ");
  fprintf(f_debug, "ExprMemberAccess ");
  fprintf(f_debug, "===============================\n");
  db_node_an_ifc_expr_member_access_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_member_initializer_indent(const an_ifc_expr_member_initializer universal, unsigned indent)
/*
Given the universal representation of ExprMemberInitializer, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_base_an_ifc_expr_member_initializer(universal)) {
    an_ifc_type_index field = get_ifc_base_an_ifc_expr_member_initializer(universal);

    db_print_indent(indent);
    fprintf(f_debug, "base:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_initializer_an_ifc_expr_member_initializer(universal)) {
    an_ifc_expr_index field = get_ifc_initializer_an_ifc_expr_member_initializer(universal);

    db_print_indent(indent);
    fprintf(f_debug, "initializer:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_expr_member_initializer(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_member_initializer(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_member_an_ifc_expr_member_initializer(universal)) {
    an_ifc_decl_index field = get_ifc_member_an_ifc_expr_member_initializer(universal);

    db_print_indent(indent);
    fprintf(f_debug, "member:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_type_an_ifc_expr_member_initializer(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_member_initializer(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_member_initializer(const an_ifc_expr_member_initializer universal)
/*
Given the universal representation of ExprMemberInitializer, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================ ");
  fprintf(f_debug, "ExprMemberInitializer ");
  fprintf(f_debug, "=============================\n");
  db_node_an_ifc_expr_member_initializer_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_monad_indent(const an_ifc_expr_monad universal, unsigned indent)
/*
Given the universal representation of ExprMonad, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_argument_an_ifc_expr_monad(universal)) {
    an_ifc_expr_index field = get_ifc_argument_an_ifc_expr_monad(universal);

    db_print_indent(indent);
    fprintf(f_debug, "argument:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_assoc_an_ifc_expr_monad(universal)) {
    an_ifc_monadic_operator_sort field = get_ifc_assoc_an_ifc_expr_monad(universal);

    db_print_indent(indent);
    fprintf(f_debug, "assoc: %s\n", str_for_an_ifc_monadic_operator_sort(field));
  }  /* if */
  if (has_ifc_impl_an_ifc_expr_monad(universal)) {
    an_ifc_decl_index field = get_ifc_impl_an_ifc_expr_monad(universal);

    db_print_indent(indent);
    fprintf(f_debug, "impl:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_expr_monad(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_monad(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_type_an_ifc_expr_monad(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_monad(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_monad(const an_ifc_expr_monad universal)
/*
Given the universal representation of ExprMonad, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "ExprMonad ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_expr_monad_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_named_decl_indent(const an_ifc_expr_named_decl universal, unsigned indent)
/*
Given the universal representation of ExprNamedDecl, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_expr_named_decl(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_named_decl(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_resolution_an_ifc_expr_named_decl(universal)) {
    an_ifc_decl_index field = get_ifc_resolution_an_ifc_expr_named_decl(universal);

    db_print_indent(indent);
    fprintf(f_debug, "resolution:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_type_an_ifc_expr_named_decl(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_named_decl(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_named_decl(const an_ifc_expr_named_decl universal)
/*
Given the universal representation of ExprNamedDecl, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "ExprNamedDecl ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_expr_named_decl_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_nullptr_indent(const an_ifc_expr_nullptr universal, unsigned indent)
/*
Given the universal representation of ExprNullptr, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_expr_nullptr(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_nullptr(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_type_an_ifc_expr_nullptr(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_nullptr(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_nullptr(const an_ifc_expr_nullptr universal)
/*
Given the universal representation of ExprNullptr, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "ExprNullptr ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_expr_nullptr_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_packed_template_arguments_indent(const an_ifc_expr_packed_template_arguments universal,
             unsigned                                    indent)
/*
Given the universal representation of ExprPackedTemplateArguments, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_arguments_an_ifc_expr_packed_template_arguments(universal)) {
    an_ifc_expr_index field = get_ifc_arguments_an_ifc_expr_packed_template_arguments(universal);

    db_print_indent(indent);
    fprintf(f_debug, "arguments:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_expr_packed_template_arguments(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_packed_template_arguments(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_type_an_ifc_expr_packed_template_arguments(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_packed_template_arguments(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_packed_template_arguments(const an_ifc_expr_packed_template_arguments universal)
/*
Given the universal representation of ExprPackedTemplateArguments, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================= ");
  fprintf(f_debug, "ExprPackedTemplateArguments ");
  fprintf(f_debug, "==========================\n");
  db_node_an_ifc_expr_packed_template_arguments_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_path_indent(const an_ifc_expr_path universal, unsigned indent)
/*
Given the universal representation of ExprPath, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_expr_path(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_path(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_member_an_ifc_expr_path(universal)) {
    an_ifc_expr_index field = get_ifc_member_an_ifc_expr_path(universal);

    db_print_indent(indent);
    fprintf(f_debug, "member:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_scope_an_ifc_expr_path(universal)) {
    an_ifc_expr_index field = get_ifc_scope_an_ifc_expr_path(universal);

    db_print_indent(indent);
    fprintf(f_debug, "scope:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_type_an_ifc_expr_path(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_path(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_path(const an_ifc_expr_path universal)
/*
Given the universal representation of ExprPath, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "=================================== ");
  fprintf(f_debug, "ExprPath ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_expr_path_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_placeholder_indent(const an_ifc_expr_placeholder universal, unsigned indent)
/*
Given the universal representation of ExprPlaceholder, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_expr_placeholder(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_placeholder(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_type_an_ifc_expr_placeholder(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_placeholder(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_placeholder(const an_ifc_expr_placeholder universal)
/*
Given the universal representation of ExprPlaceholder, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "=============================== ");
  fprintf(f_debug, "ExprPlaceholder ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_expr_placeholder_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_pointer_indent(const an_ifc_expr_pointer universal, unsigned indent)
/*
Given the universal representation of ExprPointer, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_expr_pointer(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_pointer(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_pointer(const an_ifc_expr_pointer universal)
/*
Given the universal representation of ExprPointer, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "ExprPointer ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_expr_pointer_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_product_type_value_indent(const an_ifc_expr_product_type_value universal, unsigned indent)
/*
Given the universal representation of ExprProductTypeValue, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_base_subobjects_an_ifc_expr_product_type_value(universal)) {
    an_ifc_expr_index field = get_ifc_base_subobjects_an_ifc_expr_product_type_value(universal);

    db_print_indent(indent);
    fprintf(f_debug, "base_subobjects:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_class_decl_an_ifc_expr_product_type_value(universal)) {
    an_ifc_type_index field = get_ifc_class_decl_an_ifc_expr_product_type_value(universal);

    db_print_indent(indent);
    fprintf(f_debug, "class_decl:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_expr_product_type_value(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_product_type_value(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_members_an_ifc_expr_product_type_value(universal)) {
    an_ifc_expr_index field = get_ifc_members_an_ifc_expr_product_type_value(universal);

    db_print_indent(indent);
    fprintf(f_debug, "members:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_type_an_ifc_expr_product_type_value(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_product_type_value(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_product_type_value(const an_ifc_expr_product_type_value universal)
/*
Given the universal representation of ExprProductTypeValue, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================= ");
  fprintf(f_debug, "ExprProductTypeValue ");
  fprintf(f_debug, "=============================\n");
  db_node_an_ifc_expr_product_type_value_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_push_state_indent(const an_ifc_expr_push_state universal, unsigned indent)
/*
Given the universal representation of ExprPushState, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_ctor_call_an_ifc_expr_push_state(universal)) {
    an_ifc_expr_index field = get_ifc_ctor_call_an_ifc_expr_push_state(universal);

    db_print_indent(indent);
    fprintf(f_debug, "ctor_call:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_dtor_call_an_ifc_expr_push_state(universal)) {
    an_ifc_expr_index field = get_ifc_dtor_call_an_ifc_expr_push_state(universal);

    db_print_indent(indent);
    fprintf(f_debug, "dtor_call:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_flags_an_ifc_expr_push_state(universal)) {
    an_ifc_eh_flags field = get_ifc_flags_an_ifc_expr_push_state(universal);

    db_print_indent(indent);
    fprintf(f_debug, "flags: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_locus_an_ifc_expr_push_state(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_push_state(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_type_an_ifc_expr_push_state(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_push_state(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_push_state(const an_ifc_expr_push_state universal)
/*
Given the universal representation of ExprPushState, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "ExprPushState ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_expr_push_state_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_qualified_name_indent(const an_ifc_expr_qualified_name universal, unsigned indent)
/*
Given the universal representation of ExprQualifiedName, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_elements_an_ifc_expr_qualified_name(universal)) {
    an_ifc_expr_index field = get_ifc_elements_an_ifc_expr_qualified_name(universal);

    db_print_indent(indent);
    fprintf(f_debug, "elements:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_expr_qualified_name(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_qualified_name(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_type_an_ifc_expr_qualified_name(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_qualified_name(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_typename_keyword_an_ifc_expr_qualified_name(universal)) {
    an_ifc_source_location field = get_ifc_typename_keyword_an_ifc_expr_qualified_name(universal);

    db_print_indent(indent);
    fprintf(f_debug, "typename_keyword:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_qualified_name(const an_ifc_expr_qualified_name universal)
/*
Given the universal representation of ExprQualifiedName, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================== ");
  fprintf(f_debug, "ExprQualifiedName ");
  fprintf(f_debug, "===============================\n");
  db_node_an_ifc_expr_qualified_name_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_read_indent(const an_ifc_expr_read universal, unsigned indent)
/*
Given the universal representation of ExprRead, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_address_an_ifc_expr_read(universal)) {
    an_ifc_expr_index field = get_ifc_address_an_ifc_expr_read(universal);

    db_print_indent(indent);
    fprintf(f_debug, "address:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_expr_read(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_read(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_sort_an_ifc_expr_read(universal)) {
    an_ifc_read_conversion_sort field = get_ifc_sort_an_ifc_expr_read(universal);

    db_print_indent(indent);
    fprintf(f_debug, "sort: %s\n", str_for_an_ifc_read_conversion_sort(field));
  }  /* if */
  if (has_ifc_type_an_ifc_expr_read(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_read(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_read(const an_ifc_expr_read universal)
/*
Given the universal representation of ExprRead, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "=================================== ");
  fprintf(f_debug, "ExprRead ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_expr_read_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_requires_indent(const an_ifc_expr_requires universal, unsigned indent)
/*
Given the universal representation of ExprRequires, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_body_an_ifc_expr_requires(universal)) {
    an_ifc_syntax_index field = get_ifc_body_an_ifc_expr_requires(universal);

    db_print_indent(indent);
    fprintf(f_debug, "body:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_expr_requires(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_requires(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_parameters_an_ifc_expr_requires(universal)) {
    an_ifc_syntax_index field = get_ifc_parameters_an_ifc_expr_requires(universal);

    db_print_indent(indent);
    fprintf(f_debug, "parameters:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_type_an_ifc_expr_requires(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_requires(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_requires(const an_ifc_expr_requires universal)
/*
Given the universal representation of ExprRequires, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "ExprRequires ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_expr_requires_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_simple_identifier_indent(const an_ifc_expr_simple_identifier universal, unsigned indent)
/*
Given the universal representation of ExprSimpleIdentifier, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_expr_simple_identifier(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_simple_identifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_expr_simple_identifier(universal)) {
    an_ifc_name_index field = get_ifc_name_an_ifc_expr_simple_identifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name:");
    if (is_null_index_an_ifc_name_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_name_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_type_an_ifc_expr_simple_identifier(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_simple_identifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_simple_identifier(const an_ifc_expr_simple_identifier universal)
/*
Given the universal representation of ExprSimpleIdentifier, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================= ");
  fprintf(f_debug, "ExprSimpleIdentifier ");
  fprintf(f_debug, "=============================\n");
  db_node_an_ifc_expr_simple_identifier_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_sizeof_type_indent(const an_ifc_expr_sizeof_type universal, unsigned indent)
/*
Given the universal representation of ExprSizeofType, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_expr_sizeof_type(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_sizeof_type(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_operand_an_ifc_expr_sizeof_type(universal)) {
    an_ifc_type_index field = get_ifc_operand_an_ifc_expr_sizeof_type(universal);

    db_print_indent(indent);
    fprintf(f_debug, "operand:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_type_an_ifc_expr_sizeof_type(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_sizeof_type(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_sizeof_type(const an_ifc_expr_sizeof_type universal)
/*
Given the universal representation of ExprSizeofType, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "ExprSizeofType ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_expr_sizeof_type_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_string_indent(const an_ifc_expr_string universal, unsigned indent)
/*
Given the universal representation of ExprString, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_expr_string(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_string(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_string_index_an_ifc_expr_string(universal)) {
    an_ifc_string_index field = get_ifc_string_index_an_ifc_expr_string(universal);

    db_print_indent(indent);
    fprintf(f_debug, "string_index:");
    if (is_null_index_an_ifc_string_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_string_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_type_an_ifc_expr_string(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_string(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_string(const an_ifc_expr_string universal)
/*
Given the universal representation of ExprString, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "ExprString ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_expr_string_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_string_sequence_indent(const an_ifc_expr_string_sequence universal, unsigned indent)
/*
Given the universal representation of ExprStringSequence, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_expr_string_sequence(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_string_sequence(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_strings_an_ifc_expr_string_sequence(universal)) {
    an_ifc_expr_index field = get_ifc_strings_an_ifc_expr_string_sequence(universal);

    db_print_indent(indent);
    fprintf(f_debug, "strings:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_type_an_ifc_expr_string_sequence(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_string_sequence(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_string_sequence(const an_ifc_expr_string_sequence universal)
/*
Given the universal representation of ExprStringSequence, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================== ");
  fprintf(f_debug, "ExprStringSequence ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_expr_string_sequence_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_subobject_value_indent(const an_ifc_expr_subobject_value universal, unsigned indent)
/*
Given the universal representation of ExprSubobjectValue, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_value_an_ifc_expr_subobject_value(universal)) {
    an_ifc_expr_index field = get_ifc_value_an_ifc_expr_subobject_value(universal);

    db_print_indent(indent);
    fprintf(f_debug, "value:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_subobject_value(const an_ifc_expr_subobject_value universal)
/*
Given the universal representation of ExprSubobjectValue, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================== ");
  fprintf(f_debug, "ExprSubobjectValue ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_expr_subobject_value_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_sum_type_value_indent(const an_ifc_expr_sum_type_value universal, unsigned indent)
/*
Given the universal representation of ExprSumTypeValue, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_discriminant_an_ifc_expr_sum_type_value(universal)) {
    an_ifc_active_member field = get_ifc_discriminant_an_ifc_expr_sum_type_value(universal);

    db_print_indent(indent);
    fprintf(f_debug, "discriminant: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_locus_an_ifc_expr_sum_type_value(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_sum_type_value(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_type_an_ifc_expr_sum_type_value(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_sum_type_value(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_value_an_ifc_expr_sum_type_value(universal)) {
    an_ifc_expr_index field = get_ifc_value_an_ifc_expr_sum_type_value(universal);

    db_print_indent(indent);
    fprintf(f_debug, "value:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_variant_an_ifc_expr_sum_type_value(universal)) {
    an_ifc_decl_index field = get_ifc_variant_an_ifc_expr_sum_type_value(universal);

    db_print_indent(indent);
    fprintf(f_debug, "variant:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_sum_type_value(const an_ifc_expr_sum_type_value universal)
/*
Given the universal representation of ExprSumTypeValue, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "=============================== ");
  fprintf(f_debug, "ExprSumTypeValue ");
  fprintf(f_debug, "===============================\n");
  db_node_an_ifc_expr_sum_type_value_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_syntax_tree_indent(const an_ifc_expr_syntax_tree universal, unsigned indent)
/*
Given the universal representation of ExprSyntaxTree, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_syntax_an_ifc_expr_syntax_tree(universal)) {
    an_ifc_syntax_index field = get_ifc_syntax_an_ifc_expr_syntax_tree(universal);

    db_print_indent(indent);
    fprintf(f_debug, "syntax:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_syntax_tree(const an_ifc_expr_syntax_tree universal)
/*
Given the universal representation of ExprSyntaxTree, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "ExprSyntaxTree ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_expr_syntax_tree_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_template_id_indent(const an_ifc_expr_template_id universal, unsigned indent)
/*
Given the universal representation of ExprTemplateId, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_arguments_an_ifc_expr_template_id(universal)) {
    an_ifc_expr_index field = get_ifc_arguments_an_ifc_expr_template_id(universal);

    db_print_indent(indent);
    fprintf(f_debug, "arguments:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_expr_template_id(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_template_id(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_primary_an_ifc_expr_template_id(universal)) {
    an_ifc_expr_index field = get_ifc_primary_an_ifc_expr_template_id(universal);

    db_print_indent(indent);
    fprintf(f_debug, "primary:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_type_an_ifc_expr_template_id(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_template_id(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_template_id(const an_ifc_expr_template_id universal)
/*
Given the universal representation of ExprTemplateId, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "ExprTemplateId ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_expr_template_id_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_template_reference_indent(const an_ifc_expr_template_reference universal, unsigned indent)
/*
Given the universal representation of ExprTemplateReference, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_arguments_an_ifc_expr_template_reference(universal)) {
    an_ifc_expr_index field = get_ifc_arguments_an_ifc_expr_template_reference(universal);

    db_print_indent(indent);
    fprintf(f_debug, "arguments:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_expr_template_reference(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_template_reference(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_member_an_ifc_expr_template_reference(universal)) {
    an_ifc_decl_index field = get_ifc_member_an_ifc_expr_template_reference(universal);

    db_print_indent(indent);
    fprintf(f_debug, "member:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_member_locus_an_ifc_expr_template_reference(universal)) {
    an_ifc_source_location field = get_ifc_member_locus_an_ifc_expr_template_reference(universal);

    db_print_indent(indent);
    fprintf(f_debug, "member_locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_member_name_an_ifc_expr_template_reference(universal)) {
    an_ifc_name_index field = get_ifc_member_name_an_ifc_expr_template_reference(universal);

    db_print_indent(indent);
    fprintf(f_debug, "member_name:");
    if (is_null_index_an_ifc_name_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_name_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_scope_an_ifc_expr_template_reference(universal)) {
    an_ifc_type_index field = get_ifc_scope_an_ifc_expr_template_reference(universal);

    db_print_indent(indent);
    fprintf(f_debug, "scope:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_type_an_ifc_expr_template_reference(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_template_reference(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_template_reference(const an_ifc_expr_template_reference universal)
/*
Given the universal representation of ExprTemplateReference, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================ ");
  fprintf(f_debug, "ExprTemplateReference ");
  fprintf(f_debug, "=============================\n");
  db_node_an_ifc_expr_template_reference_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_temporary_indent(const an_ifc_expr_temporary universal, unsigned indent)
/*
Given the universal representation of ExprTemporary, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_id_an_ifc_expr_temporary(universal)) {
    an_ifc_unique_id field = get_ifc_id_an_ifc_expr_temporary(universal);

    db_print_indent(indent);
    fprintf(f_debug, "id: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_locus_an_ifc_expr_temporary(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_temporary(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_type_an_ifc_expr_temporary(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_temporary(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_temporary(const an_ifc_expr_temporary universal)
/*
Given the universal representation of ExprTemporary, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "ExprTemporary ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_expr_temporary_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_this_indent(const an_ifc_expr_this universal, unsigned indent)
/*
Given the universal representation of ExprThis, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_expr_this(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_this(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_type_an_ifc_expr_this(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_this(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_this(const an_ifc_expr_this universal)
/*
Given the universal representation of ExprThis, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "=================================== ");
  fprintf(f_debug, "ExprThis ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_expr_this_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_tokens_indent(const an_ifc_expr_tokens universal, unsigned indent)
/*
Given the universal representation of ExprTokens, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_expr_tokens(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_tokens(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_type_an_ifc_expr_tokens(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_tokens(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_words_an_ifc_expr_tokens(universal)) {
    an_ifc_sentence_index field = get_ifc_words_an_ifc_expr_tokens(universal);

    db_print_indent(indent);
    fprintf(f_debug, "words: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_tokens(const an_ifc_expr_tokens universal)
/*
Given the universal representation of ExprTokens, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "ExprTokens ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_expr_tokens_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_triad_indent(const an_ifc_expr_triad universal, unsigned indent)
/*
Given the universal representation of ExprTriad, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_argument_0_an_ifc_expr_triad(universal)) {
    an_ifc_expr_index field = get_ifc_argument_0_an_ifc_expr_triad(universal);

    db_print_indent(indent);
    fprintf(f_debug, "argument_0:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_argument_1_an_ifc_expr_triad(universal)) {
    an_ifc_expr_index field = get_ifc_argument_1_an_ifc_expr_triad(universal);

    db_print_indent(indent);
    fprintf(f_debug, "argument_1:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_argument_2_an_ifc_expr_triad(universal)) {
    an_ifc_expr_index field = get_ifc_argument_2_an_ifc_expr_triad(universal);

    db_print_indent(indent);
    fprintf(f_debug, "argument_2:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_assoc_an_ifc_expr_triad(universal)) {
    an_ifc_triadic_operator_sort field = get_ifc_assoc_an_ifc_expr_triad(universal);

    db_print_indent(indent);
    fprintf(f_debug, "assoc: %s\n", str_for_an_ifc_triadic_operator_sort(field));
  }  /* if */
  if (has_ifc_impl_an_ifc_expr_triad(universal)) {
    an_ifc_decl_index field = get_ifc_impl_an_ifc_expr_triad(universal);

    db_print_indent(indent);
    fprintf(f_debug, "impl:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_expr_triad(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_triad(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_type_an_ifc_expr_triad(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_triad(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_triad(const an_ifc_expr_triad universal)
/*
Given the universal representation of ExprTriad, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "ExprTriad ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_expr_triad_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_tuple_indent(const an_ifc_expr_tuple universal, unsigned indent)
/*
Given the universal representation of ExprTuple, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_cardinality_an_ifc_expr_tuple(universal)) {
    an_ifc_cardinality field = get_ifc_cardinality_an_ifc_expr_tuple(universal);

    db_print_indent(indent);
    fprintf(f_debug, "cardinality: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_locus_an_ifc_expr_tuple(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_tuple(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_start_an_ifc_expr_tuple(universal)) {
    an_ifc_index field = get_ifc_start_an_ifc_expr_tuple(universal);

    db_print_indent(indent);
    fprintf(f_debug, "start: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_type_an_ifc_expr_tuple(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_tuple(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_tuple(const an_ifc_expr_tuple universal)
/*
Given the universal representation of ExprTuple, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "ExprTuple ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_expr_tuple_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_type_indent(const an_ifc_expr_type universal, unsigned indent)
/*
Given the universal representation of ExprType, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_denotation_an_ifc_expr_type(universal)) {
    an_ifc_type_index field = get_ifc_denotation_an_ifc_expr_type(universal);

    db_print_indent(indent);
    fprintf(f_debug, "denotation:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_expr_type(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_type(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_type_an_ifc_expr_type(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_type(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_type(const an_ifc_expr_type universal)
/*
Given the universal representation of ExprType, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "=================================== ");
  fprintf(f_debug, "ExprType ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_expr_type_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_type_trait_intrinsic_indent(const an_ifc_expr_type_trait_intrinsic universal,
             unsigned                               indent)
/*
Given the universal representation of ExprTypeTraitIntrinsic, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_arguments_an_ifc_expr_type_trait_intrinsic(universal)) {
    an_ifc_type_index field = get_ifc_arguments_an_ifc_expr_type_trait_intrinsic(universal);

    db_print_indent(indent);
    fprintf(f_debug, "arguments:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_intrinsic_an_ifc_expr_type_trait_intrinsic(universal)) {
    an_ifc_operator_category field = get_ifc_intrinsic_an_ifc_expr_type_trait_intrinsic(universal);

    db_print_indent(indent);
    fprintf(f_debug, "intrinsic:\n");
    db_print_indent(indent);
    fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
    ++indent;
    switch (field.sort) {
      case ifc_os_dyadic_operator:
        { an_ifc_dyadic_operator_sort do_ref = field.variant.dyadic_operator;

          db_print_indent(indent);
          fprintf(f_debug, "dyadic_operator: %s\n", str_for_an_ifc_dyadic_operator_sort(do_ref));
        }
        break;
      case ifc_os_monadic_operator:
        { an_ifc_monadic_operator_sort mo_ref =
                                                field.variant.monadic_operator;

          db_print_indent(indent);
          fprintf(f_debug, "monadic_operator: %s\n", str_for_an_ifc_monadic_operator_sort(mo_ref));
        }
        break;
      case ifc_os_niladic_operator:
        { an_ifc_niladic_operator_sort no_ref =
                                                field.variant.niladic_operator;

          db_print_indent(indent);
          fprintf(f_debug, "niladic_operator: %s\n", str_for_an_ifc_niladic_operator_sort(no_ref));
        }
        break;
      case ifc_os_storage_instruction_operator:
        { an_ifc_storage_instruction_operator_sort sio_ref =
                                    field.variant.storage_instruction_operator;

          db_print_indent(indent);
          fprintf(f_debug, "storage_instruction_operator: %s\n",
                  str_for_an_ifc_storage_instruction_operator_sort(sio_ref));
        }
        break;
      case ifc_os_triadic_operator:
        { an_ifc_triadic_operator_sort to_ref =
                                                field.variant.triadic_operator;

          db_print_indent(indent);
          fprintf(f_debug, "triadic_operator: %s\n", str_for_an_ifc_triadic_operator_sort(to_ref));
        }
        break;
      case ifc_os_variadic_operator:
        { an_ifc_variadic_operator_sort vo_ref =
                                               field.variant.variadic_operator;

          db_print_indent(indent);
          fprintf(f_debug, "variadic_operator: %s\n", str_for_an_ifc_variadic_operator_sort(vo_ref));
        }
        break;
      default_is_unexpected();
    }  /* switch */
    --indent;
  }  /* if */
  if (has_ifc_locus_an_ifc_expr_type_trait_intrinsic(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_type_trait_intrinsic(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_type_an_ifc_expr_type_trait_intrinsic(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_type_trait_intrinsic(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_type_trait_intrinsic(const an_ifc_expr_type_trait_intrinsic universal)
/*
Given the universal representation of ExprTypeTraitIntrinsic, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "============================ ");
  fprintf(f_debug, "ExprTypeTraitIntrinsic ");
  fprintf(f_debug, "============================\n");
  db_node_an_ifc_expr_type_trait_intrinsic_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_typeid_indent(const an_ifc_expr_typeid universal, unsigned indent)
/*
Given the universal representation of ExprTypeid, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_expr_typeid(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_typeid(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_operand_an_ifc_expr_typeid(universal)) {
    an_ifc_type_index field = get_ifc_operand_an_ifc_expr_typeid(universal);

    db_print_indent(indent);
    fprintf(f_debug, "operand:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_type_an_ifc_expr_typeid(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_typeid(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_typeid(const an_ifc_expr_typeid universal)
/*
Given the universal representation of ExprTypeid, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "ExprTypeid ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_expr_typeid_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_unary_fold_indent(const an_ifc_expr_unary_fold universal, unsigned indent)
/*
Given the universal representation of ExprUnaryFold, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_associativity_an_ifc_expr_unary_fold(universal)) {
    an_ifc_associativity field = get_ifc_associativity_an_ifc_expr_unary_fold(universal);

    db_print_indent(indent);
    fprintf(f_debug, "associativity: %llu\n",
            (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_expr_an_ifc_expr_unary_fold(universal)) {
    an_ifc_expr_index field = get_ifc_expr_an_ifc_expr_unary_fold(universal);

    db_print_indent(indent);
    fprintf(f_debug, "expr:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_expr_unary_fold(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_unary_fold(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_operation_an_ifc_expr_unary_fold(universal)) {
    an_ifc_dyadic_operator_sort field = get_ifc_operation_an_ifc_expr_unary_fold(universal);

    db_print_indent(indent);
    fprintf(f_debug, "operation: %s\n", str_for_an_ifc_dyadic_operator_sort(field));
  }  /* if */
  if (has_ifc_type_an_ifc_expr_unary_fold(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_unary_fold(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_unary_fold(const an_ifc_expr_unary_fold universal)
/*
Given the universal representation of ExprUnaryFold, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "ExprUnaryFold ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_expr_unary_fold_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_unqualified_id_indent(const an_ifc_expr_unqualified_id universal, unsigned indent)
/*
Given the universal representation of ExprUnqualifiedId, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_expr_unqualified_id(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_unqualified_id(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_expr_unqualified_id(universal)) {
    an_ifc_name_index field = get_ifc_name_an_ifc_expr_unqualified_id(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name:");
    if (is_null_index_an_ifc_name_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_name_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_resolution_an_ifc_expr_unqualified_id(universal)) {
    an_ifc_expr_index field = get_ifc_resolution_an_ifc_expr_unqualified_id(universal);

    db_print_indent(indent);
    fprintf(f_debug, "resolution:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_template_keyword_an_ifc_expr_unqualified_id(universal)) {
    an_ifc_source_location field = get_ifc_template_keyword_an_ifc_expr_unqualified_id(universal);

    db_print_indent(indent);
    fprintf(f_debug, "template_keyword:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_type_an_ifc_expr_unqualified_id(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_unqualified_id(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_unqualified_id(const an_ifc_expr_unqualified_id universal)
/*
Given the universal representation of ExprUnqualifiedId, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================== ");
  fprintf(f_debug, "ExprUnqualifiedId ");
  fprintf(f_debug, "===============================\n");
  db_node_an_ifc_expr_unqualified_id_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_unresolved_id_indent(const an_ifc_expr_unresolved_id universal, unsigned indent)
/*
Given the universal representation of ExprUnresolvedId, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_expr_unresolved_id(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_unresolved_id(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_expr_unresolved_id(universal)) {
    an_ifc_name_index field = get_ifc_name_an_ifc_expr_unresolved_id(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name:");
    if (is_null_index_an_ifc_name_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_name_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_type_an_ifc_expr_unresolved_id(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_unresolved_id(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_unresolved_id(const an_ifc_expr_unresolved_id universal)
/*
Given the universal representation of ExprUnresolvedId, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "=============================== ");
  fprintf(f_debug, "ExprUnresolvedId ");
  fprintf(f_debug, "===============================\n");
  db_node_an_ifc_expr_unresolved_id_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_expr_virtual_function_conversion_indent(const an_ifc_expr_virtual_function_conversion universal,
             unsigned                                      indent)
/*
Given the universal representation of ExprVirtualFunctionConversion, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_function_an_ifc_expr_virtual_function_conversion(universal)) {
    an_ifc_decl_index field = get_ifc_function_an_ifc_expr_virtual_function_conversion(universal);

    db_print_indent(indent);
    fprintf(f_debug, "function:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_expr_virtual_function_conversion(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_expr_virtual_function_conversion(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_type_an_ifc_expr_virtual_function_conversion(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_expr_virtual_function_conversion(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_expr_virtual_function_conversion(const an_ifc_expr_virtual_function_conversion universal)
/*
Given the universal representation of ExprVirtualFunctionConversion, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "======================== ");
  fprintf(f_debug, "ExprVirtualFunctionConversion ");
  fprintf(f_debug, "=========================\n");
  db_node_an_ifc_expr_virtual_function_conversion_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_form_catenate_indent(const an_ifc_form_catenate universal, unsigned indent)
/*
Given the universal representation of FormCatenate, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_first_an_ifc_form_catenate(universal)) {
    an_ifc_form_index field = get_ifc_first_an_ifc_form_catenate(universal);

    db_print_indent(indent);
    fprintf(f_debug, "first:");
    if (is_null_index_an_ifc_form_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_form_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_form_catenate(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_form_catenate(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_second_an_ifc_form_catenate(universal)) {
    an_ifc_form_index field = get_ifc_second_an_ifc_form_catenate(universal);

    db_print_indent(indent);
    fprintf(f_debug, "second:");
    if (is_null_index_an_ifc_form_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_form_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_form_catenate(const an_ifc_form_catenate universal)
/*
Given the universal representation of FormCatenate, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "FormCatenate ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_form_catenate_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_form_character_indent(const an_ifc_form_character universal, unsigned indent)
/*
Given the universal representation of FormCharacter, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_form_character(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_form_character(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_spelling_an_ifc_form_character(universal)) {
    an_ifc_text_offset field = get_ifc_spelling_an_ifc_form_character(universal);

    db_print_indent(indent);
    fprintf(f_debug, "spelling: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_form_character(const an_ifc_form_character universal)
/*
Given the universal representation of FormCharacter, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "FormCharacter ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_form_character_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_form_header_indent(const an_ifc_form_header universal, unsigned indent)
/*
Given the universal representation of FormHeader, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_form_header(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_form_header(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_spelling_an_ifc_form_header(universal)) {
    an_ifc_text_offset field = get_ifc_spelling_an_ifc_form_header(universal);

    db_print_indent(indent);
    fprintf(f_debug, "spelling: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_form_header(const an_ifc_form_header universal)
/*
Given the universal representation of FormHeader, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "FormHeader ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_form_header_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_form_identifier_indent(const an_ifc_form_identifier universal, unsigned indent)
/*
Given the universal representation of FormIdentifier, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_form_identifier(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_form_identifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_spelling_an_ifc_form_identifier(universal)) {
    an_ifc_text_offset field = get_ifc_spelling_an_ifc_form_identifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "spelling: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_form_identifier(const an_ifc_form_identifier universal)
/*
Given the universal representation of FormIdentifier, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "FormIdentifier ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_form_identifier_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_form_junk_indent(const an_ifc_form_junk universal, unsigned indent)
/*
Given the universal representation of FormJunk, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_form_junk(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_form_junk(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_spelling_an_ifc_form_junk(universal)) {
    an_ifc_text_offset field = get_ifc_spelling_an_ifc_form_junk(universal);

    db_print_indent(indent);
    fprintf(f_debug, "spelling: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_form_junk(const an_ifc_form_junk universal)
/*
Given the universal representation of FormJunk, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "=================================== ");
  fprintf(f_debug, "FormJunk ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_form_junk_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_form_keyword_indent(const an_ifc_form_keyword universal, unsigned indent)
/*
Given the universal representation of FormKeyword, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_form_keyword(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_form_keyword(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_spelling_an_ifc_form_keyword(universal)) {
    an_ifc_text_offset field = get_ifc_spelling_an_ifc_form_keyword(universal);

    db_print_indent(indent);
    fprintf(f_debug, "spelling: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_form_keyword(const an_ifc_form_keyword universal)
/*
Given the universal representation of FormKeyword, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "FormKeyword ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_form_keyword_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_form_number_indent(const an_ifc_form_number universal, unsigned indent)
/*
Given the universal representation of FormNumber, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_form_number(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_form_number(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_spelling_an_ifc_form_number(universal)) {
    an_ifc_text_offset field = get_ifc_spelling_an_ifc_form_number(universal);

    db_print_indent(indent);
    fprintf(f_debug, "spelling: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_form_number(const an_ifc_form_number universal)
/*
Given the universal representation of FormNumber, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "FormNumber ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_form_number_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_form_operator_indent(const an_ifc_form_operator universal, unsigned indent)
/*
Given the universal representation of FormOperator, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_form_operator(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_form_operator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_op_an_ifc_form_operator(universal)) {
    an_ifc_form_operator_sort field = get_ifc_op_an_ifc_form_operator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "op: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_spelling_an_ifc_form_operator(universal)) {
    an_ifc_text_offset field = get_ifc_spelling_an_ifc_form_operator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "spelling: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_form_operator(const an_ifc_form_operator universal)
/*
Given the universal representation of FormOperator, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "FormOperator ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_form_operator_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_form_parameter_indent(const an_ifc_form_parameter universal, unsigned indent)
/*
Given the universal representation of FormParameter, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_form_parameter(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_form_parameter(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_spelling_an_ifc_form_parameter(universal)) {
    an_ifc_text_offset field = get_ifc_spelling_an_ifc_form_parameter(universal);

    db_print_indent(indent);
    fprintf(f_debug, "spelling: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_form_parameter(const an_ifc_form_parameter universal)
/*
Given the universal representation of FormParameter, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "FormParameter ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_form_parameter_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_form_parenthesized_indent(const an_ifc_form_parenthesized universal, unsigned indent)
/*
Given the universal representation of FormParenthesized, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_form_parenthesized(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_form_parenthesized(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_operand_an_ifc_form_parenthesized(universal)) {
    an_ifc_form_index field = get_ifc_operand_an_ifc_form_parenthesized(universal);

    db_print_indent(indent);
    fprintf(f_debug, "operand:");
    if (is_null_index_an_ifc_form_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_form_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_form_parenthesized(const an_ifc_form_parenthesized universal)
/*
Given the universal representation of FormParenthesized, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================== ");
  fprintf(f_debug, "FormParenthesized ");
  fprintf(f_debug, "===============================\n");
  db_node_an_ifc_form_parenthesized_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_form_pragma_indent(const an_ifc_form_pragma universal, unsigned indent)
/*
Given the universal representation of FormPragma, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_form_pragma(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_form_pragma(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_operand_an_ifc_form_pragma(universal)) {
    an_ifc_form_index field = get_ifc_operand_an_ifc_form_pragma(universal);

    db_print_indent(indent);
    fprintf(f_debug, "operand:");
    if (is_null_index_an_ifc_form_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_form_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_form_pragma(const an_ifc_form_pragma universal)
/*
Given the universal representation of FormPragma, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "FormPragma ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_form_pragma_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_form_spec_indent(const an_ifc_form_spec universal, unsigned indent)
/*
Given the universal representation of FormSpec, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_arguments_an_ifc_form_spec(universal)) {
    an_ifc_expr_index field = get_ifc_arguments_an_ifc_form_spec(universal);

    db_print_indent(indent);
    fprintf(f_debug, "arguments:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_primary_template_an_ifc_form_spec(universal)) {
    an_ifc_decl_index field = get_ifc_primary_template_an_ifc_form_spec(universal);

    db_print_indent(indent);
    fprintf(f_debug, "primary_template:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_form_spec(const an_ifc_form_spec universal)
/*
Given the universal representation of FormSpec, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "=================================== ");
  fprintf(f_debug, "FormSpec ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_form_spec_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_form_string_indent(const an_ifc_form_string universal, unsigned indent)
/*
Given the universal representation of FormString, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_form_string(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_form_string(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_spelling_an_ifc_form_string(universal)) {
    an_ifc_text_offset field = get_ifc_spelling_an_ifc_form_string(universal);

    db_print_indent(indent);
    fprintf(f_debug, "spelling: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_form_string(const an_ifc_form_string universal)
/*
Given the universal representation of FormString, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "FormString ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_form_string_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_form_stringize_indent(const an_ifc_form_stringize universal, unsigned indent)
/*
Given the universal representation of FormStringize, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_form_stringize(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_form_stringize(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_operand_an_ifc_form_stringize(universal)) {
    an_ifc_form_index field = get_ifc_operand_an_ifc_form_stringize(universal);

    db_print_indent(indent);
    fprintf(f_debug, "operand:");
    if (is_null_index_an_ifc_form_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_form_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_form_stringize(const an_ifc_form_stringize universal)
/*
Given the universal representation of FormStringize, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "FormStringize ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_form_stringize_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_form_tuple_indent(const an_ifc_form_tuple universal, unsigned indent)
/*
Given the universal representation of FormTuple, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_cardinality_an_ifc_form_tuple(universal)) {
    an_ifc_cardinality field = get_ifc_cardinality_an_ifc_form_tuple(universal);

    db_print_indent(indent);
    fprintf(f_debug, "cardinality: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_start_an_ifc_form_tuple(universal)) {
    an_ifc_index field = get_ifc_start_an_ifc_form_tuple(universal);

    db_print_indent(indent);
    fprintf(f_debug, "start: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_form_tuple(const an_ifc_form_tuple universal)
/*
Given the universal representation of FormTuple, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "FormTuple ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_form_tuple_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_form_whitespace_indent(const an_ifc_form_whitespace universal, unsigned indent)
/*
Given the universal representation of FormWhitespace, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_form_whitespace(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_form_whitespace(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_form_whitespace(const an_ifc_form_whitespace universal)
/*
Given the universal representation of FormWhitespace, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "FormWhitespace ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_form_whitespace_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_heap_attr_indent(const an_ifc_heap_attr universal, unsigned indent)
/*
Given the universal representation of HeapAttr, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_value_an_ifc_heap_attr(universal)) {
    an_ifc_attr_index field = get_ifc_value_an_ifc_heap_attr(universal);

    db_print_indent(indent);
    fprintf(f_debug, "value:");
    if (is_null_index_an_ifc_attr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_attr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_heap_attr(const an_ifc_heap_attr universal)
/*
Given the universal representation of HeapAttr, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "=================================== ");
  fprintf(f_debug, "HeapAttr ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_heap_attr_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_heap_chart_indent(const an_ifc_heap_chart universal, unsigned indent)
/*
Given the universal representation of HeapChart, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_value_an_ifc_heap_chart(universal)) {
    an_ifc_chart_index field = get_ifc_value_an_ifc_heap_chart(universal);

    db_print_indent(indent);
    fprintf(f_debug, "value:");
    if (is_null_index_an_ifc_chart_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_chart_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_heap_chart(const an_ifc_heap_chart universal)
/*
Given the universal representation of HeapChart, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "HeapChart ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_heap_chart_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_heap_decl_indent(const an_ifc_heap_decl universal, unsigned indent)
/*
Given the universal representation of HeapDecl, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_value_an_ifc_heap_decl(universal)) {
    an_ifc_decl_index field = get_ifc_value_an_ifc_heap_decl(universal);

    db_print_indent(indent);
    fprintf(f_debug, "value:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_heap_decl(const an_ifc_heap_decl universal)
/*
Given the universal representation of HeapDecl, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "=================================== ");
  fprintf(f_debug, "HeapDecl ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_heap_decl_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_heap_expr_indent(const an_ifc_heap_expr universal, unsigned indent)
/*
Given the universal representation of HeapExpr, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_value_an_ifc_heap_expr(universal)) {
    an_ifc_expr_index field = get_ifc_value_an_ifc_heap_expr(universal);

    db_print_indent(indent);
    fprintf(f_debug, "value:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_heap_expr(const an_ifc_heap_expr universal)
/*
Given the universal representation of HeapExpr, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "=================================== ");
  fprintf(f_debug, "HeapExpr ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_heap_expr_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_heap_form_indent(const an_ifc_heap_form universal, unsigned indent)
/*
Given the universal representation of HeapForm, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_value_an_ifc_heap_form(universal)) {
    an_ifc_form_index field = get_ifc_value_an_ifc_heap_form(universal);

    db_print_indent(indent);
    fprintf(f_debug, "value:");
    if (is_null_index_an_ifc_form_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_form_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_heap_form(const an_ifc_heap_form universal)
/*
Given the universal representation of HeapForm, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "=================================== ");
  fprintf(f_debug, "HeapForm ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_heap_form_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_heap_pp_form_indent(const an_ifc_heap_pp_form universal, unsigned indent)
/*
Given the universal representation of HeapPPForm, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_value_an_ifc_heap_pp_form(universal)) {
    an_ifc_form_index field = get_ifc_value_an_ifc_heap_pp_form(universal);

    db_print_indent(indent);
    fprintf(f_debug, "value:");
    if (is_null_index_an_ifc_form_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_form_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_heap_pp_form(const an_ifc_heap_pp_form universal)
/*
Given the universal representation of HeapPPForm, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "HeapPPForm ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_heap_pp_form_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_heap_stmt_indent(const an_ifc_heap_stmt universal, unsigned indent)
/*
Given the universal representation of HeapStmt, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_value_an_ifc_heap_stmt(universal)) {
    an_ifc_stmt_index field = get_ifc_value_an_ifc_heap_stmt(universal);

    db_print_indent(indent);
    fprintf(f_debug, "value:");
    if (is_null_index_an_ifc_stmt_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_stmt_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_heap_stmt(const an_ifc_heap_stmt universal)
/*
Given the universal representation of HeapStmt, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "=================================== ");
  fprintf(f_debug, "HeapStmt ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_heap_stmt_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_heap_string_indent(const an_ifc_heap_string universal, unsigned indent)
/*
Given the universal representation of HeapString, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_value_an_ifc_heap_string(universal)) {
    an_ifc_text_offset field = get_ifc_value_an_ifc_heap_string(universal);

    db_print_indent(indent);
    fprintf(f_debug, "value: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_heap_string(const an_ifc_heap_string universal)
/*
Given the universal representation of HeapString, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "HeapString ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_heap_string_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_heap_syntax_indent(const an_ifc_heap_syntax universal, unsigned indent)
/*
Given the universal representation of HeapSyntax, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_value_an_ifc_heap_syntax(universal)) {
    an_ifc_syntax_index field = get_ifc_value_an_ifc_heap_syntax(universal);

    db_print_indent(indent);
    fprintf(f_debug, "value:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_heap_syntax(const an_ifc_heap_syntax universal)
/*
Given the universal representation of HeapSyntax, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "HeapSyntax ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_heap_syntax_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_heap_type_indent(const an_ifc_heap_type universal, unsigned indent)
/*
Given the universal representation of HeapType, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_value_an_ifc_heap_type(universal)) {
    an_ifc_type_index field = get_ifc_value_an_ifc_heap_type(universal);

    db_print_indent(indent);
    fprintf(f_debug, "value:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_heap_type(const an_ifc_heap_type universal)
/*
Given the universal representation of HeapType, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "=================================== ");
  fprintf(f_debug, "HeapType ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_heap_type_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_macro_function_like_indent(const an_ifc_macro_function_like universal, unsigned indent)
/*
Given the universal representation of MacroFunctionLike, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_arity_variadic_an_ifc_macro_function_like(universal)) {
    db_print_indent(indent);
    fprintf(f_debug, "arity_variadic: UNIMPLEMENTED\n");
  }  /* if */
  if (has_ifc_body_an_ifc_macro_function_like(universal)) {
    an_ifc_form_index field = get_ifc_body_an_ifc_macro_function_like(universal);

    db_print_indent(indent);
    fprintf(f_debug, "body:");
    if (is_null_index_an_ifc_form_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_form_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_macro_function_like(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_macro_function_like(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_macro_function_like(universal)) {
    an_ifc_text_offset field = get_ifc_name_an_ifc_macro_function_like(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_parameters_an_ifc_macro_function_like(universal)) {
    an_ifc_form_index field = get_ifc_parameters_an_ifc_macro_function_like(universal);

    db_print_indent(indent);
    fprintf(f_debug, "parameters:");
    if (is_null_index_an_ifc_form_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_form_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_macro_function_like(const an_ifc_macro_function_like universal)
/*
Given the universal representation of MacroFunctionLike, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================== ");
  fprintf(f_debug, "MacroFunctionLike ");
  fprintf(f_debug, "===============================\n");
  db_node_an_ifc_macro_function_like_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_macro_object_like_indent(const an_ifc_macro_object_like universal, unsigned indent)
/*
Given the universal representation of MacroObjectLike, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_body_an_ifc_macro_object_like(universal)) {
    an_ifc_form_index field = get_ifc_body_an_ifc_macro_object_like(universal);

    db_print_indent(indent);
    fprintf(f_debug, "body:");
    if (is_null_index_an_ifc_form_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_form_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_macro_object_like(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_macro_object_like(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_macro_object_like(universal)) {
    an_ifc_text_offset field = get_ifc_name_an_ifc_macro_object_like(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_macro_object_like(const an_ifc_macro_object_like universal)
/*
Given the universal representation of MacroObjectLike, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "=============================== ");
  fprintf(f_debug, "MacroObjectLike ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_macro_object_like_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_meta_tool_invocation_indent(const an_ifc_meta_tool_invocation universal, unsigned indent)
/*
Given the universal representation of MetaToolInvocation, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_args_an_ifc_meta_tool_invocation(universal)) {
    an_ifc_sequence field = get_ifc_args_an_ifc_meta_tool_invocation(universal);

    db_print_indent(indent);
    fprintf(f_debug, "args:\n");
    db_node_an_ifc_sequence_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_cmd_an_ifc_meta_tool_invocation(universal)) {
    an_ifc_text_offset field = get_ifc_cmd_an_ifc_meta_tool_invocation(universal);

    db_print_indent(indent);
    fprintf(f_debug, "cmd: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_meta_tool_invocation(const an_ifc_meta_tool_invocation universal)
/*
Given the universal representation of MetaToolInvocation, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================== ");
  fprintf(f_debug, "MetaToolInvocation ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_meta_tool_invocation_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_meta_tool_maker_indent(const an_ifc_meta_tool_maker universal, unsigned indent)
/*
Given the universal representation of MetaToolMaker, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_value_an_ifc_meta_tool_maker(universal)) {
    an_ifc_text_offset field = get_ifc_value_an_ifc_meta_tool_maker(universal);

    db_print_indent(indent);
    fprintf(f_debug, "value: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_meta_tool_maker(const an_ifc_meta_tool_maker universal)
/*
Given the universal representation of MetaToolMaker, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "MetaToolMaker ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_meta_tool_maker_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_module_export_reference_indent(const an_ifc_module_export_reference universal, unsigned indent)
/*
Given the universal representation of ModuleExportReference, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_reference_an_ifc_module_export_reference(universal)) {
    an_ifc_module_reference field = get_ifc_reference_an_ifc_module_export_reference(universal);

    db_print_indent(indent);
    fprintf(f_debug, "reference:\n");
    db_node_an_ifc_module_reference_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_module_export_reference(const an_ifc_module_export_reference universal)
/*
Given the universal representation of ModuleExportReference, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================ ");
  fprintf(f_debug, "ModuleExportReference ");
  fprintf(f_debug, "=============================\n");
  db_node_an_ifc_module_export_reference_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_module_import_reference_indent(const an_ifc_module_import_reference universal, unsigned indent)
/*
Given the universal representation of ModuleImportReference, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_reference_an_ifc_module_import_reference(universal)) {
    an_ifc_module_reference field = get_ifc_reference_an_ifc_module_import_reference(universal);

    db_print_indent(indent);
    fprintf(f_debug, "reference:\n");
    db_node_an_ifc_module_reference_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_module_import_reference(const an_ifc_module_import_reference universal)
/*
Given the universal representation of ModuleImportReference, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================ ");
  fprintf(f_debug, "ModuleImportReference ");
  fprintf(f_debug, "=============================\n");
  db_node_an_ifc_module_import_reference_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_name_conversion_indent(const an_ifc_name_conversion universal, unsigned indent)
/*
Given the universal representation of NameConversion, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_encoded_an_ifc_name_conversion(universal)) {
    an_ifc_text_offset field = get_ifc_encoded_an_ifc_name_conversion(universal);

    db_print_indent(indent);
    fprintf(f_debug, "encoded: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_target_an_ifc_name_conversion(universal)) {
    an_ifc_type_index field = get_ifc_target_an_ifc_name_conversion(universal);

    db_print_indent(indent);
    fprintf(f_debug, "target:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_name_conversion(const an_ifc_name_conversion universal)
/*
Given the universal representation of NameConversion, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "NameConversion ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_name_conversion_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_name_guide_indent(const an_ifc_name_guide universal, unsigned indent)
/*
Given the universal representation of NameGuide, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_primary_template_an_ifc_name_guide(universal)) {
    an_ifc_decl_index field = get_ifc_primary_template_an_ifc_name_guide(universal);

    db_print_indent(indent);
    fprintf(f_debug, "primary_template:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_name_guide(const an_ifc_name_guide universal)
/*
Given the universal representation of NameGuide, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "NameGuide ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_name_guide_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_name_literal_indent(const an_ifc_name_literal universal, unsigned indent)
/*
Given the universal representation of NameLiteral, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_encoded_an_ifc_name_literal(universal)) {
    an_ifc_text_offset field = get_ifc_encoded_an_ifc_name_literal(universal);

    db_print_indent(indent);
    fprintf(f_debug, "encoded: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_name_literal(const an_ifc_name_literal universal)
/*
Given the universal representation of NameLiteral, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "NameLiteral ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_name_literal_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_name_operator_indent(const an_ifc_name_operator universal, unsigned indent)
/*
Given the universal representation of NameOperator, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_encoded_an_ifc_name_operator(universal)) {
    an_ifc_text_offset field = get_ifc_encoded_an_ifc_name_operator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "encoded: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_operator_an_ifc_name_operator(universal)) {
    an_ifc_operator_category field = get_ifc_operator_an_ifc_name_operator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "operator:\n");
    db_print_indent(indent);
    fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_operator_sort(field.sort));
    ++indent;
    switch (field.sort) {
      case ifc_os_dyadic_operator:
        { an_ifc_dyadic_operator_sort do_ref = field.variant.dyadic_operator;

          db_print_indent(indent);
          fprintf(f_debug, "dyadic_operator: %s\n", str_for_an_ifc_dyadic_operator_sort(do_ref));
        }
        break;
      case ifc_os_monadic_operator:
        { an_ifc_monadic_operator_sort mo_ref =
                                                field.variant.monadic_operator;

          db_print_indent(indent);
          fprintf(f_debug, "monadic_operator: %s\n", str_for_an_ifc_monadic_operator_sort(mo_ref));
        }
        break;
      case ifc_os_niladic_operator:
        { an_ifc_niladic_operator_sort no_ref =
                                                field.variant.niladic_operator;

          db_print_indent(indent);
          fprintf(f_debug, "niladic_operator: %s\n", str_for_an_ifc_niladic_operator_sort(no_ref));
        }
        break;
      case ifc_os_storage_instruction_operator:
        { an_ifc_storage_instruction_operator_sort sio_ref =
                                    field.variant.storage_instruction_operator;

          db_print_indent(indent);
          fprintf(f_debug, "storage_instruction_operator: %s\n",
                  str_for_an_ifc_storage_instruction_operator_sort(sio_ref));
        }
        break;
      case ifc_os_triadic_operator:
        { an_ifc_triadic_operator_sort to_ref =
                                                field.variant.triadic_operator;

          db_print_indent(indent);
          fprintf(f_debug, "triadic_operator: %s\n", str_for_an_ifc_triadic_operator_sort(to_ref));
        }
        break;
      case ifc_os_variadic_operator:
        { an_ifc_variadic_operator_sort vo_ref =
                                               field.variant.variadic_operator;

          db_print_indent(indent);
          fprintf(f_debug, "variadic_operator: %s\n", str_for_an_ifc_variadic_operator_sort(vo_ref));
        }
        break;
      default_is_unexpected();
    }  /* switch */
    --indent;
  }  /* if */
}  /* db_node */


void db_node_an_ifc_name_operator(const an_ifc_name_operator universal)
/*
Given the universal representation of NameOperator, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "NameOperator ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_name_operator_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_name_source_file_indent(const an_ifc_name_source_file universal, unsigned indent)
/*
Given the universal representation of NameSourceFile, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_guard_an_ifc_name_source_file(universal)) {
    an_ifc_text_offset field = get_ifc_guard_an_ifc_name_source_file(universal);

    db_print_indent(indent);
    fprintf(f_debug, "guard: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_path_an_ifc_name_source_file(universal)) {
    an_ifc_text_offset field = get_ifc_path_an_ifc_name_source_file(universal);

    db_print_indent(indent);
    fprintf(f_debug, "path: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_name_source_file(const an_ifc_name_source_file universal)
/*
Given the universal representation of NameSourceFile, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "NameSourceFile ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_name_source_file_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_name_specialization_indent(const an_ifc_name_specialization universal, unsigned indent)
/*
Given the universal representation of NameSpecialization, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_arguments_an_ifc_name_specialization(universal)) {
    an_ifc_expr_index field = get_ifc_arguments_an_ifc_name_specialization(universal);

    db_print_indent(indent);
    fprintf(f_debug, "arguments:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_primary_an_ifc_name_specialization(universal)) {
    an_ifc_name_index field = get_ifc_primary_an_ifc_name_specialization(universal);

    db_print_indent(indent);
    fprintf(f_debug, "primary:");
    if (is_null_index_an_ifc_name_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_name_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_name_specialization(const an_ifc_name_specialization universal)
/*
Given the universal representation of NameSpecialization, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================== ");
  fprintf(f_debug, "NameSpecialization ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_name_specialization_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_name_template_indent(const an_ifc_name_template universal, unsigned indent)
/*
Given the universal representation of NameTemplate, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_name_an_ifc_name_template(universal)) {
    an_ifc_name_index field = get_ifc_name_an_ifc_name_template(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name:");
    if (is_null_index_an_ifc_name_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_name_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_name_template(const an_ifc_name_template universal)
/*
Given the universal representation of NameTemplate, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "NameTemplate ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_name_template_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_scope_descriptor_indent(const an_ifc_scope_descriptor universal, unsigned indent)
/*
Given the universal representation of ScopeDescriptor, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_cardinality_an_ifc_scope_descriptor(universal)) {
    an_ifc_cardinality field = get_ifc_cardinality_an_ifc_scope_descriptor(universal);

    db_print_indent(indent);
    fprintf(f_debug, "cardinality: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_start_an_ifc_scope_descriptor(universal)) {
    an_ifc_index field = get_ifc_start_an_ifc_scope_descriptor(universal);

    db_print_indent(indent);
    fprintf(f_debug, "start: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_scope_descriptor(const an_ifc_scope_descriptor universal)
/*
Given the universal representation of ScopeDescriptor, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "=============================== ");
  fprintf(f_debug, "ScopeDescriptor ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_scope_descriptor_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_scope_member_indent(const an_ifc_scope_member universal, unsigned indent)
/*
Given the universal representation of ScopeMember, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_index_an_ifc_scope_member(universal)) {
    an_ifc_decl_index field = get_ifc_index_an_ifc_scope_member(universal);

    db_print_indent(indent);
    fprintf(f_debug, "index:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_scope_member(const an_ifc_scope_member universal)
/*
Given the universal representation of ScopeMember, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "ScopeMember ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_scope_member_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_source_line_indent(const an_ifc_source_line universal, unsigned indent)
/*
Given the universal representation of SourceLine, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_file_an_ifc_source_line(universal)) {
    an_ifc_name_index field = get_ifc_file_an_ifc_source_line(universal);

    db_print_indent(indent);
    fprintf(f_debug, "file:");
    if (is_null_index_an_ifc_name_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_name_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_line_an_ifc_source_line(universal)) {
    an_ifc_line_number field = get_ifc_line_an_ifc_source_line(universal);

    db_print_indent(indent);
    fprintf(f_debug, "line: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_source_line(const an_ifc_source_line universal)
/*
Given the universal representation of SourceLine, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "SourceLine ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_source_line_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_source_sentence_indent(const an_ifc_source_sentence universal, unsigned indent)
/*
Given the universal representation of SourceSentence, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_cardinality_an_ifc_source_sentence(universal)) {
    an_ifc_cardinality field = get_ifc_cardinality_an_ifc_source_sentence(universal);

    db_print_indent(indent);
    fprintf(f_debug, "cardinality: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_locus_an_ifc_source_sentence(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_source_sentence(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_start_an_ifc_source_sentence(universal)) {
    an_ifc_index field = get_ifc_start_an_ifc_source_sentence(universal);

    db_print_indent(indent);
    fprintf(f_debug, "start: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_source_sentence(const an_ifc_source_sentence universal)
/*
Given the universal representation of SourceSentence, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "SourceSentence ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_source_sentence_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_source_word_indent(const an_ifc_source_word universal, unsigned indent)
/*
Given the universal representation of SourceWord, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_category_an_ifc_source_word(universal)) {
    an_ifc_word_category field = get_ifc_category_an_ifc_source_word(universal);

    db_print_indent(indent);
    fprintf(f_debug, "category:\n");
    db_print_indent(indent);
    fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_word_sort(field.sort));
    ++indent;
    switch (field.sort) {
      case ifc_ws_source_directive:
        { an_ifc_source_directive_sort sd_ref =
                                                field.variant.source_directive;

          db_print_indent(indent);
          fprintf(f_debug, "source_directive: %s\n", str_for_an_ifc_source_directive_sort(sd_ref));
        }
        break;
      case ifc_ws_source_identifier:
        { an_ifc_source_identifier_category si_ref =
                                               field.variant.source_identifier;

          db_print_indent(indent);
          fprintf(f_debug, "source_identifier:\n");
          db_print_indent(indent);
          fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_source_identifier_sort(si_ref.sort));
          ++indent;
          switch (si_ref.sort) {
            case ifc_sis_msvc:
              { an_ifc_source_unknown_identifier m_ref = si_ref.variant.msvc;

                db_print_indent(indent);
                fprintf(f_debug, "msvc: %llu\n",
                        (unsigned long long)m_ref.value);
              }
              break;
            case ifc_sis_msvc_builtin_huge_val:
              { an_ifc_source_unknown_identifier mbhv_ref =
                                          si_ref.variant.msvc_builtin_huge_val;

                db_print_indent(indent);
                fprintf(f_debug, "msvc_builtin_huge_val: %llu\n",
                        (unsigned long long)mbhv_ref.value);
              }
              break;
            case ifc_sis_msvc_builtin_huge_valf:
              { an_ifc_source_unknown_identifier mbhv_ref =
                                         si_ref.variant.msvc_builtin_huge_valf;

                db_print_indent(indent);
                fprintf(f_debug, "msvc_builtin_huge_valf: %llu\n",
                        (unsigned long long)mbhv_ref.value);
              }
              break;
            case ifc_sis_msvc_builtin_nan:
              { an_ifc_source_unknown_identifier mbn_ref =
                                               si_ref.variant.msvc_builtin_nan;

                db_print_indent(indent);
                fprintf(f_debug, "msvc_builtin_nan: %llu\n",
                        (unsigned long long)mbn_ref.value);
              }
              break;
            case ifc_sis_msvc_builtin_nanf:
              { an_ifc_source_unknown_identifier mbn_ref =
                                              si_ref.variant.msvc_builtin_nanf;

                db_print_indent(indent);
                fprintf(f_debug, "msvc_builtin_nanf: %llu\n",
                        (unsigned long long)mbn_ref.value);
              }
              break;
            case ifc_sis_msvc_builtin_nans:
              { an_ifc_source_unknown_identifier mbn_ref =
                                              si_ref.variant.msvc_builtin_nans;

                db_print_indent(indent);
                fprintf(f_debug, "msvc_builtin_nans: %llu\n",
                        (unsigned long long)mbn_ref.value);
              }
              break;
            case ifc_sis_msvc_builtin_nansf:
              { an_ifc_source_unknown_identifier mbn_ref =
                                             si_ref.variant.msvc_builtin_nansf;

                db_print_indent(indent);
                fprintf(f_debug, "msvc_builtin_nansf: %llu\n",
                        (unsigned long long)mbn_ref.value);
              }
              break;
            case ifc_sis_plain:
              { an_ifc_text_offset p_ref = si_ref.variant.plain;

                db_print_indent(indent);
                fprintf(f_debug, "plain: %llu\n",
                        (unsigned long long)p_ref.value);
              }
              break;
            default_is_unexpected();
          }  /* switch */
          --indent;
        }
        break;
      case ifc_ws_source_keyword:
        { an_ifc_source_keyword_sort sk_ref = field.variant.source_keyword;

          db_print_indent(indent);
          fprintf(f_debug, "source_keyword: %s\n", str_for_an_ifc_source_keyword_sort(sk_ref));
        }
        break;
      case ifc_ws_source_literal:
        { an_ifc_source_literal_category sl_ref =
                                                  field.variant.source_literal;

          db_print_indent(indent);
          fprintf(f_debug, "source_literal:\n");
          db_print_indent(indent);
          fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_source_literal_sort(sl_ref.sort));
          ++indent;
          switch (sl_ref.sort) {
            case ifc_sls_defined_string:
              { an_ifc_string_index ds_ref = sl_ref.variant.defined_string;

                db_print_indent(indent);
                fprintf(f_debug, "defined_string:");
                if (is_null_index_an_ifc_string_index(ds_ref)) {
                  fprintf(f_debug, " NULL\n");
                } else {
                  fprintf(f_debug, "\n");
                  db_print_indent(indent);
                  fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_string_sort(ds_ref.sort));
                  db_print_indent(indent);
                  fprintf(f_debug, "  value: %llu\n",
                          (unsigned long long)ds_ref.value);
                }  /* if */
              }
              break;
            case ifc_sls_msvc:
              { an_ifc_source_unknown_literal m_ref = sl_ref.variant.msvc;

                db_print_indent(indent);
                fprintf(f_debug, "msvc: %llu\n",
                        (unsigned long long)m_ref.value);
              }
              break;
            case ifc_sls_msvc_binding:
              { an_ifc_expr_index mb_ref = sl_ref.variant.msvc_binding;

                db_print_indent(indent);
                fprintf(f_debug, "msvc_binding:");
                if (is_null_index_an_ifc_expr_index(mb_ref)) {
                  fprintf(f_debug, " NULL\n");
                } else {
                  fprintf(f_debug, "\n");
                  db_print_indent(indent);
                  fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(mb_ref.sort));
                  db_print_indent(indent);
                  fprintf(f_debug, "  value: %llu\n",
                          (unsigned long long)mb_ref.value);
                }  /* if */
              }
              break;
            case ifc_sls_msvc_cast_target_type:
              { an_ifc_type_index mctt_ref =
                                          sl_ref.variant.msvc_cast_target_type;

                db_print_indent(indent);
                fprintf(f_debug, "msvc_cast_target_type:");
                if (is_null_index_an_ifc_type_index(mctt_ref)) {
                  fprintf(f_debug, " NULL\n");
                } else {
                  fprintf(f_debug, "\n");
                  db_print_indent(indent);
                  fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(mctt_ref.sort));
                  db_print_indent(indent);
                  fprintf(f_debug, "  value: %llu\n",
                          (unsigned long long)mctt_ref.value);
                }  /* if */
              }
              break;
            case ifc_sls_msvc_defined_constant:
              { an_ifc_expr_index mdc_ref =
                                          sl_ref.variant.msvc_defined_constant;

                db_print_indent(indent);
                fprintf(f_debug, "msvc_defined_constant:");
                if (is_null_index_an_ifc_expr_index(mdc_ref)) {
                  fprintf(f_debug, " NULL\n");
                } else {
                  fprintf(f_debug, "\n");
                  db_print_indent(indent);
                  fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(mdc_ref.sort));
                  db_print_indent(indent);
                  fprintf(f_debug, "  value: %llu\n",
                          (unsigned long long)mdc_ref.value);
                }  /* if */
              }
              break;
            case ifc_sls_msvc_function_name_macro:
              { an_ifc_text_offset mfnm_ref =
                                       sl_ref.variant.msvc_function_name_macro;

                db_print_indent(indent);
                fprintf(f_debug, "msvc_function_name_macro: %llu\n",
                        (unsigned long long)mfnm_ref.value);
              }
              break;
            case ifc_sls_msvc_resolved_type:
              { an_ifc_type_index mrt_ref = sl_ref.variant.msvc_resolved_type;

                db_print_indent(indent);
                fprintf(f_debug, "msvc_resolved_type:");
                if (is_null_index_an_ifc_type_index(mrt_ref)) {
                  fprintf(f_debug, " NULL\n");
                } else {
                  fprintf(f_debug, "\n");
                  db_print_indent(indent);
                  fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(mrt_ref.sort));
                  db_print_indent(indent);
                  fprintf(f_debug, "  value: %llu\n",
                          (unsigned long long)mrt_ref.value);
                }  /* if */
              }
              break;
            case ifc_sls_msvc_string_prefix_macro:
              { an_ifc_text_offset mspm_ref =
                                       sl_ref.variant.msvc_string_prefix_macro;

                db_print_indent(indent);
                fprintf(f_debug, "msvc_string_prefix_macro: %llu\n",
                        (unsigned long long)mspm_ref.value);
              }
              break;
            case ifc_sls_scalar:
              { an_ifc_expr_index s_ref = sl_ref.variant.scalar;

                db_print_indent(indent);
                fprintf(f_debug, "scalar:");
                if (is_null_index_an_ifc_expr_index(s_ref)) {
                  fprintf(f_debug, " NULL\n");
                } else {
                  fprintf(f_debug, "\n");
                  db_print_indent(indent);
                  fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(s_ref.sort));
                  db_print_indent(indent);
                  fprintf(f_debug, "  value: %llu\n",
                          (unsigned long long)s_ref.value);
                }  /* if */
              }
              break;
            case ifc_sls_string:
              { an_ifc_string_index s_ref = sl_ref.variant.string;

                db_print_indent(indent);
                fprintf(f_debug, "string:");
                if (is_null_index_an_ifc_string_index(s_ref)) {
                  fprintf(f_debug, " NULL\n");
                } else {
                  fprintf(f_debug, "\n");
                  db_print_indent(indent);
                  fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_string_sort(s_ref.sort));
                  db_print_indent(indent);
                  fprintf(f_debug, "  value: %llu\n",
                          (unsigned long long)s_ref.value);
                }  /* if */
              }
              break;
            case ifc_sls_unknown:
              { an_ifc_source_unknown_literal u_ref = sl_ref.variant.unknown;

                db_print_indent(indent);
                fprintf(f_debug, "unknown: %llu\n",
                        (unsigned long long)u_ref.value);
              }
              break;
            default_is_unexpected();
          }  /* switch */
          --indent;
        }
        break;
      case ifc_ws_source_operator:
        { an_ifc_source_operator_sort so_ref = field.variant.source_operator;

          db_print_indent(indent);
          fprintf(f_debug, "source_operator: %s\n", str_for_an_ifc_source_operator_sort(so_ref));
        }
        break;
      case ifc_ws_source_punctuator:
        { an_ifc_source_punctuator_sort sp_ref =
                                               field.variant.source_punctuator;

          db_print_indent(indent);
          fprintf(f_debug, "source_punctuator: %s\n", str_for_an_ifc_source_punctuator_sort(sp_ref));
        }
        break;
      case ifc_ws_unknown:
        { an_ifc_source_unknown_word u_ref = field.variant.unknown;

          db_print_indent(indent);
          fprintf(f_debug, "unknown: %llu\n",
                  (unsigned long long)u_ref.value);
        }
        break;
      default_is_unexpected();
    }  /* switch */
    --indent;
  }  /* if */
  if (has_ifc_index_an_ifc_source_word(universal)) {
    an_ifc_index field = get_ifc_index_an_ifc_source_word(universal);

    db_print_indent(indent);
    fprintf(f_debug, "index: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_locus_an_ifc_source_word(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_source_word(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_sort_an_ifc_source_word(universal)) {
    an_ifc_word_sort field = get_ifc_sort_an_ifc_source_word(universal);

    db_print_indent(indent);
    fprintf(f_debug, "sort: %s\n", str_for_an_ifc_word_sort(field));
  }  /* if */
  if (has_ifc_value_an_ifc_source_word(universal)) {
    an_ifc_u16 field = get_ifc_value_an_ifc_source_word(universal);

    db_print_indent(indent);
    fprintf(f_debug, "value: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_source_word(const an_ifc_source_word universal)
/*
Given the universal representation of SourceWord, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "SourceWord ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_source_word_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_stmt_block_indent(const an_ifc_stmt_block universal, unsigned indent)
/*
Given the universal representation of StmtBlock, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_cardinality_an_ifc_stmt_block(universal)) {
    an_ifc_cardinality field = get_ifc_cardinality_an_ifc_stmt_block(universal);

    db_print_indent(indent);
    fprintf(f_debug, "cardinality: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_locus_an_ifc_stmt_block(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_stmt_block(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_start_an_ifc_stmt_block(universal)) {
    an_ifc_index field = get_ifc_start_an_ifc_stmt_block(universal);

    db_print_indent(indent);
    fprintf(f_debug, "start: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_stmt_block(const an_ifc_stmt_block universal)
/*
Given the universal representation of StmtBlock, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "StmtBlock ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_stmt_block_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_stmt_break_indent(const an_ifc_stmt_break universal, unsigned indent)
/*
Given the universal representation of StmtBreak, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_stmt_break(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_stmt_break(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_stmt_break(const an_ifc_stmt_break universal)
/*
Given the universal representation of StmtBreak, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "StmtBreak ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_stmt_break_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_stmt_case_indent(const an_ifc_stmt_case universal, unsigned indent)
/*
Given the universal representation of StmtCase, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_expr_an_ifc_stmt_case(universal)) {
    an_ifc_expr_index field = get_ifc_expr_an_ifc_stmt_case(universal);

    db_print_indent(indent);
    fprintf(f_debug, "expr:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_stmt_case(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_stmt_case(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_stmt_case(const an_ifc_stmt_case universal)
/*
Given the universal representation of StmtCase, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "=================================== ");
  fprintf(f_debug, "StmtCase ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_stmt_case_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_stmt_continue_indent(const an_ifc_stmt_continue universal, unsigned indent)
/*
Given the universal representation of StmtContinue, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_stmt_continue(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_stmt_continue(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_stmt_continue(const an_ifc_stmt_continue universal)
/*
Given the universal representation of StmtContinue, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "StmtContinue ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_stmt_continue_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_stmt_decl_indent(const an_ifc_stmt_decl universal, unsigned indent)
/*
Given the universal representation of StmtDecl, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_decl_an_ifc_stmt_decl(universal)) {
    an_ifc_decl_index field = get_ifc_decl_an_ifc_stmt_decl(universal);

    db_print_indent(indent);
    fprintf(f_debug, "decl:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_stmt_decl(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_stmt_decl(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_stmt_decl(const an_ifc_stmt_decl universal)
/*
Given the universal representation of StmtDecl, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "=================================== ");
  fprintf(f_debug, "StmtDecl ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_stmt_decl_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_stmt_default_indent(const an_ifc_stmt_default universal, unsigned indent)
/*
Given the universal representation of StmtDefault, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_stmt_default(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_stmt_default(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_stmt_default(const an_ifc_stmt_default universal)
/*
Given the universal representation of StmtDefault, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "StmtDefault ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_stmt_default_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_stmt_do_while_indent(const an_ifc_stmt_do_while universal, unsigned indent)
/*
Given the universal representation of StmtDoWhile, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_body_an_ifc_stmt_do_while(universal)) {
    an_ifc_stmt_index field = get_ifc_body_an_ifc_stmt_do_while(universal);

    db_print_indent(indent);
    fprintf(f_debug, "body:");
    if (is_null_index_an_ifc_stmt_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_stmt_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_condition_an_ifc_stmt_do_while(universal)) {
    an_ifc_stmt_index field = get_ifc_condition_an_ifc_stmt_do_while(universal);

    db_print_indent(indent);
    fprintf(f_debug, "condition:");
    if (is_null_index_an_ifc_stmt_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_stmt_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_stmt_do_while(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_stmt_do_while(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_stmt_do_while(const an_ifc_stmt_do_while universal)
/*
Given the universal representation of StmtDoWhile, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "StmtDoWhile ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_stmt_do_while_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_stmt_empty_indent(const an_ifc_stmt_empty universal, unsigned indent)
/*
Given the universal representation of StmtEmpty, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_stmt_empty(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_stmt_empty(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_stmt_empty(const an_ifc_stmt_empty universal)
/*
Given the universal representation of StmtEmpty, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "StmtEmpty ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_stmt_empty_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_stmt_expansion_indent(const an_ifc_stmt_expansion universal, unsigned indent)
/*
Given the universal representation of StmtExpansion, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_stmt_expansion(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_stmt_expansion(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_operand_an_ifc_stmt_expansion(universal)) {
    an_ifc_stmt_index field = get_ifc_operand_an_ifc_stmt_expansion(universal);

    db_print_indent(indent);
    fprintf(f_debug, "operand:");
    if (is_null_index_an_ifc_stmt_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_stmt_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_stmt_expansion(const an_ifc_stmt_expansion universal)
/*
Given the universal representation of StmtExpansion, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "StmtExpansion ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_stmt_expansion_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_stmt_expression_indent(const an_ifc_stmt_expression universal, unsigned indent)
/*
Given the universal representation of StmtExpression, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_expr_an_ifc_stmt_expression(universal)) {
    an_ifc_expr_index field = get_ifc_expr_an_ifc_stmt_expression(universal);

    db_print_indent(indent);
    fprintf(f_debug, "expr:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_stmt_expression(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_stmt_expression(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_stmt_expression(const an_ifc_stmt_expression universal)
/*
Given the universal representation of StmtExpression, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "StmtExpression ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_stmt_expression_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_stmt_for_indent(const an_ifc_stmt_for universal, unsigned indent)
/*
Given the universal representation of StmtFor, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_body_an_ifc_stmt_for(universal)) {
    an_ifc_stmt_index field = get_ifc_body_an_ifc_stmt_for(universal);

    db_print_indent(indent);
    fprintf(f_debug, "body:");
    if (is_null_index_an_ifc_stmt_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_stmt_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_condition_an_ifc_stmt_for(universal)) {
    an_ifc_stmt_index field = get_ifc_condition_an_ifc_stmt_for(universal);

    db_print_indent(indent);
    fprintf(f_debug, "condition:");
    if (is_null_index_an_ifc_stmt_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_stmt_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_continuation_an_ifc_stmt_for(universal)) {
    an_ifc_stmt_index field = get_ifc_continuation_an_ifc_stmt_for(universal);

    db_print_indent(indent);
    fprintf(f_debug, "continuation:");
    if (is_null_index_an_ifc_stmt_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_stmt_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_initialization_an_ifc_stmt_for(universal)) {
    an_ifc_stmt_index field = get_ifc_initialization_an_ifc_stmt_for(universal);

    db_print_indent(indent);
    fprintf(f_debug, "initialization:");
    if (is_null_index_an_ifc_stmt_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_stmt_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_stmt_for(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_stmt_for(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_stmt_for(const an_ifc_stmt_for universal)
/*
Given the universal representation of StmtFor, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "=================================== ");
  fprintf(f_debug, "StmtFor ");
  fprintf(f_debug, "====================================\n");
  db_node_an_ifc_stmt_for_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_stmt_goto_indent(const an_ifc_stmt_goto universal, unsigned indent)
/*
Given the universal representation of StmtGoto, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_stmt_goto(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_stmt_goto(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_target_an_ifc_stmt_goto(universal)) {
    an_ifc_expr_index field = get_ifc_target_an_ifc_stmt_goto(universal);

    db_print_indent(indent);
    fprintf(f_debug, "target:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_stmt_goto(const an_ifc_stmt_goto universal)
/*
Given the universal representation of StmtGoto, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "=================================== ");
  fprintf(f_debug, "StmtGoto ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_stmt_goto_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_stmt_handler_indent(const an_ifc_stmt_handler universal, unsigned indent)
/*
Given the universal representation of StmtHandler, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_body_an_ifc_stmt_handler(universal)) {
    an_ifc_stmt_index field = get_ifc_body_an_ifc_stmt_handler(universal);

    db_print_indent(indent);
    fprintf(f_debug, "body:");
    if (is_null_index_an_ifc_stmt_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_stmt_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_exception_an_ifc_stmt_handler(universal)) {
    an_ifc_decl_index field = get_ifc_exception_an_ifc_stmt_handler(universal);

    db_print_indent(indent);
    fprintf(f_debug, "exception:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_stmt_handler(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_stmt_handler(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_stmt_handler(const an_ifc_stmt_handler universal)
/*
Given the universal representation of StmtHandler, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "StmtHandler ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_stmt_handler_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_stmt_if_indent(const an_ifc_stmt_if universal, unsigned indent)
/*
Given the universal representation of StmtIf, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_alternative_an_ifc_stmt_if(universal)) {
    an_ifc_stmt_index field = get_ifc_alternative_an_ifc_stmt_if(universal);

    db_print_indent(indent);
    fprintf(f_debug, "alternative:");
    if (is_null_index_an_ifc_stmt_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_stmt_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_condition_an_ifc_stmt_if(universal)) {
    an_ifc_stmt_index field = get_ifc_condition_an_ifc_stmt_if(universal);

    db_print_indent(indent);
    fprintf(f_debug, "condition:");
    if (is_null_index_an_ifc_stmt_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_stmt_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_consequence_an_ifc_stmt_if(universal)) {
    an_ifc_stmt_index field = get_ifc_consequence_an_ifc_stmt_if(universal);

    db_print_indent(indent);
    fprintf(f_debug, "consequence:");
    if (is_null_index_an_ifc_stmt_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_stmt_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_initialization_an_ifc_stmt_if(universal)) {
    an_ifc_stmt_index field = get_ifc_initialization_an_ifc_stmt_if(universal);

    db_print_indent(indent);
    fprintf(f_debug, "initialization:");
    if (is_null_index_an_ifc_stmt_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_stmt_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_stmt_if(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_stmt_if(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_stmt_if(const an_ifc_stmt_if universal)
/*
Given the universal representation of StmtIf, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "==================================== ");
  fprintf(f_debug, "StmtIf ");
  fprintf(f_debug, "====================================\n");
  db_node_an_ifc_stmt_if_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_stmt_labeled_indent(const an_ifc_stmt_labeled universal, unsigned indent)
/*
Given the universal representation of StmtLabeled, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_label_an_ifc_stmt_labeled(universal)) {
    an_ifc_expr_index field = get_ifc_label_an_ifc_stmt_labeled(universal);

    db_print_indent(indent);
    fprintf(f_debug, "label:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_stmt_labeled(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_stmt_labeled(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_stmt_an_ifc_stmt_labeled(universal)) {
    an_ifc_stmt_index field = get_ifc_stmt_an_ifc_stmt_labeled(universal);

    db_print_indent(indent);
    fprintf(f_debug, "stmt:");
    if (is_null_index_an_ifc_stmt_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_stmt_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_type_an_ifc_stmt_labeled(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_stmt_labeled(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_stmt_labeled(const an_ifc_stmt_labeled universal)
/*
Given the universal representation of StmtLabeled, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "StmtLabeled ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_stmt_labeled_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_stmt_return_indent(const an_ifc_stmt_return universal, unsigned indent)
/*
Given the universal representation of StmtReturn, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_expr_an_ifc_stmt_return(universal)) {
    an_ifc_expr_index field = get_ifc_expr_an_ifc_stmt_return(universal);

    db_print_indent(indent);
    fprintf(f_debug, "expr:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_function_type_an_ifc_stmt_return(universal)) {
    an_ifc_type_index field = get_ifc_function_type_an_ifc_stmt_return(universal);

    db_print_indent(indent);
    fprintf(f_debug, "function_type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_stmt_return(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_stmt_return(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_type_an_ifc_stmt_return(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_stmt_return(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_stmt_return(const an_ifc_stmt_return universal)
/*
Given the universal representation of StmtReturn, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "StmtReturn ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_stmt_return_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_stmt_switch_indent(const an_ifc_stmt_switch universal, unsigned indent)
/*
Given the universal representation of StmtSwitch, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_body_an_ifc_stmt_switch(universal)) {
    an_ifc_stmt_index field = get_ifc_body_an_ifc_stmt_switch(universal);

    db_print_indent(indent);
    fprintf(f_debug, "body:");
    if (is_null_index_an_ifc_stmt_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_stmt_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_condition_an_ifc_stmt_switch(universal)) {
    an_ifc_expr_index field = get_ifc_condition_an_ifc_stmt_switch(universal);

    db_print_indent(indent);
    fprintf(f_debug, "condition:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_initialization_an_ifc_stmt_switch(universal)) {
    an_ifc_stmt_index field = get_ifc_initialization_an_ifc_stmt_switch(universal);

    db_print_indent(indent);
    fprintf(f_debug, "initialization:");
    if (is_null_index_an_ifc_stmt_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_stmt_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_stmt_switch(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_stmt_switch(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_stmt_switch(const an_ifc_stmt_switch universal)
/*
Given the universal representation of StmtSwitch, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "StmtSwitch ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_stmt_switch_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_stmt_try_indent(const an_ifc_stmt_try universal, unsigned indent)
/*
Given the universal representation of StmtTry, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_cardinality_an_ifc_stmt_try(universal)) {
    an_ifc_cardinality field = get_ifc_cardinality_an_ifc_stmt_try(universal);

    db_print_indent(indent);
    fprintf(f_debug, "cardinality: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_handlers_an_ifc_stmt_try(universal)) {
    an_ifc_stmt_index field = get_ifc_handlers_an_ifc_stmt_try(universal);

    db_print_indent(indent);
    fprintf(f_debug, "handlers:");
    if (is_null_index_an_ifc_stmt_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_stmt_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_stmt_try(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_stmt_try(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_start_an_ifc_stmt_try(universal)) {
    an_ifc_index field = get_ifc_start_an_ifc_stmt_try(universal);

    db_print_indent(indent);
    fprintf(f_debug, "start: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_stmt_try(const an_ifc_stmt_try universal)
/*
Given the universal representation of StmtTry, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "=================================== ");
  fprintf(f_debug, "StmtTry ");
  fprintf(f_debug, "====================================\n");
  db_node_an_ifc_stmt_try_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_stmt_tuple_indent(const an_ifc_stmt_tuple universal, unsigned indent)
/*
Given the universal representation of StmtTuple, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_cardinality_an_ifc_stmt_tuple(universal)) {
    an_ifc_cardinality field = get_ifc_cardinality_an_ifc_stmt_tuple(universal);

    db_print_indent(indent);
    fprintf(f_debug, "cardinality: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_locus_an_ifc_stmt_tuple(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_stmt_tuple(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_start_an_ifc_stmt_tuple(universal)) {
    an_ifc_index field = get_ifc_start_an_ifc_stmt_tuple(universal);

    db_print_indent(indent);
    fprintf(f_debug, "start: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_stmt_tuple(const an_ifc_stmt_tuple universal)
/*
Given the universal representation of StmtTuple, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "StmtTuple ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_stmt_tuple_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_stmt_variable_decl_indent(const an_ifc_stmt_variable_decl universal, unsigned indent)
/*
Given the universal representation of StmtVariableDecl, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_decl_an_ifc_stmt_variable_decl(universal)) {
    an_ifc_decl_index field = get_ifc_decl_an_ifc_stmt_variable_decl(universal);

    db_print_indent(indent);
    fprintf(f_debug, "decl:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_stmt_variable_decl(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_stmt_variable_decl(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_stmt_variable_decl(const an_ifc_stmt_variable_decl universal)
/*
Given the universal representation of StmtVariableDecl, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "=============================== ");
  fprintf(f_debug, "StmtVariableDecl ");
  fprintf(f_debug, "===============================\n");
  db_node_an_ifc_stmt_variable_decl_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_stmt_while_indent(const an_ifc_stmt_while universal, unsigned indent)
/*
Given the universal representation of StmtWhile, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_body_an_ifc_stmt_while(universal)) {
    an_ifc_stmt_index field = get_ifc_body_an_ifc_stmt_while(universal);

    db_print_indent(indent);
    fprintf(f_debug, "body:");
    if (is_null_index_an_ifc_stmt_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_stmt_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_condition_an_ifc_stmt_while(universal)) {
    an_ifc_stmt_index field = get_ifc_condition_an_ifc_stmt_while(universal);

    db_print_indent(indent);
    fprintf(f_debug, "condition:");
    if (is_null_index_an_ifc_stmt_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_stmt_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_stmt_while(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_stmt_while(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_stmt_while(const an_ifc_stmt_while universal)
/*
Given the universal representation of StmtWhile, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "StmtWhile ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_stmt_while_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_access_specifier_indent(const an_ifc_syntax_access_specifier universal, unsigned indent)
/*
Given the universal representation of SyntaxAccessSpecifier, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_access_an_ifc_syntax_access_specifier(universal)) {
    an_ifc_keyword_syntax field = get_ifc_access_an_ifc_syntax_access_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "access:\n");
    db_node_an_ifc_keyword_syntax_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_comma_an_ifc_syntax_access_specifier(universal)) {
    an_ifc_source_location field = get_ifc_comma_an_ifc_syntax_access_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "comma:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_designator_an_ifc_syntax_access_specifier(universal)) {
    an_ifc_expr_index field = get_ifc_designator_an_ifc_syntax_access_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "designator:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_access_specifier(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_access_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_virtual_kw_an_ifc_syntax_access_specifier(universal)) {
    an_ifc_source_location field = get_ifc_virtual_kw_an_ifc_syntax_access_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "virtual_kw:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_virtual_kw2_an_ifc_syntax_access_specifier(universal)) {
    an_ifc_source_location field = get_ifc_virtual_kw2_an_ifc_syntax_access_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "virtual_kw2:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_access_specifier(const an_ifc_syntax_access_specifier universal)
/*
Given the universal representation of SyntaxAccessSpecifier, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================ ");
  fprintf(f_debug, "SyntaxAccessSpecifier ");
  fprintf(f_debug, "=============================\n");
  db_node_an_ifc_syntax_access_specifier_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_alias_declaration_indent(const an_ifc_syntax_alias_declaration universal, unsigned indent)
/*
Given the universal representation of SyntaxAliasDeclaration, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_aliasee_an_ifc_syntax_alias_declaration(universal)) {
    an_ifc_syntax_index field = get_ifc_aliasee_an_ifc_syntax_alias_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "aliasee:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_equal_an_ifc_syntax_alias_declaration(universal)) {
    an_ifc_source_location field = get_ifc_equal_an_ifc_syntax_alias_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "equal:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_alias_declaration(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_alias_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_syntax_alias_declaration(universal)) {
    an_ifc_expr_index field = get_ifc_name_an_ifc_syntax_alias_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_semicolon_an_ifc_syntax_alias_declaration(universal)) {
    an_ifc_source_location field = get_ifc_semicolon_an_ifc_syntax_alias_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "semicolon:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_alias_declaration(const an_ifc_syntax_alias_declaration universal)
/*
Given the universal representation of SyntaxAliasDeclaration, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "============================ ");
  fprintf(f_debug, "SyntaxAliasDeclaration ");
  fprintf(f_debug, "============================\n");
  db_node_an_ifc_syntax_alias_declaration_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_alignas_indent(const an_ifc_syntax_alignas universal, unsigned indent)
/*
Given the universal representation of SyntaxAlignas, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_left_paren_an_ifc_syntax_alignas(universal)) {
    an_ifc_source_location field = get_ifc_left_paren_an_ifc_syntax_alignas(universal);

    db_print_indent(indent);
    fprintf(f_debug, "left_paren:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_alignas(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_alignas(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_operand_an_ifc_syntax_alignas(universal)) {
    an_ifc_syntax_index field = get_ifc_operand_an_ifc_syntax_alignas(universal);

    db_print_indent(indent);
    fprintf(f_debug, "operand:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_right_paren_an_ifc_syntax_alignas(universal)) {
    an_ifc_source_location field = get_ifc_right_paren_an_ifc_syntax_alignas(universal);

    db_print_indent(indent);
    fprintf(f_debug, "right_paren:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_alignas(const an_ifc_syntax_alignas universal)
/*
Given the universal representation of SyntaxAlignas, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "SyntaxAlignas ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_syntax_alignas_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_array_declarator_indent(const an_ifc_syntax_array_declarator universal, unsigned indent)
/*
Given the universal representation of SyntaxArrayDeclarator, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_bound_an_ifc_syntax_array_declarator(universal)) {
    an_ifc_expr_index field = get_ifc_bound_an_ifc_syntax_array_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "bound:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_left_bracket_an_ifc_syntax_array_declarator(universal)) {
    an_ifc_source_location field = get_ifc_left_bracket_an_ifc_syntax_array_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "left_bracket:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_right_bracket_an_ifc_syntax_array_declarator(universal)) {
    an_ifc_source_location field = get_ifc_right_bracket_an_ifc_syntax_array_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "right_bracket:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_array_declarator(const an_ifc_syntax_array_declarator universal)
/*
Given the universal representation of SyntaxArrayDeclarator, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================ ");
  fprintf(f_debug, "SyntaxArrayDeclarator ");
  fprintf(f_debug, "=============================\n");
  db_node_an_ifc_syntax_array_declarator_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_array_index_indent(const an_ifc_syntax_array_index universal, unsigned indent)
/*
Given the universal representation of SyntaxArrayIndex, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_array_an_ifc_syntax_array_index(universal)) {
    an_ifc_expr_index field = get_ifc_array_an_ifc_syntax_array_index(universal);

    db_print_indent(indent);
    fprintf(f_debug, "array:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_index_an_ifc_syntax_array_index(universal)) {
    an_ifc_expr_index field = get_ifc_index_an_ifc_syntax_array_index(universal);

    db_print_indent(indent);
    fprintf(f_debug, "index:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_left_bracket_an_ifc_syntax_array_index(universal)) {
    an_ifc_source_location field = get_ifc_left_bracket_an_ifc_syntax_array_index(universal);

    db_print_indent(indent);
    fprintf(f_debug, "left_bracket:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_right_bracket_an_ifc_syntax_array_index(universal)) {
    an_ifc_source_location field = get_ifc_right_bracket_an_ifc_syntax_array_index(universal);

    db_print_indent(indent);
    fprintf(f_debug, "right_bracket:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_array_index(const an_ifc_syntax_array_index universal)
/*
Given the universal representation of SyntaxArrayIndex, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "=============================== ");
  fprintf(f_debug, "SyntaxArrayIndex ");
  fprintf(f_debug, "===============================\n");
  db_node_an_ifc_syntax_array_index_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_array_or_function_declarator_indent(const an_ifc_syntax_array_or_function_declarator universal,
             unsigned                                         indent)
/*
Given the universal representation of SyntaxArrayOrFunctionDeclarator, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_declarator_an_ifc_syntax_array_or_function_declarator(universal)) {
    an_ifc_syntax_index field = get_ifc_declarator_an_ifc_syntax_array_or_function_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "declarator:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_next_an_ifc_syntax_array_or_function_declarator(universal)) {
    an_ifc_syntax_index field = get_ifc_next_an_ifc_syntax_array_or_function_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "next:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_array_or_function_declarator(const an_ifc_syntax_array_or_function_declarator universal)
/*
Given the universal representation of SyntaxArrayOrFunctionDeclarator, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "======================= ");
  fprintf(f_debug, "SyntaxArrayOrFunctionDeclarator ");
  fprintf(f_debug, "========================\n");
  db_node_an_ifc_syntax_array_or_function_declarator_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_asm_statement_indent(const an_ifc_syntax_asm_statement universal, unsigned indent)
/*
Given the universal representation of SyntaxAsmStatement, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_syntax_asm_statement(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_asm_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_tokens_an_ifc_syntax_asm_statement(universal)) {
    an_ifc_sentence_index field = get_ifc_tokens_an_ifc_syntax_asm_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "tokens: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_asm_statement(const an_ifc_syntax_asm_statement universal)
/*
Given the universal representation of SyntaxAsmStatement, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================== ");
  fprintf(f_debug, "SyntaxAsmStatement ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_syntax_asm_statement_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_attribute_indent(const an_ifc_syntax_attribute universal, unsigned indent)
/*
Given the universal representation of SyntaxAttribute, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_argument_clause_an_ifc_syntax_attribute(universal)) {
    an_ifc_syntax_index field = get_ifc_argument_clause_an_ifc_syntax_attribute(universal);

    db_print_indent(indent);
    fprintf(f_debug, "argument_clause:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_colons_an_ifc_syntax_attribute(universal)) {
    an_ifc_source_location field = get_ifc_colons_an_ifc_syntax_attribute(universal);

    db_print_indent(indent);
    fprintf(f_debug, "colons:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_comma_an_ifc_syntax_attribute(universal)) {
    an_ifc_source_location field = get_ifc_comma_an_ifc_syntax_attribute(universal);

    db_print_indent(indent);
    fprintf(f_debug, "comma:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_expander_an_ifc_syntax_attribute(universal)) {
    an_ifc_source_location field = get_ifc_expander_an_ifc_syntax_attribute(universal);

    db_print_indent(indent);
    fprintf(f_debug, "expander:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_syntax_attribute(universal)) {
    an_ifc_expr_index field = get_ifc_name_an_ifc_syntax_attribute(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_scope_an_ifc_syntax_attribute(universal)) {
    an_ifc_expr_index field = get_ifc_scope_an_ifc_syntax_attribute(universal);

    db_print_indent(indent);
    fprintf(f_debug, "scope:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_attribute(const an_ifc_syntax_attribute universal)
/*
Given the universal representation of SyntaxAttribute, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "=============================== ");
  fprintf(f_debug, "SyntaxAttribute ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_syntax_attribute_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_attribute_argument_clause_indent(const an_ifc_syntax_attribute_argument_clause universal,
             unsigned                                      indent)
/*
Given the universal representation of SyntaxAttributeArgumentClause, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_left_paren_an_ifc_syntax_attribute_argument_clause(universal)) {
    an_ifc_source_location field = get_ifc_left_paren_an_ifc_syntax_attribute_argument_clause(universal);

    db_print_indent(indent);
    fprintf(f_debug, "left_paren:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_right_paren_an_ifc_syntax_attribute_argument_clause(universal)) {
    an_ifc_source_location field = get_ifc_right_paren_an_ifc_syntax_attribute_argument_clause(universal);

    db_print_indent(indent);
    fprintf(f_debug, "right_paren:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_tokens_an_ifc_syntax_attribute_argument_clause(universal)) {
    an_ifc_sentence_index field = get_ifc_tokens_an_ifc_syntax_attribute_argument_clause(universal);

    db_print_indent(indent);
    fprintf(f_debug, "tokens: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_attribute_argument_clause(const an_ifc_syntax_attribute_argument_clause universal)
/*
Given the universal representation of SyntaxAttributeArgumentClause, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "======================== ");
  fprintf(f_debug, "SyntaxAttributeArgumentClause ");
  fprintf(f_debug, "=========================\n");
  db_node_an_ifc_syntax_attribute_argument_clause_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_attribute_specifier_indent(const an_ifc_syntax_attribute_specifier universal,
             unsigned                                indent)
/*
Given the universal representation of SyntaxAttributeSpecifier, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_attributes_an_ifc_syntax_attribute_specifier(universal)) {
    an_ifc_syntax_index field = get_ifc_attributes_an_ifc_syntax_attribute_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "attributes:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_left_paren_1_an_ifc_syntax_attribute_specifier(universal)) {
    an_ifc_source_location field = get_ifc_left_paren_1_an_ifc_syntax_attribute_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "left_paren_1:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_left_paren_2_an_ifc_syntax_attribute_specifier(universal)) {
    an_ifc_source_location field = get_ifc_left_paren_2_an_ifc_syntax_attribute_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "left_paren_2:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_prefix_an_ifc_syntax_attribute_specifier(universal)) {
    an_ifc_syntax_index field = get_ifc_prefix_an_ifc_syntax_attribute_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "prefix:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_right_paren_1_an_ifc_syntax_attribute_specifier(universal)) {
    an_ifc_source_location field = get_ifc_right_paren_1_an_ifc_syntax_attribute_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "right_paren_1:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_right_paren_2_an_ifc_syntax_attribute_specifier(universal)) {
    an_ifc_source_location field = get_ifc_right_paren_2_an_ifc_syntax_attribute_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "right_paren_2:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_attribute_specifier(const an_ifc_syntax_attribute_specifier universal)
/*
Given the universal representation of SyntaxAttributeSpecifier, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "=========================== ");
  fprintf(f_debug, "SyntaxAttributeSpecifier ");
  fprintf(f_debug, "===========================\n");
  db_node_an_ifc_syntax_attribute_specifier_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_attribute_specifier_seq_indent(const an_ifc_syntax_attribute_specifier_seq universal,
             unsigned                                    indent)
/*
Given the universal representation of SyntaxAttributeSpecifierSeq, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_attributes_an_ifc_syntax_attribute_specifier_seq(universal)) {
    an_ifc_syntax_index field = get_ifc_attributes_an_ifc_syntax_attribute_specifier_seq(universal);

    db_print_indent(indent);
    fprintf(f_debug, "attributes:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_attribute_specifier_seq(const an_ifc_syntax_attribute_specifier_seq universal)
/*
Given the universal representation of SyntaxAttributeSpecifierSeq, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================= ");
  fprintf(f_debug, "SyntaxAttributeSpecifierSeq ");
  fprintf(f_debug, "==========================\n");
  db_node_an_ifc_syntax_attribute_specifier_seq_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_attribute_using_prefix_indent(const an_ifc_syntax_attribute_using_prefix universal,
             unsigned                                   indent)
/*
Given the universal representation of SyntaxAttributeUsingPrefix, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_syntax_attribute_using_prefix(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_attribute_using_prefix(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_scope_an_ifc_syntax_attribute_using_prefix(universal)) {
    an_ifc_source_location field = get_ifc_scope_an_ifc_syntax_attribute_using_prefix(universal);

    db_print_indent(indent);
    fprintf(f_debug, "scope:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_attribute_using_prefix(const an_ifc_syntax_attribute_using_prefix universal)
/*
Given the universal representation of SyntaxAttributeUsingPrefix, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================== ");
  fprintf(f_debug, "SyntaxAttributeUsingPrefix ");
  fprintf(f_debug, "==========================\n");
  db_node_an_ifc_syntax_attribute_using_prefix_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_attributed_declaration_indent(const an_ifc_syntax_attributed_declaration universal,
             unsigned                                   indent)
/*
Given the universal representation of SyntaxAttributedDeclaration, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_attributes_an_ifc_syntax_attributed_declaration(universal)) {
    an_ifc_syntax_index field = get_ifc_attributes_an_ifc_syntax_attributed_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "attributes:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_decl_an_ifc_syntax_attributed_declaration(universal)) {
    an_ifc_syntax_index field = get_ifc_decl_an_ifc_syntax_attributed_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "decl:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_attributed_declaration(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_attributed_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_attributed_declaration(const an_ifc_syntax_attributed_declaration universal)
/*
Given the universal representation of SyntaxAttributedDeclaration, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================= ");
  fprintf(f_debug, "SyntaxAttributedDeclaration ");
  fprintf(f_debug, "==========================\n");
  db_node_an_ifc_syntax_attributed_declaration_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_attributed_statement_indent(const an_ifc_syntax_attributed_statement universal,
             unsigned                                 indent)
/*
Given the universal representation of SyntaxAttributedStatement, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_attributes_an_ifc_syntax_attributed_statement(universal)) {
    an_ifc_syntax_index field = get_ifc_attributes_an_ifc_syntax_attributed_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "attributes:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_pragma_an_ifc_syntax_attributed_statement(universal)) {
    an_ifc_sentence_index field = get_ifc_pragma_an_ifc_syntax_attributed_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "pragma: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_stmt_an_ifc_syntax_attributed_statement(universal)) {
    an_ifc_syntax_index field = get_ifc_stmt_an_ifc_syntax_attributed_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "stmt:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_attributed_statement(const an_ifc_syntax_attributed_statement universal)
/*
Given the universal representation of SyntaxAttributedStatement, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================== ");
  fprintf(f_debug, "SyntaxAttributedStatement ");
  fprintf(f_debug, "===========================\n");
  db_node_an_ifc_syntax_attributed_statement_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_base_specifier_indent(const an_ifc_syntax_base_specifier universal, unsigned indent)
/*
Given the universal representation of SyntaxBaseSpecifier, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_access_an_ifc_syntax_base_specifier(universal)) {
    an_ifc_keyword_syntax field = get_ifc_access_an_ifc_syntax_base_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "access:\n");
    db_node_an_ifc_keyword_syntax_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_colon_an_ifc_syntax_base_specifier(universal)) {
    an_ifc_source_location field = get_ifc_colon_an_ifc_syntax_base_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "colon:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_base_specifier(const an_ifc_syntax_base_specifier universal)
/*
Given the universal representation of SyntaxBaseSpecifier, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================= ");
  fprintf(f_debug, "SyntaxBaseSpecifier ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_syntax_base_specifier_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_base_specifier_list_indent(const an_ifc_syntax_base_specifier_list universal,
             unsigned                                indent)
/*
Given the universal representation of SyntaxBaseSpecifierList, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_base_specifiers_an_ifc_syntax_base_specifier_list(universal)) {
    an_ifc_syntax_index field = get_ifc_base_specifiers_an_ifc_syntax_base_specifier_list(universal);

    db_print_indent(indent);
    fprintf(f_debug, "base_specifiers:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_colon_an_ifc_syntax_base_specifier_list(universal)) {
    an_ifc_source_location field = get_ifc_colon_an_ifc_syntax_base_specifier_list(universal);

    db_print_indent(indent);
    fprintf(f_debug, "colon:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_base_specifier_list(const an_ifc_syntax_base_specifier_list universal)
/*
Given the universal representation of SyntaxBaseSpecifierList, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "=========================== ");
  fprintf(f_debug, "SyntaxBaseSpecifierList ");
  fprintf(f_debug, "============================\n");
  db_node_an_ifc_syntax_base_specifier_list_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_binary_fold_expression_indent(const an_ifc_syntax_binary_fold_expression universal,
             unsigned                                   indent)
/*
Given the universal representation of SyntaxBinaryFoldExpression, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_direction_an_ifc_syntax_binary_fold_expression(universal)) {
    an_ifc_fold_direction_sort field = get_ifc_direction_an_ifc_syntax_binary_fold_expression(universal);

    db_print_indent(indent);
    fprintf(f_debug, "direction: %s\n", str_for_an_ifc_fold_direction_sort(field));
  }  /* if */
  if (has_ifc_dyad_an_ifc_syntax_binary_fold_expression(universal)) {
    an_ifc_dyadic_operator_sort field = get_ifc_dyad_an_ifc_syntax_binary_fold_expression(universal);

    db_print_indent(indent);
    fprintf(f_debug, "dyad: %s\n", str_for_an_ifc_dyadic_operator_sort(field));
  }  /* if */
  if (has_ifc_ellipsis_an_ifc_syntax_binary_fold_expression(universal)) {
    an_ifc_source_location field = get_ifc_ellipsis_an_ifc_syntax_binary_fold_expression(universal);

    db_print_indent(indent);
    fprintf(f_debug, "ellipsis:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_glyph_loci_1_an_ifc_syntax_binary_fold_expression(universal)) {
    an_ifc_source_location field = get_ifc_glyph_loci_1_an_ifc_syntax_binary_fold_expression(universal);

    db_print_indent(indent);
    fprintf(f_debug, "glyph_loci_1:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_glyph_loci_2_an_ifc_syntax_binary_fold_expression(universal)) {
    an_ifc_source_location field = get_ifc_glyph_loci_2_an_ifc_syntax_binary_fold_expression(universal);

    db_print_indent(indent);
    fprintf(f_debug, "glyph_loci_2:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_binary_fold_expression(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_binary_fold_expression(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_operand_1_an_ifc_syntax_binary_fold_expression(universal)) {
    an_ifc_expr_index field = get_ifc_operand_1_an_ifc_syntax_binary_fold_expression(universal);

    db_print_indent(indent);
    fprintf(f_debug, "operand_1:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_operand_2_an_ifc_syntax_binary_fold_expression(universal)) {
    an_ifc_expr_index field = get_ifc_operand_2_an_ifc_syntax_binary_fold_expression(universal);

    db_print_indent(indent);
    fprintf(f_debug, "operand_2:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_right_paren_an_ifc_syntax_binary_fold_expression(universal)) {
    an_ifc_source_location field = get_ifc_right_paren_an_ifc_syntax_binary_fold_expression(universal);

    db_print_indent(indent);
    fprintf(f_debug, "right_paren:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_binary_fold_expression(const an_ifc_syntax_binary_fold_expression universal)
/*
Given the universal representation of SyntaxBinaryFoldExpression, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================== ");
  fprintf(f_debug, "SyntaxBinaryFoldExpression ");
  fprintf(f_debug, "==========================\n");
  db_node_an_ifc_syntax_binary_fold_expression_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_break_statement_indent(const an_ifc_syntax_break_statement universal, unsigned indent)
/*
Given the universal representation of SyntaxBreakStatement, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_break_an_ifc_syntax_break_statement(universal)) {
    an_ifc_source_location field = get_ifc_break_an_ifc_syntax_break_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "break:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_semicolon_an_ifc_syntax_break_statement(universal)) {
    an_ifc_source_location field = get_ifc_semicolon_an_ifc_syntax_break_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "semicolon:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_break_statement(const an_ifc_syntax_break_statement universal)
/*
Given the universal representation of SyntaxBreakStatement, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================= ");
  fprintf(f_debug, "SyntaxBreakStatement ");
  fprintf(f_debug, "=============================\n");
  db_node_an_ifc_syntax_break_statement_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_capture_default_indent(const an_ifc_syntax_capture_default universal, unsigned indent)
/*
Given the universal representation of SyntaxCaptureDefault, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_by_ref_an_ifc_syntax_capture_default(universal)) {
    an_ifc_bool field = get_ifc_by_ref_an_ifc_syntax_capture_default(universal);

    db_print_indent(indent);
    fprintf(f_debug, "by_ref: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_comma_an_ifc_syntax_capture_default(universal)) {
    an_ifc_source_location field = get_ifc_comma_an_ifc_syntax_capture_default(universal);

    db_print_indent(indent);
    fprintf(f_debug, "comma:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_capture_default(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_capture_default(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_capture_default(const an_ifc_syntax_capture_default universal)
/*
Given the universal representation of SyntaxCaptureDefault, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================= ");
  fprintf(f_debug, "SyntaxCaptureDefault ");
  fprintf(f_debug, "=============================\n");
  db_node_an_ifc_syntax_capture_default_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_class_specifier_indent(const an_ifc_syntax_class_specifier universal, unsigned indent)
/*
Given the universal representation of SyntaxClassSpecifier, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_bases_an_ifc_syntax_class_specifier(universal)) {
    an_ifc_syntax_index field = get_ifc_bases_an_ifc_syntax_class_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "bases:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_class_key_an_ifc_syntax_class_specifier(universal)) {
    an_ifc_keyword_syntax field = get_ifc_class_key_an_ifc_syntax_class_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "class_key:\n");
    db_node_an_ifc_keyword_syntax_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_left_paren_an_ifc_syntax_class_specifier(universal)) {
    an_ifc_syntax_index field = get_ifc_left_paren_an_ifc_syntax_class_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "left_paren:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_members_an_ifc_syntax_class_specifier(universal)) {
    an_ifc_syntax_index field = get_ifc_members_an_ifc_syntax_class_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "members:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_name_an_ifc_syntax_class_specifier(universal)) {
    an_ifc_expr_index field = get_ifc_name_an_ifc_syntax_class_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_right_paren_an_ifc_syntax_class_specifier(universal)) {
    an_ifc_syntax_index field = get_ifc_right_paren_an_ifc_syntax_class_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "right_paren:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_class_specifier(const an_ifc_syntax_class_specifier universal)
/*
Given the universal representation of SyntaxClassSpecifier, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================= ");
  fprintf(f_debug, "SyntaxClassSpecifier ");
  fprintf(f_debug, "=============================\n");
  db_node_an_ifc_syntax_class_specifier_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_compound_requirement_indent(const an_ifc_syntax_compound_requirement universal,
             unsigned                                 indent)
/*
Given the universal representation of SyntaxCompoundRequirement, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_condition_an_ifc_syntax_compound_requirement(universal)) {
    an_ifc_expr_index field = get_ifc_condition_an_ifc_syntax_compound_requirement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "condition:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_constraint_an_ifc_syntax_compound_requirement(universal)) {
    an_ifc_expr_index field = get_ifc_constraint_an_ifc_syntax_compound_requirement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "constraint:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_compound_requirement(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_compound_requirement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_noexcept_loc_an_ifc_syntax_compound_requirement(universal)) {
    an_ifc_source_location field = get_ifc_noexcept_loc_an_ifc_syntax_compound_requirement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "noexcept_loc:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_right_curly_an_ifc_syntax_compound_requirement(universal)) {
    an_ifc_source_location field = get_ifc_right_curly_an_ifc_syntax_compound_requirement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "right_curly:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_compound_requirement(const an_ifc_syntax_compound_requirement universal)
/*
Given the universal representation of SyntaxCompoundRequirement, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================== ");
  fprintf(f_debug, "SyntaxCompoundRequirement ");
  fprintf(f_debug, "===========================\n");
  db_node_an_ifc_syntax_compound_requirement_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_compound_statement_indent(const an_ifc_syntax_compound_statement universal,
             unsigned                               indent)
/*
Given the universal representation of SyntaxCompoundStatement, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_left_curly_an_ifc_syntax_compound_statement(universal)) {
    an_ifc_source_location field = get_ifc_left_curly_an_ifc_syntax_compound_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "left_curly:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_pragam_an_ifc_syntax_compound_statement(universal)) {
    an_ifc_sentence_index field = get_ifc_pragam_an_ifc_syntax_compound_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "pragam: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_right_curly_an_ifc_syntax_compound_statement(universal)) {
    an_ifc_source_location field = get_ifc_right_curly_an_ifc_syntax_compound_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "right_curly:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_stmts_an_ifc_syntax_compound_statement(universal)) {
    an_ifc_syntax_index field = get_ifc_stmts_an_ifc_syntax_compound_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "stmts:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_compound_statement(const an_ifc_syntax_compound_statement universal)
/*
Given the universal representation of SyntaxCompoundStatement, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "=========================== ");
  fprintf(f_debug, "SyntaxCompoundStatement ");
  fprintf(f_debug, "============================\n");
  db_node_an_ifc_syntax_compound_statement_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_concept_definition_indent(const an_ifc_syntax_concept_definition universal,
             unsigned                               indent)
/*
Given the universal representation of SyntaxConceptDefinition, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_concept_keyword_an_ifc_syntax_concept_definition(universal)) {
    an_ifc_source_location field = get_ifc_concept_keyword_an_ifc_syntax_concept_definition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "concept_keyword:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_equal_an_ifc_syntax_concept_definition(universal)) {
    an_ifc_source_location field = get_ifc_equal_an_ifc_syntax_concept_definition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "equal:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_initializer_an_ifc_syntax_concept_definition(universal)) {
    an_ifc_expr_index field = get_ifc_initializer_an_ifc_syntax_concept_definition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "initializer:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_concept_definition(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_concept_definition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_syntax_concept_definition(universal)) {
    an_ifc_text_offset field = get_ifc_name_an_ifc_syntax_concept_definition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_parameters_an_ifc_syntax_concept_definition(universal)) {
    an_ifc_syntax_index field = get_ifc_parameters_an_ifc_syntax_concept_definition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "parameters:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_semicolon_an_ifc_syntax_concept_definition(universal)) {
    an_ifc_source_location field = get_ifc_semicolon_an_ifc_syntax_concept_definition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "semicolon:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_concept_definition(const an_ifc_syntax_concept_definition universal)
/*
Given the universal representation of SyntaxConceptDefinition, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "=========================== ");
  fprintf(f_debug, "SyntaxConceptDefinition ");
  fprintf(f_debug, "============================\n");
  db_node_an_ifc_syntax_concept_definition_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_condition_declaration_indent(const an_ifc_syntax_condition_declaration universal,
             unsigned                                  indent)
/*
Given the universal representation of SyntaxConditionDeclaration, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_decl_specifier_an_ifc_syntax_condition_declaration(universal)) {
    an_ifc_syntax_index field = get_ifc_decl_specifier_an_ifc_syntax_condition_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "decl_specifier:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_initializaerion_an_ifc_syntax_condition_declaration(universal)) {
    an_ifc_syntax_index field = get_ifc_initializaerion_an_ifc_syntax_condition_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "initializaerion:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_condition_declaration(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_condition_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_condition_declaration(const an_ifc_syntax_condition_declaration universal)
/*
Given the universal representation of SyntaxConditionDeclaration, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================== ");
  fprintf(f_debug, "SyntaxConditionDeclaration ");
  fprintf(f_debug, "==========================\n");
  db_node_an_ifc_syntax_condition_declaration_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_continue_statement_indent(const an_ifc_syntax_continue_statement universal,
             unsigned                               indent)
/*
Given the universal representation of SyntaxContinueStatement, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_continue_an_ifc_syntax_continue_statement(universal)) {
    an_ifc_source_location field = get_ifc_continue_an_ifc_syntax_continue_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "continue:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_semicolon_an_ifc_syntax_continue_statement(universal)) {
    an_ifc_source_location field = get_ifc_semicolon_an_ifc_syntax_continue_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "semicolon:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_continue_statement(const an_ifc_syntax_continue_statement universal)
/*
Given the universal representation of SyntaxContinueStatement, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "=========================== ");
  fprintf(f_debug, "SyntaxContinueStatement ");
  fprintf(f_debug, "============================\n");
  db_node_an_ifc_syntax_continue_statement_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_ctor_initializer_indent(const an_ifc_syntax_ctor_initializer universal, unsigned indent)
/*
Given the universal representation of SyntaxCtorInitializer, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_colon_an_ifc_syntax_ctor_initializer(universal)) {
    an_ifc_source_location field = get_ifc_colon_an_ifc_syntax_ctor_initializer(universal);

    db_print_indent(indent);
    fprintf(f_debug, "colon:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_initializers_an_ifc_syntax_ctor_initializer(universal)) {
    an_ifc_syntax_index field = get_ifc_initializers_an_ifc_syntax_ctor_initializer(universal);

    db_print_indent(indent);
    fprintf(f_debug, "initializers:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_ctor_initializer(const an_ifc_syntax_ctor_initializer universal)
/*
Given the universal representation of SyntaxCtorInitializer, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================ ");
  fprintf(f_debug, "SyntaxCtorInitializer ");
  fprintf(f_debug, "=============================\n");
  db_node_an_ifc_syntax_ctor_initializer_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_decl_specifier_seq_indent(const an_ifc_syntax_decl_specifier_seq universal,
             unsigned                               indent)
/*
Given the universal representation of SyntaxDeclSpecifierSeq, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_declspec_an_ifc_syntax_decl_specifier_seq(universal)) {
    an_ifc_sentence_index field = get_ifc_declspec_an_ifc_syntax_decl_specifier_seq(universal);

    db_print_indent(indent);
    fprintf(f_debug, "declspec: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_explicit_kw_an_ifc_syntax_decl_specifier_seq(universal)) {
    an_ifc_syntax_index field = get_ifc_explicit_kw_an_ifc_syntax_decl_specifier_seq(universal);

    db_print_indent(indent);
    fprintf(f_debug, "explicit_kw:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_decl_specifier_seq(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_decl_specifier_seq(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_qualifiers_an_ifc_syntax_decl_specifier_seq(universal)) {
    an_ifc_qualifier_bitfield field = get_ifc_qualifiers_an_ifc_syntax_decl_specifier_seq(universal);

    fprintf(f_debug, "qualifiers:\n");
    ++indent;
    if (test_bitmask_ifc_qb_const(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Const\n");
    }  /* if */
    if (test_bitmask_ifc_qb_none(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- None\n");
    }  /* if */
    if (test_bitmask_ifc_qb_restrict(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Restrict\n");
    }  /* if */
    if (test_bitmask_ifc_qb_volatile(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Volatile\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_storage_class_an_ifc_syntax_decl_specifier_seq(universal)) {
    db_print_indent(indent);
    fprintf(f_debug, "storage_class: UNIMPLEMENTED\n");
  }  /* if */
  if (has_ifc_type_an_ifc_syntax_decl_specifier_seq(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_syntax_decl_specifier_seq(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_type_name_an_ifc_syntax_decl_specifier_seq(universal)) {
    an_ifc_syntax_index field = get_ifc_type_name_an_ifc_syntax_decl_specifier_seq(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type_name:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_decl_specifier_seq(const an_ifc_syntax_decl_specifier_seq universal)
/*
Given the universal representation of SyntaxDeclSpecifierSeq, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "============================ ");
  fprintf(f_debug, "SyntaxDeclSpecifierSeq ");
  fprintf(f_debug, "============================\n");
  db_node_an_ifc_syntax_decl_specifier_seq_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_declaration_statement_indent(const an_ifc_syntax_declaration_statement universal,
             unsigned                                  indent)
/*
Given the universal representation of SyntaxDeclarationStatement, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_decl_an_ifc_syntax_declaration_statement(universal)) {
    an_ifc_syntax_index field = get_ifc_decl_an_ifc_syntax_declaration_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "decl:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_pragma_an_ifc_syntax_declaration_statement(universal)) {
    an_ifc_sentence_index field = get_ifc_pragma_an_ifc_syntax_declaration_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "pragma: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_declaration_statement(const an_ifc_syntax_declaration_statement universal)
/*
Given the universal representation of SyntaxDeclarationStatement, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================== ");
  fprintf(f_debug, "SyntaxDeclarationStatement ");
  fprintf(f_debug, "==========================\n");
  db_node_an_ifc_syntax_declaration_statement_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_declarator_indent(const an_ifc_syntax_declarator universal, unsigned indent)
/*
Given the universal representation of SyntaxDeclarator, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_array_or_function_an_ifc_syntax_declarator(universal)) {
    an_ifc_syntax_index field = get_ifc_array_or_function_an_ifc_syntax_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "array_or_function:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_callable_an_ifc_syntax_declarator(universal)) {
    an_ifc_bool field = get_ifc_callable_an_ifc_syntax_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "callable: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_convention_an_ifc_syntax_declarator(universal)) {
    an_ifc_calling_convention_sort field = get_ifc_convention_an_ifc_syntax_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "convention: %s\n", str_for_an_ifc_calling_convention_sort(field));
  }  /* if */
  if (has_ifc_ellipsis_an_ifc_syntax_declarator(universal)) {
    an_ifc_source_location field = get_ifc_ellipsis_an_ifc_syntax_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "ellipsis:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_declarator(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_syntax_declarator(universal)) {
    an_ifc_expr_index field = get_ifc_name_an_ifc_syntax_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_parenthesized_an_ifc_syntax_declarator(universal)) {
    an_ifc_syntax_index field = get_ifc_parenthesized_an_ifc_syntax_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "parenthesized:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_pointer_an_ifc_syntax_declarator(universal)) {
    an_ifc_syntax_index field = get_ifc_pointer_an_ifc_syntax_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "pointer:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_qualifiers_an_ifc_syntax_declarator(universal)) {
    an_ifc_qualifier_bitfield field = get_ifc_qualifiers_an_ifc_syntax_declarator(universal);

    fprintf(f_debug, "qualifiers:\n");
    ++indent;
    if (test_bitmask_ifc_qb_const(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Const\n");
    }  /* if */
    if (test_bitmask_ifc_qb_none(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- None\n");
    }  /* if */
    if (test_bitmask_ifc_qb_restrict(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Restrict\n");
    }  /* if */
    if (test_bitmask_ifc_qb_volatile(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Volatile\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_trailing_target_an_ifc_syntax_declarator(universal)) {
    an_ifc_syntax_index field = get_ifc_trailing_target_an_ifc_syntax_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "trailing_target:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_virtual_specifiers_an_ifc_syntax_declarator(universal)) {
    an_ifc_syntax_index field = get_ifc_virtual_specifiers_an_ifc_syntax_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "virtual_specifiers:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_declarator(const an_ifc_syntax_declarator universal)
/*
Given the universal representation of SyntaxDeclarator, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "=============================== ");
  fprintf(f_debug, "SyntaxDeclarator ");
  fprintf(f_debug, "===============================\n");
  db_node_an_ifc_syntax_declarator_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_decltype_specifier_indent(const an_ifc_syntax_decltype_specifier universal,
             unsigned                               indent)
/*
Given the universal representation of SyntaxDecltypeSpecifier, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_decltype_keyword_an_ifc_syntax_decltype_specifier(universal)) {
    an_ifc_source_location field = get_ifc_decltype_keyword_an_ifc_syntax_decltype_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "decltype_keyword:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_expr_an_ifc_syntax_decltype_specifier(universal)) {
    an_ifc_expr_index field = get_ifc_expr_an_ifc_syntax_decltype_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "expr:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_left_paren_an_ifc_syntax_decltype_specifier(universal)) {
    an_ifc_source_location field = get_ifc_left_paren_an_ifc_syntax_decltype_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "left_paren:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_right_paren_an_ifc_syntax_decltype_specifier(universal)) {
    an_ifc_source_location field = get_ifc_right_paren_an_ifc_syntax_decltype_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "right_paren:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_decltype_specifier(const an_ifc_syntax_decltype_specifier universal)
/*
Given the universal representation of SyntaxDecltypeSpecifier, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "=========================== ");
  fprintf(f_debug, "SyntaxDecltypeSpecifier ");
  fprintf(f_debug, "============================\n");
  db_node_an_ifc_syntax_decltype_specifier_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_do_while_statement_indent(const an_ifc_syntax_do_while_statement universal,
             unsigned                               indent)
/*
Given the universal representation of SyntaxDoWhileStatement, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_body_an_ifc_syntax_do_while_statement(universal)) {
    an_ifc_syntax_index field = get_ifc_body_an_ifc_syntax_do_while_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "body:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_condition_an_ifc_syntax_do_while_statement(universal)) {
    an_ifc_expr_index field = get_ifc_condition_an_ifc_syntax_do_while_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "condition:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_do_an_ifc_syntax_do_while_statement(universal)) {
    an_ifc_source_location field = get_ifc_do_an_ifc_syntax_do_while_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "do:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_pragma_an_ifc_syntax_do_while_statement(universal)) {
    an_ifc_sentence_index field = get_ifc_pragma_an_ifc_syntax_do_while_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "pragma: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_semicolon_an_ifc_syntax_do_while_statement(universal)) {
    an_ifc_source_location field = get_ifc_semicolon_an_ifc_syntax_do_while_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "semicolon:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_while_an_ifc_syntax_do_while_statement(universal)) {
    an_ifc_source_location field = get_ifc_while_an_ifc_syntax_do_while_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "while:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_do_while_statement(const an_ifc_syntax_do_while_statement universal)
/*
Given the universal representation of SyntaxDoWhileStatement, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "============================ ");
  fprintf(f_debug, "SyntaxDoWhileStatement ");
  fprintf(f_debug, "============================\n");
  db_node_an_ifc_syntax_do_while_statement_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_dynamic_exception_spec_indent(const an_ifc_syntax_dynamic_exception_spec universal,
             unsigned                                   indent)
/*
Given the universal representation of SyntaxDynamicExceptionSpec, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_expander_an_ifc_syntax_dynamic_exception_spec(universal)) {
    an_ifc_source_location field = get_ifc_expander_an_ifc_syntax_dynamic_exception_spec(universal);

    db_print_indent(indent);
    fprintf(f_debug, "expander:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_left_paren_an_ifc_syntax_dynamic_exception_spec(universal)) {
    an_ifc_source_location field = get_ifc_left_paren_an_ifc_syntax_dynamic_exception_spec(universal);

    db_print_indent(indent);
    fprintf(f_debug, "left_paren:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_right_paren_an_ifc_syntax_dynamic_exception_spec(universal)) {
    an_ifc_source_location field = get_ifc_right_paren_an_ifc_syntax_dynamic_exception_spec(universal);

    db_print_indent(indent);
    fprintf(f_debug, "right_paren:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_throw_an_ifc_syntax_dynamic_exception_spec(universal)) {
    an_ifc_source_location field = get_ifc_throw_an_ifc_syntax_dynamic_exception_spec(universal);

    db_print_indent(indent);
    fprintf(f_debug, "throw:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_type_list_an_ifc_syntax_dynamic_exception_spec(universal)) {
    an_ifc_syntax_index field = get_ifc_type_list_an_ifc_syntax_dynamic_exception_spec(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type_list:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_dynamic_exception_spec(const an_ifc_syntax_dynamic_exception_spec universal)
/*
Given the universal representation of SyntaxDynamicExceptionSpec, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================== ");
  fprintf(f_debug, "SyntaxDynamicExceptionSpec ");
  fprintf(f_debug, "==========================\n");
  db_node_an_ifc_syntax_dynamic_exception_spec_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_empty_statement_indent(const an_ifc_syntax_empty_statement universal, unsigned indent)
/*
Given the universal representation of SyntaxEmptyStatement, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_syntax_empty_statement(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_empty_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_empty_statement(const an_ifc_syntax_empty_statement universal)
/*
Given the universal representation of SyntaxEmptyStatement, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================= ");
  fprintf(f_debug, "SyntaxEmptyStatement ");
  fprintf(f_debug, "=============================\n");
  db_node_an_ifc_syntax_empty_statement_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_enum_specifier_indent(const an_ifc_syntax_enum_specifier universal, unsigned indent)
/*
Given the universal representation of SyntaxEnumSpecifier, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_base_an_ifc_syntax_enum_specifier(universal)) {
    an_ifc_syntax_index field = get_ifc_base_an_ifc_syntax_enum_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "base:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_class_key_an_ifc_syntax_enum_specifier(universal)) {
    an_ifc_keyword_syntax field = get_ifc_class_key_an_ifc_syntax_enum_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "class_key:\n");
    db_node_an_ifc_keyword_syntax_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_colon_an_ifc_syntax_enum_specifier(universal)) {
    an_ifc_source_location field = get_ifc_colon_an_ifc_syntax_enum_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "colon:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_enumerators_an_ifc_syntax_enum_specifier(universal)) {
    an_ifc_syntax_index field = get_ifc_enumerators_an_ifc_syntax_enum_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "enumerators:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_left_brace_an_ifc_syntax_enum_specifier(universal)) {
    an_ifc_source_location field = get_ifc_left_brace_an_ifc_syntax_enum_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "left_brace:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_enum_specifier(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_enum_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_syntax_enum_specifier(universal)) {
    an_ifc_expr_index field = get_ifc_name_an_ifc_syntax_enum_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_right_brace_an_ifc_syntax_enum_specifier(universal)) {
    an_ifc_source_location field = get_ifc_right_brace_an_ifc_syntax_enum_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "right_brace:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_enum_specifier(const an_ifc_syntax_enum_specifier universal)
/*
Given the universal representation of SyntaxEnumSpecifier, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================= ");
  fprintf(f_debug, "SyntaxEnumSpecifier ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_syntax_enum_specifier_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_enumerator_definition_indent(const an_ifc_syntax_enumerator_definition universal,
             unsigned                                  indent)
/*
Given the universal representation of SyntaxEnumeratorDefinition, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_comma_an_ifc_syntax_enumerator_definition(universal)) {
    an_ifc_source_location field = get_ifc_comma_an_ifc_syntax_enumerator_definition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "comma:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_equal_an_ifc_syntax_enumerator_definition(universal)) {
    an_ifc_source_location field = get_ifc_equal_an_ifc_syntax_enumerator_definition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "equal:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_initializer_an_ifc_syntax_enumerator_definition(universal)) {
    an_ifc_expr_index field = get_ifc_initializer_an_ifc_syntax_enumerator_definition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "initializer:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_enumerator_definition(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_enumerator_definition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_syntax_enumerator_definition(universal)) {
    an_ifc_text_offset field = get_ifc_name_an_ifc_syntax_enumerator_definition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_enumerator_definition(const an_ifc_syntax_enumerator_definition universal)
/*
Given the universal representation of SyntaxEnumeratorDefinition, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================== ");
  fprintf(f_debug, "SyntaxEnumeratorDefinition ");
  fprintf(f_debug, "==========================\n");
  db_node_an_ifc_syntax_enumerator_definition_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_exception_declaration_indent(const an_ifc_syntax_exception_declaration universal,
             unsigned                                  indent)
/*
Given the universal representation of SyntaxExceptionDeclaration, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_declarator_an_ifc_syntax_exception_declaration(universal)) {
    an_ifc_syntax_index field = get_ifc_declarator_an_ifc_syntax_exception_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "declarator:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_ellipsis_an_ifc_syntax_exception_declaration(universal)) {
    an_ifc_source_location field = get_ifc_ellipsis_an_ifc_syntax_exception_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "ellipsis:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_exception_declaration(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_exception_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_type_specifiers_an_ifc_syntax_exception_declaration(universal)) {
    an_ifc_syntax_index field = get_ifc_type_specifiers_an_ifc_syntax_exception_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type_specifiers:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_exception_declaration(const an_ifc_syntax_exception_declaration universal)
/*
Given the universal representation of SyntaxExceptionDeclaration, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================== ");
  fprintf(f_debug, "SyntaxExceptionDeclaration ");
  fprintf(f_debug, "==========================\n");
  db_node_an_ifc_syntax_exception_declaration_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_explicit_specifier_indent(const an_ifc_syntax_explicit_specifier universal,
             unsigned                               indent)
/*
Given the universal representation of SyntaxExplicitSpecifier, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_condition_an_ifc_syntax_explicit_specifier(universal)) {
    an_ifc_expr_index field = get_ifc_condition_an_ifc_syntax_explicit_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "condition:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_left_paren_an_ifc_syntax_explicit_specifier(universal)) {
    an_ifc_source_location field = get_ifc_left_paren_an_ifc_syntax_explicit_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "left_paren:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_explicit_specifier(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_explicit_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_right_paren_an_ifc_syntax_explicit_specifier(universal)) {
    an_ifc_source_location field = get_ifc_right_paren_an_ifc_syntax_explicit_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "right_paren:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_explicit_specifier(const an_ifc_syntax_explicit_specifier universal)
/*
Given the universal representation of SyntaxExplicitSpecifier, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "=========================== ");
  fprintf(f_debug, "SyntaxExplicitSpecifier ");
  fprintf(f_debug, "============================\n");
  db_node_an_ifc_syntax_explicit_specifier_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_expression_indent(const an_ifc_syntax_expression universal, unsigned indent)
/*
Given the universal representation of SyntaxExpression, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_expression_an_ifc_syntax_expression(universal)) {
    an_ifc_expr_index field = get_ifc_expression_an_ifc_syntax_expression(universal);

    db_print_indent(indent);
    fprintf(f_debug, "expression:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_expression(const an_ifc_syntax_expression universal)
/*
Given the universal representation of SyntaxExpression, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "=============================== ");
  fprintf(f_debug, "SyntaxExpression ");
  fprintf(f_debug, "===============================\n");
  db_node_an_ifc_syntax_expression_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_expression_statement_indent(const an_ifc_syntax_expression_statement universal,
             unsigned                                 indent)
/*
Given the universal representation of SyntaxExpressionStatement, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_expr_an_ifc_syntax_expression_statement(universal)) {
    an_ifc_expr_index field = get_ifc_expr_an_ifc_syntax_expression_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "expr:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_pragma_an_ifc_syntax_expression_statement(universal)) {
    an_ifc_sentence_index field = get_ifc_pragma_an_ifc_syntax_expression_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "pragma: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_semicolon_an_ifc_syntax_expression_statement(universal)) {
    an_ifc_source_location field = get_ifc_semicolon_an_ifc_syntax_expression_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "semicolon:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_expression_statement(const an_ifc_syntax_expression_statement universal)
/*
Given the universal representation of SyntaxExpressionStatement, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================== ");
  fprintf(f_debug, "SyntaxExpressionStatement ");
  fprintf(f_debug, "===========================\n");
  db_node_an_ifc_syntax_expression_statement_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_for_range_declaration_indent(const an_ifc_syntax_for_range_declaration universal,
             unsigned                                  indent)
/*
Given the universal representation of SyntaxForRangeDeclaration, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_declarator_an_ifc_syntax_for_range_declaration(universal)) {
    an_ifc_syntax_index field = get_ifc_declarator_an_ifc_syntax_for_range_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "declarator:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_specifiers_an_ifc_syntax_for_range_declaration(universal)) {
    an_ifc_syntax_index field = get_ifc_specifiers_an_ifc_syntax_for_range_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "specifiers:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_for_range_declaration(const an_ifc_syntax_for_range_declaration universal)
/*
Given the universal representation of SyntaxForRangeDeclaration, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================== ");
  fprintf(f_debug, "SyntaxForRangeDeclaration ");
  fprintf(f_debug, "===========================\n");
  db_node_an_ifc_syntax_for_range_declaration_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_for_statement_indent(const an_ifc_syntax_for_statement universal, unsigned indent)
/*
Given the universal representation of SyntaxForStatement, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_body_an_ifc_syntax_for_statement(universal)) {
    an_ifc_syntax_index field = get_ifc_body_an_ifc_syntax_for_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "body:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_condition_an_ifc_syntax_for_statement(universal)) {
    an_ifc_expr_index field = get_ifc_condition_an_ifc_syntax_for_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "condition:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_continuation_an_ifc_syntax_for_statement(universal)) {
    an_ifc_expr_index field = get_ifc_continuation_an_ifc_syntax_for_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "continuation:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_for_an_ifc_syntax_for_statement(universal)) {
    an_ifc_source_location field = get_ifc_for_an_ifc_syntax_for_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "for:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_initialization_an_ifc_syntax_for_statement(universal)) {
    an_ifc_syntax_index field = get_ifc_initialization_an_ifc_syntax_for_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "initialization:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_left_paren_an_ifc_syntax_for_statement(universal)) {
    an_ifc_source_location field = get_ifc_left_paren_an_ifc_syntax_for_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "left_paren:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_pragma_an_ifc_syntax_for_statement(universal)) {
    an_ifc_sentence_index field = get_ifc_pragma_an_ifc_syntax_for_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "pragma: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_right_paren_an_ifc_syntax_for_statement(universal)) {
    an_ifc_source_location field = get_ifc_right_paren_an_ifc_syntax_for_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "right_paren:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_semicolon_an_ifc_syntax_for_statement(universal)) {
    an_ifc_source_location field = get_ifc_semicolon_an_ifc_syntax_for_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "semicolon:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_for_statement(const an_ifc_syntax_for_statement universal)
/*
Given the universal representation of SyntaxForStatement, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================== ");
  fprintf(f_debug, "SyntaxForStatement ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_syntax_for_statement_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_function_body_indent(const an_ifc_syntax_function_body universal, unsigned indent)
/*
Given the universal representation of SyntaxFunctionBody, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_assign_an_ifc_syntax_function_body(universal)) {
    an_ifc_source_location field = get_ifc_assign_an_ifc_syntax_function_body(universal);

    db_print_indent(indent);
    fprintf(f_debug, "assign:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_generate_an_ifc_syntax_function_body(universal)) {
    an_ifc_keyword_syntax field = get_ifc_generate_an_ifc_syntax_function_body(universal);

    db_print_indent(indent);
    fprintf(f_debug, "generate:\n");
    db_node_an_ifc_keyword_syntax_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_initializers_an_ifc_syntax_function_body(universal)) {
    an_ifc_syntax_index field = get_ifc_initializers_an_ifc_syntax_function_body(universal);

    db_print_indent(indent);
    fprintf(f_debug, "initializers:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_semicolon_an_ifc_syntax_function_body(universal)) {
    an_ifc_source_location field = get_ifc_semicolon_an_ifc_syntax_function_body(universal);

    db_print_indent(indent);
    fprintf(f_debug, "semicolon:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_stmts_an_ifc_syntax_function_body(universal)) {
    an_ifc_syntax_index field = get_ifc_stmts_an_ifc_syntax_function_body(universal);

    db_print_indent(indent);
    fprintf(f_debug, "stmts:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_try_block_an_ifc_syntax_function_body(universal)) {
    an_ifc_syntax_index field = get_ifc_try_block_an_ifc_syntax_function_body(universal);

    db_print_indent(indent);
    fprintf(f_debug, "try_block:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_function_body(const an_ifc_syntax_function_body universal)
/*
Given the universal representation of SyntaxFunctionBody, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================== ");
  fprintf(f_debug, "SyntaxFunctionBody ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_syntax_function_body_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_function_declarator_indent(const an_ifc_syntax_function_declarator universal,
             unsigned                                indent)
/*
Given the universal representation of SyntaxFunctionDeclarator, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_eh_spec_an_ifc_syntax_function_declarator(universal)) {
    an_ifc_syntax_index field = get_ifc_eh_spec_an_ifc_syntax_function_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "eh_spec:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_ellipsis_an_ifc_syntax_function_declarator(universal)) {
    an_ifc_source_location field = get_ifc_ellipsis_an_ifc_syntax_function_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "ellipsis:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_left_paren_an_ifc_syntax_function_declarator(universal)) {
    an_ifc_source_location field = get_ifc_left_paren_an_ifc_syntax_function_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "left_paren:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_parameters_an_ifc_syntax_function_declarator(universal)) {
    an_ifc_syntax_index field = get_ifc_parameters_an_ifc_syntax_function_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "parameters:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_ref_an_ifc_syntax_function_declarator(universal)) {
    an_ifc_source_location field = get_ifc_ref_an_ifc_syntax_function_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "ref:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_right_paren_an_ifc_syntax_function_declarator(universal)) {
    an_ifc_source_location field = get_ifc_right_paren_an_ifc_syntax_function_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "right_paren:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_traits_an_ifc_syntax_function_declarator(universal)) {
    an_ifc_function_type_traits_bitfield field = get_ifc_traits_an_ifc_syntax_function_declarator(universal);

    fprintf(f_debug, "traits:\n");
    ++indent;
    if (test_bitmask_ifc_fttb_const(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Const\n");
    }  /* if */
    if (test_bitmask_ifc_fttb_lvalue(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Lvalue\n");
    }  /* if */
    if (test_bitmask_ifc_fttb_none(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- None\n");
    }  /* if */
    if (test_bitmask_ifc_fttb_rvalue(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Rvalue\n");
    }  /* if */
    if (test_bitmask_ifc_fttb_volatile(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Volatile\n");
    }  /* if */
    --indent;
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_function_declarator(const an_ifc_syntax_function_declarator universal)
/*
Given the universal representation of SyntaxFunctionDeclarator, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "=========================== ");
  fprintf(f_debug, "SyntaxFunctionDeclarator ");
  fprintf(f_debug, "===========================\n");
  db_node_an_ifc_syntax_function_declarator_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_function_definition_indent(const an_ifc_syntax_function_definition universal,
             unsigned                                indent)
/*
Given the universal representation of SyntaxFunctionDefinition, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_assign_an_ifc_syntax_function_definition(universal)) {
    an_ifc_source_location field = get_ifc_assign_an_ifc_syntax_function_definition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "assign:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_initializers_an_ifc_syntax_function_definition(universal)) {
    an_ifc_syntax_index field = get_ifc_initializers_an_ifc_syntax_function_definition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "initializers:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_semicolon_an_ifc_syntax_function_definition(universal)) {
    an_ifc_source_location field = get_ifc_semicolon_an_ifc_syntax_function_definition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "semicolon:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_stmts_an_ifc_syntax_function_definition(universal)) {
    an_ifc_syntax_index field = get_ifc_stmts_an_ifc_syntax_function_definition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "stmts:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_synthesis_an_ifc_syntax_function_definition(universal)) {
    an_ifc_keyword_syntax field = get_ifc_synthesis_an_ifc_syntax_function_definition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "synthesis:\n");
    db_node_an_ifc_keyword_syntax_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_try_block_an_ifc_syntax_function_definition(universal)) {
    an_ifc_syntax_index field = get_ifc_try_block_an_ifc_syntax_function_definition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "try_block:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_function_definition(const an_ifc_syntax_function_definition universal)
/*
Given the universal representation of SyntaxFunctionDefinition, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "=========================== ");
  fprintf(f_debug, "SyntaxFunctionDefinition ");
  fprintf(f_debug, "===========================\n");
  db_node_an_ifc_syntax_function_definition_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_function_try_block_indent(const an_ifc_syntax_function_try_block universal,
             unsigned                               indent)
/*
Given the universal representation of SyntaxFunctionTryBlock, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_body_an_ifc_syntax_function_try_block(universal)) {
    an_ifc_syntax_index field = get_ifc_body_an_ifc_syntax_function_try_block(universal);

    db_print_indent(indent);
    fprintf(f_debug, "body:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_handlers_an_ifc_syntax_function_try_block(universal)) {
    an_ifc_syntax_index field = get_ifc_handlers_an_ifc_syntax_function_try_block(universal);

    db_print_indent(indent);
    fprintf(f_debug, "handlers:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_initializers_an_ifc_syntax_function_try_block(universal)) {
    an_ifc_syntax_index field = get_ifc_initializers_an_ifc_syntax_function_try_block(universal);

    db_print_indent(indent);
    fprintf(f_debug, "initializers:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_function_try_block(const an_ifc_syntax_function_try_block universal)
/*
Given the universal representation of SyntaxFunctionTryBlock, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "============================ ");
  fprintf(f_debug, "SyntaxFunctionTryBlock ");
  fprintf(f_debug, "============================\n");
  db_node_an_ifc_syntax_function_try_block_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_goto_statement_indent(const an_ifc_syntax_goto_statement universal, unsigned indent)
/*
Given the universal representation of SyntaxGotoStatement, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_label_an_ifc_syntax_goto_statement(universal)) {
    an_ifc_source_location field = get_ifc_label_an_ifc_syntax_goto_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "label:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_goto_statement(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_goto_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_pragma_an_ifc_syntax_goto_statement(universal)) {
    an_ifc_sentence_index field = get_ifc_pragma_an_ifc_syntax_goto_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "pragma: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_semicolon_an_ifc_syntax_goto_statement(universal)) {
    an_ifc_source_location field = get_ifc_semicolon_an_ifc_syntax_goto_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "semicolon:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_target_an_ifc_syntax_goto_statement(universal)) {
    an_ifc_text_offset field = get_ifc_target_an_ifc_syntax_goto_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "target: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_goto_statement(const an_ifc_syntax_goto_statement universal)
/*
Given the universal representation of SyntaxGotoStatement, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================= ");
  fprintf(f_debug, "SyntaxGotoStatement ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_syntax_goto_statement_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_handler_indent(const an_ifc_syntax_handler universal, unsigned indent)
/*
Given the universal representation of SyntaxHandler, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_body_an_ifc_syntax_handler(universal)) {
    an_ifc_syntax_index field = get_ifc_body_an_ifc_syntax_handler(universal);

    db_print_indent(indent);
    fprintf(f_debug, "body:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_catch_an_ifc_syntax_handler(universal)) {
    an_ifc_source_location field = get_ifc_catch_an_ifc_syntax_handler(universal);

    db_print_indent(indent);
    fprintf(f_debug, "catch:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_exception_an_ifc_syntax_handler(universal)) {
    an_ifc_syntax_index field = get_ifc_exception_an_ifc_syntax_handler(universal);

    db_print_indent(indent);
    fprintf(f_debug, "exception:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_left_paren_an_ifc_syntax_handler(universal)) {
    an_ifc_source_location field = get_ifc_left_paren_an_ifc_syntax_handler(universal);

    db_print_indent(indent);
    fprintf(f_debug, "left_paren:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_pragma_an_ifc_syntax_handler(universal)) {
    an_ifc_sentence_index field = get_ifc_pragma_an_ifc_syntax_handler(universal);

    db_print_indent(indent);
    fprintf(f_debug, "pragma: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_right_paren_an_ifc_syntax_handler(universal)) {
    an_ifc_source_location field = get_ifc_right_paren_an_ifc_syntax_handler(universal);

    db_print_indent(indent);
    fprintf(f_debug, "right_paren:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_handler(const an_ifc_syntax_handler universal)
/*
Given the universal representation of SyntaxHandler, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "SyntaxHandler ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_syntax_handler_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_handler_seq_indent(const an_ifc_syntax_handler_seq universal, unsigned indent)
/*
Given the universal representation of SyntaxHandlerSeq, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_handlers_an_ifc_syntax_handler_seq(universal)) {
    an_ifc_syntax_index field = get_ifc_handlers_an_ifc_syntax_handler_seq(universal);

    db_print_indent(indent);
    fprintf(f_debug, "handlers:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_handler_seq(const an_ifc_syntax_handler_seq universal)
/*
Given the universal representation of SyntaxHandlerSeq, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "=============================== ");
  fprintf(f_debug, "SyntaxHandlerSeq ");
  fprintf(f_debug, "===============================\n");
  db_node_an_ifc_syntax_handler_seq_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_if_statement_indent(const an_ifc_syntax_if_statement universal, unsigned indent)
/*
Given the universal representation of SyntaxIfStatement, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_alternative_an_ifc_syntax_if_statement(universal)) {
    an_ifc_syntax_index field = get_ifc_alternative_an_ifc_syntax_if_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "alternative:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_condition_an_ifc_syntax_if_statement(universal)) {
    an_ifc_index field = get_ifc_condition_an_ifc_syntax_if_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "condition: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_consequence_an_ifc_syntax_if_statement(universal)) {
    an_ifc_syntax_index field = get_ifc_consequence_an_ifc_syntax_if_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "consequence:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_constexpr_an_ifc_syntax_if_statement(universal)) {
    an_ifc_source_location field = get_ifc_constexpr_an_ifc_syntax_if_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "constexpr:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_else_an_ifc_syntax_if_statement(universal)) {
    an_ifc_source_location field = get_ifc_else_an_ifc_syntax_if_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "else:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_if_an_ifc_syntax_if_statement(universal)) {
    an_ifc_source_location field = get_ifc_if_an_ifc_syntax_if_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "if:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_initialization_an_ifc_syntax_if_statement(universal)) {
    an_ifc_syntax_index field = get_ifc_initialization_an_ifc_syntax_if_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "initialization:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_pragma_an_ifc_syntax_if_statement(universal)) {
    an_ifc_sentence_index field = get_ifc_pragma_an_ifc_syntax_if_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "pragma: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_if_statement(const an_ifc_syntax_if_statement universal)
/*
Given the universal representation of SyntaxIfStatement, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================== ");
  fprintf(f_debug, "SyntaxIfStatement ");
  fprintf(f_debug, "===============================\n");
  db_node_an_ifc_syntax_if_statement_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_init_capture_indent(const an_ifc_syntax_init_capture universal, unsigned indent)
/*
Given the universal representation of SyntaxInitCapture, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_ampersand_an_ifc_syntax_init_capture(universal)) {
    an_ifc_source_location field = get_ifc_ampersand_an_ifc_syntax_init_capture(universal);

    db_print_indent(indent);
    fprintf(f_debug, "ampersand:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_comma_an_ifc_syntax_init_capture(universal)) {
    an_ifc_source_location field = get_ifc_comma_an_ifc_syntax_init_capture(universal);

    db_print_indent(indent);
    fprintf(f_debug, "comma:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_expander_an_ifc_syntax_init_capture(universal)) {
    an_ifc_source_location field = get_ifc_expander_an_ifc_syntax_init_capture(universal);

    db_print_indent(indent);
    fprintf(f_debug, "expander:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_initializer_an_ifc_syntax_init_capture(universal)) {
    an_ifc_expr_index field = get_ifc_initializer_an_ifc_syntax_init_capture(universal);

    db_print_indent(indent);
    fprintf(f_debug, "initializer:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_name_an_ifc_syntax_init_capture(universal)) {
    an_ifc_expr_index field = get_ifc_name_an_ifc_syntax_init_capture(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_init_capture(const an_ifc_syntax_init_capture universal)
/*
Given the universal representation of SyntaxInitCapture, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================== ");
  fprintf(f_debug, "SyntaxInitCapture ");
  fprintf(f_debug, "===============================\n");
  db_node_an_ifc_syntax_init_capture_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_init_declarator_indent(const an_ifc_syntax_init_declarator universal, unsigned indent)
/*
Given the universal representation of SyntaxInitDeclarator, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_comma_an_ifc_syntax_init_declarator(universal)) {
    an_ifc_source_location field = get_ifc_comma_an_ifc_syntax_init_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "comma:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_constraint_an_ifc_syntax_init_declarator(universal)) {
    an_ifc_syntax_index field = get_ifc_constraint_an_ifc_syntax_init_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "constraint:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_declarator_an_ifc_syntax_init_declarator(universal)) {
    an_ifc_syntax_index field = get_ifc_declarator_an_ifc_syntax_init_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "declarator:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_initializer_an_ifc_syntax_init_declarator(universal)) {
    an_ifc_expr_index field = get_ifc_initializer_an_ifc_syntax_init_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "initializer:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_init_declarator(const an_ifc_syntax_init_declarator universal)
/*
Given the universal representation of SyntaxInitDeclarator, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================= ");
  fprintf(f_debug, "SyntaxInitDeclarator ");
  fprintf(f_debug, "=============================\n");
  db_node_an_ifc_syntax_init_declarator_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_init_statement_indent(const an_ifc_syntax_init_statement universal, unsigned indent)
/*
Given the universal representation of SyntaxInitStatement, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_init_an_ifc_syntax_init_statement(universal)) {
    an_ifc_syntax_index field = get_ifc_init_an_ifc_syntax_init_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "init:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_pragma_an_ifc_syntax_init_statement(universal)) {
    an_ifc_sentence_index field = get_ifc_pragma_an_ifc_syntax_init_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "pragma: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_init_statement(const an_ifc_syntax_init_statement universal)
/*
Given the universal representation of SyntaxInitStatement, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================= ");
  fprintf(f_debug, "SyntaxInitStatement ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_syntax_init_statement_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_labeled_statement_indent(const an_ifc_syntax_labeled_statement universal, unsigned indent)
/*
Given the universal representation of SyntaxLabeledStatement, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_label_an_ifc_syntax_labeled_statement(universal)) {
    an_ifc_expr_index field = get_ifc_label_an_ifc_syntax_labeled_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "label:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_labeled_statement(universal)) {
    an_ifc_keyword_sort field = get_ifc_locus_an_ifc_syntax_labeled_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus: %s\n", str_for_an_ifc_keyword_sort(field));
  }  /* if */
  if (has_ifc_pragma_an_ifc_syntax_labeled_statement(universal)) {
    an_ifc_sentence_index field = get_ifc_pragma_an_ifc_syntax_labeled_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "pragma: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_sort_an_ifc_syntax_labeled_statement(universal)) {
    an_ifc_label_sort field = get_ifc_sort_an_ifc_syntax_labeled_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "sort: %s\n", str_for_an_ifc_label_sort(field));
  }  /* if */
  if (has_ifc_stmt_an_ifc_syntax_labeled_statement(universal)) {
    an_ifc_syntax_index field = get_ifc_stmt_an_ifc_syntax_labeled_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "stmt:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_labeled_statement(const an_ifc_syntax_labeled_statement universal)
/*
Given the universal representation of SyntaxLabeledStatement, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "============================ ");
  fprintf(f_debug, "SyntaxLabeledStatement ");
  fprintf(f_debug, "============================\n");
  db_node_an_ifc_syntax_labeled_statement_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_lambda_declarator_indent(const an_ifc_syntax_lambda_declarator universal, unsigned indent)
/*
Given the universal representation of SyntaxLambdaDeclarator, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_eh_spec_an_ifc_syntax_lambda_declarator(universal)) {
    an_ifc_syntax_index field = get_ifc_eh_spec_an_ifc_syntax_lambda_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "eh_spec:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_expander_an_ifc_syntax_lambda_declarator(universal)) {
    an_ifc_source_location field = get_ifc_expander_an_ifc_syntax_lambda_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "expander:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_left_paren_an_ifc_syntax_lambda_declarator(universal)) {
    an_ifc_source_location field = get_ifc_left_paren_an_ifc_syntax_lambda_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "left_paren:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_modifier_an_ifc_syntax_lambda_declarator(universal)) {
    an_ifc_keyword_sort field = get_ifc_modifier_an_ifc_syntax_lambda_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "modifier: %s\n", str_for_an_ifc_keyword_sort(field));
  }  /* if */
  if (has_ifc_parameters_an_ifc_syntax_lambda_declarator(universal)) {
    an_ifc_syntax_index field = get_ifc_parameters_an_ifc_syntax_lambda_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "parameters:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_right_paren_an_ifc_syntax_lambda_declarator(universal)) {
    an_ifc_source_location field = get_ifc_right_paren_an_ifc_syntax_lambda_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "right_paren:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_trailing_target_an_ifc_syntax_lambda_declarator(universal)) {
    an_ifc_syntax_index field = get_ifc_trailing_target_an_ifc_syntax_lambda_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "trailing_target:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_lambda_declarator(const an_ifc_syntax_lambda_declarator universal)
/*
Given the universal representation of SyntaxLambdaDeclarator, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "============================ ");
  fprintf(f_debug, "SyntaxLambdaDeclarator ");
  fprintf(f_debug, "============================\n");
  db_node_an_ifc_syntax_lambda_declarator_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_lambda_introducer_indent(const an_ifc_syntax_lambda_introducer universal, unsigned indent)
/*
Given the universal representation of SyntaxLambdaIntroducer, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_captures_an_ifc_syntax_lambda_introducer(universal)) {
    an_ifc_syntax_index field = get_ifc_captures_an_ifc_syntax_lambda_introducer(universal);

    db_print_indent(indent);
    fprintf(f_debug, "captures:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_left_bracket_an_ifc_syntax_lambda_introducer(universal)) {
    an_ifc_source_location field = get_ifc_left_bracket_an_ifc_syntax_lambda_introducer(universal);

    db_print_indent(indent);
    fprintf(f_debug, "left_bracket:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_right_bracket_an_ifc_syntax_lambda_introducer(universal)) {
    an_ifc_source_location field = get_ifc_right_bracket_an_ifc_syntax_lambda_introducer(universal);

    db_print_indent(indent);
    fprintf(f_debug, "right_bracket:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_lambda_introducer(const an_ifc_syntax_lambda_introducer universal)
/*
Given the universal representation of SyntaxLambdaIntroducer, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "============================ ");
  fprintf(f_debug, "SyntaxLambdaIntroducer ");
  fprintf(f_debug, "============================\n");
  db_node_an_ifc_syntax_lambda_introducer_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_mem_initializer_indent(const an_ifc_syntax_mem_initializer universal, unsigned indent)
/*
Given the universal representation of SyntaxMemInitializer, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_comma_an_ifc_syntax_mem_initializer(universal)) {
    an_ifc_source_location field = get_ifc_comma_an_ifc_syntax_mem_initializer(universal);

    db_print_indent(indent);
    fprintf(f_debug, "comma:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_expander_an_ifc_syntax_mem_initializer(universal)) {
    an_ifc_source_location field = get_ifc_expander_an_ifc_syntax_mem_initializer(universal);

    db_print_indent(indent);
    fprintf(f_debug, "expander:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_initializer_an_ifc_syntax_mem_initializer(universal)) {
    an_ifc_expr_index field = get_ifc_initializer_an_ifc_syntax_mem_initializer(universal);

    db_print_indent(indent);
    fprintf(f_debug, "initializer:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_member_an_ifc_syntax_mem_initializer(universal)) {
    an_ifc_expr_index field = get_ifc_member_an_ifc_syntax_mem_initializer(universal);

    db_print_indent(indent);
    fprintf(f_debug, "member:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_mem_initializer(const an_ifc_syntax_mem_initializer universal)
/*
Given the universal representation of SyntaxMemInitializer, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================= ");
  fprintf(f_debug, "SyntaxMemInitializer ");
  fprintf(f_debug, "=============================\n");
  db_node_an_ifc_syntax_mem_initializer_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_member_declaration_indent(const an_ifc_syntax_member_declaration universal,
             unsigned                               indent)
/*
Given the universal representation of SyntaxMemberDeclaration, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_decl_specifiers_an_ifc_syntax_member_declaration(universal)) {
    an_ifc_syntax_index field = get_ifc_decl_specifiers_an_ifc_syntax_member_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "decl_specifiers:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_declarations_an_ifc_syntax_member_declaration(universal)) {
    an_ifc_syntax_index field = get_ifc_declarations_an_ifc_syntax_member_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "declarations:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_semicolon_an_ifc_syntax_member_declaration(universal)) {
    an_ifc_source_location field = get_ifc_semicolon_an_ifc_syntax_member_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "semicolon:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_member_declaration(const an_ifc_syntax_member_declaration universal)
/*
Given the universal representation of SyntaxMemberDeclaration, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "=========================== ");
  fprintf(f_debug, "SyntaxMemberDeclaration ");
  fprintf(f_debug, "============================\n");
  db_node_an_ifc_syntax_member_declaration_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_member_declarator_indent(const an_ifc_syntax_member_declarator universal, unsigned indent)
/*
Given the universal representation of SyntaxMemberDeclarator, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_bitwidth_an_ifc_syntax_member_declarator(universal)) {
    an_ifc_expr_index field = get_ifc_bitwidth_an_ifc_syntax_member_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "bitwidth:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_colon_an_ifc_syntax_member_declarator(universal)) {
    an_ifc_source_location field = get_ifc_colon_an_ifc_syntax_member_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "colon:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_comma_an_ifc_syntax_member_declarator(universal)) {
    an_ifc_source_location field = get_ifc_comma_an_ifc_syntax_member_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "comma:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_constraint_an_ifc_syntax_member_declarator(universal)) {
    an_ifc_syntax_index field = get_ifc_constraint_an_ifc_syntax_member_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "constraint:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_declarator_an_ifc_syntax_member_declarator(universal)) {
    an_ifc_syntax_index field = get_ifc_declarator_an_ifc_syntax_member_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "declarator:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_initializer_an_ifc_syntax_member_declarator(universal)) {
    an_ifc_expr_index field = get_ifc_initializer_an_ifc_syntax_member_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "initializer:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_member_declarator(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_member_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_member_declarator(const an_ifc_syntax_member_declarator universal)
/*
Given the universal representation of SyntaxMemberDeclarator, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "============================ ");
  fprintf(f_debug, "SyntaxMemberDeclarator ");
  fprintf(f_debug, "============================\n");
  db_node_an_ifc_syntax_member_declarator_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_member_function_declaration_indent(const an_ifc_syntax_member_function_declaration universal,
             unsigned                                        indent)
/*
Given the universal representation of SyntaxMemberFunctionDeclaration, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_definition_an_ifc_syntax_member_function_declaration(universal)) {
    an_ifc_syntax_index field = get_ifc_definition_an_ifc_syntax_member_function_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "definition:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_member_function_declaration(const an_ifc_syntax_member_function_declaration universal)
/*
Given the universal representation of SyntaxMemberFunctionDeclaration, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "======================= ");
  fprintf(f_debug, "SyntaxMemberFunctionDeclaration ");
  fprintf(f_debug, "========================\n");
  db_node_an_ifc_syntax_member_function_declaration_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_member_specification_indent(const an_ifc_syntax_member_specification universal,
             unsigned                                 indent)
/*
Given the universal representation of SyntaxMemberSpecification, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_member_declarations_an_ifc_syntax_member_specification(universal)) {
    an_ifc_syntax_index field = get_ifc_member_declarations_an_ifc_syntax_member_specification(universal);

    db_print_indent(indent);
    fprintf(f_debug, "member_declarations:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_member_specification(const an_ifc_syntax_member_specification universal)
/*
Given the universal representation of SyntaxMemberSpecification, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================== ");
  fprintf(f_debug, "SyntaxMemberSpecification ");
  fprintf(f_debug, "===========================\n");
  db_node_an_ifc_syntax_member_specification_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_namespace_alias_definition_indent(const an_ifc_syntax_namespace_alias_definition universal,
             unsigned                                       indent)
/*
Given the universal representation of SyntaxNamespaceAliasDefinition, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_assign_an_ifc_syntax_namespace_alias_definition(universal)) {
    an_ifc_source_location field = get_ifc_assign_an_ifc_syntax_namespace_alias_definition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "assign:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_syntax_namespace_alias_definition(universal)) {
    an_ifc_expr_index field = get_ifc_name_an_ifc_syntax_namespace_alias_definition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_namespace_kw_an_ifc_syntax_namespace_alias_definition(universal)) {
    an_ifc_source_location field = get_ifc_namespace_kw_an_ifc_syntax_namespace_alias_definition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "namespace_kw:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_semicolon_an_ifc_syntax_namespace_alias_definition(universal)) {
    an_ifc_source_location field = get_ifc_semicolon_an_ifc_syntax_namespace_alias_definition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "semicolon:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_target_an_ifc_syntax_namespace_alias_definition(universal)) {
    an_ifc_expr_index field = get_ifc_target_an_ifc_syntax_namespace_alias_definition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "target:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_namespace_alias_definition(const an_ifc_syntax_namespace_alias_definition universal)
/*
Given the universal representation of SyntaxNamespaceAliasDefinition, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "======================== ");
  fprintf(f_debug, "SyntaxNamespaceAliasDefinition ");
  fprintf(f_debug, "========================\n");
  db_node_an_ifc_syntax_namespace_alias_definition_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_nested_requirement_indent(const an_ifc_syntax_nested_requirement universal,
             unsigned                               indent)
/*
Given the universal representation of SyntaxNestedRequirement, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_condition_an_ifc_syntax_nested_requirement(universal)) {
    an_ifc_expr_index field = get_ifc_condition_an_ifc_syntax_nested_requirement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "condition:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_nested_requirement(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_nested_requirement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_nested_requirement(const an_ifc_syntax_nested_requirement universal)
/*
Given the universal representation of SyntaxNestedRequirement, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "=========================== ");
  fprintf(f_debug, "SyntaxNestedRequirement ");
  fprintf(f_debug, "============================\n");
  db_node_an_ifc_syntax_nested_requirement_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_new_declarator_indent(const an_ifc_syntax_new_declarator universal, unsigned indent)
/*
Given the universal representation of SyntaxNewDeclarator, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_declarator_an_ifc_syntax_new_declarator(universal)) {
    an_ifc_syntax_index field = get_ifc_declarator_an_ifc_syntax_new_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "declarator:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_new_declarator(const an_ifc_syntax_new_declarator universal)
/*
Given the universal representation of SyntaxNewDeclarator, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================= ");
  fprintf(f_debug, "SyntaxNewDeclarator ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_syntax_new_declarator_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_noexcept_specification_indent(const an_ifc_syntax_noexcept_specification universal,
             unsigned                                   indent)
/*
Given the universal representation of SyntaxNoexceptSpecification, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_expr_an_ifc_syntax_noexcept_specification(universal)) {
    an_ifc_syntax_index field = get_ifc_expr_an_ifc_syntax_noexcept_specification(universal);

    db_print_indent(indent);
    fprintf(f_debug, "expr:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_left_paren_an_ifc_syntax_noexcept_specification(universal)) {
    an_ifc_source_location field = get_ifc_left_paren_an_ifc_syntax_noexcept_specification(universal);

    db_print_indent(indent);
    fprintf(f_debug, "left_paren:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_noexcept_specification(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_noexcept_specification(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_right_paren_an_ifc_syntax_noexcept_specification(universal)) {
    an_ifc_source_location field = get_ifc_right_paren_an_ifc_syntax_noexcept_specification(universal);

    db_print_indent(indent);
    fprintf(f_debug, "right_paren:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_noexcept_specification(const an_ifc_syntax_noexcept_specification universal)
/*
Given the universal representation of SyntaxNoexceptSpecification, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================= ");
  fprintf(f_debug, "SyntaxNoexceptSpecification ");
  fprintf(f_debug, "==========================\n");
  db_node_an_ifc_syntax_noexcept_specification_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_non_type_template_argument_indent(const an_ifc_syntax_non_type_template_argument universal,
             unsigned                                       indent)
/*
Given the universal representation of SyntaxNonTypeTemplateArgument, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_argument_an_ifc_syntax_non_type_template_argument(universal)) {
    an_ifc_expr_index field = get_ifc_argument_an_ifc_syntax_non_type_template_argument(universal);

    db_print_indent(indent);
    fprintf(f_debug, "argument:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_comma_an_ifc_syntax_non_type_template_argument(universal)) {
    an_ifc_source_location field = get_ifc_comma_an_ifc_syntax_non_type_template_argument(universal);

    db_print_indent(indent);
    fprintf(f_debug, "comma:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_ellipsis_an_ifc_syntax_non_type_template_argument(universal)) {
    an_ifc_source_location field = get_ifc_ellipsis_an_ifc_syntax_non_type_template_argument(universal);

    db_print_indent(indent);
    fprintf(f_debug, "ellipsis:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_non_type_template_argument(const an_ifc_syntax_non_type_template_argument universal)
/*
Given the universal representation of SyntaxNonTypeTemplateArgument, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "======================== ");
  fprintf(f_debug, "SyntaxNonTypeTemplateArgument ");
  fprintf(f_debug, "=========================\n");
  db_node_an_ifc_syntax_non_type_template_argument_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_parameter_declarator_indent(const an_ifc_syntax_parameter_declarator universal,
             unsigned                                 indent)
/*
Given the universal representation of SyntaxParameterDeclarator, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_decl_specifiers_an_ifc_syntax_parameter_declarator(universal)) {
    an_ifc_syntax_index field = get_ifc_decl_specifiers_an_ifc_syntax_parameter_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "decl_specifiers:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_declarator_an_ifc_syntax_parameter_declarator(universal)) {
    an_ifc_syntax_index field = get_ifc_declarator_an_ifc_syntax_parameter_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "declarator:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_default_expr_an_ifc_syntax_parameter_declarator(universal)) {
    an_ifc_expr_index field = get_ifc_default_expr_an_ifc_syntax_parameter_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "default_expr:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_parameter_declarator(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_parameter_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_sort_an_ifc_syntax_parameter_declarator(universal)) {
    an_ifc_parameter_sort field = get_ifc_sort_an_ifc_syntax_parameter_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "sort: %s\n", str_for_an_ifc_parameter_sort(field));
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_parameter_declarator(const an_ifc_syntax_parameter_declarator universal)
/*
Given the universal representation of SyntaxParameterDeclarator, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================== ");
  fprintf(f_debug, "SyntaxParameterDeclarator ");
  fprintf(f_debug, "===========================\n");
  db_node_an_ifc_syntax_parameter_declarator_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_placeholder_type_specifier_indent(const an_ifc_syntax_placeholder_type_specifier universal,
             unsigned                                       indent)
/*
Given the universal representation of SyntaxPlaceholderTypeSpecifier, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_basis_an_ifc_syntax_placeholder_type_specifier(universal)) {
    an_ifc_type_basis_sort field = get_ifc_basis_an_ifc_syntax_placeholder_type_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "basis: %s\n", str_for_an_ifc_type_basis_sort(field));
  }  /* if */
  if (has_ifc_constraint_an_ifc_syntax_placeholder_type_specifier(universal)) {
    an_ifc_expr_index field = get_ifc_constraint_an_ifc_syntax_placeholder_type_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "constraint:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_keyword_an_ifc_syntax_placeholder_type_specifier(universal)) {
    an_ifc_source_location field = get_ifc_keyword_an_ifc_syntax_placeholder_type_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "keyword:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_placeholder_type_specifier(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_placeholder_type_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_placeholder_type_specifier(const an_ifc_syntax_placeholder_type_specifier universal)
/*
Given the universal representation of SyntaxPlaceholderTypeSpecifier, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "======================== ");
  fprintf(f_debug, "SyntaxPlaceholderTypeSpecifier ");
  fprintf(f_debug, "========================\n");
  db_node_an_ifc_syntax_placeholder_type_specifier_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_pointer_declarator_indent(const an_ifc_syntax_pointer_declarator universal,
             unsigned                               indent)
/*
Given the universal representation of SyntaxPointerDeclarator, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_callable_an_ifc_syntax_pointer_declarator(universal)) {
    an_ifc_bool field = get_ifc_callable_an_ifc_syntax_pointer_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "callable: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_convention_an_ifc_syntax_pointer_declarator(universal)) {
    an_ifc_calling_convention_sort field = get_ifc_convention_an_ifc_syntax_pointer_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "convention: %s\n", str_for_an_ifc_calling_convention_sort(field));
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_pointer_declarator(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_pointer_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_next_an_ifc_syntax_pointer_declarator(universal)) {
    an_ifc_syntax_index field = get_ifc_next_an_ifc_syntax_pointer_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "next:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_qualifiers_an_ifc_syntax_pointer_declarator(universal)) {
    an_ifc_qualifier_bitfield field = get_ifc_qualifiers_an_ifc_syntax_pointer_declarator(universal);

    fprintf(f_debug, "qualifiers:\n");
    ++indent;
    if (test_bitmask_ifc_qb_const(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Const\n");
    }  /* if */
    if (test_bitmask_ifc_qb_none(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- None\n");
    }  /* if */
    if (test_bitmask_ifc_qb_restrict(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Restrict\n");
    }  /* if */
    if (test_bitmask_ifc_qb_volatile(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Volatile\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_sort_an_ifc_syntax_pointer_declarator(universal)) {
    an_ifc_pointer_declarator_sort field = get_ifc_sort_an_ifc_syntax_pointer_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "sort: %s\n", str_for_an_ifc_pointer_declarator_sort(field));
  }  /* if */
  if (has_ifc_whole_an_ifc_syntax_pointer_declarator(universal)) {
    an_ifc_syntax_index field = get_ifc_whole_an_ifc_syntax_pointer_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "whole:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_pointer_declarator(const an_ifc_syntax_pointer_declarator universal)
/*
Given the universal representation of SyntaxPointerDeclarator, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "=========================== ");
  fprintf(f_debug, "SyntaxPointerDeclarator ");
  fprintf(f_debug, "============================\n");
  db_node_an_ifc_syntax_pointer_declarator_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_range_based_for_statement_indent(const an_ifc_syntax_range_based_for_statement universal,
             unsigned                                      indent)
/*
Given the universal representation of SyntaxRangeBasedForStatement, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_body_an_ifc_syntax_range_based_for_statement(universal)) {
    an_ifc_syntax_index field = get_ifc_body_an_ifc_syntax_range_based_for_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "body:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_colon_an_ifc_syntax_range_based_for_statement(universal)) {
    an_ifc_source_location field = get_ifc_colon_an_ifc_syntax_range_based_for_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "colon:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_decl_an_ifc_syntax_range_based_for_statement(universal)) {
    an_ifc_syntax_index field = get_ifc_decl_an_ifc_syntax_range_based_for_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "decl:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_for_an_ifc_syntax_range_based_for_statement(universal)) {
    an_ifc_source_location field = get_ifc_for_an_ifc_syntax_range_based_for_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "for:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_init_an_ifc_syntax_range_based_for_statement(universal)) {
    an_ifc_syntax_index field = get_ifc_init_an_ifc_syntax_range_based_for_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "init:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_initializer_an_ifc_syntax_range_based_for_statement(universal)) {
    an_ifc_syntax_index field = get_ifc_initializer_an_ifc_syntax_range_based_for_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "initializer:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_left_paren_an_ifc_syntax_range_based_for_statement(universal)) {
    an_ifc_source_location field = get_ifc_left_paren_an_ifc_syntax_range_based_for_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "left_paren:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_pragma_an_ifc_syntax_range_based_for_statement(universal)) {
    an_ifc_sentence_index field = get_ifc_pragma_an_ifc_syntax_range_based_for_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "pragma: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_right_paren_an_ifc_syntax_range_based_for_statement(universal)) {
    an_ifc_source_location field = get_ifc_right_paren_an_ifc_syntax_range_based_for_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "right_paren:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_range_based_for_statement(const an_ifc_syntax_range_based_for_statement universal)
/*
Given the universal representation of SyntaxRangeBasedForStatement, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================= ");
  fprintf(f_debug, "SyntaxRangeBasedForStatement ");
  fprintf(f_debug, "=========================\n");
  db_node_an_ifc_syntax_range_based_for_statement_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_requirement_body_indent(const an_ifc_syntax_requirement_body universal, unsigned indent)
/*
Given the universal representation of SyntaxRequirementBody, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_syntax_requirement_body(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_requirement_body(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_requirements_an_ifc_syntax_requirement_body(universal)) {
    an_ifc_syntax_index field = get_ifc_requirements_an_ifc_syntax_requirement_body(universal);

    db_print_indent(indent);
    fprintf(f_debug, "requirements:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_right_curly_an_ifc_syntax_requirement_body(universal)) {
    an_ifc_source_location field = get_ifc_right_curly_an_ifc_syntax_requirement_body(universal);

    db_print_indent(indent);
    fprintf(f_debug, "right_curly:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_requirement_body(const an_ifc_syntax_requirement_body universal)
/*
Given the universal representation of SyntaxRequirementBody, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================ ");
  fprintf(f_debug, "SyntaxRequirementBody ");
  fprintf(f_debug, "=============================\n");
  db_node_an_ifc_syntax_requirement_body_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_requires_clause_indent(const an_ifc_syntax_requires_clause universal, unsigned indent)
/*
Given the universal representation of SyntaxRequiresClause, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_condition_an_ifc_syntax_requires_clause(universal)) {
    an_ifc_expr_index field = get_ifc_condition_an_ifc_syntax_requires_clause(universal);

    db_print_indent(indent);
    fprintf(f_debug, "condition:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_requires_clause(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_requires_clause(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_requires_clause(const an_ifc_syntax_requires_clause universal)
/*
Given the universal representation of SyntaxRequiresClause, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================= ");
  fprintf(f_debug, "SyntaxRequiresClause ");
  fprintf(f_debug, "=============================\n");
  db_node_an_ifc_syntax_requires_clause_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_return_statement_indent(const an_ifc_syntax_return_statement universal, unsigned indent)
/*
Given the universal representation of SyntaxReturnStatement, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_expr_an_ifc_syntax_return_statement(universal)) {
    an_ifc_expr_index field = get_ifc_expr_an_ifc_syntax_return_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "expr:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_pragma_an_ifc_syntax_return_statement(universal)) {
    an_ifc_sentence_index field = get_ifc_pragma_an_ifc_syntax_return_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "pragma: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_return_an_ifc_syntax_return_statement(universal)) {
    an_ifc_source_location field = get_ifc_return_an_ifc_syntax_return_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "return:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_semicolon_an_ifc_syntax_return_statement(universal)) {
    an_ifc_source_location field = get_ifc_semicolon_an_ifc_syntax_return_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "semicolon:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_sort_an_ifc_syntax_return_statement(universal)) {
    an_ifc_return_sort field = get_ifc_sort_an_ifc_syntax_return_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "sort: %s\n", str_for_an_ifc_return_sort(field));
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_return_statement(const an_ifc_syntax_return_statement universal)
/*
Given the universal representation of SyntaxReturnStatement, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================ ");
  fprintf(f_debug, "SyntaxReturnStatement ");
  fprintf(f_debug, "=============================\n");
  db_node_an_ifc_syntax_return_statement_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_seh_except_indent(const an_ifc_syntax_seh_except universal, unsigned indent)
/*
Given the universal representation of SyntaxSEHExcept, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_body_an_ifc_syntax_seh_except(universal)) {
    an_ifc_syntax_index field = get_ifc_body_an_ifc_syntax_seh_except(universal);

    db_print_indent(indent);
    fprintf(f_debug, "body:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_condition_an_ifc_syntax_seh_except(universal)) {
    an_ifc_expr_index field = get_ifc_condition_an_ifc_syntax_seh_except(universal);

    db_print_indent(indent);
    fprintf(f_debug, "condition:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_except_kw_an_ifc_syntax_seh_except(universal)) {
    an_ifc_source_location field = get_ifc_except_kw_an_ifc_syntax_seh_except(universal);

    db_print_indent(indent);
    fprintf(f_debug, "except_kw:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_left_paren_an_ifc_syntax_seh_except(universal)) {
    an_ifc_source_location field = get_ifc_left_paren_an_ifc_syntax_seh_except(universal);

    db_print_indent(indent);
    fprintf(f_debug, "left_paren:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_right_paren_an_ifc_syntax_seh_except(universal)) {
    an_ifc_source_location field = get_ifc_right_paren_an_ifc_syntax_seh_except(universal);

    db_print_indent(indent);
    fprintf(f_debug, "right_paren:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_seh_except(const an_ifc_syntax_seh_except universal)
/*
Given the universal representation of SyntaxSEHExcept, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "=============================== ");
  fprintf(f_debug, "SyntaxSEHExcept ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_syntax_seh_except_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_seh_finally_indent(const an_ifc_syntax_seh_finally universal, unsigned indent)
/*
Given the universal representation of SyntaxSEHFinally, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_body_an_ifc_syntax_seh_finally(universal)) {
    an_ifc_syntax_index field = get_ifc_body_an_ifc_syntax_seh_finally(universal);

    db_print_indent(indent);
    fprintf(f_debug, "body:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_finally_kw_an_ifc_syntax_seh_finally(universal)) {
    an_ifc_source_location field = get_ifc_finally_kw_an_ifc_syntax_seh_finally(universal);

    db_print_indent(indent);
    fprintf(f_debug, "finally_kw:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_seh_finally(const an_ifc_syntax_seh_finally universal)
/*
Given the universal representation of SyntaxSEHFinally, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "=============================== ");
  fprintf(f_debug, "SyntaxSEHFinally ");
  fprintf(f_debug, "===============================\n");
  db_node_an_ifc_syntax_seh_finally_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_seh_leave_indent(const an_ifc_syntax_seh_leave universal, unsigned indent)
/*
Given the universal representation of SyntaxSEHLeave, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_leave_kw_an_ifc_syntax_seh_leave(universal)) {
    an_ifc_source_location field = get_ifc_leave_kw_an_ifc_syntax_seh_leave(universal);

    db_print_indent(indent);
    fprintf(f_debug, "leave_kw:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_semicolon_an_ifc_syntax_seh_leave(universal)) {
    an_ifc_source_location field = get_ifc_semicolon_an_ifc_syntax_seh_leave(universal);

    db_print_indent(indent);
    fprintf(f_debug, "semicolon:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_seh_leave(const an_ifc_syntax_seh_leave universal)
/*
Given the universal representation of SyntaxSEHLeave, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "SyntaxSEHLeave ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_syntax_seh_leave_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_seh_try_indent(const an_ifc_syntax_seh_try universal, unsigned indent)
/*
Given the universal representation of SyntaxSEHTry, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_body_an_ifc_syntax_seh_try(universal)) {
    an_ifc_syntax_index field = get_ifc_body_an_ifc_syntax_seh_try(universal);

    db_print_indent(indent);
    fprintf(f_debug, "body:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_handler_an_ifc_syntax_seh_try(universal)) {
    an_ifc_syntax_index field = get_ifc_handler_an_ifc_syntax_seh_try(universal);

    db_print_indent(indent);
    fprintf(f_debug, "handler:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_try_kw_an_ifc_syntax_seh_try(universal)) {
    an_ifc_source_location field = get_ifc_try_kw_an_ifc_syntax_seh_try(universal);

    db_print_indent(indent);
    fprintf(f_debug, "try_kw:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_seh_try(const an_ifc_syntax_seh_try universal)
/*
Given the universal representation of SyntaxSEHTry, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "SyntaxSEHTry ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_syntax_seh_try_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_simple_capture_indent(const an_ifc_syntax_simple_capture universal, unsigned indent)
/*
Given the universal representation of SyntaxSimpleCapture, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_ampersand_an_ifc_syntax_simple_capture(universal)) {
    an_ifc_source_location field = get_ifc_ampersand_an_ifc_syntax_simple_capture(universal);

    db_print_indent(indent);
    fprintf(f_debug, "ampersand:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_comma_an_ifc_syntax_simple_capture(universal)) {
    an_ifc_source_location field = get_ifc_comma_an_ifc_syntax_simple_capture(universal);

    db_print_indent(indent);
    fprintf(f_debug, "comma:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_expander_an_ifc_syntax_simple_capture(universal)) {
    an_ifc_source_location field = get_ifc_expander_an_ifc_syntax_simple_capture(universal);

    db_print_indent(indent);
    fprintf(f_debug, "expander:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_syntax_simple_capture(universal)) {
    an_ifc_expr_index field = get_ifc_name_an_ifc_syntax_simple_capture(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_simple_capture(const an_ifc_syntax_simple_capture universal)
/*
Given the universal representation of SyntaxSimpleCapture, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================= ");
  fprintf(f_debug, "SyntaxSimpleCapture ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_syntax_simple_capture_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_simple_declaration_indent(const an_ifc_syntax_simple_declaration universal,
             unsigned                               indent)
/*
Given the universal representation of SyntaxSimpleDeclaration, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_decl_specifiers_an_ifc_syntax_simple_declaration(universal)) {
    an_ifc_syntax_index field = get_ifc_decl_specifiers_an_ifc_syntax_simple_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "decl_specifiers:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_declarators_an_ifc_syntax_simple_declaration(universal)) {
    an_ifc_syntax_index field = get_ifc_declarators_an_ifc_syntax_simple_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "declarators:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_simple_declaration(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_simple_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_semicolon_an_ifc_syntax_simple_declaration(universal)) {
    an_ifc_source_location field = get_ifc_semicolon_an_ifc_syntax_simple_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "semicolon:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_simple_declaration(const an_ifc_syntax_simple_declaration universal)
/*
Given the universal representation of SyntaxSimpleDeclaration, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "=========================== ");
  fprintf(f_debug, "SyntaxSimpleDeclaration ");
  fprintf(f_debug, "============================\n");
  db_node_an_ifc_syntax_simple_declaration_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_simple_requirement_indent(const an_ifc_syntax_simple_requirement universal,
             unsigned                               indent)
/*
Given the universal representation of SyntaxSimpleRequirement, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_condition_an_ifc_syntax_simple_requirement(universal)) {
    an_ifc_expr_index field = get_ifc_condition_an_ifc_syntax_simple_requirement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "condition:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_simple_requirement(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_simple_requirement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_simple_requirement(const an_ifc_syntax_simple_requirement universal)
/*
Given the universal representation of SyntaxSimpleRequirement, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "=========================== ");
  fprintf(f_debug, "SyntaxSimpleRequirement ");
  fprintf(f_debug, "============================\n");
  db_node_an_ifc_syntax_simple_requirement_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_simple_type_specifier_indent(const an_ifc_syntax_simple_type_specifier universal,
             unsigned                                  indent)
/*
Given the universal representation of SyntaxSimpleTypeSpecifier, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_expr_an_ifc_syntax_simple_type_specifier(universal)) {
    an_ifc_expr_index field = get_ifc_expr_an_ifc_syntax_simple_type_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "expr:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_simple_type_specifier(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_simple_type_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_type_an_ifc_syntax_simple_type_specifier(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_syntax_simple_type_specifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_simple_type_specifier(const an_ifc_syntax_simple_type_specifier universal)
/*
Given the universal representation of SyntaxSimpleTypeSpecifier, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================== ");
  fprintf(f_debug, "SyntaxSimpleTypeSpecifier ");
  fprintf(f_debug, "===========================\n");
  db_node_an_ifc_syntax_simple_type_specifier_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_statement_seq_indent(const an_ifc_syntax_statement_seq universal, unsigned indent)
/*
Given the universal representation of SyntaxStatementSeq, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_stmts_an_ifc_syntax_statement_seq(universal)) {
    an_ifc_syntax_index field = get_ifc_stmts_an_ifc_syntax_statement_seq(universal);

    db_print_indent(indent);
    fprintf(f_debug, "stmts:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_statement_seq(const an_ifc_syntax_statement_seq universal)
/*
Given the universal representation of SyntaxStatementSeq, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================== ");
  fprintf(f_debug, "SyntaxStatementSeq ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_syntax_statement_seq_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_static_assert_declaration_indent(const an_ifc_syntax_static_assert_declaration universal,
             unsigned                                      indent)
/*
Given the universal representation of SyntaxStaticAssertDeclaration, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_comma_an_ifc_syntax_static_assert_declaration(universal)) {
    an_ifc_source_location field = get_ifc_comma_an_ifc_syntax_static_assert_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "comma:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_condition_an_ifc_syntax_static_assert_declaration(universal)) {
    an_ifc_expr_index field = get_ifc_condition_an_ifc_syntax_static_assert_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "condition:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_left_paren_an_ifc_syntax_static_assert_declaration(universal)) {
    an_ifc_source_location field = get_ifc_left_paren_an_ifc_syntax_static_assert_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "left_paren:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_static_assert_declaration(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_static_assert_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_message_an_ifc_syntax_static_assert_declaration(universal)) {
    an_ifc_expr_index field = get_ifc_message_an_ifc_syntax_static_assert_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "message:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_right_paren_an_ifc_syntax_static_assert_declaration(universal)) {
    an_ifc_source_location field = get_ifc_right_paren_an_ifc_syntax_static_assert_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "right_paren:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_semicolon_an_ifc_syntax_static_assert_declaration(universal)) {
    an_ifc_source_location field = get_ifc_semicolon_an_ifc_syntax_static_assert_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "semicolon:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_static_assert_declaration(const an_ifc_syntax_static_assert_declaration universal)
/*
Given the universal representation of SyntaxStaticAssertDeclaration, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "======================== ");
  fprintf(f_debug, "SyntaxStaticAssertDeclaration ");
  fprintf(f_debug, "=========================\n");
  db_node_an_ifc_syntax_static_assert_declaration_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_structured_binding_declaration_indent(const an_ifc_syntax_structured_binding_declaration universal,
             unsigned                                           indent)
/*
Given the universal representation of SyntaxStructuredBindingDeclaration, print
a diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_initializer_an_ifc_syntax_structured_binding_declaration(universal)) {
    an_ifc_expr_index field = get_ifc_initializer_an_ifc_syntax_structured_binding_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "initializer:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_structured_binding_declaration(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_structured_binding_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_names_an_ifc_syntax_structured_binding_declaration(universal)) {
    an_ifc_syntax_index field = get_ifc_names_an_ifc_syntax_structured_binding_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "names:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_ref_an_ifc_syntax_structured_binding_declaration(universal)) {
    an_ifc_source_location field = get_ifc_ref_an_ifc_syntax_structured_binding_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "ref:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_specifiers_an_ifc_syntax_structured_binding_declaration(universal)) {
    an_ifc_syntax_index field = get_ifc_specifiers_an_ifc_syntax_structured_binding_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "specifiers:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_structured_binding_declaration(const an_ifc_syntax_structured_binding_declaration universal)
/*
Given the universal representation of SyntaxStructuredBindingDeclaration, print
a diagnostic textual representation.
*/
{
  fprintf(f_debug, "====================== ");
  fprintf(f_debug, "SyntaxStructuredBindingDeclaration ");
  fprintf(f_debug, "======================\n");
  db_node_an_ifc_syntax_structured_binding_declaration_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_structured_binding_identifier_indent(const an_ifc_syntax_structured_binding_identifier universal,
             unsigned                                          indent)
/*
Given the universal representation of SyntaxStructuredBindingIdentifier, print
a diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_comma_an_ifc_syntax_structured_binding_identifier(universal)) {
    an_ifc_source_location field = get_ifc_comma_an_ifc_syntax_structured_binding_identifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "comma:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_syntax_structured_binding_identifier(universal)) {
    an_ifc_expr_index field = get_ifc_name_an_ifc_syntax_structured_binding_identifier(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_structured_binding_identifier(const an_ifc_syntax_structured_binding_identifier universal)
/*
Given the universal representation of SyntaxStructuredBindingIdentifier, print
a diagnostic textual representation.
*/
{
  fprintf(f_debug, "====================== ");
  fprintf(f_debug, "SyntaxStructuredBindingIdentifier ");
  fprintf(f_debug, "=======================\n");
  db_node_an_ifc_syntax_structured_binding_identifier_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_super_indent(const an_ifc_syntax_super universal, unsigned indent)
/*
Given the universal representation of SyntaxSuper, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_syntax_super(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_super(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_super(const an_ifc_syntax_super universal)
/*
Given the universal representation of SyntaxSuper, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "SyntaxSuper ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_syntax_super_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_switch_statement_indent(const an_ifc_syntax_switch_statement universal, unsigned indent)
/*
Given the universal representation of SyntaxSwitchStatement, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_body_an_ifc_syntax_switch_statement(universal)) {
    an_ifc_syntax_index field = get_ifc_body_an_ifc_syntax_switch_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "body:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_condition_an_ifc_syntax_switch_statement(universal)) {
    an_ifc_syntax_index field = get_ifc_condition_an_ifc_syntax_switch_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "condition:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_init_an_ifc_syntax_switch_statement(universal)) {
    an_ifc_syntax_index field = get_ifc_init_an_ifc_syntax_switch_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "init:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_pragma_an_ifc_syntax_switch_statement(universal)) {
    an_ifc_sentence_index field = get_ifc_pragma_an_ifc_syntax_switch_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "pragma: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_switch_an_ifc_syntax_switch_statement(universal)) {
    an_ifc_source_location field = get_ifc_switch_an_ifc_syntax_switch_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "switch:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_switch_statement(const an_ifc_syntax_switch_statement universal)
/*
Given the universal representation of SyntaxSwitchStatement, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================ ");
  fprintf(f_debug, "SyntaxSwitchStatement ");
  fprintf(f_debug, "=============================\n");
  db_node_an_ifc_syntax_switch_statement_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_template_argument_list_indent(const an_ifc_syntax_template_argument_list universal,
             unsigned                                   indent)
/*
Given the universal representation of SyntaxTemplateArgumentList, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_arguments_an_ifc_syntax_template_argument_list(universal)) {
    an_ifc_syntax_index field = get_ifc_arguments_an_ifc_syntax_template_argument_list(universal);

    db_print_indent(indent);
    fprintf(f_debug, "arguments:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_left_angle_an_ifc_syntax_template_argument_list(universal)) {
    an_ifc_source_location field = get_ifc_left_angle_an_ifc_syntax_template_argument_list(universal);

    db_print_indent(indent);
    fprintf(f_debug, "left_angle:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_right_angle_an_ifc_syntax_template_argument_list(universal)) {
    an_ifc_source_location field = get_ifc_right_angle_an_ifc_syntax_template_argument_list(universal);

    db_print_indent(indent);
    fprintf(f_debug, "right_angle:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_template_argument_list(const an_ifc_syntax_template_argument_list universal)
/*
Given the universal representation of SyntaxTemplateArgumentList, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================== ");
  fprintf(f_debug, "SyntaxTemplateArgumentList ");
  fprintf(f_debug, "==========================\n");
  db_node_an_ifc_syntax_template_argument_list_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_template_declaration_indent(const an_ifc_syntax_template_declaration universal,
             unsigned                                 indent)
/*
Given the universal representation of SyntaxTemplateDeclaration, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_syntax_template_declaration(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_template_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_parameters_an_ifc_syntax_template_declaration(universal)) {
    an_ifc_syntax_index field = get_ifc_parameters_an_ifc_syntax_template_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "parameters:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_subject_an_ifc_syntax_template_declaration(universal)) {
    an_ifc_syntax_index field = get_ifc_subject_an_ifc_syntax_template_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "subject:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_template_declaration(const an_ifc_syntax_template_declaration universal)
/*
Given the universal representation of SyntaxTemplateDeclaration, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================== ");
  fprintf(f_debug, "SyntaxTemplateDeclaration ");
  fprintf(f_debug, "===========================\n");
  db_node_an_ifc_syntax_template_declaration_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_template_id_indent(const an_ifc_syntax_template_id universal, unsigned indent)
/*
Given the universal representation of SyntaxTemplateId, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_arguments_an_ifc_syntax_template_id(universal)) {
    an_ifc_syntax_index field = get_ifc_arguments_an_ifc_syntax_template_id(universal);

    db_print_indent(indent);
    fprintf(f_debug, "arguments:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_template_id(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_template_id(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_syntax_template_id(universal)) {
    an_ifc_syntax_index field = get_ifc_name_an_ifc_syntax_template_id(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_symbol_an_ifc_syntax_template_id(universal)) {
    an_ifc_expr_index field = get_ifc_symbol_an_ifc_syntax_template_id(universal);

    db_print_indent(indent);
    fprintf(f_debug, "symbol:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_template_kw_an_ifc_syntax_template_id(universal)) {
    an_ifc_source_location field = get_ifc_template_kw_an_ifc_syntax_template_id(universal);

    db_print_indent(indent);
    fprintf(f_debug, "template_kw:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_template_id(const an_ifc_syntax_template_id universal)
/*
Given the universal representation of SyntaxTemplateId, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "=============================== ");
  fprintf(f_debug, "SyntaxTemplateId ");
  fprintf(f_debug, "===============================\n");
  db_node_an_ifc_syntax_template_id_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_template_parameter_list_indent(const an_ifc_syntax_template_parameter_list universal,
             unsigned                                    indent)
/*
Given the universal representation of SyntaxTemplateParameterList, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_clause_an_ifc_syntax_template_parameter_list(universal)) {
    an_ifc_syntax_index field = get_ifc_clause_an_ifc_syntax_template_parameter_list(universal);

    db_print_indent(indent);
    fprintf(f_debug, "clause:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_left_angle_an_ifc_syntax_template_parameter_list(universal)) {
    an_ifc_source_location field = get_ifc_left_angle_an_ifc_syntax_template_parameter_list(universal);

    db_print_indent(indent);
    fprintf(f_debug, "left_angle:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_parameters_an_ifc_syntax_template_parameter_list(universal)) {
    an_ifc_syntax_index field = get_ifc_parameters_an_ifc_syntax_template_parameter_list(universal);

    db_print_indent(indent);
    fprintf(f_debug, "parameters:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_right_angle_an_ifc_syntax_template_parameter_list(universal)) {
    an_ifc_source_location field = get_ifc_right_angle_an_ifc_syntax_template_parameter_list(universal);

    db_print_indent(indent);
    fprintf(f_debug, "right_angle:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_template_parameter_list(const an_ifc_syntax_template_parameter_list universal)
/*
Given the universal representation of SyntaxTemplateParameterList, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================= ");
  fprintf(f_debug, "SyntaxTemplateParameterList ");
  fprintf(f_debug, "==========================\n");
  db_node_an_ifc_syntax_template_parameter_list_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_template_template_parameter_indent(const an_ifc_syntax_template_template_parameter universal,
             unsigned                                        indent)
/*
Given the universal representation of SyntaxTemplateTemplateParameter, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_argument_an_ifc_syntax_template_template_parameter(universal)) {
    an_ifc_syntax_index field = get_ifc_argument_an_ifc_syntax_template_template_parameter(universal);

    db_print_indent(indent);
    fprintf(f_debug, "argument:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_comma_an_ifc_syntax_template_template_parameter(universal)) {
    an_ifc_source_location field = get_ifc_comma_an_ifc_syntax_template_template_parameter(universal);

    db_print_indent(indent);
    fprintf(f_debug, "comma:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_ellipsis_an_ifc_syntax_template_template_parameter(universal)) {
    an_ifc_source_location field = get_ifc_ellipsis_an_ifc_syntax_template_template_parameter(universal);

    db_print_indent(indent);
    fprintf(f_debug, "ellipsis:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_key_an_ifc_syntax_template_template_parameter(universal)) {
    an_ifc_keyword_syntax field = get_ifc_key_an_ifc_syntax_template_template_parameter(universal);

    db_print_indent(indent);
    fprintf(f_debug, "key:\n");
    db_node_an_ifc_keyword_syntax_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_template_template_parameter(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_template_template_parameter(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_syntax_template_template_parameter(universal)) {
    an_ifc_text_offset field = get_ifc_name_an_ifc_syntax_template_template_parameter(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_parameters_an_ifc_syntax_template_template_parameter(universal)) {
    an_ifc_syntax_index field = get_ifc_parameters_an_ifc_syntax_template_template_parameter(universal);

    db_print_indent(indent);
    fprintf(f_debug, "parameters:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_template_template_parameter(const an_ifc_syntax_template_template_parameter universal)
/*
Given the universal representation of SyntaxTemplateTemplateParameter, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "======================= ");
  fprintf(f_debug, "SyntaxTemplateTemplateParameter ");
  fprintf(f_debug, "========================\n");
  db_node_an_ifc_syntax_template_template_parameter_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_this_capture_indent(const an_ifc_syntax_this_capture universal, unsigned indent)
/*
Given the universal representation of SyntaxThisCapture, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_asterisk_an_ifc_syntax_this_capture(universal)) {
    an_ifc_source_location field = get_ifc_asterisk_an_ifc_syntax_this_capture(universal);

    db_print_indent(indent);
    fprintf(f_debug, "asterisk:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_comma_an_ifc_syntax_this_capture(universal)) {
    an_ifc_source_location field = get_ifc_comma_an_ifc_syntax_this_capture(universal);

    db_print_indent(indent);
    fprintf(f_debug, "comma:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_this_capture(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_this_capture(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_this_capture(const an_ifc_syntax_this_capture universal)
/*
Given the universal representation of SyntaxThisCapture, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================== ");
  fprintf(f_debug, "SyntaxThisCapture ");
  fprintf(f_debug, "===============================\n");
  db_node_an_ifc_syntax_this_capture_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_trailing_return_type_indent(const an_ifc_syntax_trailing_return_type universal,
             unsigned                                 indent)
/*
Given the universal representation of SyntaxTrailingReturnType, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_arrow_an_ifc_syntax_trailing_return_type(universal)) {
    an_ifc_source_location field = get_ifc_arrow_an_ifc_syntax_trailing_return_type(universal);

    db_print_indent(indent);
    fprintf(f_debug, "arrow:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_target_an_ifc_syntax_trailing_return_type(universal)) {
    an_ifc_syntax_index field = get_ifc_target_an_ifc_syntax_trailing_return_type(universal);

    db_print_indent(indent);
    fprintf(f_debug, "target:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_trailing_return_type(const an_ifc_syntax_trailing_return_type universal)
/*
Given the universal representation of SyntaxTrailingReturnType, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "=========================== ");
  fprintf(f_debug, "SyntaxTrailingReturnType ");
  fprintf(f_debug, "===========================\n");
  db_node_an_ifc_syntax_trailing_return_type_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_try_block_indent(const an_ifc_syntax_try_block universal, unsigned indent)
/*
Given the universal representation of SyntaxTryBlock, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_body_an_ifc_syntax_try_block(universal)) {
    an_ifc_syntax_index field = get_ifc_body_an_ifc_syntax_try_block(universal);

    db_print_indent(indent);
    fprintf(f_debug, "body:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_handlers_an_ifc_syntax_try_block(universal)) {
    an_ifc_syntax_index field = get_ifc_handlers_an_ifc_syntax_try_block(universal);

    db_print_indent(indent);
    fprintf(f_debug, "handlers:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_pragma_an_ifc_syntax_try_block(universal)) {
    an_ifc_sentence_index field = get_ifc_pragma_an_ifc_syntax_try_block(universal);

    db_print_indent(indent);
    fprintf(f_debug, "pragma: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_try_an_ifc_syntax_try_block(universal)) {
    an_ifc_source_location field = get_ifc_try_an_ifc_syntax_try_block(universal);

    db_print_indent(indent);
    fprintf(f_debug, "try:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_try_block(const an_ifc_syntax_try_block universal)
/*
Given the universal representation of SyntaxTryBlock, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "SyntaxTryBlock ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_syntax_try_block_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_tuple_indent(const an_ifc_syntax_tuple universal, unsigned indent)
/*
Given the universal representation of SyntaxTuple, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_cardinality_an_ifc_syntax_tuple(universal)) {
    an_ifc_cardinality field = get_ifc_cardinality_an_ifc_syntax_tuple(universal);

    db_print_indent(indent);
    fprintf(f_debug, "cardinality: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_start_an_ifc_syntax_tuple(universal)) {
    an_ifc_index field = get_ifc_start_an_ifc_syntax_tuple(universal);

    db_print_indent(indent);
    fprintf(f_debug, "start: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_tuple(const an_ifc_syntax_tuple universal)
/*
Given the universal representation of SyntaxTuple, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "SyntaxTuple ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_syntax_tuple_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_type_id_indent(const an_ifc_syntax_type_id universal, unsigned indent)
/*
Given the universal representation of SyntaxTypeId, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_abstract_declarator_an_ifc_syntax_type_id(universal)) {
    an_ifc_syntax_index field = get_ifc_abstract_declarator_an_ifc_syntax_type_id(universal);

    db_print_indent(indent);
    fprintf(f_debug, "abstract_declarator:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_type_id(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_type_id(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_type_specifier_an_ifc_syntax_type_id(universal)) {
    an_ifc_syntax_index field = get_ifc_type_specifier_an_ifc_syntax_type_id(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type_specifier:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_type_id(const an_ifc_syntax_type_id universal)
/*
Given the universal representation of SyntaxTypeId, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "SyntaxTypeId ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_syntax_type_id_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_type_id_list_element_indent(const an_ifc_syntax_type_id_list_element universal,
             unsigned                                 indent)
/*
Given the universal representation of SyntaxTypeIdListElement, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_ellipsis_an_ifc_syntax_type_id_list_element(universal)) {
    an_ifc_source_location field = get_ifc_ellipsis_an_ifc_syntax_type_id_list_element(universal);

    db_print_indent(indent);
    fprintf(f_debug, "ellipsis:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_type_id_an_ifc_syntax_type_id_list_element(universal)) {
    an_ifc_syntax_index field = get_ifc_type_id_an_ifc_syntax_type_id_list_element(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type_id:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_type_id_list_element(const an_ifc_syntax_type_id_list_element universal)
/*
Given the universal representation of SyntaxTypeIdListElement, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "=========================== ");
  fprintf(f_debug, "SyntaxTypeIdListElement ");
  fprintf(f_debug, "============================\n");
  db_node_an_ifc_syntax_type_id_list_element_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_type_requirement_indent(const an_ifc_syntax_type_requirement universal, unsigned indent)
/*
Given the universal representation of SyntaxTypeRequirement, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_syntax_type_requirement(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_type_requirement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_type_an_ifc_syntax_type_requirement(universal)) {
    an_ifc_expr_index field = get_ifc_type_an_ifc_syntax_type_requirement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_type_requirement(const an_ifc_syntax_type_requirement universal)
/*
Given the universal representation of SyntaxTypeRequirement, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================ ");
  fprintf(f_debug, "SyntaxTypeRequirement ");
  fprintf(f_debug, "=============================\n");
  db_node_an_ifc_syntax_type_requirement_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_type_specifier_seq_indent(const an_ifc_syntax_type_specifier_seq universal,
             unsigned                               indent)
/*
Given the universal representation of SyntaxTypeSpecifierSeq, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_locus_an_ifc_syntax_type_specifier_seq(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_type_specifier_seq(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_qualifiers_an_ifc_syntax_type_specifier_seq(universal)) {
    an_ifc_qualifier_bitfield field = get_ifc_qualifiers_an_ifc_syntax_type_specifier_seq(universal);

    fprintf(f_debug, "qualifiers:\n");
    ++indent;
    if (test_bitmask_ifc_qb_const(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Const\n");
    }  /* if */
    if (test_bitmask_ifc_qb_none(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- None\n");
    }  /* if */
    if (test_bitmask_ifc_qb_restrict(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Restrict\n");
    }  /* if */
    if (test_bitmask_ifc_qb_volatile(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Volatile\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_type_an_ifc_syntax_type_specifier_seq(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_syntax_type_specifier_seq(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_type_name_an_ifc_syntax_type_specifier_seq(universal)) {
    an_ifc_syntax_index field = get_ifc_type_name_an_ifc_syntax_type_specifier_seq(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type_name:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_unhashed_an_ifc_syntax_type_specifier_seq(universal)) {
    an_ifc_bool field = get_ifc_unhashed_an_ifc_syntax_type_specifier_seq(universal);

    db_print_indent(indent);
    fprintf(f_debug, "unhashed: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_type_specifier_seq(const an_ifc_syntax_type_specifier_seq universal)
/*
Given the universal representation of SyntaxTypeSpecifierSeq, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "============================ ");
  fprintf(f_debug, "SyntaxTypeSpecifierSeq ");
  fprintf(f_debug, "============================\n");
  db_node_an_ifc_syntax_type_specifier_seq_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_type_template_argument_indent(const an_ifc_syntax_type_template_argument universal,
             unsigned                                   indent)
/*
Given the universal representation of SyntaxTypeTemplateArgument, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_argument_an_ifc_syntax_type_template_argument(universal)) {
    an_ifc_syntax_index field = get_ifc_argument_an_ifc_syntax_type_template_argument(universal);

    db_print_indent(indent);
    fprintf(f_debug, "argument:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_comma_an_ifc_syntax_type_template_argument(universal)) {
    an_ifc_source_location field = get_ifc_comma_an_ifc_syntax_type_template_argument(universal);

    db_print_indent(indent);
    fprintf(f_debug, "comma:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_ellipsis_an_ifc_syntax_type_template_argument(universal)) {
    an_ifc_source_location field = get_ifc_ellipsis_an_ifc_syntax_type_template_argument(universal);

    db_print_indent(indent);
    fprintf(f_debug, "ellipsis:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_type_template_argument(const an_ifc_syntax_type_template_argument universal)
/*
Given the universal representation of SyntaxTypeTemplateArgument, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================== ");
  fprintf(f_debug, "SyntaxTypeTemplateArgument ");
  fprintf(f_debug, "==========================\n");
  db_node_an_ifc_syntax_type_template_argument_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_type_template_parameter_indent(const an_ifc_syntax_type_template_parameter universal,
             unsigned                                    indent)
/*
Given the universal representation of SyntaxTypeTemplateParameter, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_argument_an_ifc_syntax_type_template_parameter(universal)) {
    an_ifc_syntax_index field = get_ifc_argument_an_ifc_syntax_type_template_parameter(universal);

    db_print_indent(indent);
    fprintf(f_debug, "argument:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_constraint_an_ifc_syntax_type_template_parameter(universal)) {
    an_ifc_syntax_index field = get_ifc_constraint_an_ifc_syntax_type_template_parameter(universal);

    db_print_indent(indent);
    fprintf(f_debug, "constraint:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_ellipsis_an_ifc_syntax_type_template_parameter(universal)) {
    an_ifc_source_location field = get_ifc_ellipsis_an_ifc_syntax_type_template_parameter(universal);

    db_print_indent(indent);
    fprintf(f_debug, "ellipsis:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_type_template_parameter(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_type_template_parameter(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_syntax_type_template_parameter(universal)) {
    an_ifc_text_offset field = get_ifc_name_an_ifc_syntax_type_template_parameter(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_type_template_parameter(const an_ifc_syntax_type_template_parameter universal)
/*
Given the universal representation of SyntaxTypeTemplateParameter, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================= ");
  fprintf(f_debug, "SyntaxTypeTemplateParameter ");
  fprintf(f_debug, "==========================\n");
  db_node_an_ifc_syntax_type_template_parameter_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_type_trait_intrinsic_indent(const an_ifc_syntax_type_trait_intrinsic universal,
             unsigned                                 indent)
/*
Given the universal representation of SyntaxTypeTraitIntrinsic, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_arguments_an_ifc_syntax_type_trait_intrinsic(universal)) {
    an_ifc_syntax_index field = get_ifc_arguments_an_ifc_syntax_type_trait_intrinsic(universal);

    db_print_indent(indent);
    fprintf(f_debug, "arguments:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_intrinsic_an_ifc_syntax_type_trait_intrinsic(universal)) {
    an_ifc_operator_category field = get_ifc_intrinsic_an_ifc_syntax_type_trait_intrinsic(universal);

    db_print_indent(indent);
    fprintf(f_debug, "intrinsic:\n");
    db_print_indent(indent);
    fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_operator_sort(field.sort));
    ++indent;
    switch (field.sort) {
      case ifc_os_dyadic_operator:
        { an_ifc_dyadic_operator_sort do_ref = field.variant.dyadic_operator;

          db_print_indent(indent);
          fprintf(f_debug, "dyadic_operator: %s\n", str_for_an_ifc_dyadic_operator_sort(do_ref));
        }
        break;
      case ifc_os_monadic_operator:
        { an_ifc_monadic_operator_sort mo_ref =
                                                field.variant.monadic_operator;

          db_print_indent(indent);
          fprintf(f_debug, "monadic_operator: %s\n", str_for_an_ifc_monadic_operator_sort(mo_ref));
        }
        break;
      case ifc_os_niladic_operator:
        { an_ifc_niladic_operator_sort no_ref =
                                                field.variant.niladic_operator;

          db_print_indent(indent);
          fprintf(f_debug, "niladic_operator: %s\n", str_for_an_ifc_niladic_operator_sort(no_ref));
        }
        break;
      case ifc_os_storage_instruction_operator:
        { an_ifc_storage_instruction_operator_sort sio_ref =
                                    field.variant.storage_instruction_operator;

          db_print_indent(indent);
          fprintf(f_debug, "storage_instruction_operator: %s\n",
                  str_for_an_ifc_storage_instruction_operator_sort(sio_ref));
        }
        break;
      case ifc_os_triadic_operator:
        { an_ifc_triadic_operator_sort to_ref =
                                                field.variant.triadic_operator;

          db_print_indent(indent);
          fprintf(f_debug, "triadic_operator: %s\n", str_for_an_ifc_triadic_operator_sort(to_ref));
        }
        break;
      case ifc_os_variadic_operator:
        { an_ifc_variadic_operator_sort vo_ref =
                                               field.variant.variadic_operator;

          db_print_indent(indent);
          fprintf(f_debug, "variadic_operator: %s\n", str_for_an_ifc_variadic_operator_sort(vo_ref));
        }
        break;
      default_is_unexpected();
    }  /* switch */
    --indent;
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_type_trait_intrinsic(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_type_trait_intrinsic(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_type_trait_intrinsic(const an_ifc_syntax_type_trait_intrinsic universal)
/*
Given the universal representation of SyntaxTypeTraitIntrinsic, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "=========================== ");
  fprintf(f_debug, "SyntaxTypeTraitIntrinsic ");
  fprintf(f_debug, "===========================\n");
  db_node_an_ifc_syntax_type_trait_intrinsic_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_unary_fold_expression_indent(const an_ifc_syntax_unary_fold_expression universal,
             unsigned                                  indent)
/*
Given the universal representation of SyntaxUnaryFoldExpression, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_direction_an_ifc_syntax_unary_fold_expression(universal)) {
    an_ifc_fold_direction_sort field = get_ifc_direction_an_ifc_syntax_unary_fold_expression(universal);

    db_print_indent(indent);
    fprintf(f_debug, "direction: %s\n", str_for_an_ifc_fold_direction_sort(field));
  }  /* if */
  if (has_ifc_dyad_an_ifc_syntax_unary_fold_expression(universal)) {
    an_ifc_dyadic_operator_sort field = get_ifc_dyad_an_ifc_syntax_unary_fold_expression(universal);

    db_print_indent(indent);
    fprintf(f_debug, "dyad: %s\n", str_for_an_ifc_dyadic_operator_sort(field));
  }  /* if */
  if (has_ifc_ellipsis_an_ifc_syntax_unary_fold_expression(universal)) {
    an_ifc_source_location field = get_ifc_ellipsis_an_ifc_syntax_unary_fold_expression(universal);

    db_print_indent(indent);
    fprintf(f_debug, "ellipsis:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_glyph_locus_an_ifc_syntax_unary_fold_expression(universal)) {
    an_ifc_source_location field = get_ifc_glyph_locus_an_ifc_syntax_unary_fold_expression(universal);

    db_print_indent(indent);
    fprintf(f_debug, "glyph_locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_unary_fold_expression(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_unary_fold_expression(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_operand_an_ifc_syntax_unary_fold_expression(universal)) {
    an_ifc_expr_index field = get_ifc_operand_an_ifc_syntax_unary_fold_expression(universal);

    db_print_indent(indent);
    fprintf(f_debug, "operand:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_right_paren_an_ifc_syntax_unary_fold_expression(universal)) {
    an_ifc_source_location field = get_ifc_right_paren_an_ifc_syntax_unary_fold_expression(universal);

    db_print_indent(indent);
    fprintf(f_debug, "right_paren:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_unary_fold_expression(const an_ifc_syntax_unary_fold_expression universal)
/*
Given the universal representation of SyntaxUnaryFoldExpression, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================== ");
  fprintf(f_debug, "SyntaxUnaryFoldExpression ");
  fprintf(f_debug, "===========================\n");
  db_node_an_ifc_syntax_unary_fold_expression_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_using_declaration_indent(const an_ifc_syntax_using_declaration universal, unsigned indent)
/*
Given the universal representation of SyntaxUsingDeclaration, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_declarators_an_ifc_syntax_using_declaration(universal)) {
    an_ifc_syntax_index field = get_ifc_declarators_an_ifc_syntax_using_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "declarators:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_keyword_an_ifc_syntax_using_declaration(universal)) {
    an_ifc_source_location field = get_ifc_keyword_an_ifc_syntax_using_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "keyword:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_semicolon_an_ifc_syntax_using_declaration(universal)) {
    an_ifc_source_location field = get_ifc_semicolon_an_ifc_syntax_using_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "semicolon:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_using_declaration(const an_ifc_syntax_using_declaration universal)
/*
Given the universal representation of SyntaxUsingDeclaration, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "============================ ");
  fprintf(f_debug, "SyntaxUsingDeclaration ");
  fprintf(f_debug, "============================\n");
  db_node_an_ifc_syntax_using_declaration_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_using_declarator_indent(const an_ifc_syntax_using_declarator universal, unsigned indent)
/*
Given the universal representation of SyntaxUsingDeclarator, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_comma_an_ifc_syntax_using_declarator(universal)) {
    an_ifc_source_location field = get_ifc_comma_an_ifc_syntax_using_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "comma:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_expander_an_ifc_syntax_using_declarator(universal)) {
    an_ifc_source_location field = get_ifc_expander_an_ifc_syntax_using_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "expander:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_qualified_name_an_ifc_syntax_using_declarator(universal)) {
    an_ifc_expr_index field = get_ifc_qualified_name_an_ifc_syntax_using_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "qualified_name:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_typename_kw_an_ifc_syntax_using_declarator(universal)) {
    an_ifc_source_location field = get_ifc_typename_kw_an_ifc_syntax_using_declarator(universal);

    db_print_indent(indent);
    fprintf(f_debug, "typename_kw:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_using_declarator(const an_ifc_syntax_using_declarator universal)
/*
Given the universal representation of SyntaxUsingDeclarator, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================ ");
  fprintf(f_debug, "SyntaxUsingDeclarator ");
  fprintf(f_debug, "=============================\n");
  db_node_an_ifc_syntax_using_declarator_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_using_directive_indent(const an_ifc_syntax_using_directive universal, unsigned indent)
/*
Given the universal representation of SyntaxUsingDirective, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_namespace_kw_an_ifc_syntax_using_directive(universal)) {
    an_ifc_source_location field = get_ifc_namespace_kw_an_ifc_syntax_using_directive(universal);

    db_print_indent(indent);
    fprintf(f_debug, "namespace_kw:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_qualified_name_an_ifc_syntax_using_directive(universal)) {
    an_ifc_expr_index field = get_ifc_qualified_name_an_ifc_syntax_using_directive(universal);

    db_print_indent(indent);
    fprintf(f_debug, "qualified_name:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_semicolon_an_ifc_syntax_using_directive(universal)) {
    an_ifc_source_location field = get_ifc_semicolon_an_ifc_syntax_using_directive(universal);

    db_print_indent(indent);
    fprintf(f_debug, "semicolon:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_using_kw_an_ifc_syntax_using_directive(universal)) {
    an_ifc_source_location field = get_ifc_using_kw_an_ifc_syntax_using_directive(universal);

    db_print_indent(indent);
    fprintf(f_debug, "using_kw:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_using_directive(const an_ifc_syntax_using_directive universal)
/*
Given the universal representation of SyntaxUsingDirective, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================= ");
  fprintf(f_debug, "SyntaxUsingDirective ");
  fprintf(f_debug, "=============================\n");
  db_node_an_ifc_syntax_using_directive_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_using_enum_declaration_indent(const an_ifc_syntax_using_enum_declaration universal,
             unsigned                                   indent)
/*
Given the universal representation of SyntaxUsingEnumDeclaration, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_enum_kw_an_ifc_syntax_using_enum_declaration(universal)) {
    an_ifc_source_location field = get_ifc_enum_kw_an_ifc_syntax_using_enum_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "enum_kw:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_name_an_ifc_syntax_using_enum_declaration(universal)) {
    an_ifc_expr_index field = get_ifc_name_an_ifc_syntax_using_enum_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "name:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_semicolon_an_ifc_syntax_using_enum_declaration(universal)) {
    an_ifc_source_location field = get_ifc_semicolon_an_ifc_syntax_using_enum_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "semicolon:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_using_kw_an_ifc_syntax_using_enum_declaration(universal)) {
    an_ifc_source_location field = get_ifc_using_kw_an_ifc_syntax_using_enum_declaration(universal);

    db_print_indent(indent);
    fprintf(f_debug, "using_kw:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_using_enum_declaration(const an_ifc_syntax_using_enum_declaration universal)
/*
Given the universal representation of SyntaxUsingEnumDeclaration, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================== ");
  fprintf(f_debug, "SyntaxUsingEnumDeclaration ");
  fprintf(f_debug, "==========================\n");
  db_node_an_ifc_syntax_using_enum_declaration_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_virtual_specifier_seq_indent(const an_ifc_syntax_virtual_specifier_seq universal,
             unsigned                                  indent)
/*
Given the universal representation of SyntaxVirtualSpecifierSeq, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_final_kw_an_ifc_syntax_virtual_specifier_seq(universal)) {
    an_ifc_source_location field = get_ifc_final_kw_an_ifc_syntax_virtual_specifier_seq(universal);

    db_print_indent(indent);
    fprintf(f_debug, "final_kw:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_locus_an_ifc_syntax_virtual_specifier_seq(universal)) {
    an_ifc_source_location field = get_ifc_locus_an_ifc_syntax_virtual_specifier_seq(universal);

    db_print_indent(indent);
    fprintf(f_debug, "locus:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_override_kw_an_ifc_syntax_virtual_specifier_seq(universal)) {
    an_ifc_source_location field = get_ifc_override_kw_an_ifc_syntax_virtual_specifier_seq(universal);

    db_print_indent(indent);
    fprintf(f_debug, "override_kw:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_pure_an_ifc_syntax_virtual_specifier_seq(universal)) {
    an_ifc_bool field = get_ifc_pure_an_ifc_syntax_virtual_specifier_seq(universal);

    db_print_indent(indent);
    fprintf(f_debug, "pure: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_virtual_specifier_seq(const an_ifc_syntax_virtual_specifier_seq universal)
/*
Given the universal representation of SyntaxVirtualSpecifierSeq, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "========================== ");
  fprintf(f_debug, "SyntaxVirtualSpecifierSeq ");
  fprintf(f_debug, "===========================\n");
  db_node_an_ifc_syntax_virtual_specifier_seq_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_syntax_while_statement_indent(const an_ifc_syntax_while_statement universal, unsigned indent)
/*
Given the universal representation of SyntaxWhileStatement, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_body_an_ifc_syntax_while_statement(universal)) {
    an_ifc_syntax_index field = get_ifc_body_an_ifc_syntax_while_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "body:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_condition_an_ifc_syntax_while_statement(universal)) {
    an_ifc_expr_index field = get_ifc_condition_an_ifc_syntax_while_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "condition:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_pragma_an_ifc_syntax_while_statement(universal)) {
    an_ifc_sentence_index field = get_ifc_pragma_an_ifc_syntax_while_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "pragma: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_while_an_ifc_syntax_while_statement(universal)) {
    an_ifc_source_location field = get_ifc_while_an_ifc_syntax_while_statement(universal);

    db_print_indent(indent);
    fprintf(f_debug, "while:\n");
    db_node_an_ifc_source_location_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_syntax_while_statement(const an_ifc_syntax_while_statement universal)
/*
Given the universal representation of SyntaxWhileStatement, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================= ");
  fprintf(f_debug, "SyntaxWhileStatement ");
  fprintf(f_debug, "=============================\n");
  db_node_an_ifc_syntax_while_statement_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_trait_alias_template_indent(const an_ifc_trait_alias_template universal, unsigned indent)
/*
Given the universal representation of TraitAliasTemplate, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_decl_an_ifc_trait_alias_template(universal)) {
    an_ifc_decl_index field = get_ifc_decl_an_ifc_trait_alias_template(universal);

    db_print_indent(indent);
    fprintf(f_debug, "decl:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_encoded_decl_an_ifc_trait_alias_template(universal)) {
    an_ifc_encoded_decl_index field = get_ifc_encoded_decl_an_ifc_trait_alias_template(universal);

    db_print_indent(indent);
    fprintf(f_debug, "encoded_decl: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_trait_an_ifc_trait_alias_template(universal)) {
    an_ifc_syntax_index field = get_ifc_trait_an_ifc_trait_alias_template(universal);

    db_print_indent(indent);
    fprintf(f_debug, "trait:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_trait_alias_template(const an_ifc_trait_alias_template universal)
/*
Given the universal representation of TraitAliasTemplate, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================== ");
  fprintf(f_debug, "TraitAliasTemplate ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_trait_alias_template_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_trait_attribute_indent(const an_ifc_trait_attribute universal, unsigned indent)
/*
Given the universal representation of TraitAttribute, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_decl_an_ifc_trait_attribute(universal)) {
    an_ifc_decl_index field = get_ifc_decl_an_ifc_trait_attribute(universal);

    db_print_indent(indent);
    fprintf(f_debug, "decl:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_encoded_decl_an_ifc_trait_attribute(universal)) {
    an_ifc_encoded_decl_index field = get_ifc_encoded_decl_an_ifc_trait_attribute(universal);

    db_print_indent(indent);
    fprintf(f_debug, "encoded_decl: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_trait_an_ifc_trait_attribute(universal)) {
    an_ifc_attr_index field = get_ifc_trait_an_ifc_trait_attribute(universal);

    db_print_indent(indent);
    fprintf(f_debug, "trait:");
    if (is_null_index_an_ifc_attr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_attr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_trait_attribute(const an_ifc_trait_attribute universal)
/*
Given the universal representation of TraitAttribute, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "TraitAttribute ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_trait_attribute_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_trait_deduction_guide_indent(const an_ifc_trait_deduction_guide universal, unsigned indent)
/*
Given the universal representation of TraitDeductionGuide, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_decl_an_ifc_trait_deduction_guide(universal)) {
    an_ifc_decl_index field = get_ifc_decl_an_ifc_trait_deduction_guide(universal);

    db_print_indent(indent);
    fprintf(f_debug, "decl:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_encoded_decl_an_ifc_trait_deduction_guide(universal)) {
    an_ifc_encoded_decl_index field = get_ifc_encoded_decl_an_ifc_trait_deduction_guide(universal);

    db_print_indent(indent);
    fprintf(f_debug, "encoded_decl: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_trait_an_ifc_trait_deduction_guide(universal)) {
    an_ifc_decl_index field = get_ifc_trait_an_ifc_trait_deduction_guide(universal);

    db_print_indent(indent);
    fprintf(f_debug, "trait:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_trait_deduction_guide(const an_ifc_trait_deduction_guide universal)
/*
Given the universal representation of TraitDeductionGuide, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================= ");
  fprintf(f_debug, "TraitDeductionGuide ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_trait_deduction_guide_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_trait_deprecated_indent(const an_ifc_trait_deprecated universal, unsigned indent)
/*
Given the universal representation of TraitDeprecated, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_decl_an_ifc_trait_deprecated(universal)) {
    an_ifc_decl_index field = get_ifc_decl_an_ifc_trait_deprecated(universal);

    db_print_indent(indent);
    fprintf(f_debug, "decl:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_encoded_decl_an_ifc_trait_deprecated(universal)) {
    an_ifc_encoded_decl_index field = get_ifc_encoded_decl_an_ifc_trait_deprecated(universal);

    db_print_indent(indent);
    fprintf(f_debug, "encoded_decl: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_trait_an_ifc_trait_deprecated(universal)) {
    an_ifc_text_offset field = get_ifc_trait_an_ifc_trait_deprecated(universal);

    db_print_indent(indent);
    fprintf(f_debug, "trait: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_trait_deprecated(const an_ifc_trait_deprecated universal)
/*
Given the universal representation of TraitDeprecated, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "=============================== ");
  fprintf(f_debug, "TraitDeprecated ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_trait_deprecated_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_trait_friend_indent(const an_ifc_trait_friend universal, unsigned indent)
/*
Given the universal representation of TraitFriend, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_decl_an_ifc_trait_friend(universal)) {
    an_ifc_decl_index field = get_ifc_decl_an_ifc_trait_friend(universal);

    db_print_indent(indent);
    fprintf(f_debug, "decl:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_encoded_decl_an_ifc_trait_friend(universal)) {
    an_ifc_encoded_decl_index field = get_ifc_encoded_decl_an_ifc_trait_friend(universal);

    db_print_indent(indent);
    fprintf(f_debug, "encoded_decl: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_trait_an_ifc_trait_friend(universal)) {
    an_ifc_sequence field = get_ifc_trait_an_ifc_trait_friend(universal);

    db_print_indent(indent);
    fprintf(f_debug, "trait:\n");
    db_node_an_ifc_sequence_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_trait_friend(const an_ifc_trait_friend universal)
/*
Given the universal representation of TraitFriend, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "TraitFriend ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_trait_friend_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_trait_function_definition_indent(const an_ifc_trait_function_definition universal,
             unsigned                               indent)
/*
Given the universal representation of TraitFunctionDefinition, print a
diagnostic textual representation with the given indent.
*/
{
  if (has_ifc_body_an_ifc_trait_function_definition(universal)) {
    an_ifc_stmt_index field = get_ifc_body_an_ifc_trait_function_definition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "body:");
    if (is_null_index_an_ifc_stmt_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_stmt_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_decl_an_ifc_trait_function_definition(universal)) {
    an_ifc_decl_index field = get_ifc_decl_an_ifc_trait_function_definition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "decl:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_encoded_decl_an_ifc_trait_function_definition(universal)) {
    an_ifc_encoded_decl_index field = get_ifc_encoded_decl_an_ifc_trait_function_definition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "encoded_decl: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_initializers_an_ifc_trait_function_definition(universal)) {
    an_ifc_expr_index field = get_ifc_initializers_an_ifc_trait_function_definition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "initializers:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_parameters_an_ifc_trait_function_definition(universal)) {
    an_ifc_chart_index field = get_ifc_parameters_an_ifc_trait_function_definition(universal);

    db_print_indent(indent);
    fprintf(f_debug, "parameters:");
    if (is_null_index_an_ifc_chart_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_chart_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_trait_function_definition(const an_ifc_trait_function_definition universal)
/*
Given the universal representation of TraitFunctionDefinition, print a
diagnostic textual representation.
*/
{
  fprintf(f_debug, "=========================== ");
  fprintf(f_debug, "TraitFunctionDefinition ");
  fprintf(f_debug, "============================\n");
  db_node_an_ifc_trait_function_definition_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_trait_msvc_decl_attrs_indent(const an_ifc_trait_msvc_decl_attrs universal, unsigned indent)
/*
Given the universal representation of TraitMsvcDeclAttrs, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_decl_an_ifc_trait_msvc_decl_attrs(universal)) {
    an_ifc_decl_index field = get_ifc_decl_an_ifc_trait_msvc_decl_attrs(universal);

    db_print_indent(indent);
    fprintf(f_debug, "decl:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_encoded_decl_an_ifc_trait_msvc_decl_attrs(universal)) {
    an_ifc_encoded_decl_index field = get_ifc_encoded_decl_an_ifc_trait_msvc_decl_attrs(universal);

    db_print_indent(indent);
    fprintf(f_debug, "encoded_decl: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_trait_an_ifc_trait_msvc_decl_attrs(universal)) {
    an_ifc_attr_index field = get_ifc_trait_an_ifc_trait_msvc_decl_attrs(universal);

    db_print_indent(indent);
    fprintf(f_debug, "trait:");
    if (is_null_index_an_ifc_attr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_attr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_trait_msvc_decl_attrs(const an_ifc_trait_msvc_decl_attrs universal)
/*
Given the universal representation of TraitMsvcDeclAttrs, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================== ");
  fprintf(f_debug, "TraitMsvcDeclAttrs ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_trait_msvc_decl_attrs_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_trait_msvc_func_params_indent(const an_ifc_trait_msvc_func_params universal, unsigned indent)
/*
Given the universal representation of TraitMsvcFuncParams, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_decl_an_ifc_trait_msvc_func_params(universal)) {
    an_ifc_decl_index field = get_ifc_decl_an_ifc_trait_msvc_func_params(universal);

    db_print_indent(indent);
    fprintf(f_debug, "decl:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_encoded_decl_an_ifc_trait_msvc_func_params(universal)) {
    an_ifc_encoded_decl_index field = get_ifc_encoded_decl_an_ifc_trait_msvc_func_params(universal);

    db_print_indent(indent);
    fprintf(f_debug, "encoded_decl: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_params_an_ifc_trait_msvc_func_params(universal)) {
    an_ifc_chart_index field = get_ifc_params_an_ifc_trait_msvc_func_params(universal);

    db_print_indent(indent);
    fprintf(f_debug, "params:");
    if (is_null_index_an_ifc_chart_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_chart_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_trait_msvc_func_params(const an_ifc_trait_msvc_func_params universal)
/*
Given the universal representation of TraitMsvcFuncParams, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================= ");
  fprintf(f_debug, "TraitMsvcFuncParams ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_trait_msvc_func_params_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_trait_msvc_uuid_indent(const an_ifc_trait_msvc_uuid universal, unsigned indent)
/*
Given the universal representation of TraitMsvcUuid, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_decl_an_ifc_trait_msvc_uuid(universal)) {
    an_ifc_decl_index field = get_ifc_decl_an_ifc_trait_msvc_uuid(universal);

    db_print_indent(indent);
    fprintf(f_debug, "decl:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_encoded_decl_an_ifc_trait_msvc_uuid(universal)) {
    an_ifc_encoded_decl_index field = get_ifc_encoded_decl_an_ifc_trait_msvc_uuid(universal);

    db_print_indent(indent);
    fprintf(f_debug, "encoded_decl: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_uuid_an_ifc_trait_msvc_uuid(universal)) {
    db_print_indent(indent);
    fprintf(f_debug, "uuid: UNIMPLEMENTED\n");
  }  /* if */
}  /* db_node */


void db_node_an_ifc_trait_msvc_uuid(const an_ifc_trait_msvc_uuid universal)
/*
Given the universal representation of TraitMsvcUuid, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "TraitMsvcUuid ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_trait_msvc_uuid_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_trait_msvc_vendor_trait_indent(const an_ifc_trait_msvc_vendor_trait universal, unsigned indent)
/*
Given the universal representation of TraitMsvcVendorTrait, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_decl_an_ifc_trait_msvc_vendor_trait(universal)) {
    an_ifc_decl_index field = get_ifc_decl_an_ifc_trait_msvc_vendor_trait(universal);

    db_print_indent(indent);
    fprintf(f_debug, "decl:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_encoded_decl_an_ifc_trait_msvc_vendor_trait(universal)) {
    an_ifc_encoded_decl_index field = get_ifc_encoded_decl_an_ifc_trait_msvc_vendor_trait(universal);

    db_print_indent(indent);
    fprintf(f_debug, "encoded_decl: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_trait_an_ifc_trait_msvc_vendor_trait(universal)) {
    an_ifc_msvc_traits_bitfield field = get_ifc_trait_an_ifc_trait_msvc_vendor_trait(universal);

    fprintf(f_debug, "trait:\n");
    ++indent;
    if (test_bitmask_ifc_mtb_allocate(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Allocate\n");
    }  /* if */
    if (test_bitmask_ifc_mtb_code_segment(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- CodeSegment\n");
    }  /* if */
    if (test_bitmask_ifc_mtb_comdat(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Comdat\n");
    }  /* if */
    if (test_bitmask_ifc_mtb_dll_export(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- DllExport\n");
    }  /* if */
    if (test_bitmask_ifc_mtb_dll_import(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- DllImport\n");
    }  /* if */
    if (test_bitmask_ifc_mtb_empty_bases(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- EmptyBases\n");
    }  /* if */
    if (test_bitmask_ifc_mtb_force_inline(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- ForceInline\n");
    }  /* if */
    if (test_bitmask_ifc_mtb_intrinsic_type(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- IntrinsicType\n");
    }  /* if */
    if (test_bitmask_ifc_mtb_naked(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Naked\n");
    }  /* if */
    if (test_bitmask_ifc_mtb_no_alias(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- NoAlias\n");
    }  /* if */
    if (test_bitmask_ifc_mtb_no_inline(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- NoInline\n");
    }  /* if */
    if (test_bitmask_ifc_mtb_none(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- None\n");
    }  /* if */
    if (test_bitmask_ifc_mtb_novtable(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Novtable\n");
    }  /* if */
    if (test_bitmask_ifc_mtb_process(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Process\n");
    }  /* if */
    if (test_bitmask_ifc_mtb_restrict(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Restrict\n");
    }  /* if */
    if (test_bitmask_ifc_mtb_safe_buffers(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- SafeBuffers\n");
    }  /* if */
    if (test_bitmask_ifc_mtb_select_any(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- SelectAny\n");
    }  /* if */
    if (test_bitmask_ifc_mtb_uuid(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Uuid\n");
    }  /* if */
    --indent;
  }  /* if */
}  /* db_node */


void db_node_an_ifc_trait_msvc_vendor_trait(const an_ifc_trait_msvc_vendor_trait universal)
/*
Given the universal representation of TraitMsvcVendorTrait, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================= ");
  fprintf(f_debug, "TraitMsvcVendorTrait ");
  fprintf(f_debug, "=============================\n");
  db_node_an_ifc_trait_msvc_vendor_trait_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_trait_requires_indent(const an_ifc_trait_requires universal, unsigned indent)
/*
Given the universal representation of TraitRequires, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_decl_an_ifc_trait_requires(universal)) {
    an_ifc_decl_index field = get_ifc_decl_an_ifc_trait_requires(universal);

    db_print_indent(indent);
    fprintf(f_debug, "decl:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_encoded_decl_an_ifc_trait_requires(universal)) {
    an_ifc_encoded_decl_index field = get_ifc_encoded_decl_an_ifc_trait_requires(universal);

    db_print_indent(indent);
    fprintf(f_debug, "encoded_decl: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_trait_an_ifc_trait_requires(universal)) {
    an_ifc_syntax_index field = get_ifc_trait_an_ifc_trait_requires(universal);

    db_print_indent(indent);
    fprintf(f_debug, "trait:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_trait_requires(const an_ifc_trait_requires universal)
/*
Given the universal representation of TraitRequires, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "TraitRequires ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_trait_requires_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_trait_specialization_indent(const an_ifc_trait_specialization universal, unsigned indent)
/*
Given the universal representation of TraitSpecialization, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_decl_an_ifc_trait_specialization(universal)) {
    an_ifc_decl_index field = get_ifc_decl_an_ifc_trait_specialization(universal);

    db_print_indent(indent);
    fprintf(f_debug, "decl:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_encoded_decl_an_ifc_trait_specialization(universal)) {
    an_ifc_encoded_decl_index field = get_ifc_encoded_decl_an_ifc_trait_specialization(universal);

    db_print_indent(indent);
    fprintf(f_debug, "encoded_decl: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_trait_an_ifc_trait_specialization(universal)) {
    an_ifc_sequence field = get_ifc_trait_an_ifc_trait_specialization(universal);

    db_print_indent(indent);
    fprintf(f_debug, "trait:\n");
    db_node_an_ifc_sequence_indent(field, indent + 1);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_trait_specialization(const an_ifc_trait_specialization universal)
/*
Given the universal representation of TraitSpecialization, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================= ");
  fprintf(f_debug, "TraitSpecialization ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_trait_specialization_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_type_array_indent(const an_ifc_type_array universal, unsigned indent)
/*
Given the universal representation of TypeArray, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_element_an_ifc_type_array(universal)) {
    an_ifc_type_index field = get_ifc_element_an_ifc_type_array(universal);

    db_print_indent(indent);
    fprintf(f_debug, "element:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_extent_an_ifc_type_array(universal)) {
    an_ifc_expr_index field = get_ifc_extent_an_ifc_type_array(universal);

    db_print_indent(indent);
    fprintf(f_debug, "extent:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_type_array(const an_ifc_type_array universal)
/*
Given the universal representation of TypeArray, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "TypeArray ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_type_array_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_type_base_indent(const an_ifc_type_base universal, unsigned indent)
/*
Given the universal representation of TypeBase, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_access_an_ifc_type_base(universal)) {
    an_ifc_access_sort field = get_ifc_access_an_ifc_type_base(universal);

    db_print_indent(indent);
    fprintf(f_debug, "access: %s\n", str_for_an_ifc_access_sort(field));
  }  /* if */
  if (has_ifc_pack_expanded_an_ifc_type_base(universal)) {
    an_ifc_bool field = get_ifc_pack_expanded_an_ifc_type_base(universal);

    db_print_indent(indent);
    fprintf(f_debug, "pack_expanded: %llu\n",
            (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_shared_an_ifc_type_base(universal)) {
    an_ifc_bool field = get_ifc_shared_an_ifc_type_base(universal);

    db_print_indent(indent);
    fprintf(f_debug, "shared: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_type_an_ifc_type_base(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_type_base(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_type_base(const an_ifc_type_base universal)
/*
Given the universal representation of TypeBase, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "=================================== ");
  fprintf(f_debug, "TypeBase ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_type_base_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_type_decltype_indent(const an_ifc_type_decltype universal, unsigned indent)
/*
Given the universal representation of TypeDecltype, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_expr_an_ifc_type_decltype(universal)) {
    an_ifc_syntax_index field = get_ifc_expr_an_ifc_type_decltype(universal);

    db_print_indent(indent);
    fprintf(f_debug, "expr:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_type_decltype(const an_ifc_type_decltype universal)
/*
Given the universal representation of TypeDecltype, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "TypeDecltype ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_type_decltype_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_type_designated_indent(const an_ifc_type_designated universal, unsigned indent)
/*
Given the universal representation of TypeDesignated, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_decl_an_ifc_type_designated(universal)) {
    an_ifc_decl_index field = get_ifc_decl_an_ifc_type_designated(universal);

    db_print_indent(indent);
    fprintf(f_debug, "decl:");
    if (is_null_index_an_ifc_decl_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_decl_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_type_designated(const an_ifc_type_designated universal)
/*
Given the universal representation of TypeDesignated, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "TypeDesignated ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_type_designated_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_type_expansion_indent(const an_ifc_type_expansion universal, unsigned indent)
/*
Given the universal representation of TypeExpansion, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_mode_an_ifc_type_expansion(universal)) {
    an_ifc_expansion_mode_sort field = get_ifc_mode_an_ifc_type_expansion(universal);

    db_print_indent(indent);
    fprintf(f_debug, "mode: %s\n", str_for_an_ifc_expansion_mode_sort(field));
  }  /* if */
  if (has_ifc_pack_an_ifc_type_expansion(universal)) {
    an_ifc_type_index field = get_ifc_pack_an_ifc_type_expansion(universal);

    db_print_indent(indent);
    fprintf(f_debug, "pack:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_type_expansion(const an_ifc_type_expansion universal)
/*
Given the universal representation of TypeExpansion, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "TypeExpansion ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_type_expansion_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_type_forall_indent(const an_ifc_type_forall universal, unsigned indent)
/*
Given the universal representation of TypeForall, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_chart_an_ifc_type_forall(universal)) {
    an_ifc_chart_index field = get_ifc_chart_an_ifc_type_forall(universal);

    db_print_indent(indent);
    fprintf(f_debug, "chart:");
    if (is_null_index_an_ifc_chart_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_chart_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_subject_an_ifc_type_forall(universal)) {
    an_ifc_type_index field = get_ifc_subject_an_ifc_type_forall(universal);

    db_print_indent(indent);
    fprintf(f_debug, "subject:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_type_forall(const an_ifc_type_forall universal)
/*
Given the universal representation of TypeForall, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "TypeForall ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_type_forall_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_type_function_indent(const an_ifc_type_function universal, unsigned indent)
/*
Given the universal representation of TypeFunction, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_convention_an_ifc_type_function(universal)) {
    an_ifc_calling_convention_sort field = get_ifc_convention_an_ifc_type_function(universal);

    db_print_indent(indent);
    fprintf(f_debug, "convention: %s\n", str_for_an_ifc_calling_convention_sort(field));
  }  /* if */
  if (has_ifc_eh_spec_an_ifc_type_function(universal)) {
    an_ifc_noexcept_specification field = get_ifc_eh_spec_an_ifc_type_function(universal);

    db_print_indent(indent);
    fprintf(f_debug, "eh_spec:\n");
    db_node_an_ifc_noexcept_specification_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_source_an_ifc_type_function(universal)) {
    an_ifc_type_index field = get_ifc_source_an_ifc_type_function(universal);

    db_print_indent(indent);
    fprintf(f_debug, "source:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_target_an_ifc_type_function(universal)) {
    an_ifc_type_index field = get_ifc_target_an_ifc_type_function(universal);

    db_print_indent(indent);
    fprintf(f_debug, "target:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_traits_an_ifc_type_function(universal)) {
    an_ifc_function_type_traits_bitfield field = get_ifc_traits_an_ifc_type_function(universal);

    fprintf(f_debug, "traits:\n");
    ++indent;
    if (test_bitmask_ifc_fttb_const(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Const\n");
    }  /* if */
    if (test_bitmask_ifc_fttb_lvalue(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Lvalue\n");
    }  /* if */
    if (test_bitmask_ifc_fttb_none(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- None\n");
    }  /* if */
    if (test_bitmask_ifc_fttb_rvalue(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Rvalue\n");
    }  /* if */
    if (test_bitmask_ifc_fttb_volatile(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Volatile\n");
    }  /* if */
    --indent;
  }  /* if */
}  /* db_node */


void db_node_an_ifc_type_function(const an_ifc_type_function universal)
/*
Given the universal representation of TypeFunction, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "TypeFunction ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_type_function_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_type_fundamental_indent(const an_ifc_type_fundamental universal, unsigned indent)
/*
Given the universal representation of TypeFundamental, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_basis_an_ifc_type_fundamental(universal)) {
    an_ifc_type_basis_sort field = get_ifc_basis_an_ifc_type_fundamental(universal);

    db_print_indent(indent);
    fprintf(f_debug, "basis: %s\n", str_for_an_ifc_type_sign_sort(field));
  }  /* if */
  if (has_ifc_precision_an_ifc_type_fundamental(universal)) {
    an_ifc_type_precision_sort field = get_ifc_precision_an_ifc_type_fundamental(universal);

    db_print_indent(indent);
    fprintf(f_debug, "precision: %s\n", str_for_an_ifc_type_sign_sort(field));
  }  /* if */
  if (has_ifc_sign_an_ifc_type_fundamental(universal)) {
    an_ifc_type_sign_sort field = get_ifc_sign_an_ifc_type_fundamental(universal);

    db_print_indent(indent);
    fprintf(f_debug, "sign: %s\n", str_for_an_ifc_type_sign_sort(field));
  }  /* if */
}  /* db_node */


void db_node_an_ifc_type_fundamental(const an_ifc_type_fundamental universal)
/*
Given the universal representation of TypeFundamental, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "=============================== ");
  fprintf(f_debug, "TypeFundamental ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_type_fundamental_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_type_lvalue_reference_indent(const an_ifc_type_lvalue_reference universal, unsigned indent)
/*
Given the universal representation of TypeLvalueReference, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_referee_an_ifc_type_lvalue_reference(universal)) {
    an_ifc_type_index field = get_ifc_referee_an_ifc_type_lvalue_reference(universal);

    db_print_indent(indent);
    fprintf(f_debug, "referee:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_type_lvalue_reference(const an_ifc_type_lvalue_reference universal)
/*
Given the universal representation of TypeLvalueReference, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================= ");
  fprintf(f_debug, "TypeLvalueReference ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_type_lvalue_reference_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_type_method_indent(const an_ifc_type_method universal, unsigned indent)
/*
Given the universal representation of TypeMethod, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_convention_an_ifc_type_method(universal)) {
    an_ifc_calling_convention_sort field = get_ifc_convention_an_ifc_type_method(universal);

    db_print_indent(indent);
    fprintf(f_debug, "convention: %s\n", str_for_an_ifc_calling_convention_sort(field));
  }  /* if */
  if (has_ifc_eh_spec_an_ifc_type_method(universal)) {
    an_ifc_noexcept_specification field = get_ifc_eh_spec_an_ifc_type_method(universal);

    db_print_indent(indent);
    fprintf(f_debug, "eh_spec:\n");
    db_node_an_ifc_noexcept_specification_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_scope_an_ifc_type_method(universal)) {
    an_ifc_type_index field = get_ifc_scope_an_ifc_type_method(universal);

    db_print_indent(indent);
    fprintf(f_debug, "scope:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_source_an_ifc_type_method(universal)) {
    an_ifc_type_index field = get_ifc_source_an_ifc_type_method(universal);

    db_print_indent(indent);
    fprintf(f_debug, "source:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_target_an_ifc_type_method(universal)) {
    an_ifc_type_index field = get_ifc_target_an_ifc_type_method(universal);

    db_print_indent(indent);
    fprintf(f_debug, "target:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_traits_an_ifc_type_method(universal)) {
    an_ifc_function_type_traits_bitfield field = get_ifc_traits_an_ifc_type_method(universal);

    fprintf(f_debug, "traits:\n");
    ++indent;
    if (test_bitmask_ifc_fttb_const(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Const\n");
    }  /* if */
    if (test_bitmask_ifc_fttb_lvalue(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Lvalue\n");
    }  /* if */
    if (test_bitmask_ifc_fttb_none(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- None\n");
    }  /* if */
    if (test_bitmask_ifc_fttb_rvalue(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Rvalue\n");
    }  /* if */
    if (test_bitmask_ifc_fttb_volatile(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Volatile\n");
    }  /* if */
    --indent;
  }  /* if */
}  /* db_node */


void db_node_an_ifc_type_method(const an_ifc_type_method universal)
/*
Given the universal representation of TypeMethod, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "TypeMethod ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_type_method_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_type_placeholder_indent(const an_ifc_type_placeholder universal, unsigned indent)
/*
Given the universal representation of TypePlaceholder, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_basis_an_ifc_type_placeholder(universal)) {
    an_ifc_type_basis_sort field = get_ifc_basis_an_ifc_type_placeholder(universal);

    db_print_indent(indent);
    fprintf(f_debug, "basis: %s\n", str_for_an_ifc_type_basis_sort(field));
  }  /* if */
  if (has_ifc_constraint_an_ifc_type_placeholder(universal)) {
    an_ifc_expr_index field = get_ifc_constraint_an_ifc_type_placeholder(universal);

    db_print_indent(indent);
    fprintf(f_debug, "constraint:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_elaboration_an_ifc_type_placeholder(universal)) {
    an_ifc_type_index field = get_ifc_elaboration_an_ifc_type_placeholder(universal);

    db_print_indent(indent);
    fprintf(f_debug, "elaboration:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_type_placeholder(const an_ifc_type_placeholder universal)
/*
Given the universal representation of TypePlaceholder, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "=============================== ");
  fprintf(f_debug, "TypePlaceholder ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_type_placeholder_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_type_pointer_indent(const an_ifc_type_pointer universal, unsigned indent)
/*
Given the universal representation of TypePointer, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_pointee_an_ifc_type_pointer(universal)) {
    an_ifc_type_index field = get_ifc_pointee_an_ifc_type_pointer(universal);

    db_print_indent(indent);
    fprintf(f_debug, "pointee:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_type_pointer(const an_ifc_type_pointer universal)
/*
Given the universal representation of TypePointer, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "TypePointer ");
  fprintf(f_debug, "==================================\n");
  db_node_an_ifc_type_pointer_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_type_pointer_to_member_indent(const an_ifc_type_pointer_to_member universal, unsigned indent)
/*
Given the universal representation of TypePointerToMember, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_member_an_ifc_type_pointer_to_member(universal)) {
    an_ifc_type_index field = get_ifc_member_an_ifc_type_pointer_to_member(universal);

    db_print_indent(indent);
    fprintf(f_debug, "member:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
  if (has_ifc_scope_an_ifc_type_pointer_to_member(universal)) {
    an_ifc_type_index field = get_ifc_scope_an_ifc_type_pointer_to_member(universal);

    db_print_indent(indent);
    fprintf(f_debug, "scope:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_type_pointer_to_member(const an_ifc_type_pointer_to_member universal)
/*
Given the universal representation of TypePointerToMember, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================= ");
  fprintf(f_debug, "TypePointerToMember ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_type_pointer_to_member_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_type_qualified_indent(const an_ifc_type_qualified universal, unsigned indent)
/*
Given the universal representation of TypeQualified, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_qualifiers_an_ifc_type_qualified(universal)) {
    an_ifc_qualifier_bitfield field = get_ifc_qualifiers_an_ifc_type_qualified(universal);

    fprintf(f_debug, "qualifiers:\n");
    ++indent;
    if (test_bitmask_ifc_qb_const(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Const\n");
    }  /* if */
    if (test_bitmask_ifc_qb_none(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- None\n");
    }  /* if */
    if (test_bitmask_ifc_qb_restrict(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Restrict\n");
    }  /* if */
    if (test_bitmask_ifc_qb_volatile(field)) {
      db_print_indent(indent);
      fprintf(f_debug, "- Volatile\n");
    }  /* if */
    --indent;
  }  /* if */
  if (has_ifc_unqualified_an_ifc_type_qualified(universal)) {
    an_ifc_type_index field = get_ifc_unqualified_an_ifc_type_qualified(universal);

    db_print_indent(indent);
    fprintf(f_debug, "unqualified:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_type_qualified(const an_ifc_type_qualified universal)
/*
Given the universal representation of TypeQualified, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "TypeQualified ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_type_qualified_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_type_rvalue_reference_indent(const an_ifc_type_rvalue_reference universal, unsigned indent)
/*
Given the universal representation of TypeRvalueReference, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_referee_an_ifc_type_rvalue_reference(universal)) {
    an_ifc_type_index field = get_ifc_referee_an_ifc_type_rvalue_reference(universal);

    db_print_indent(indent);
    fprintf(f_debug, "referee:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_type_rvalue_reference(const an_ifc_type_rvalue_reference universal)
/*
Given the universal representation of TypeRvalueReference, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "============================= ");
  fprintf(f_debug, "TypeRvalueReference ");
  fprintf(f_debug, "==============================\n");
  db_node_an_ifc_type_rvalue_reference_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_type_syntactic_indent(const an_ifc_type_syntactic universal, unsigned indent)
/*
Given the universal representation of TypeSyntactic, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_expr_an_ifc_type_syntactic(universal)) {
    an_ifc_expr_index field = get_ifc_expr_an_ifc_type_syntactic(universal);

    db_print_indent(indent);
    fprintf(f_debug, "expr:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_type_syntactic(const an_ifc_type_syntactic universal)
/*
Given the universal representation of TypeSyntactic, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "TypeSyntactic ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_type_syntactic_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_type_syntax_tree_indent(const an_ifc_type_syntax_tree universal, unsigned indent)
/*
Given the universal representation of TypeSyntaxTree, print a diagnostic
textual representation with the given indent.
*/
{
  if (has_ifc_syntax_an_ifc_type_syntax_tree(universal)) {
    an_ifc_syntax_index field = get_ifc_syntax_an_ifc_type_syntax_tree(universal);

    db_print_indent(indent);
    fprintf(f_debug, "syntax:");
    if (is_null_index_an_ifc_syntax_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_syntax_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_type_syntax_tree(const an_ifc_type_syntax_tree universal)
/*
Given the universal representation of TypeSyntaxTree, print a diagnostic
textual representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "TypeSyntaxTree ");
  fprintf(f_debug, "================================\n");
  db_node_an_ifc_type_syntax_tree_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_type_tor_indent(const an_ifc_type_tor universal, unsigned indent)
/*
Given the universal representation of TypeTor, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_convention_an_ifc_type_tor(universal)) {
    an_ifc_calling_convention_sort field = get_ifc_convention_an_ifc_type_tor(universal);

    db_print_indent(indent);
    fprintf(f_debug, "convention: %s\n", str_for_an_ifc_calling_convention_sort(field));
  }  /* if */
  if (has_ifc_eh_spec_an_ifc_type_tor(universal)) {
    an_ifc_noexcept_specification field = get_ifc_eh_spec_an_ifc_type_tor(universal);

    db_print_indent(indent);
    fprintf(f_debug, "eh_spec:\n");
    db_node_an_ifc_noexcept_specification_indent(field, indent + 1);
  }  /* if */
  if (has_ifc_source_an_ifc_type_tor(universal)) {
    an_ifc_type_index field = get_ifc_source_an_ifc_type_tor(universal);

    db_print_indent(indent);
    fprintf(f_debug, "source:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_type_tor(const an_ifc_type_tor universal)
/*
Given the universal representation of TypeTor, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "=================================== ");
  fprintf(f_debug, "TypeTor ");
  fprintf(f_debug, "====================================\n");
  db_node_an_ifc_type_tor_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_type_tuple_indent(const an_ifc_type_tuple universal, unsigned indent)
/*
Given the universal representation of TypeTuple, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_cardinality_an_ifc_type_tuple(universal)) {
    an_ifc_cardinality field = get_ifc_cardinality_an_ifc_type_tuple(universal);

    db_print_indent(indent);
    fprintf(f_debug, "cardinality: %llu\n", (unsigned long long)field.value);
  }  /* if */
  if (has_ifc_start_an_ifc_type_tuple(universal)) {
    an_ifc_index field = get_ifc_start_an_ifc_type_tuple(universal);

    db_print_indent(indent);
    fprintf(f_debug, "start: %llu\n", (unsigned long long)field.value);
  }  /* if */
}  /* db_node */


void db_node_an_ifc_type_tuple(const an_ifc_type_tuple universal)
/*
Given the universal representation of TypeTuple, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================== ");
  fprintf(f_debug, "TypeTuple ");
  fprintf(f_debug, "===================================\n");
  db_node_an_ifc_type_tuple_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_type_typename_indent(const an_ifc_type_typename universal, unsigned indent)
/*
Given the universal representation of TypeTypename, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_path_an_ifc_type_typename(universal)) {
    an_ifc_expr_index field = get_ifc_path_an_ifc_type_typename(universal);

    db_print_indent(indent);
    fprintf(f_debug, "path:");
    if (is_null_index_an_ifc_expr_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_expr_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_type_typename(const an_ifc_type_typename universal)
/*
Given the universal representation of TypeTypename, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================= ");
  fprintf(f_debug, "TypeTypename ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_type_typename_indent(universal, 0);
}  /* db_node */


void db_node_an_ifc_type_unaligned_indent(const an_ifc_type_unaligned universal, unsigned indent)
/*
Given the universal representation of TypeUnaligned, print a diagnostic textual
representation with the given indent.
*/
{
  if (has_ifc_type_an_ifc_type_unaligned(universal)) {
    an_ifc_type_index field = get_ifc_type_an_ifc_type_unaligned(universal);

    db_print_indent(indent);
    fprintf(f_debug, "type:");
    if (is_null_index_an_ifc_type_index(field)) {
      fprintf(f_debug, " NULL\n");
    } else {
      fprintf(f_debug, "\n");
      db_print_indent(indent);
      fprintf(f_debug, "  sort: %s\n", str_for_an_ifc_type_sort(field.sort));
      db_print_indent(indent);
      fprintf(f_debug, "  value: %llu\n", (unsigned long long)field.value);
    }  /* if */
  }  /* if */
}  /* db_node */


void db_node_an_ifc_type_unaligned(const an_ifc_type_unaligned universal)
/*
Given the universal representation of TypeUnaligned, print a diagnostic textual
representation.
*/
{
  fprintf(f_debug, "================================ ");
  fprintf(f_debug, "TypeUnaligned ");
  fprintf(f_debug, "=================================\n");
  db_node_an_ifc_type_unaligned_indent(universal, 0);
}  /* db_node */


/*
Visitor functions for printing diagnostic textual representations on a given
index.
*/


void db_node_at_idx_an_ifc_attr_index(an_ifc_attr_index idx)
/*
Given the AttrIndex, print a diagnostic textual representation of the
associated node.
*/
{
  if (validate_an_ifc_attr_index(idx)) {
    switch (idx.sort) {
      case ifc_as_attr_basic:
        { an_ifc_attr_basic universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_attr_basic(universal);
        }
        break;
      case ifc_as_attr_called:
        { an_ifc_attr_called universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_attr_called(universal);
        }
        break;
      case ifc_as_attr_elaborated:
        { an_ifc_attr_elaborated universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_attr_elaborated(universal);
        }
        break;
      case ifc_as_attr_expanded:
        { an_ifc_attr_expanded universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_attr_expanded(universal);
        }
        break;
      case ifc_as_attr_factored:
        { an_ifc_attr_factored universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_attr_factored(universal);
        }
        break;
      case ifc_as_attr_labeled:
        { an_ifc_attr_labeled universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_attr_labeled(universal);
        }
        break;
      case ifc_as_attr_scoped:
        { an_ifc_attr_scoped universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_attr_scoped(universal);
        }
        break;
      case ifc_as_attr_tuple:
        { an_ifc_attr_tuple universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_attr_tuple(universal);
        }
        break;
      default:
        fprintf(f_debug, "Node not found.");
        break;
    }  /* switch */
  } else {
    fprintf(f_debug, "Invalid %s node.\n", str_for_an_ifc_attr_sort(idx.sort));
  }  /* if */
}  /* db_node_at_idx */


void db_node_at_idx_an_ifc_chart_index(an_ifc_chart_index idx)
/*
Given the ChartIndex, print a diagnostic textual representation of the
associated node.
*/
{
  if (validate_an_ifc_chart_index(idx)) {
    switch (idx.sort) {
      case ifc_cs_chart_multilevel:
        { an_ifc_chart_multilevel universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_chart_multilevel(universal);
        }
        break;
      case ifc_cs_chart_unilevel:
        { an_ifc_chart_unilevel universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_chart_unilevel(universal);
        }
        break;
      default:
        fprintf(f_debug, "Node not found.");
        break;
    }  /* switch */
  } else {
    fprintf(f_debug, "Invalid %s node.\n", str_for_an_ifc_chart_sort(idx.sort));
  }  /* if */
}  /* db_node_at_idx */


void db_node_at_idx_an_ifc_decl_index(an_ifc_decl_index idx)
/*
Given the DeclIndex, print a diagnostic textual representation of the
associated node.
*/
{
  if (validate_an_ifc_decl_index(idx)) {
    switch (idx.sort) {
      case ifc_ds_decl_alias:
        { an_ifc_decl_alias universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_decl_alias(universal);
        }
        break;
      case ifc_ds_decl_barren:
        { an_ifc_decl_barren universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_decl_barren(universal);
        }
        break;
      case ifc_ds_decl_bitfield:
        { an_ifc_decl_bitfield universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_decl_bitfield(universal);
        }
        break;
      case ifc_ds_decl_concept:
        { an_ifc_decl_concept universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_decl_concept(universal);
        }
        break;
      case ifc_ds_decl_constructor:
        { an_ifc_decl_constructor universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_decl_constructor(universal);
        }
        break;
      case ifc_ds_decl_deduction_guide:
        { an_ifc_decl_deduction_guide universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_decl_deduction_guide(universal);
        }
        break;
      case ifc_ds_decl_default_argument:
        { an_ifc_decl_default_argument universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_decl_default_argument(universal);
        }
        break;
      case ifc_ds_decl_destructor:
        { an_ifc_decl_destructor universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_decl_destructor(universal);
        }
        break;
      case ifc_ds_decl_enumeration:
        { an_ifc_decl_enumeration universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_decl_enumeration(universal);
        }
        break;
      case ifc_ds_decl_enumerator:
        { an_ifc_decl_enumerator universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_decl_enumerator(universal);
        }
        break;
      case ifc_ds_decl_expansion:
        { an_ifc_decl_expansion universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_decl_expansion(universal);
        }
        break;
      case ifc_ds_decl_explicit_instantiation:
        { an_ifc_decl_explicit_instantiation universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_decl_explicit_instantiation(universal);
        }
        break;
      case ifc_ds_decl_explicit_specialization:
        { an_ifc_decl_explicit_specialization universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_decl_explicit_specialization(universal);
        }
        break;
      case ifc_ds_decl_field:
        { an_ifc_decl_field universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_decl_field(universal);
        }
        break;
      case ifc_ds_decl_friend:
        { an_ifc_decl_friend universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_decl_friend(universal);
        }
        break;
      case ifc_ds_decl_function:
        { an_ifc_decl_function universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_decl_function(universal);
        }
        break;
      case ifc_ds_decl_inherited_constructor:
        { an_ifc_decl_inherited_constructor universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_decl_inherited_constructor(universal);
        }
        break;
      case ifc_ds_decl_intrinsic:
        { an_ifc_decl_intrinsic universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_decl_intrinsic(universal);
        }
        break;
      case ifc_ds_decl_method:
        { an_ifc_decl_method universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_decl_method(universal);
        }
        break;
      case ifc_ds_decl_output_segment:
        { an_ifc_decl_output_segment universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_decl_output_segment(universal);
        }
        break;
      case ifc_ds_decl_parameter:
        { an_ifc_decl_parameter universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_decl_parameter(universal);
        }
        break;
      case ifc_ds_decl_partial_specialization:
        { an_ifc_decl_partial_specialization universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_decl_partial_specialization(universal);
        }
        break;
      case ifc_ds_decl_prolongation:
        { an_ifc_decl_prolongation universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_decl_prolongation(universal);
        }
        break;
      case ifc_ds_decl_property:
        { an_ifc_decl_property universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_decl_property(universal);
        }
        break;
      case ifc_ds_decl_reference:
        { an_ifc_decl_reference universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_decl_reference(universal);
        }
        break;
      case ifc_ds_decl_scope:
        { an_ifc_decl_scope universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_decl_scope(universal);
        }
        break;
      case ifc_ds_decl_specialization:
        { an_ifc_decl_specialization universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_decl_specialization(universal);
        }
        break;
      case ifc_ds_decl_syntax_tree:
        { an_ifc_decl_syntax_tree universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_decl_syntax_tree(universal);
        }
        break;
      case ifc_ds_decl_template:
        { an_ifc_decl_template universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_decl_template(universal);
        }
        break;
      case ifc_ds_decl_temploid:
        { an_ifc_decl_temploid universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_decl_temploid(universal);
        }
        break;
      case ifc_ds_decl_tuple:
        { an_ifc_decl_tuple universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_decl_tuple(universal);
        }
        break;
      case ifc_ds_decl_using_declaration:
        { an_ifc_decl_using_declaration universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_decl_using_declaration(universal);
        }
        break;
      case ifc_ds_decl_variable:
        { an_ifc_decl_variable universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_decl_variable(universal);
        }
        break;
      default:
        fprintf(f_debug, "Node not found.");
        break;
    }  /* switch */
  } else {
    fprintf(f_debug, "Invalid %s node.\n", str_for_an_ifc_decl_sort(idx.sort));
  }  /* if */
}  /* db_node_at_idx */


void db_node_at_idx_an_ifc_dir_index(an_ifc_dir_index idx)
/*
Given the DirIndex, print a diagnostic textual representation of the associated
node.
*/
{
  if (validate_an_ifc_dir_index(idx)) {
    switch (idx.sort) {
      case ifc_ds_dir_attribute:
        { an_ifc_dir_attribute universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_dir_attribute(universal);
        }
        break;
      case ifc_ds_dir_decl_use:
        { an_ifc_dir_decl_use universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_dir_decl_use(universal);
        }
        break;
      case ifc_ds_dir_empty:
        { an_ifc_dir_empty universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_dir_empty(universal);
        }
        break;
      case ifc_ds_dir_expr:
        { an_ifc_dir_expr universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_dir_expr(universal);
        }
        break;
      case ifc_ds_dir_pragma:
        { an_ifc_dir_pragma universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_dir_pragma(universal);
        }
        break;
      case ifc_ds_dir_tuple:
        { an_ifc_dir_tuple universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_dir_tuple(universal);
        }
        break;
      case ifc_ds_dir_using:
        { an_ifc_dir_using universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_dir_using(universal);
        }
        break;
      default:
        fprintf(f_debug, "Node not found.");
        break;
    }  /* switch */
  } else {
    fprintf(f_debug, "Invalid %s node.\n", str_for_an_ifc_dir_sort(idx.sort));
  }  /* if */
}  /* db_node_at_idx */


void db_node_at_idx_an_ifc_edg_complex_token_index(an_ifc_edg_complex_token_index idx)
/*
Given the EdgComplexTokenIndex, print a diagnostic textual representation of
the associated node.
*/
{
  if (validate_an_ifc_edg_complex_token_index(idx)) {
    switch (idx.sort) {
      case ifc_ects_edg_token_constant:
        { an_ifc_edg_token_constant universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_edg_token_constant(universal);
        }
        break;
      case ifc_ects_edg_token_identifier:
        { an_ifc_edg_token_identifier universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_edg_token_identifier(universal);
        }
        break;
      case ifc_ects_edg_token_textual:
        { an_ifc_edg_token_textual universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_edg_token_textual(universal);
        }
        break;
      default:
        fprintf(f_debug, "Node not found.");
        break;
    }  /* switch */
  } else {
    fprintf(f_debug, "Invalid %s node.\n", str_for_an_ifc_edg_complex_token_sort(idx.sort));
  }  /* if */
}  /* db_node_at_idx */


void db_node_at_idx_an_ifc_edg_constant_index(an_ifc_edg_constant_index idx)
/*
Given the EdgConstantIndex, print a diagnostic textual representation of the
associated node.
*/
{
  if (validate_an_ifc_edg_constant_index(idx)) {
    switch (idx.sort) {
      case ifc_ecs_edg_constant_integer:
        { an_ifc_edg_constant_integer universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_edg_constant_integer(universal);
        }
        break;
      default:
        fprintf(f_debug, "Node not found.");
        break;
    }  /* switch */
  } else {
    fprintf(f_debug, "Invalid %s node.\n", str_for_an_ifc_edg_constant_sort(idx.sort));
  }  /* if */
}  /* db_node_at_idx */


void db_node_at_idx_an_ifc_edg_constant_integer_word_offset(an_ifc_edg_constant_integer_word_offset idx)
/*
Given the EdgConstantIntegerWordOffset, print a diagnostic textual
representation of the associated node.
*/
{
  if (validate_an_ifc_edg_constant_integer_word_offset(idx)) {
    an_ifc_edg_constant_integer_word universal = {0};

    construct_node_prechecked(&universal, idx);
    db_node_an_ifc_edg_constant_integer_word(universal);
  } else {
    fputs("Invalid EdgConstantIntegerWord node.\n", f_debug);
  }  /* if */
}  /* db_node_at_idx */


void db_node_at_idx_an_ifc_edg_expr_index(an_ifc_edg_expr_index idx)
/*
Given the EdgExprIndex, print a diagnostic textual representation of the
associated node.
*/
{
  if (validate_an_ifc_edg_expr_index(idx)) {
    switch (idx.sort) {
      case ifc_ees_edg_expr_template_argument:
        { an_ifc_edg_expr_template_argument universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_edg_expr_template_argument(universal);
        }
        break;
      case ifc_ees_edg_token_cache:
        { an_ifc_edg_token_cache universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_edg_token_cache(universal);
        }
        break;
      default:
        fprintf(f_debug, "Node not found.");
        break;
    }  /* switch */
  } else {
    fprintf(f_debug, "Invalid %s node.\n", str_for_an_ifc_edg_expr_sort(idx.sort));
  }  /* if */
}  /* db_node_at_idx */


void db_node_at_idx_an_ifc_edg_extension_expr_offset(an_ifc_edg_extension_expr_offset idx)
/*
Given the EdgExtensionExprOffset, print a diagnostic textual representation of
the associated node.
*/
{
  if (validate_an_ifc_edg_extension_expr_offset(idx)) {
    an_ifc_edg_extension_expr universal = {0};

    construct_node_prechecked(&universal, idx);
    db_node_an_ifc_edg_extension_expr(universal);
  } else {
    fputs("Invalid EdgExtensionExpr node.\n", f_debug);
  }  /* if */
}  /* db_node_at_idx */


void db_node_at_idx_an_ifc_edg_extension_type_offset(an_ifc_edg_extension_type_offset idx)
/*
Given the EdgExtensionTypeOffset, print a diagnostic textual representation of
the associated node.
*/
{
  if (validate_an_ifc_edg_extension_type_offset(idx)) {
    an_ifc_edg_extension_type universal = {0};

    construct_node_prechecked(&universal, idx);
    db_node_an_ifc_edg_extension_type(universal);
  } else {
    fputs("Invalid EdgExtensionType node.\n", f_debug);
  }  /* if */
}  /* db_node_at_idx */


void db_node_at_idx_an_ifc_edg_heap_complex_token_offset(an_ifc_edg_heap_complex_token_offset idx)
/*
Given the EdgHeapComplexTokenOffset, print a diagnostic textual representation
of the associated node.
*/
{
  if (validate_an_ifc_edg_heap_complex_token_offset(idx)) {
    an_ifc_edg_heap_complex_token universal = {0};

    construct_node_prechecked(&universal, idx);
    db_node_an_ifc_edg_heap_complex_token(universal);
  } else {
    fputs("Invalid EdgHeapComplexToken node.\n", f_debug);
  }  /* if */
}  /* db_node_at_idx */


void db_node_at_idx_an_ifc_edg_heap_template_argument_offset(an_ifc_edg_heap_template_argument_offset idx)
/*
Given the EdgHeapTemplateArgumentOffset, print a diagnostic textual
representation of the associated node.
*/
{
  if (validate_an_ifc_edg_heap_template_argument_offset(idx)) {
    an_ifc_edg_heap_template_argument universal = {0};

    construct_node_prechecked(&universal, idx);
    db_node_an_ifc_edg_heap_template_argument(universal);
  } else {
    fputs("Invalid EdgHeapTemplateArgument node.\n", f_debug);
  }  /* if */
}  /* db_node_at_idx */


void db_node_at_idx_an_ifc_edg_template_argument_index(an_ifc_edg_template_argument_index idx)
/*
Given the EdgTemplateArgumentIndex, print a diagnostic textual representation
of the associated node.
*/
{
  if (validate_an_ifc_edg_template_argument_index(idx)) {
    switch (idx.sort) {
      case ifc_etas_edg_template_argument_non_type:
        { an_ifc_edg_template_argument_non_type universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_edg_template_argument_non_type(universal);
        }
        break;
      case ifc_etas_edg_template_argument_template:
        { an_ifc_edg_template_argument_template universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_edg_template_argument_template(universal);
        }
        break;
      case ifc_etas_edg_template_argument_type:
        { an_ifc_edg_template_argument_type universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_edg_template_argument_type(universal);
        }
        break;
      default:
        fprintf(f_debug, "Node not found.");
        break;
    }  /* switch */
  } else {
    fprintf(f_debug, "Invalid %s node.\n", str_for_an_ifc_edg_template_argument_sort(idx.sort));
  }  /* if */
}  /* db_node_at_idx */


void db_node_at_idx_an_ifc_edg_token_basic_offset(an_ifc_edg_token_basic_offset idx)
/*
Given the EdgTokenBasicOffset, print a diagnostic textual representation of the
associated node.
*/
{
  if (validate_an_ifc_edg_token_basic_offset(idx)) {
    an_ifc_edg_token_basic universal = {0};

    construct_node_prechecked(&universal, idx);
    db_node_an_ifc_edg_token_basic(universal);
  } else {
    fputs("Invalid EdgTokenBasic node.\n", f_debug);
  }  /* if */
}  /* db_node_at_idx */


void db_node_at_idx_an_ifc_edg_token_cache_offset(an_ifc_edg_token_cache_offset idx)
/*
Given the EdgTokenCacheOffset, print a diagnostic textual representation of the
associated node.
*/
{
  if (validate_an_ifc_edg_token_cache_offset(idx)) {
    an_ifc_edg_token_cache universal = {0};

    construct_node_prechecked(&universal, idx);
    db_node_an_ifc_edg_token_cache(universal);
  } else {
    fputs("Invalid EdgTokenCache node.\n", f_debug);
  }  /* if */
}  /* db_node_at_idx */


void db_node_at_idx_an_ifc_edg_type_index(an_ifc_edg_type_index idx)
/*
Given the EdgTypeIndex, print a diagnostic textual representation of the
associated node.
*/
{
  if (validate_an_ifc_edg_type_index(idx)) {
    switch (idx.sort) {
      case ifc_ets_edg_type_substituted:
        { an_ifc_edg_type_substituted universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_edg_type_substituted(universal);
        }
        break;
      default:
        fprintf(f_debug, "Node not found.");
        break;
    }  /* switch */
  } else {
    fprintf(f_debug, "Invalid %s node.\n", str_for_an_ifc_edg_type_sort(idx.sort));
  }  /* if */
}  /* db_node_at_idx */


void db_node_at_idx_an_ifc_expr_index(an_ifc_expr_index idx)
/*
Given the ExprIndex, print a diagnostic textual representation of the
associated node.
*/
{
  if (validate_an_ifc_expr_index(idx)) {
    switch (idx.sort) {
      case ifc_es_expr_alignof:
        { an_ifc_expr_alignof universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_alignof(universal);
        }
        break;
      case ifc_es_expr_array_value:
        { an_ifc_expr_array_value universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_array_value(universal);
        }
        break;
      case ifc_es_expr_assign_initializer:
        { an_ifc_expr_assign_initializer universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_assign_initializer(universal);
        }
        break;
      case ifc_es_expr_binary_fold:
        { an_ifc_expr_binary_fold universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_binary_fold(universal);
        }
        break;
      case ifc_es_expr_call:
        { an_ifc_expr_call universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_call(universal);
        }
        break;
      case ifc_es_expr_cast:
        { an_ifc_expr_cast universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_cast(universal);
        }
        break;
      case ifc_es_expr_compound_string:
        { an_ifc_expr_compound_string universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_compound_string(universal);
        }
        break;
      case ifc_es_expr_condition:
        { an_ifc_expr_condition universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_condition(universal);
        }
        break;
      case ifc_es_expr_designated_initializer:
        { an_ifc_expr_designated_initializer universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_designated_initializer(universal);
        }
        break;
      case ifc_es_expr_destructor_call:
        { an_ifc_expr_destructor_call universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_destructor_call(universal);
        }
        break;
      case ifc_es_expr_dyad:
        { an_ifc_expr_dyad universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_dyad(universal);
        }
        break;
      case ifc_es_expr_dynamic_dispatch:
        { an_ifc_expr_dynamic_dispatch universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_dynamic_dispatch(universal);
        }
        break;
      case ifc_es_expr_empty:
        { an_ifc_expr_empty universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_empty(universal);
        }
        break;
      case ifc_es_expr_expansion:
        { an_ifc_expr_expansion universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_expansion(universal);
        }
        break;
      case ifc_es_expr_expression_list:
        { an_ifc_expr_expression_list universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_expression_list(universal);
        }
        break;
      case ifc_es_expr_function_string:
        { an_ifc_expr_function_string universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_function_string(universal);
        }
        break;
      case ifc_es_expr_hierarchy_conversion:
        { an_ifc_expr_hierarchy_conversion universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_hierarchy_conversion(universal);
        }
        break;
      case ifc_es_expr_inheritance_path:
        { an_ifc_expr_inheritance_path universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_inheritance_path(universal);
        }
        break;
      case ifc_es_expr_initializer:
        { an_ifc_expr_initializer universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_initializer(universal);
        }
        break;
      case ifc_es_expr_initializer_list:
        { an_ifc_expr_initializer_list universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_initializer_list(universal);
        }
        break;
      case ifc_es_expr_label:
        { an_ifc_expr_label universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_label(universal);
        }
        break;
      case ifc_es_expr_lambda:
        { an_ifc_expr_lambda universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_lambda(universal);
        }
        break;
      case ifc_es_expr_literal:
        { an_ifc_expr_literal universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_literal(universal);
        }
        break;
      case ifc_es_expr_member_access:
        { an_ifc_expr_member_access universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_member_access(universal);
        }
        break;
      case ifc_es_expr_member_initializer:
        { an_ifc_expr_member_initializer universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_member_initializer(universal);
        }
        break;
      case ifc_es_expr_monad:
        { an_ifc_expr_monad universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_monad(universal);
        }
        break;
      case ifc_es_expr_named_decl:
        { an_ifc_expr_named_decl universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_named_decl(universal);
        }
        break;
      case ifc_es_expr_nullptr:
        { an_ifc_expr_nullptr universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_nullptr(universal);
        }
        break;
      case ifc_es_expr_packed_template_arguments:
        { an_ifc_expr_packed_template_arguments universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_packed_template_arguments(universal);
        }
        break;
      case ifc_es_expr_path:
        { an_ifc_expr_path universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_path(universal);
        }
        break;
      case ifc_es_expr_placeholder:
        { an_ifc_expr_placeholder universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_placeholder(universal);
        }
        break;
      case ifc_es_expr_pointer:
        { an_ifc_expr_pointer universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_pointer(universal);
        }
        break;
      case ifc_es_expr_product_type_value:
        { an_ifc_expr_product_type_value universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_product_type_value(universal);
        }
        break;
      case ifc_es_expr_push_state:
        { an_ifc_expr_push_state universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_push_state(universal);
        }
        break;
      case ifc_es_expr_qualified_name:
        { an_ifc_expr_qualified_name universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_qualified_name(universal);
        }
        break;
      case ifc_es_expr_read:
        { an_ifc_expr_read universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_read(universal);
        }
        break;
      case ifc_es_expr_requires:
        { an_ifc_expr_requires universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_requires(universal);
        }
        break;
      case ifc_es_expr_simple_identifier:
        { an_ifc_expr_simple_identifier universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_simple_identifier(universal);
        }
        break;
      case ifc_es_expr_sizeof_type:
        { an_ifc_expr_sizeof_type universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_sizeof_type(universal);
        }
        break;
      case ifc_es_expr_string:
        { an_ifc_expr_string universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_string(universal);
        }
        break;
      case ifc_es_expr_string_sequence:
        { an_ifc_expr_string_sequence universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_string_sequence(universal);
        }
        break;
      case ifc_es_expr_subobject_value:
        { an_ifc_expr_subobject_value universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_subobject_value(universal);
        }
        break;
      case ifc_es_expr_sum_type_value:
        { an_ifc_expr_sum_type_value universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_sum_type_value(universal);
        }
        break;
      case ifc_es_expr_syntax_tree:
        { an_ifc_expr_syntax_tree universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_syntax_tree(universal);
        }
        break;
      case ifc_es_expr_template_id:
        { an_ifc_expr_template_id universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_template_id(universal);
        }
        break;
      case ifc_es_expr_template_reference:
        { an_ifc_expr_template_reference universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_template_reference(universal);
        }
        break;
      case ifc_es_expr_temporary:
        { an_ifc_expr_temporary universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_temporary(universal);
        }
        break;
      case ifc_es_expr_this:
        { an_ifc_expr_this universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_this(universal);
        }
        break;
      case ifc_es_expr_tokens:
        { an_ifc_expr_tokens universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_tokens(universal);
        }
        break;
      case ifc_es_expr_triad:
        { an_ifc_expr_triad universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_triad(universal);
        }
        break;
      case ifc_es_expr_tuple:
        { an_ifc_expr_tuple universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_tuple(universal);
        }
        break;
      case ifc_es_expr_type:
        { an_ifc_expr_type universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_type(universal);
        }
        break;
      case ifc_es_expr_type_trait_intrinsic:
        { an_ifc_expr_type_trait_intrinsic universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_type_trait_intrinsic(universal);
        }
        break;
      case ifc_es_expr_typeid:
        { an_ifc_expr_typeid universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_typeid(universal);
        }
        break;
      case ifc_es_expr_unary_fold:
        { an_ifc_expr_unary_fold universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_unary_fold(universal);
        }
        break;
      case ifc_es_expr_unqualified_id:
        { an_ifc_expr_unqualified_id universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_unqualified_id(universal);
        }
        break;
      case ifc_es_expr_unresolved_id:
        { an_ifc_expr_unresolved_id universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_unresolved_id(universal);
        }
        break;
      case ifc_es_expr_virtual_function_conversion:
        { an_ifc_expr_virtual_function_conversion universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_expr_virtual_function_conversion(universal);
        }
        break;
      default:
        fprintf(f_debug, "Node not found.");
        break;
    }  /* switch */
  } else {
    fprintf(f_debug, "Invalid %s node.\n", str_for_an_ifc_expr_sort(idx.sort));
  }  /* if */
}  /* db_node_at_idx */


void db_node_at_idx_an_ifc_expr_named_decl_offset(an_ifc_expr_named_decl_offset idx)
/*
Given the ExprNamedDeclOffset, print a diagnostic textual representation of the
associated node.
*/
{
  if (validate_an_ifc_expr_named_decl_offset(idx)) {
    an_ifc_expr_named_decl universal = {0};

    construct_node_prechecked(&universal, idx);
    db_node_an_ifc_expr_named_decl(universal);
  } else {
    fputs("Invalid ExprNamedDecl node.\n", f_debug);
  }  /* if */
}  /* db_node_at_idx */


void db_node_at_idx_an_ifc_form_index(an_ifc_form_index idx)
/*
Given the FormIndex, print a diagnostic textual representation of the
associated node.
*/
{
  if (validate_an_ifc_form_index(idx)) {
    switch (idx.sort) {
      case ifc_fs_form_catenate:
        { an_ifc_form_catenate universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_form_catenate(universal);
        }
        break;
      case ifc_fs_form_character:
        { an_ifc_form_character universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_form_character(universal);
        }
        break;
      case ifc_fs_form_header:
        { an_ifc_form_header universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_form_header(universal);
        }
        break;
      case ifc_fs_form_identifier:
        { an_ifc_form_identifier universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_form_identifier(universal);
        }
        break;
      case ifc_fs_form_junk:
        { an_ifc_form_junk universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_form_junk(universal);
        }
        break;
      case ifc_fs_form_keyword:
        { an_ifc_form_keyword universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_form_keyword(universal);
        }
        break;
      case ifc_fs_form_number:
        { an_ifc_form_number universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_form_number(universal);
        }
        break;
      case ifc_fs_form_operator:
        { an_ifc_form_operator universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_form_operator(universal);
        }
        break;
      case ifc_fs_form_parameter:
        { an_ifc_form_parameter universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_form_parameter(universal);
        }
        break;
      case ifc_fs_form_parenthesized:
        { an_ifc_form_parenthesized universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_form_parenthesized(universal);
        }
        break;
      case ifc_fs_form_pragma:
        { an_ifc_form_pragma universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_form_pragma(universal);
        }
        break;
      case ifc_fs_form_string:
        { an_ifc_form_string universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_form_string(universal);
        }
        break;
      case ifc_fs_form_stringize:
        { an_ifc_form_stringize universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_form_stringize(universal);
        }
        break;
      case ifc_fs_form_tuple:
        { an_ifc_form_tuple universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_form_tuple(universal);
        }
        break;
      case ifc_fs_form_whitespace:
        { an_ifc_form_whitespace universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_form_whitespace(universal);
        }
        break;
      default:
        fprintf(f_debug, "Node not found.");
        break;
    }  /* switch */
  } else {
    fprintf(f_debug, "Invalid %s node.\n", str_for_an_ifc_form_sort(idx.sort));
  }  /* if */
}  /* db_node_at_idx */


void db_node_at_idx_an_ifc_form_spec_offset(an_ifc_form_spec_offset idx)
/*
Given the FormSpecOffset, print a diagnostic textual representation of the
associated node.
*/
{
  if (validate_an_ifc_form_spec_offset(idx)) {
    an_ifc_form_spec universal = {0};

    construct_node_prechecked(&universal, idx);
    db_node_an_ifc_form_spec(universal);
  } else {
    fputs("Invalid FormSpec node.\n", f_debug);
  }  /* if */
}  /* db_node_at_idx */


void db_node_at_idx_an_ifc_line_offset(an_ifc_line_offset idx)
/*
Given the LineOffset, print a diagnostic textual representation of the
associated node.
*/
{
  if (validate_an_ifc_line_offset(idx)) {
    an_ifc_source_line universal = {0};

    construct_node_prechecked(&universal, idx);
    db_node_an_ifc_source_line(universal);
  } else {
    fputs("Invalid SourceLine node.\n", f_debug);
  }  /* if */
}  /* db_node_at_idx */


void db_node_at_idx_an_ifc_macro_index(an_ifc_macro_index idx)
/*
Given the MacroIndex, print a diagnostic textual representation of the
associated node.
*/
{
  if (validate_an_ifc_macro_index(idx)) {
    switch (idx.sort) {
      case ifc_ms_macro_function_like:
        { an_ifc_macro_function_like universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_macro_function_like(universal);
        }
        break;
      case ifc_ms_macro_object_like:
        { an_ifc_macro_object_like universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_macro_object_like(universal);
        }
        break;
      default:
        fprintf(f_debug, "Node not found.");
        break;
    }  /* switch */
  } else {
    fprintf(f_debug, "Invalid %s node.\n", str_for_an_ifc_macro_sort(idx.sort));
  }  /* if */
}  /* db_node_at_idx */


void db_node_at_idx_an_ifc_name_index(an_ifc_name_index idx)
/*
Given the NameIndex, print a diagnostic textual representation of the
associated node.
*/
{
  if (validate_an_ifc_name_index(idx)) {
    switch (idx.sort) {
      case ifc_ns_name_conversion:
        { an_ifc_name_conversion universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_name_conversion(universal);
        }
        break;
      case ifc_ns_name_guide:
        { an_ifc_name_guide universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_name_guide(universal);
        }
        break;
      case ifc_ns_name_literal:
        { an_ifc_name_literal universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_name_literal(universal);
        }
        break;
      case ifc_ns_name_operator:
        { an_ifc_name_operator universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_name_operator(universal);
        }
        break;
      case ifc_ns_name_source_file:
        { an_ifc_name_source_file universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_name_source_file(universal);
        }
        break;
      case ifc_ns_name_specialization:
        { an_ifc_name_specialization universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_name_specialization(universal);
        }
        break;
      case ifc_ns_name_template:
        { an_ifc_name_template universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_name_template(universal);
        }
        break;
      default:
        fprintf(f_debug, "Node not found.");
        break;
    }  /* switch */
  } else {
    fprintf(f_debug, "Invalid %s node.\n", str_for_an_ifc_name_sort(idx.sort));
  }  /* if */
}  /* db_node_at_idx */


void db_node_at_idx_an_ifc_scope_offset(an_ifc_scope_offset idx)
/*
Given the ScopeOffset, print a diagnostic textual representation of the
associated node.
*/
{
  if (validate_an_ifc_scope_offset(idx)) {
    an_ifc_scope_descriptor universal = {0};

    construct_node_prechecked(&universal, idx);
    db_node_an_ifc_scope_descriptor(universal);
  } else {
    fputs("Invalid ScopeDescriptor node.\n", f_debug);
  }  /* if */
}  /* db_node_at_idx */


void db_node_at_idx_an_ifc_stmt_index(an_ifc_stmt_index idx)
/*
Given the StmtIndex, print a diagnostic textual representation of the
associated node.
*/
{
  if (validate_an_ifc_stmt_index(idx)) {
    switch (idx.sort) {
      case ifc_ss_stmt_block:
        { an_ifc_stmt_block universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_stmt_block(universal);
        }
        break;
      case ifc_ss_stmt_break:
        { an_ifc_stmt_break universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_stmt_break(universal);
        }
        break;
      case ifc_ss_stmt_case:
        { an_ifc_stmt_case universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_stmt_case(universal);
        }
        break;
      case ifc_ss_stmt_continue:
        { an_ifc_stmt_continue universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_stmt_continue(universal);
        }
        break;
      case ifc_ss_stmt_decl:
        { an_ifc_stmt_decl universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_stmt_decl(universal);
        }
        break;
      case ifc_ss_stmt_default:
        { an_ifc_stmt_default universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_stmt_default(universal);
        }
        break;
      case ifc_ss_stmt_do_while:
        { an_ifc_stmt_do_while universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_stmt_do_while(universal);
        }
        break;
      case ifc_ss_stmt_empty:
        { an_ifc_stmt_empty universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_stmt_empty(universal);
        }
        break;
      case ifc_ss_stmt_expansion:
        { an_ifc_stmt_expansion universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_stmt_expansion(universal);
        }
        break;
      case ifc_ss_stmt_expression:
        { an_ifc_stmt_expression universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_stmt_expression(universal);
        }
        break;
      case ifc_ss_stmt_for:
        { an_ifc_stmt_for universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_stmt_for(universal);
        }
        break;
      case ifc_ss_stmt_goto:
        { an_ifc_stmt_goto universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_stmt_goto(universal);
        }
        break;
      case ifc_ss_stmt_handler:
        { an_ifc_stmt_handler universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_stmt_handler(universal);
        }
        break;
      case ifc_ss_stmt_if:
        { an_ifc_stmt_if universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_stmt_if(universal);
        }
        break;
      case ifc_ss_stmt_labeled:
        { an_ifc_stmt_labeled universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_stmt_labeled(universal);
        }
        break;
      case ifc_ss_stmt_return:
        { an_ifc_stmt_return universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_stmt_return(universal);
        }
        break;
      case ifc_ss_stmt_switch:
        { an_ifc_stmt_switch universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_stmt_switch(universal);
        }
        break;
      case ifc_ss_stmt_try:
        { an_ifc_stmt_try universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_stmt_try(universal);
        }
        break;
      case ifc_ss_stmt_tuple:
        { an_ifc_stmt_tuple universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_stmt_tuple(universal);
        }
        break;
      case ifc_ss_stmt_variable_decl:
        { an_ifc_stmt_variable_decl universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_stmt_variable_decl(universal);
        }
        break;
      case ifc_ss_stmt_while:
        { an_ifc_stmt_while universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_stmt_while(universal);
        }
        break;
      default:
        fprintf(f_debug, "Node not found.");
        break;
    }  /* switch */
  } else {
    fprintf(f_debug, "Invalid %s node.\n", str_for_an_ifc_stmt_sort(idx.sort));
  }  /* if */
}  /* db_node_at_idx */


void db_node_at_idx_an_ifc_syntax_index(an_ifc_syntax_index idx)
/*
Given the SyntaxIndex, print a diagnostic textual representation of the
associated node.
*/
{
  if (validate_an_ifc_syntax_index(idx)) {
    switch (idx.sort) {
      case ifc_ss_syntax_access_specifier:
        { an_ifc_syntax_access_specifier universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_access_specifier(universal);
        }
        break;
      case ifc_ss_syntax_alias_declaration:
        { an_ifc_syntax_alias_declaration universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_alias_declaration(universal);
        }
        break;
      case ifc_ss_syntax_alignas:
        { an_ifc_syntax_alignas universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_alignas(universal);
        }
        break;
      case ifc_ss_syntax_array_declarator:
        { an_ifc_syntax_array_declarator universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_array_declarator(universal);
        }
        break;
      case ifc_ss_syntax_array_index:
        { an_ifc_syntax_array_index universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_array_index(universal);
        }
        break;
      case ifc_ss_syntax_array_or_function_declarator:
        { an_ifc_syntax_array_or_function_declarator universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_array_or_function_declarator(universal);
        }
        break;
      case ifc_ss_syntax_asm_statement:
        { an_ifc_syntax_asm_statement universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_asm_statement(universal);
        }
        break;
      case ifc_ss_syntax_attribute:
        { an_ifc_syntax_attribute universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_attribute(universal);
        }
        break;
      case ifc_ss_syntax_attribute_argument_clause:
        { an_ifc_syntax_attribute_argument_clause universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_attribute_argument_clause(universal);
        }
        break;
      case ifc_ss_syntax_attribute_specifier:
        { an_ifc_syntax_attribute_specifier universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_attribute_specifier(universal);
        }
        break;
      case ifc_ss_syntax_attribute_specifier_seq:
        { an_ifc_syntax_attribute_specifier_seq universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_attribute_specifier_seq(universal);
        }
        break;
      case ifc_ss_syntax_attribute_using_prefix:
        { an_ifc_syntax_attribute_using_prefix universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_attribute_using_prefix(universal);
        }
        break;
      case ifc_ss_syntax_attributed_declaration:
        { an_ifc_syntax_attributed_declaration universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_attributed_declaration(universal);
        }
        break;
      case ifc_ss_syntax_attributed_statement:
        { an_ifc_syntax_attributed_statement universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_attributed_statement(universal);
        }
        break;
      case ifc_ss_syntax_base_specifier:
        { an_ifc_syntax_base_specifier universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_base_specifier(universal);
        }
        break;
      case ifc_ss_syntax_base_specifier_list:
        { an_ifc_syntax_base_specifier_list universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_base_specifier_list(universal);
        }
        break;
      case ifc_ss_syntax_binary_fold_expression:
        { an_ifc_syntax_binary_fold_expression universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_binary_fold_expression(universal);
        }
        break;
      case ifc_ss_syntax_break_statement:
        { an_ifc_syntax_break_statement universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_break_statement(universal);
        }
        break;
      case ifc_ss_syntax_capture_default:
        { an_ifc_syntax_capture_default universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_capture_default(universal);
        }
        break;
      case ifc_ss_syntax_class_specifier:
        { an_ifc_syntax_class_specifier universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_class_specifier(universal);
        }
        break;
      case ifc_ss_syntax_compound_requirement:
        { an_ifc_syntax_compound_requirement universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_compound_requirement(universal);
        }
        break;
      case ifc_ss_syntax_compound_statement:
        { an_ifc_syntax_compound_statement universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_compound_statement(universal);
        }
        break;
      case ifc_ss_syntax_concept_definition:
        { an_ifc_syntax_concept_definition universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_concept_definition(universal);
        }
        break;
      case ifc_ss_syntax_condition_declaration:
        { an_ifc_syntax_condition_declaration universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_condition_declaration(universal);
        }
        break;
      case ifc_ss_syntax_continue_statement:
        { an_ifc_syntax_continue_statement universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_continue_statement(universal);
        }
        break;
      case ifc_ss_syntax_ctor_initializer:
        { an_ifc_syntax_ctor_initializer universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_ctor_initializer(universal);
        }
        break;
      case ifc_ss_syntax_decl_specifier_seq:
        { an_ifc_syntax_decl_specifier_seq universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_decl_specifier_seq(universal);
        }
        break;
      case ifc_ss_syntax_declaration_statement:
        { an_ifc_syntax_declaration_statement universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_declaration_statement(universal);
        }
        break;
      case ifc_ss_syntax_declarator:
        { an_ifc_syntax_declarator universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_declarator(universal);
        }
        break;
      case ifc_ss_syntax_decltype_specifier:
        { an_ifc_syntax_decltype_specifier universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_decltype_specifier(universal);
        }
        break;
      case ifc_ss_syntax_do_while_statement:
        { an_ifc_syntax_do_while_statement universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_do_while_statement(universal);
        }
        break;
      case ifc_ss_syntax_dynamic_exception_spec:
        { an_ifc_syntax_dynamic_exception_spec universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_dynamic_exception_spec(universal);
        }
        break;
      case ifc_ss_syntax_empty_statement:
        { an_ifc_syntax_empty_statement universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_empty_statement(universal);
        }
        break;
      case ifc_ss_syntax_enum_specifier:
        { an_ifc_syntax_enum_specifier universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_enum_specifier(universal);
        }
        break;
      case ifc_ss_syntax_enumerator_definition:
        { an_ifc_syntax_enumerator_definition universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_enumerator_definition(universal);
        }
        break;
      case ifc_ss_syntax_exception_declaration:
        { an_ifc_syntax_exception_declaration universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_exception_declaration(universal);
        }
        break;
      case ifc_ss_syntax_explicit_specifier:
        { an_ifc_syntax_explicit_specifier universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_explicit_specifier(universal);
        }
        break;
      case ifc_ss_syntax_expression:
        { an_ifc_syntax_expression universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_expression(universal);
        }
        break;
      case ifc_ss_syntax_expression_statement:
        { an_ifc_syntax_expression_statement universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_expression_statement(universal);
        }
        break;
      case ifc_ss_syntax_for_range_declaration:
        { an_ifc_syntax_for_range_declaration universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_for_range_declaration(universal);
        }
        break;
      case ifc_ss_syntax_for_statement:
        { an_ifc_syntax_for_statement universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_for_statement(universal);
        }
        break;
      case ifc_ss_syntax_function_body:
        { an_ifc_syntax_function_body universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_function_body(universal);
        }
        break;
      case ifc_ss_syntax_function_declarator:
        { an_ifc_syntax_function_declarator universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_function_declarator(universal);
        }
        break;
      case ifc_ss_syntax_function_definition:
        { an_ifc_syntax_function_definition universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_function_definition(universal);
        }
        break;
      case ifc_ss_syntax_function_try_block:
        { an_ifc_syntax_function_try_block universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_function_try_block(universal);
        }
        break;
      case ifc_ss_syntax_goto_statement:
        { an_ifc_syntax_goto_statement universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_goto_statement(universal);
        }
        break;
      case ifc_ss_syntax_handler:
        { an_ifc_syntax_handler universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_handler(universal);
        }
        break;
      case ifc_ss_syntax_handler_seq:
        { an_ifc_syntax_handler_seq universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_handler_seq(universal);
        }
        break;
      case ifc_ss_syntax_if_statement:
        { an_ifc_syntax_if_statement universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_if_statement(universal);
        }
        break;
      case ifc_ss_syntax_init_capture:
        { an_ifc_syntax_init_capture universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_init_capture(universal);
        }
        break;
      case ifc_ss_syntax_init_declarator:
        { an_ifc_syntax_init_declarator universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_init_declarator(universal);
        }
        break;
      case ifc_ss_syntax_init_statement:
        { an_ifc_syntax_init_statement universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_init_statement(universal);
        }
        break;
      case ifc_ss_syntax_labeled_statement:
        { an_ifc_syntax_labeled_statement universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_labeled_statement(universal);
        }
        break;
      case ifc_ss_syntax_lambda_declarator:
        { an_ifc_syntax_lambda_declarator universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_lambda_declarator(universal);
        }
        break;
      case ifc_ss_syntax_lambda_introducer:
        { an_ifc_syntax_lambda_introducer universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_lambda_introducer(universal);
        }
        break;
      case ifc_ss_syntax_mem_initializer:
        { an_ifc_syntax_mem_initializer universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_mem_initializer(universal);
        }
        break;
      case ifc_ss_syntax_member_declaration:
        { an_ifc_syntax_member_declaration universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_member_declaration(universal);
        }
        break;
      case ifc_ss_syntax_member_declarator:
        { an_ifc_syntax_member_declarator universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_member_declarator(universal);
        }
        break;
      case ifc_ss_syntax_member_function_declaration:
        { an_ifc_syntax_member_function_declaration universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_member_function_declaration(universal);
        }
        break;
      case ifc_ss_syntax_member_specification:
        { an_ifc_syntax_member_specification universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_member_specification(universal);
        }
        break;
      case ifc_ss_syntax_namespace_alias_definition:
        { an_ifc_syntax_namespace_alias_definition universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_namespace_alias_definition(universal);
        }
        break;
      case ifc_ss_syntax_nested_requirement:
        { an_ifc_syntax_nested_requirement universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_nested_requirement(universal);
        }
        break;
      case ifc_ss_syntax_new_declarator:
        { an_ifc_syntax_new_declarator universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_new_declarator(universal);
        }
        break;
      case ifc_ss_syntax_noexcept_specification:
        { an_ifc_syntax_noexcept_specification universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_noexcept_specification(universal);
        }
        break;
      case ifc_ss_syntax_non_type_template_argument:
        { an_ifc_syntax_non_type_template_argument universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_non_type_template_argument(universal);
        }
        break;
      case ifc_ss_syntax_parameter_declarator:
        { an_ifc_syntax_parameter_declarator universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_parameter_declarator(universal);
        }
        break;
      case ifc_ss_syntax_placeholder_type_specifier:
        { an_ifc_syntax_placeholder_type_specifier universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_placeholder_type_specifier(universal);
        }
        break;
      case ifc_ss_syntax_pointer_declarator:
        { an_ifc_syntax_pointer_declarator universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_pointer_declarator(universal);
        }
        break;
      case ifc_ss_syntax_range_based_for_statement:
        { an_ifc_syntax_range_based_for_statement universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_range_based_for_statement(universal);
        }
        break;
      case ifc_ss_syntax_requirement_body:
        { an_ifc_syntax_requirement_body universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_requirement_body(universal);
        }
        break;
      case ifc_ss_syntax_requires_clause:
        { an_ifc_syntax_requires_clause universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_requires_clause(universal);
        }
        break;
      case ifc_ss_syntax_return_statement:
        { an_ifc_syntax_return_statement universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_return_statement(universal);
        }
        break;
      case ifc_ss_syntax_seh_except:
        { an_ifc_syntax_seh_except universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_seh_except(universal);
        }
        break;
      case ifc_ss_syntax_seh_finally:
        { an_ifc_syntax_seh_finally universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_seh_finally(universal);
        }
        break;
      case ifc_ss_syntax_seh_leave:
        { an_ifc_syntax_seh_leave universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_seh_leave(universal);
        }
        break;
      case ifc_ss_syntax_seh_try:
        { an_ifc_syntax_seh_try universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_seh_try(universal);
        }
        break;
      case ifc_ss_syntax_simple_capture:
        { an_ifc_syntax_simple_capture universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_simple_capture(universal);
        }
        break;
      case ifc_ss_syntax_simple_declaration:
        { an_ifc_syntax_simple_declaration universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_simple_declaration(universal);
        }
        break;
      case ifc_ss_syntax_simple_requirement:
        { an_ifc_syntax_simple_requirement universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_simple_requirement(universal);
        }
        break;
      case ifc_ss_syntax_simple_type_specifier:
        { an_ifc_syntax_simple_type_specifier universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_simple_type_specifier(universal);
        }
        break;
      case ifc_ss_syntax_statement_seq:
        { an_ifc_syntax_statement_seq universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_statement_seq(universal);
        }
        break;
      case ifc_ss_syntax_static_assert_declaration:
        { an_ifc_syntax_static_assert_declaration universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_static_assert_declaration(universal);
        }
        break;
      case ifc_ss_syntax_structured_binding_declaration:
        { an_ifc_syntax_structured_binding_declaration universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_structured_binding_declaration(universal);
        }
        break;
      case ifc_ss_syntax_structured_binding_identifier:
        { an_ifc_syntax_structured_binding_identifier universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_structured_binding_identifier(universal);
        }
        break;
      case ifc_ss_syntax_super:
        { an_ifc_syntax_super universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_super(universal);
        }
        break;
      case ifc_ss_syntax_switch_statement:
        { an_ifc_syntax_switch_statement universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_switch_statement(universal);
        }
        break;
      case ifc_ss_syntax_template_argument_list:
        { an_ifc_syntax_template_argument_list universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_template_argument_list(universal);
        }
        break;
      case ifc_ss_syntax_template_declaration:
        { an_ifc_syntax_template_declaration universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_template_declaration(universal);
        }
        break;
      case ifc_ss_syntax_template_id:
        { an_ifc_syntax_template_id universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_template_id(universal);
        }
        break;
      case ifc_ss_syntax_template_parameter_list:
        { an_ifc_syntax_template_parameter_list universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_template_parameter_list(universal);
        }
        break;
      case ifc_ss_syntax_template_template_parameter:
        { an_ifc_syntax_template_template_parameter universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_template_template_parameter(universal);
        }
        break;
      case ifc_ss_syntax_this_capture:
        { an_ifc_syntax_this_capture universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_this_capture(universal);
        }
        break;
      case ifc_ss_syntax_trailing_return_type:
        { an_ifc_syntax_trailing_return_type universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_trailing_return_type(universal);
        }
        break;
      case ifc_ss_syntax_try_block:
        { an_ifc_syntax_try_block universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_try_block(universal);
        }
        break;
      case ifc_ss_syntax_tuple:
        { an_ifc_syntax_tuple universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_tuple(universal);
        }
        break;
      case ifc_ss_syntax_type_id:
        { an_ifc_syntax_type_id universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_type_id(universal);
        }
        break;
      case ifc_ss_syntax_type_id_list_element:
        { an_ifc_syntax_type_id_list_element universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_type_id_list_element(universal);
        }
        break;
      case ifc_ss_syntax_type_requirement:
        { an_ifc_syntax_type_requirement universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_type_requirement(universal);
        }
        break;
      case ifc_ss_syntax_type_specifier_seq:
        { an_ifc_syntax_type_specifier_seq universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_type_specifier_seq(universal);
        }
        break;
      case ifc_ss_syntax_type_template_argument:
        { an_ifc_syntax_type_template_argument universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_type_template_argument(universal);
        }
        break;
      case ifc_ss_syntax_type_template_parameter:
        { an_ifc_syntax_type_template_parameter universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_type_template_parameter(universal);
        }
        break;
      case ifc_ss_syntax_type_trait_intrinsic:
        { an_ifc_syntax_type_trait_intrinsic universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_type_trait_intrinsic(universal);
        }
        break;
      case ifc_ss_syntax_unary_fold_expression:
        { an_ifc_syntax_unary_fold_expression universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_unary_fold_expression(universal);
        }
        break;
      case ifc_ss_syntax_using_declaration:
        { an_ifc_syntax_using_declaration universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_using_declaration(universal);
        }
        break;
      case ifc_ss_syntax_using_declarator:
        { an_ifc_syntax_using_declarator universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_using_declarator(universal);
        }
        break;
      case ifc_ss_syntax_using_directive:
        { an_ifc_syntax_using_directive universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_using_directive(universal);
        }
        break;
      case ifc_ss_syntax_using_enum_declaration:
        { an_ifc_syntax_using_enum_declaration universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_using_enum_declaration(universal);
        }
        break;
      case ifc_ss_syntax_virtual_specifier_seq:
        { an_ifc_syntax_virtual_specifier_seq universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_virtual_specifier_seq(universal);
        }
        break;
      case ifc_ss_syntax_while_statement:
        { an_ifc_syntax_while_statement universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_syntax_while_statement(universal);
        }
        break;
      default:
        fprintf(f_debug, "Node not found.");
        break;
    }  /* switch */
  } else {
    fprintf(f_debug, "Invalid %s node.\n", str_for_an_ifc_syntax_sort(idx.sort));
  }  /* if */
}  /* db_node_at_idx */


void db_node_at_idx_an_ifc_type_index(an_ifc_type_index idx)
/*
Given the TypeIndex, print a diagnostic textual representation of the
associated node.
*/
{
  if (validate_an_ifc_type_index(idx)) {
    switch (idx.sort) {
      case ifc_ts_type_array:
        { an_ifc_type_array universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_type_array(universal);
        }
        break;
      case ifc_ts_type_base:
        { an_ifc_type_base universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_type_base(universal);
        }
        break;
      case ifc_ts_type_decltype:
        { an_ifc_type_decltype universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_type_decltype(universal);
        }
        break;
      case ifc_ts_type_designated:
        { an_ifc_type_designated universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_type_designated(universal);
        }
        break;
      case ifc_ts_type_expansion:
        { an_ifc_type_expansion universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_type_expansion(universal);
        }
        break;
      case ifc_ts_type_forall:
        { an_ifc_type_forall universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_type_forall(universal);
        }
        break;
      case ifc_ts_type_function:
        { an_ifc_type_function universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_type_function(universal);
        }
        break;
      case ifc_ts_type_fundamental:
        { an_ifc_type_fundamental universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_type_fundamental(universal);
        }
        break;
      case ifc_ts_type_lvalue_reference:
        { an_ifc_type_lvalue_reference universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_type_lvalue_reference(universal);
        }
        break;
      case ifc_ts_type_method:
        { an_ifc_type_method universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_type_method(universal);
        }
        break;
      case ifc_ts_type_placeholder:
        { an_ifc_type_placeholder universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_type_placeholder(universal);
        }
        break;
      case ifc_ts_type_pointer:
        { an_ifc_type_pointer universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_type_pointer(universal);
        }
        break;
      case ifc_ts_type_pointer_to_member:
        { an_ifc_type_pointer_to_member universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_type_pointer_to_member(universal);
        }
        break;
      case ifc_ts_type_qualified:
        { an_ifc_type_qualified universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_type_qualified(universal);
        }
        break;
      case ifc_ts_type_rvalue_reference:
        { an_ifc_type_rvalue_reference universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_type_rvalue_reference(universal);
        }
        break;
      case ifc_ts_type_syntactic:
        { an_ifc_type_syntactic universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_type_syntactic(universal);
        }
        break;
      case ifc_ts_type_syntax_tree:
        { an_ifc_type_syntax_tree universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_type_syntax_tree(universal);
        }
        break;
      case ifc_ts_type_tor:
        { an_ifc_type_tor universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_type_tor(universal);
        }
        break;
      case ifc_ts_type_tuple:
        { an_ifc_type_tuple universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_type_tuple(universal);
        }
        break;
      case ifc_ts_type_typename:
        { an_ifc_type_typename universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_type_typename(universal);
        }
        break;
      case ifc_ts_type_unaligned:
        { an_ifc_type_unaligned universal = {0};

          construct_node_prechecked(&universal, idx);
          db_node_an_ifc_type_unaligned(universal);
        }
        break;
      default:
        fprintf(f_debug, "Node not found.");
        break;
    }  /* switch */
  } else {
    fprintf(f_debug, "Invalid %s node.\n", str_for_an_ifc_type_sort(idx.sort));
  }  /* if */
}  /* db_node_at_idx */


/* End the suppression of GCC warnings. */
END_DISABLE_GCC_WARNING_MAYBE_UNITIALIZED

/* Conditionally close the "edg" namespace. */
END_EDG_NAMESPACE

#endif /* DEBUG */

#endif /* !STANDALONE_UTILITY_PROGRAM */

