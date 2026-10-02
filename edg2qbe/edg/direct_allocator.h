/*
Part of the EDG Compiler Project, under the Apache License v2.0 with LLVM
Exceptions.
See https://edgcpp.org/LICENSE.txt for license information.
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
*/

#ifndef EDG_DIRECT_ALLOCATOR_H
#define EDG_DIRECT_ALLOCATOR_H 1

#include "util.h"

/* Direct allocation never enters frontend or general-memory bookkeeping. */
static inline void *edg_direct_alloc(size_t bytes, size_t *actual)
{
  return edg_alloc(EDG_ALLOC_DIRECT, bytes, actual);
}

static inline void *edg_direct_realloc(void *ptr, size_t bytes)
{
  void *result;
  if (bytes > (size_t)PTRDIFF_MAX) edg_util_allocation_failure();
  if (bytes == 0) {
    free(ptr);
    return NULL;
  }
  result = realloc(ptr, bytes);
  if (result == NULL) edg_util_allocation_failure();
  return result;
}

static inline void edg_direct_dealloc(void *ptr)
{
  free(ptr);
}

/* Byte-relocatable objects can grow with realloc.  Objects with self-relative
   state instead use a consuming relocation callback before releasing the old
   storage.  No destructor is run on the consumed source elements. */
static inline void *edg_direct_replace_alloc(void *ptr, size_t capacity,
                                                size_t count, size_t elem_size,
                                                Edg_relocate_fn relocate)
{
  size_t bytes = edg_size_multiply(capacity, elem_size);
  void *result;
  EDG_UTIL_ASSERT(count <= capacity);
  if (relocate == NULL) return edg_direct_realloc(ptr, bytes);
  result = edg_direct_alloc(bytes, NULL);
  edg_relocate_elements(result, ptr, count, elem_size, relocate);
  free(ptr);
  return result;
}

static inline void *edg_new_direct(size_t size, const void *value,
                                     Edg_copy_fn copy)
{
  return edg_new_object(EDG_ALLOC_DIRECT, size, value, copy);
}

/* pointer_address points at a typed pointer field; memcpy avoids violating
   aliasing rules and, like the original deleter, clears the caller's pointer. */
static inline void edg_delete_direct(void *pointer_address,
                                        Edg_destroy_fn destroy)
{
  void *ptr = edg_list_link_get(pointer_address);
  if (ptr != NULL) {
    if (destroy != NULL) destroy(ptr);
    free(ptr);
    edg_list_link_set(pointer_address, NULL);
  }
}

#endif /* EDG_DIRECT_ALLOCATOR_H */
