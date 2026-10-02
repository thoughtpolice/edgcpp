/*
Part of the EDG Compiler Project, under the Apache License v2.0 with LLVM
Exceptions.
See https://edgcpp.org/LICENSE.txt for license information.
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
*/

/* Concrete C IFC construction and validation helpers. */
#include "basic_hdrs.h"
#include "checking.h"
#include "error.h"
#include "header_util.h"
#include "util.h"
#include "mem_manage.h"
#include "ifc_map.h"
#include "ifc_map_functions.h"
#include "ifc_modules_internal.h"
#if !STANDALONE_UTILITY_PROGRAM

an_ifc_partition_metadata *ifc_partition_metadata(an_ifc_partition_kind_index idx)
{
  return get_partition_metadata_for_state(input_state_for(idx),idx.partition_kind);
}

Opt_size_t ifc_partition_offset(an_ifc_partition_kind_index idx)
{
  Opt_size_t result={0};
  an_ifc_partition_metadata *part=ifc_partition_metadata(idx);
  uint64_t element_offset;
  size_t offset;
  if (checked_multiplication(&element_offset,idx.value,part->entry_size) &&
      checked_addition(&offset,part->offset,element_offset)) {
    result.has_value=TRUE;
    result.value=offset;
  }
  return result;
}

static void read_partition_element(an_ifc_partition_kind_index idx)
{
#if EXPENSIVE_CHECKING
  check_assertion(validate_element_exists(idx.file,idx.partition_kind,idx.value,NULL));
#endif
  an_ifc_partition_metadata *part=ifc_partition_metadata(idx);
  Opt_size_t offset=ifc_partition_offset(idx);
  check_assertion(offset.has_value);
  init_byte_buffer(idx.file,offset.value,part->size);
}

static a_boolean has_been_validated(an_ifc_partition_kind_index idx)
{
  uint32_t index=idx.value;
  unsigned mask=1u<<(index%16);
  return (ifc_partition_metadata(idx)->format_validated[index/16]&mask)!=0;
}

static a_boolean is_marked_invalid(an_ifc_partition_kind_index idx)
{
  check_assertion(has_been_validated(idx));
  uint32_t index=idx.value;
  unsigned mask=(1u<<16)<<(index%16);
  return (ifc_partition_metadata(idx)->format_validated[index/16]&mask)!=0;
}

static void mark_validated(an_ifc_partition_kind_index idx,a_boolean valid)
{
  uint32_t index=idx.value;
  unsigned mask=1u<<(index%16);
  if (!valid) mask|=mask<<16;
  ifc_partition_metadata(idx)->format_validated[index/16]|=mask;
}

static a_boolean check_partition_kind(an_ifc_partition_kind_index idx,
                                       an_ifc_partition_kind expected)
{
  if (idx.partition_kind==expected) return TRUE;
  a_string_view expected_name=get_partition_name_from_kind(expected);
  if (idx.partition_kind==ifc_pk_none) {
    error(ec_ifc_partition_missing_element,expected_name);
  } else {
    a_string_view actual_name=get_partition_name_from_kind(idx.partition_kind);
    error(ec_ifc_partition_mismatch,expected_name,actual_name);
  }
  return FALSE;
}

#define IFC_STORAGE_TYPE(TYPE) \
size_t get_byte_buffer_size_##TYPE(an_ifc_module_file *file) \
{ return get_ifc_buffer_size_##TYPE(file); }

#define IFC_NODE_TYPE(TYPE) \
TYPE construct_node_from_module_##TYPE(an_ifc_module_file *file) \
{ \
  TYPE##_storage local; \
  TYPE##_storage *storage=get_##TYPE##_storage(file,&local,FALSE); \
  if (storage!=&local) return TYPE##_from_storage_pointer(file,storage); \
  return TYPE##_from_storage_copy(file,&local); \
} \
void construct_node_##TYPE(Opt_##TYPE *result,an_ifc_partition_kind_index idx) \
{ \
  if (!check_partition_kind(idx,get_ifc_partition_kind_##TYPE())) return; \
  read_partition_element(idx); \
  TYPE value=construct_node_from_module_##TYPE(idx.file); \
  if (!has_been_validated(idx)) { \
    a_diag_count_snapshot snapshot=edg_diag_count_snapshot(); \
    an_ifc_validation_trace trace=an_ifc_validation_trace_partition(idx.file,idx.partition_kind,idx.value,NULL); \
    a_boolean valid=validate_##TYPE(value,&trace); \
    mark_validated(idx,valid); \
    if (!valid) expect_error_since(&snapshot,"expected errors from the validator"); \
  } \
  if (!is_marked_invalid(idx)) { result->value=value;result->has_value=TRUE; } \
} \
void construct_node_prechecked_##TYPE(TYPE *result,an_ifc_partition_kind_index idx) \
{ \
  check_assertion(has_been_validated(idx) && !is_marked_invalid(idx)); \
  check_assertion(get_ifc_partition_kind_##TYPE()==idx.partition_kind); \
  read_partition_element(idx); \
  *result=construct_node_from_module_##TYPE(idx.file); \
} \
void construct_node_unchecked_##TYPE(TYPE *result,an_ifc_partition_kind_index idx) \
{ \
  check_assertion(get_ifc_partition_kind_##TYPE()==idx.partition_kind); \
  read_partition_element(idx); \
  *result=construct_node_from_module_##TYPE(idx.file); \
}
an_ifc_file_header construct_node_from_module_an_ifc_file_header(an_ifc_module_file *file)
{
  an_ifc_file_header_storage local;
  an_ifc_file_header_storage *storage=get_an_ifc_file_header_storage(file,&local,FALSE);
  if (storage!=&local) return an_ifc_file_header_from_storage_pointer(file,storage);
  return an_ifc_file_header_from_storage_copy(file,&local);
}
an_ifc_partition construct_node_from_module_an_ifc_partition(an_ifc_module_file *file)
{
  an_ifc_partition_storage local;
  an_ifc_partition_storage *storage=get_an_ifc_partition_storage(file,&local,FALSE);
  if (storage!=&local) return an_ifc_partition_from_storage_pointer(file,storage);
  return an_ifc_partition_from_storage_copy(file,&local);
}
#include "ifc_modules_inst.h"
#undef IFC_NODE_TYPE
#undef IFC_STORAGE_TYPE
#endif
