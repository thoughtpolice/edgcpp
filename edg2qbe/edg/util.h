/*
Part of the EDG Compiler Project, under the Apache License v2.0 with LLVM
Exceptions.
See https://edgcpp.org/LICENSE.txt for license information.
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
*/

#ifndef EDG_UTIL_H
#define EDG_UTIL_H 1

#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header_util.h"

#if defined(EDG_STANDALONE_UTIL) && EDG_STANDALONE_UTIL
#define EDG_UTIL_STANDALONE 1
#elif defined(STANDALONE_UTILITY_PROGRAM) && STANDALONE_UTILITY_PROGRAM
#define EDG_UTIL_STANDALONE 1
#else
#define EDG_UTIL_STANDALONE 0
extern char *alloc_fe(sizeof_t size);
extern char *alloc_fe_var_size(sizeof_t size, sizeof_t *actual_size);
extern void free_fe(a_void_ptr ptr, sizeof_t size);
extern void free_fe_var_size(a_void_ptr ptr, sizeof_t size);
extern char *alloc_general(sizeof_t size);
extern void free_general(a_void_ptr ptr, sizeof_t size);
extern char *alloc_il(sizeof_t size);
NORETURN extern void insufficient_address_space(void);
#endif

#ifdef check_assertion
#define EDG_UTIL_ASSERT(condition) check_assertion(condition)
#else
#define EDG_UTIL_ASSERT(condition) assert(condition)
#endif

typedef enum Edg_allocator {
  EDG_ALLOC_FE, EDG_ALLOC_GENERAL, EDG_ALLOC_DIRECT, EDG_ALLOC_IL
} Edg_allocator;

typedef void (*Edg_copy_fn)(void *dst, const void *src);
typedef void (*Edg_destroy_fn)(void *obj);
typedef size_t (*Edg_hash_fn)(const void *key);
typedef a_boolean (*Edg_equal_fn)(const void *a, const void *b);
typedef a_boolean (*Edg_predicate_fn)(const void *obj, void *context);
typedef int (*Edg_compare_fn)(const void *a, const void *b, void *context);
/* Relocation consumes src without copying its owned children.  Most C values
   are byte-relocatable.  Values containing pointers into their own storage
   supply this callback to repair those pointers. */
typedef void (*Edg_relocate_fn)(void *dst, void *src);

#define EDG_CONTAINER_CALLBACK_ENCODING_VERSION 1

/* FE container records can be saved in a PCH.  A code address itself is not
   persistent across PIE/ASLR loads, but its offset from the same externally
   linked code anchor is.  The supported hosts round-trip function pointers
   through uintptr_t.  Stored callbacks must only be invoked through decoding
   accessors; NULL is kept as NULL.  Standalone programs do not save PCHs. */
#if EDG_UTIL_STANDALONE
#define EDG_CALLBACK_DEFINE(kind, type) \
  static inline type edg_callback_encode_##kind(type fn) { return fn; } \
  static inline type edg_callback_decode_##kind(type fn) { return fn; }
#else
#define EDG_CALLBACK_DEFINE(kind, type) \
  static inline type edg_callback_encode_##kind(type fn) \
    { return fn == NULL ? NULL : \
        (type)((uintptr_t)fn - (uintptr_t)&alloc_fe); } \
  static inline type edg_callback_decode_##kind(type fn) \
    { return fn == NULL ? NULL : \
        (type)((uintptr_t)fn + (uintptr_t)&alloc_fe); }
#endif
EDG_CALLBACK_DEFINE(copy, Edg_copy_fn)
EDG_CALLBACK_DEFINE(destroy, Edg_destroy_fn)
EDG_CALLBACK_DEFINE(hash, Edg_hash_fn)
EDG_CALLBACK_DEFINE(equal, Edg_equal_fn)
EDG_CALLBACK_DEFINE(relocate, Edg_relocate_fn)
#undef EDG_CALLBACK_DEFINE

#define EDG_CALLBACK_ACCESSORS(owner, structure, field, kind, type) \
  static inline void edg_##owner##_set_##field(structure *obj, type fn) \
    { obj->field = edg_callback_encode_##kind(fn); } \
  static inline type edg_##owner##_get_##field(const structure *obj) \
    { return edg_callback_decode_##kind(obj->field); }

static inline void edg_util_allocation_failure(void)
{
#if EDG_UTIL_STANDALONE
  fputs("Insufficient address space\n", stderr);
  abort();
#else
  insufficient_address_space();
#endif
}

static inline size_t edg_size_add(size_t a, size_t b)
{
  if (b > (size_t)PTRDIFF_MAX || a > (size_t)PTRDIFF_MAX - b) {
    edg_util_allocation_failure();
  }
  return a + b;
}

static inline size_t edg_size_multiply(size_t a, size_t b)
{
  if (b != 0 && a > (size_t)PTRDIFF_MAX / b) {
    edg_util_allocation_failure();
  }
  return a * b;
}

static inline void *edg_alloc(Edg_allocator allocator, size_t requested,
                              size_t *actual)
{
  void *result;
  size_t bytes = requested;
  if (requested > (size_t)PTRDIFF_MAX) edg_util_allocation_failure();
  if (requested == 0) {
    if (actual != NULL) *actual = 0;
    return NULL;
  }
#if EDG_UTIL_STANDALONE
  (void)allocator;
  result = malloc(requested);
#else
  switch (allocator) {
    case EDG_ALLOC_FE: {
      sizeof_t allocated;
      result = alloc_fe_var_size((sizeof_t)requested, &allocated);
      bytes = (size_t)allocated;
      break;
    }
    case EDG_ALLOC_GENERAL:
      result = alloc_general((sizeof_t)requested);
      break;
    case EDG_ALLOC_IL:
      result = alloc_il((sizeof_t)requested);
      break;
    default:
      result = malloc(requested);
      break;
  }
#endif
  if (result == NULL) edg_util_allocation_failure();
  if (actual != NULL) *actual = bytes;
  return result;
}

static inline void edg_dealloc(Edg_allocator allocator, void *ptr, size_t bytes)
{
  if (ptr == NULL) return;
#if EDG_UTIL_STANDALONE
  (void)allocator;
  (void)bytes;
  free(ptr);
#else
  switch (allocator) {
    case EDG_ALLOC_FE:
      free_fe_var_size(ptr, (sizeof_t)bytes);
      break;
    case EDG_ALLOC_GENERAL:
      free_general(ptr, (sizeof_t)bytes);
      break;
    case EDG_ALLOC_IL:
      /* IL allocations belong to the current IL region, not a free list. */
      break;
    default:
      free(ptr);
      break;
  }
#endif
}

static inline void *edg_new_object(Edg_allocator allocator, size_t size,
                                   const void *value, Edg_copy_fn copy)
{
  void *result;
#if !EDG_UTIL_STANDALONE
  if (allocator == EDG_ALLOC_FE) {
    result = alloc_fe((sizeof_t)size);
    if (result == NULL) edg_util_allocation_failure();
  } else
#endif
  {
    result = edg_alloc(allocator, size, NULL);
  }
  if (value == NULL) memset(result, 0, size);
  else if (copy != NULL) copy(result, value);
  else memcpy(result, value, size);
  return result;
}

static inline void edg_delete_object(Edg_allocator allocator, void *object,
                                      size_t size, Edg_destroy_fn destroy)
{
  if (object == NULL) return;
  if (destroy != NULL) destroy(object);
#if !EDG_UTIL_STANDALONE
  if (allocator == EDG_ALLOC_FE) free_fe(object, (sizeof_t)size);
  else
#endif
  edg_dealloc(allocator, object, size);
}

static inline void edg_copy_elements(void *dst, const void *src, size_t count,
                                      size_t elem_size, Edg_copy_fn copy)
{
  size_t i;
  if (count == 0) return;
  if (copy == NULL) {
    memcpy(dst, src, edg_size_multiply(count, elem_size));
  } else {
    for (i = 0; i < count; ++i) {
      copy((char *)dst + i * elem_size, (const char *)src + i * elem_size);
    }
  }
}

static inline void *edg_new_copy_of_elements(const void *src, size_t count,
                                               size_t elem_size,
                                               Edg_allocator allocator,
                                               Edg_copy_fn copy)
{
  void *result = edg_alloc(allocator, edg_size_multiply(count, elem_size), NULL);
  edg_copy_elements(result, src, count, elem_size, copy);
  return result;
}

static inline char *edg_new_copy_of_string(const char *string,
                                            Edg_allocator allocator)
{
  return (char *)edg_new_copy_of_elements(string, edg_size_add(strlen(string), 1),
                                            1, allocator, NULL);
}

static inline void edg_destroy_elements(void *data, size_t count,
                                         size_t elem_size,
                                         Edg_destroy_fn destroy)
{
  size_t i;
  if (destroy != NULL) {
    for (i = 0; i < count; ++i) destroy((char *)data + i * elem_size);
  }
}

static inline void edg_relocate_elements(void *dst, void *src, size_t count,
                                          size_t elem_size,
                                          Edg_relocate_fn relocate)
{
  size_t i;
  if (dst == src || count == 0) return;
  if (relocate == NULL) {
    memmove(dst, src, edg_size_multiply(count, elem_size));
  } else if ((uintptr_t)dst < (uintptr_t)src) {
    for (i = 0; i < count; ++i) {
      relocate((char *)dst + i * elem_size, (char *)src + i * elem_size);
    }
  } else {
    for (i = count; i != 0; --i) {
      relocate((char *)dst + (i - 1) * elem_size,
               (char *)src + (i - 1) * elem_size);
    }
  }
}

static inline a_boolean edg_storage_contains(const void *data, size_t bytes,
                                               const void *ptr)
{
  uintptr_t base = (uintptr_t)data, addr = (uintptr_t)ptr;
  return data != NULL && ptr != NULL && addr >= base && addr - base < bytes;
}

typedef struct Edg_array {
  void *data;
  size_t len, cap, elem_size, allocation_bytes;
  Edg_allocator allocator;
  Edg_copy_fn copy;
  Edg_destroy_fn destroy;
  void *inline_buffer;
  size_t inline_capacity;
  Edg_relocate_fn relocate;
} Edg_array;

EDG_CALLBACK_ACCESSORS(array, Edg_array, copy, copy, Edg_copy_fn)
EDG_CALLBACK_ACCESSORS(array, Edg_array, destroy, destroy, Edg_destroy_fn)
EDG_CALLBACK_ACCESSORS(array, Edg_array, relocate, relocate, Edg_relocate_fn)

static inline void edg_array_reserve(Edg_array *array, size_t capacity)
{
  size_t bytes;
  void *data;
  if (capacity <= array->cap) return;
  EDG_UTIL_ASSERT(array->elem_size != 0);
  if ((array->allocator == EDG_ALLOC_DIRECT || EDG_UTIL_STANDALONE) &&
      array->relocate == NULL && array->data != array->inline_buffer) {
    bytes = edg_size_multiply(capacity, array->elem_size);
    data = realloc(array->data, bytes);
    if (data == NULL) edg_util_allocation_failure();
    array->data = data;
    array->allocation_bytes = bytes;
    array->cap = capacity;
    return;
  }
  data = edg_alloc(array->allocator,
                   edg_size_multiply(capacity, array->elem_size), &bytes);
  edg_relocate_elements(data, array->data, array->len, array->elem_size,
                        edg_array_get_relocate(array));
  if (array->data != array->inline_buffer) {
    edg_dealloc(array->allocator, array->data, array->allocation_bytes);
  }
  array->data = data;
  array->allocation_bytes = bytes;
  array->cap = bytes / array->elem_size;
}

static inline void edg_array_init_buffer(Edg_array *array, size_t elem_size,
                                          Edg_allocator allocator,
                                          size_t capacity, Edg_copy_fn copy,
                                          Edg_destroy_fn destroy, void *buffer,
                                          size_t inline_capacity)
{
  EDG_UTIL_ASSERT(elem_size != 0);
  EDG_UTIL_ASSERT(buffer != NULL || inline_capacity == 0);
  memset(array, 0, sizeof(*array));
  array->elem_size = elem_size;
  array->allocator = allocator;
  edg_array_set_copy(array, copy);
  edg_array_set_destroy(array, destroy);
  array->inline_buffer = buffer;
  array->inline_capacity = inline_capacity;
  array->data = buffer;
  array->cap = inline_capacity;
  edg_array_reserve(array, capacity);
}

static inline void edg_array_init(Edg_array *array, size_t elem_size,
                                  Edg_allocator allocator, size_t capacity,
                                  Edg_copy_fn copy, Edg_destroy_fn destroy)
{
  edg_array_init_buffer(array, elem_size, allocator, capacity, copy, destroy,
                        NULL, 0);
}

static inline void edg_array_clear(Edg_array *array)
{
  edg_destroy_elements(array->data, array->len, array->elem_size,
                        edg_array_get_destroy(array));
  array->len = 0;
}

static inline void edg_array_destroy(Edg_array *array)
{
  edg_array_clear(array);
  if (array->data != array->inline_buffer) {
    edg_dealloc(array->allocator, array->data, array->allocation_bytes);
  }
  array->data = array->inline_buffer;
  array->cap = array->inline_capacity;
  array->allocation_bytes = 0;
}

static inline void *edg_array_at(const Edg_array *array, size_t index)
{
#if defined(EXPENSIVE_CHECKING) && EXPENSIVE_CHECKING
  EDG_UTIL_ASSERT(index < array->len);
#endif
  return (char *)array->data + index * array->elem_size;
}

static inline void edg_array_resize(Edg_array *array, size_t len)
{
  if (len > array->len) {
    edg_array_reserve(array, len);
    memset((char *)array->data + array->len * array->elem_size, 0,
            edg_size_multiply(len - array->len, array->elem_size));
  } else if (len < array->len) {
    edg_destroy_elements((char *)array->data + len * array->elem_size,
                          array->len - len, array->elem_size,
                          edg_array_get_destroy(array));
  }
  array->len = len;
}

static inline void edg_array_grow(Edg_array *array, size_t minimum)
{
  size_t capacity = array->cap < 2 ? 2 :
                    edg_size_add(array->cap, array->cap / 2 + 1);
  edg_array_reserve(array, capacity < minimum ? minimum : capacity);
}

static inline void edg_array_insert(Edg_array *array, size_t index,
                                     const void *values, size_t count)
{
  Edg_array temporary;
  a_boolean aliased;
  size_t len;
  if (count == 0) return;
  EDG_UTIL_ASSERT(index <= array->len);
  len = edg_size_add(array->len, count);
  aliased = edg_storage_contains(array->data,
                                  edg_size_multiply(array->len,
                                                     array->elem_size), values);
  if (aliased) {
    edg_array_init(&temporary, array->elem_size, EDG_ALLOC_DIRECT, count,
                    edg_array_get_copy(array), edg_array_get_destroy(array));
    temporary.relocate = array->relocate;
    edg_copy_elements(temporary.data, values, count, array->elem_size,
                       edg_array_get_copy(array));
    temporary.len = count;
    values = temporary.data;
  }
  if (len > array->cap) edg_array_grow(array, len);
  edg_relocate_elements((char *)array->data + (index + count) * array->elem_size,
                         (char *)array->data + index * array->elem_size,
                         array->len - index, array->elem_size,
                         edg_array_get_relocate(array));
  if (values == NULL) {
    memset((char *)array->data + index * array->elem_size, 0,
            edg_size_multiply(count, array->elem_size));
  } else if (aliased) {
    edg_relocate_elements((char *)array->data + index * array->elem_size,
                           temporary.data, count, array->elem_size,
                           edg_array_get_relocate(array));
    temporary.len = 0;
    edg_array_destroy(&temporary);
  } else {
    edg_copy_elements((char *)array->data + index * array->elem_size, values,
                       count, array->elem_size, edg_array_get_copy(array));
  }
  array->len = len;
}

static inline void edg_array_push(Edg_array *array, const void *value)
{
  edg_array_insert(array, array->len, value, 1);
}

static inline void edg_array_remove(Edg_array *array, size_t index, size_t count)
{
  EDG_UTIL_ASSERT(index <= array->len && count <= array->len - index);
  if (count == 0) return;
  edg_destroy_elements((char *)array->data + index * array->elem_size, count,
                        array->elem_size, edg_array_get_destroy(array));
  edg_relocate_elements((char *)array->data + index * array->elem_size,
                         (char *)array->data + (index + count) * array->elem_size,
                         array->len - index - count, array->elem_size,
                         edg_array_get_relocate(array));
  array->len -= count;
}

static inline void edg_array_copy(Edg_array *dst, const Edg_array *src)
{
  if (dst == src) return;
  if (dst->elem_size == 0) {
    edg_array_init(dst, src->elem_size, src->allocator, 0,
                    edg_array_get_copy(src), edg_array_get_destroy(src));
  } else {
    EDG_UTIL_ASSERT(dst->elem_size == src->elem_size);
    edg_array_clear(dst);
    dst->copy = src->copy;
    dst->destroy = src->destroy;
  }
  dst->relocate = src->relocate;
  edg_array_reserve(dst, src->len);
  edg_copy_elements(dst->data, src->data, src->len, src->elem_size,
                      edg_array_get_copy(src));
  dst->len = src->len;
}

static inline void edg_array_move(Edg_array *dst, Edg_array *src)
{
  void *buffer;
  size_t capacity;
  if (dst == src) return;
  EDG_UTIL_ASSERT(dst->elem_size == 0 || dst->elem_size == src->elem_size);
  edg_array_destroy(dst);
  buffer = dst->inline_buffer;
  capacity = dst->inline_capacity;
  dst->elem_size = src->elem_size;
  dst->allocator = src->allocator;
  dst->copy = src->copy;
  dst->destroy = src->destroy;
  dst->relocate = src->relocate;
  if (src->data != src->inline_buffer) {
    dst->data = src->data;
    dst->cap = src->cap;
    dst->allocation_bytes = src->allocation_bytes;
  } else {
    dst->data = buffer;
    dst->cap = capacity;
    edg_array_reserve(dst, src->len);
    edg_relocate_elements(dst->data, src->data, src->len, src->elem_size,
                           edg_array_get_relocate(src));
  }
  dst->len = src->len;
  src->data = src->inline_buffer;
  src->cap = src->inline_capacity;
  src->len = src->allocation_bytes = 0;
}

static inline void edg_array_remove_if(Edg_array *array, size_t first,
                                        Edg_predicate_fn predicate,
                                        void *context)
{
  size_t read_index, write_index = first;
  EDG_UTIL_ASSERT(first <= array->len);
  for (read_index = first; read_index < array->len; ++read_index) {
    void *element = edg_array_at(array, read_index);
    if (predicate(element, context)) {
      if (array->destroy != NULL) edg_array_get_destroy(array)(element);
    } else {
      if (read_index != write_index) {
        edg_relocate_elements(edg_array_at(array, write_index), element, 1,
                               array->elem_size, edg_array_get_relocate(array));
      }
      ++write_index;
    }
  }
  array->len = write_index;
}

static inline void edg_array_insert_repeat(Edg_array *array, size_t index,
                                            const void *value, size_t count)
{
  size_t i, offset = 0;
  a_boolean aliased;
  if (count == 0) return;
  aliased = edg_storage_contains(array->data,
                                  edg_size_multiply(array->len, array->elem_size),
                                  value);
  if (aliased) offset = (size_t)((uintptr_t)value - (uintptr_t)array->data);
  edg_array_insert(array, index, NULL, count);
  if (value == NULL) return;
  if (aliased) {
    if (offset >= index * array->elem_size) offset += count * array->elem_size;
    value = (const char *)array->data + offset;
  }
  for (i = 0; i < count; ++i) {
    edg_copy_elements(edg_array_at(array, index + i), value, 1,
                       array->elem_size, edg_array_get_copy(array));
  }
}

static inline void *edg_array_to_storage(const Edg_array *array,
                                           Edg_allocator allocator)
{
  void *result = edg_alloc(allocator,
                            edg_size_multiply(array->len, array->elem_size),
                            NULL);
  edg_copy_elements(result, array->data, array->len, array->elem_size,
                      edg_array_get_copy(array));
  return result;
}

typedef struct Edg_array_ptr {
  Edg_array *array;
  size_t offset;
} Edg_array_ptr;

static inline void *edg_array_ptr_get(Edg_array_ptr ptr)
{
  return ptr.array == NULL ? NULL : edg_array_at(ptr.array, ptr.offset);
}

static inline size_t edg_hash_bytes(const void *data, size_t len)
{
  const unsigned char *bytes = (const unsigned char *)data;
  size_t hash = 0, i;
  for (i = 0; i < len; ++i) hash = hash * 33 + bytes[i];
  return hash;
}

static inline size_t edg_hash_pointer(const void *key)
{
  const void *ptr;
  uintptr_t hash;
  memcpy(&ptr, key, sizeof(ptr));
  hash = (uintptr_t)ptr;
#if defined(HOST_ALIGNMENT_REQUIRED)
  hash /= HOST_ALIGNMENT_REQUIRED;
#else
  hash /= sizeof(void *);
#endif
  return (size_t)hash;
}

static inline a_boolean edg_equal_pointer(const void *a, const void *b)
{
  const void *left, *right;
  memcpy(&left, a, sizeof(left));
  memcpy(&right, b, sizeof(right));
  return left == right;
}

typedef struct Edg_map {
  unsigned char *data;
  size_t len, cap, allocation_bytes;
  size_t key_size, value_size, stride, key_offset, value_offset;
  Edg_allocator allocator;
  Edg_hash_fn hash;
  Edg_equal_fn equal;
  Edg_copy_fn key_copy, value_copy;
  Edg_destroy_fn key_destroy, value_destroy;
  Edg_relocate_fn key_relocate, value_relocate;
} Edg_map;

EDG_CALLBACK_ACCESSORS(map, Edg_map, hash, hash, Edg_hash_fn)
EDG_CALLBACK_ACCESSORS(map, Edg_map, equal, equal, Edg_equal_fn)
EDG_CALLBACK_ACCESSORS(map, Edg_map, key_copy, copy, Edg_copy_fn)
EDG_CALLBACK_ACCESSORS(map, Edg_map, value_copy, copy, Edg_copy_fn)
EDG_CALLBACK_ACCESSORS(map, Edg_map, key_destroy, destroy, Edg_destroy_fn)
EDG_CALLBACK_ACCESSORS(map, Edg_map, value_destroy, destroy, Edg_destroy_fn)
EDG_CALLBACK_ACCESSORS(map, Edg_map, key_relocate, relocate, Edg_relocate_fn)
EDG_CALLBACK_ACCESSORS(map, Edg_map, value_relocate, relocate, Edg_relocate_fn)

static inline size_t edg_align_size(size_t size)
{
  size_t alignment = _Alignof(max_align_t);
  return edg_size_add(size, alignment - 1) / alignment * alignment;
}

static inline size_t edg_map_hash_at(const Edg_map *map, size_t slot)
{
  size_t hash;
  memcpy(&hash, map->data + slot * map->stride + sizeof(size_t), sizeof(hash));
  return hash;
}

static inline void edg_map_store_hash(Edg_map *map, size_t slot, size_t hash)
{
  memcpy(map->data + slot * map->stride + sizeof(size_t), &hash, sizeof(hash));
}

static inline void *edg_map_key_at(const Edg_map *map, size_t slot)
{
  return map->data + slot * map->stride + map->key_offset;
}

static inline void *edg_map_value_at(const Edg_map *map, size_t slot)
{
  return map->data + slot * map->stride + map->value_offset;
}

static inline size_t edg_map_find(const Edg_map *map, const void *key,
                                   size_t hash)
{
  size_t slot = hash & (map->cap - 1);
  for (;;) {
    if (map->data[slot * map->stride] == 0) return slot;
    if (edg_map_hash_at(map, slot) == hash &&
        (map->equal != NULL ? edg_map_get_equal(map)(edg_map_key_at(map, slot), key) :
         memcmp(edg_map_key_at(map, slot), key, map->key_size) == 0)) return slot;
    slot = (slot + 1) & (map->cap - 1);
  }
}

static inline void edg_map_rehash(Edg_map *map, size_t capacity)
{
  unsigned char *old_data = map->data;
  size_t old_capacity = map->cap, old_bytes = map->allocation_bytes, i;
  map->data = (unsigned char *)edg_alloc(map->allocator,
                                        edg_size_multiply(capacity, map->stride),
                                        &map->allocation_bytes);
  map->cap = capacity;
  for (i = 0; i < capacity; ++i) map->data[i * map->stride] = 0;
  for (i = 0; i < old_capacity; ++i) {
    unsigned char *entry = old_data + i * map->stride;
    if (entry[0] != 0) {
      size_t hash, slot;
      memcpy(&hash, entry + sizeof(size_t), sizeof(hash));
      slot = hash & (capacity - 1);
      while (map->data[slot * map->stride] != 0) slot = (slot + 1) & (capacity - 1);
      map->data[slot * map->stride] = 1;
      edg_map_store_hash(map, slot, hash);
      edg_relocate_elements(edg_map_key_at(map, slot), entry + map->key_offset,
                             1, map->key_size, edg_map_get_key_relocate(map));
      edg_relocate_elements(edg_map_value_at(map, slot), entry + map->value_offset,
                             1, map->value_size, edg_map_get_value_relocate(map));
    }
  }
  edg_dealloc(map->allocator, old_data, old_bytes);
}

static inline void edg_map_init(Edg_map *map, size_t key_size, size_t value_size,
                                 Edg_allocator allocator, unsigned mask_width,
                                 Edg_hash_fn hash, Edg_equal_fn equal,
                                 Edg_copy_fn key_copy,
                                 Edg_destroy_fn key_destroy,
                                 Edg_copy_fn value_copy,
                                 Edg_destroy_fn value_destroy)
{
  size_t capacity;
  EDG_UTIL_ASSERT(key_size != 0);
  if (mask_width >= sizeof(size_t) * CHAR_BIT - 1) edg_util_allocation_failure();
  capacity = (size_t)1 << mask_width;
  if (capacity < 2) capacity = 2;
  memset(map, 0, sizeof(*map));
  map->key_size = key_size;
  map->value_size = value_size;
  map->key_offset = edg_align_size(2 * sizeof(size_t));
  map->value_offset = edg_align_size(edg_size_add(map->key_offset, key_size));
  map->stride = edg_align_size(edg_size_add(map->value_offset,
                                            value_size == 0 ? 1 : value_size));
  map->allocator = allocator;
  edg_map_set_hash(map, hash);
  edg_map_set_equal(map, equal);
  edg_map_set_key_copy(map, key_copy);
  edg_map_set_key_destroy(map, key_destroy);
  edg_map_set_value_copy(map, value_copy);
  edg_map_set_value_destroy(map, value_destroy);
  edg_map_rehash(map, capacity);
}

static inline void *edg_map_get_with_hash(const Edg_map *map, const void *key,
                                           size_t hash)
{
  size_t slot;
  if (map->len == 0) return NULL;
  slot = edg_map_find(map, key, hash);
  return map->data[slot * map->stride] == 0 ? NULL : edg_map_value_at(map, slot);
}

static inline void *edg_map_get(const Edg_map *map, const void *key)
{
  return edg_map_get_with_hash(map, key,
                               map->hash != NULL ? edg_map_get_hash(map)(key) :
                               edg_hash_bytes(key, map->key_size));
}

static inline void *edg_map_set_with_hash(Edg_map *map, const void *key,
                                           const void *value, size_t hash)
{
  size_t slot = map->cap == 0 ? 0 : edg_map_find(map, key, hash);
  void *result;
  if (map->cap != 0 && map->data[slot * map->stride] != 0) {
    result = edg_map_value_at(map, slot);
    if (value == result) return result;
    if (map->value_destroy != NULL) {
      max_align_t temporary[(map->value_size + sizeof(max_align_t) - 1) /
                             sizeof(max_align_t) + 1];
      if (value != NULL) edg_copy_elements(temporary, value, 1, map->value_size,
                                           edg_map_get_value_copy(map));
      edg_map_get_value_destroy(map)(result);
      if (value == NULL) memset(result, 0, map->value_size);
      else edg_relocate_elements(result, temporary, 1, map->value_size,
                                  edg_map_get_value_relocate(map));
    } else if (value == NULL) {
      memset(result, 0, map->value_size);
    } else {
      edg_copy_elements(result, value, 1, map->value_size,
                          edg_map_get_value_copy(map));
    }
    return result;
  }
  if (map->cap == 0 || map->len + 1 > map->cap / 2) {
    if (edg_storage_contains(map->data, map->cap * map->stride, key) ||
        edg_storage_contains(map->data, map->cap * map->stride, value)) {
      max_align_t temporary[(map->stride + sizeof(max_align_t) - 1) /
                             sizeof(max_align_t)];
      unsigned char *bytes = (unsigned char *)temporary;
      edg_copy_elements(bytes + map->key_offset, key, 1, map->key_size,
                         edg_map_get_key_copy(map));
      if (value == NULL) memset(bytes + map->value_offset, 0, map->value_size);
      else edg_copy_elements(bytes + map->value_offset, value, 1, map->value_size,
                               edg_map_get_value_copy(map));
      edg_map_rehash(map, map->cap == 0 ? 2 : edg_size_multiply(map->cap, 2));
      slot = edg_map_find(map, bytes + map->key_offset, hash);
      edg_relocate_elements(edg_map_key_at(map, slot), bytes + map->key_offset,
                             1, map->key_size, edg_map_get_key_relocate(map));
      edg_relocate_elements(edg_map_value_at(map, slot), bytes + map->value_offset,
                             1, map->value_size, edg_map_get_value_relocate(map));
      goto inserted;
    }
    edg_map_rehash(map, map->cap == 0 ? 2 : edg_size_multiply(map->cap, 2));
    slot = edg_map_find(map, key, hash);
  }
  edg_copy_elements(edg_map_key_at(map, slot), key, 1, map->key_size,
                      edg_map_get_key_copy(map));
  if (value == NULL) memset(edg_map_value_at(map, slot), 0, map->value_size);
  else edg_copy_elements(edg_map_value_at(map, slot), value, 1, map->value_size,
                           edg_map_get_value_copy(map));
inserted:
  edg_map_store_hash(map, slot, hash);
  map->data[slot * map->stride] = 1;
  ++map->len;
  return edg_map_value_at(map, slot);
}

static inline void *edg_map_set(Edg_map *map, const void *key, const void *value)
{
  return edg_map_set_with_hash(map, key, value,
                               map->hash != NULL ? edg_map_get_hash(map)(key) :
                               edg_hash_bytes(key, map->key_size));
}

static inline a_boolean edg_map_remove_with_hash(Edg_map *map, const void *key,
                                                  size_t hash)
{
  size_t hole, slot, mask;
  if (map->len == 0) return 0;
  hole = edg_map_find(map, key, hash);
  if (map->data[hole * map->stride] == 0) return 0;
  if (map->value_destroy != NULL) {
    edg_map_get_value_destroy(map)(edg_map_value_at(map, hole));
  }
  if (map->key_destroy != NULL) {
    edg_map_get_key_destroy(map)(edg_map_key_at(map, hole));
  }
  map->data[hole * map->stride] = 0;
  mask = map->cap - 1;
  slot = (hole + 1) & mask;
  while (map->data[slot * map->stride] != 0) {
    size_t ideal = edg_map_hash_at(map, slot) & mask;
    if (((hole - ideal) & mask) < ((slot - ideal) & mask)) {
      edg_relocate_elements(edg_map_key_at(map, hole), edg_map_key_at(map, slot),
                             1, map->key_size, edg_map_get_key_relocate(map));
      edg_relocate_elements(edg_map_value_at(map, hole), edg_map_value_at(map, slot),
                             1, map->value_size, edg_map_get_value_relocate(map));
      edg_map_store_hash(map, hole, edg_map_hash_at(map, slot));
      map->data[hole * map->stride] = 1;
      map->data[slot * map->stride] = 0;
      hole = slot;
    }
    slot = (slot + 1) & mask;
  }
  --map->len;
  return 1;
}

static inline a_boolean edg_map_remove(Edg_map *map, const void *key)
{
  return edg_map_remove_with_hash(map, key,
                                  map->hash != NULL ? edg_map_get_hash(map)(key) :
                                  edg_hash_bytes(key, map->key_size));
}

static inline a_boolean edg_map_next(const Edg_map *map, size_t *cursor,
                                       void **key, void **value)
{
  while (*cursor < map->cap) {
    size_t slot = (*cursor)++;
    if (map->data[slot * map->stride] != 0) {
      if (key != NULL) *key = edg_map_key_at(map, slot);
      if (value != NULL) *value = edg_map_value_at(map, slot);
      return 1;
    }
  }
  return 0;
}

static inline void edg_map_clear(Edg_map *map)
{
  size_t cursor = 0;
  void *key, *value;
  if (map->len == 0) return;
  while (edg_map_next(map, &cursor, &key, &value)) {
    if (map->value_destroy != NULL) edg_map_get_value_destroy(map)(value);
    if (map->key_destroy != NULL) edg_map_get_key_destroy(map)(key);
    map->data[(cursor - 1) * map->stride] = 0;
  }
  map->len = 0;
}

static inline void edg_map_destroy(Edg_map *map)
{
  edg_map_clear(map);
  edg_dealloc(map->allocator, map->data, map->allocation_bytes);
  map->data = NULL;
  map->cap = map->allocation_bytes = 0;
}

typedef struct a_string {
  char *data;
  size_t len, cap, allocation_bytes;
  Edg_allocator allocator;
  char *inline_buffer;
  size_t inline_capacity;
  a_boolean small;
  a_boolean initialized;
  char small_buffer[50];
} a_string;

typedef a_string a_number_buffer;

static inline char *edg_string_data(a_string *string)
{
  return string->data != NULL ? string->data :
         string->small ? string->small_buffer : NULL;
}

static inline const char *edg_string_chars(const a_string *string)
{
  return string->data != NULL ? string->data :
         string->small ? string->small_buffer : "";
}

static inline const char *edg_string_cstr(a_string *string)
{
  return edg_string_chars(string);
}

static inline a_string_view edg_string_as_view(const a_string *string)
{
  return edg_string_view(edg_string_chars(string), string->len);
}

static inline void edg_string_init(a_string *string, Edg_allocator allocator)
{
  string->data = NULL;
  string->len = string->cap = string->allocation_bytes = 0;
  string->allocator = allocator;
  string->inline_buffer = NULL;
  string->inline_capacity = 0;
  string->small = 0;
  string->initialized = 1;
  string->small_buffer[0] = '\0';
}

static inline void edg_string_init_small(a_string *string,
                                          Edg_allocator allocator)
{
  edg_string_init(string, allocator);
  string->small = 1;
  string->cap = sizeof(string->small_buffer) - 1;
}

static inline void edg_string_init_buffer(a_string *string,
                                           Edg_allocator allocator,
                                           char *buffer, size_t capacity)
{
  EDG_UTIL_ASSERT(buffer != NULL || capacity == 0);
  edg_string_init(string, allocator);
  string->inline_buffer = capacity == 0 ? NULL : buffer;
  string->inline_capacity = capacity;
  string->data = string->inline_buffer;
  string->cap = capacity == 0 ? 0 : capacity - 1;
  if (capacity != 0) buffer[0] = '\0';
}

/* Capacity counts usable characters; every allocation includes one more byte
   for the terminator.  init_buffer instead takes the complete buffer size. */
static inline void edg_string_reserve(a_string *string, size_t capacity)
{
  char *data;
  size_t bytes;
  if (capacity <= string->cap) return;
  if ((string->allocator == EDG_ALLOC_DIRECT || EDG_UTIL_STANDALONE) &&
      string->data != NULL && string->data != string->inline_buffer) {
    bytes = edg_size_add(capacity, 1);
    data = (char *)realloc(string->data, bytes);
    if (data == NULL) edg_util_allocation_failure();
    string->data = data;
    string->allocation_bytes = bytes;
    string->cap = capacity;
    return;
  }
  data = (char *)edg_alloc(string->allocator, edg_size_add(capacity, 1), &bytes);
  memcpy(data, edg_string_chars(string), edg_size_add(string->len, 1));
  if (string->data != string->inline_buffer) {
    edg_dealloc(string->allocator, string->data, string->allocation_bytes);
  }
  string->data = data;
  string->cap = bytes - 1;
  string->allocation_bytes = bytes;
}

static inline void edg_string_clear(a_string *string)
{
  char *data = edg_string_data(string);
  string->len = 0;
  if (data != NULL) data[0] = '\0';
}

static inline void edg_string_destroy(a_string *string)
{
  if (string->data != string->inline_buffer) {
    edg_dealloc(string->allocator, string->data, string->allocation_bytes);
  }
  string->data = string->inline_buffer;
  string->allocation_bytes = string->len = 0;
  string->cap = string->inline_capacity != 0 ? string->inline_capacity - 1 :
                string->small ? sizeof(string->small_buffer) - 1 : 0;
  edg_string_clear(string);
}

static inline void edg_string_grow(a_string *string, size_t minimum)
{
  size_t capacity = string->cap == 0 ? 1 :
                    edg_size_multiply(string->cap, 2);
  edg_string_reserve(string, capacity < minimum ? minimum : capacity);
}

static inline void edg_string_append_view(a_string *string, a_string_view value)
{
  const char *original = edg_string_chars(string);
  size_t offset = 0, len;
  a_boolean aliased;
  if (value.len == 0) return;
  aliased = edg_storage_contains(original, string->len + 1, value.data);
  if (aliased) offset = (size_t)((uintptr_t)value.data - (uintptr_t)original);
  len = edg_size_add(string->len, value.len);
  if (len > string->cap) edg_string_grow(string, len);
  if (aliased) value.data = edg_string_chars(string) + offset;
  memmove(edg_string_data(string) + string->len, value.data, value.len);
  string->len = len;
  edg_string_data(string)[len] = '\0';
}

static inline void edg_string_append_cstr(a_string *string, const char *value)
{
  edg_string_append_view(string, edg_string_view_cstr(value));
}

static inline void edg_string_append_char(a_string *string, char value)
{
  size_t len = edg_size_add(string->len, 1);
  if (len > string->cap) edg_string_grow(string, len);
  edg_string_data(string)[string->len] = value;
  string->len = len;
  edg_string_data(string)[len] = '\0';
}

static inline void edg_string_resize(a_string *string, size_t len)
{
  if (len > string->cap) edg_string_reserve(string, len);
  if (len > string->len) {
    memset(edg_string_data(string) + string->len, 0, len - string->len);
  }
  string->len = len;
  if (edg_string_data(string) != NULL) edg_string_data(string)[len] = '\0';
}

static inline void edg_string_truncate(a_string *string, size_t len)
{
  if (len < string->len) edg_string_resize(string, len);
}

static inline void edg_string_write(const a_string *string, char *buffer,
                                     size_t capacity)
{
  EDG_UTIL_ASSERT(capacity > string->len);
  memcpy(buffer, edg_string_chars(string), string->len + 1);
}

static inline char *edg_string_to_storage(const a_string *string,
                                           Edg_allocator allocator)
{
  char *result = (char *)edg_alloc(allocator, edg_size_add(string->len, 1), NULL);
  memcpy(result, edg_string_chars(string), string->len + 1);
  return result;
}

static inline void edg_string_copy(a_string *dst, a_string *src)
{
  if (dst == src) return;
  if (!dst->initialized) {
    if (src->small) edg_string_init_small(dst, src->allocator);
    else edg_string_init(dst, src->allocator);
  }
  edg_string_clear(dst);
  edg_string_reserve(dst, src->len);
  if (src->len != 0) {
    memcpy(edg_string_data(dst), edg_string_chars(src), src->len + 1);
  }
  dst->len = src->len;
}

static inline void edg_string_move(a_string *dst, a_string *src)
{
  if (dst == src) return;
  edg_string_destroy(dst);
  dst->allocator = src->allocator;
  dst->initialized = 1;
  if (src->data != NULL && src->data != src->inline_buffer) {
    dst->data = src->data;
    dst->cap = src->cap;
    dst->allocation_bytes = src->allocation_bytes;
    dst->len = src->len;
    src->data = src->inline_buffer;
    src->allocation_bytes = 0;
    src->cap = src->inline_capacity != 0 ? src->inline_capacity - 1 :
               src->small ? sizeof(src->small_buffer) - 1 : 0;
  } else {
    if (dst->inline_buffer == NULL && src->small) {
      dst->small = 1;
      dst->cap = sizeof(dst->small_buffer) - 1;
    }
    edg_string_copy(dst, src);
  }
  edg_string_clear(src);
}

static inline a_boolean edg_string_equal(const void *left, const void *right)
{
  a_string_view a = edg_string_as_view((const a_string *)left);
  a_string_view b = edg_string_as_view((const a_string *)right);
  return edg_string_view_equal(&a, &b);
}

static inline size_t edg_string_hash(const void *key)
{
  a_string_view view = edg_string_as_view((const a_string *)key);
  return edg_string_view_hash(&view);
}

static inline void edg_string_copy_element(void *dst, const void *src)
{
  const a_string *source = (const a_string *)src;
  a_string *target = (a_string *)dst;
  if (source->small) edg_string_init_small(target, source->allocator);
  else edg_string_init(target, source->allocator);
  edg_string_reserve(target, source->len);
  if (source->len != 0) {
    memcpy(edg_string_data(target), edg_string_chars(source), source->len + 1);
  }
  target->len = source->len;
}

static inline void edg_string_destroy_element(void *element)
{
  edg_string_destroy((a_string *)element);
}

static inline void edg_array_copy_element(void *dst, const void *src)
{
  memset(dst, 0, sizeof(Edg_array));
  edg_array_copy((Edg_array *)dst, (const Edg_array *)src);
}

static inline void edg_array_destroy_element(void *element)
{
  edg_array_destroy((Edg_array *)element);
}

static inline void edg_string_append_repeat(a_string *string, char value,
                                              size_t count)
{
  size_t len;
  if (count == 0) return;
  len = edg_size_add(string->len, count);
  if (len > string->cap) edg_string_grow(string, len);
  memset(edg_string_data(string) + string->len, (unsigned char)value, count);
  string->len = len;
  edg_string_data(string)[len] = '\0';
}

static inline void edg_string_append_base(a_string *string,
                                           unsigned long long value,
                                           unsigned base, unsigned min_digits,
                                           char padding)
{
  char digits[sizeof(value) * CHAR_BIT];
  size_t len = 0;
  EDG_UTIL_ASSERT(base >= 2 && base <= 16);
  do {
    digits[len++] = "0123456789abcdef"[value % base];
    value /= base;
  } while (value != 0);
  if ((size_t)min_digits > len) {
    edg_string_append_repeat(string, padding, (size_t)min_digits - len);
  }
  if (edg_size_add(string->len, len) > string->cap) {
    edg_string_grow(string, string->len + len);
  }
  while (len != 0) edg_string_data(string)[string->len++] = digits[--len];
  edg_string_data(string)[string->len] = '\0';
}

static inline void edg_string_append_u64(a_string *string,
                                          unsigned long long value)
{
  edg_string_append_base(string, value, 10, 0, '0');
}

static inline void edg_string_append_i64(a_string *string, long long value)
{
  unsigned long long magnitude = (unsigned long long)value;
  if (value < 0) {
    edg_string_append_char(string, '-');
    magnitude = 0 - magnitude;
  }
  edg_string_append_u64(string, magnitude);
}

static inline void edg_string_append_hex(a_string *string,
                                          unsigned long long value,
                                          unsigned min_digits, char padding)
{
  edg_string_append_base(string, value, 16, min_digits, padding);
}

static inline void edg_string_append_octal(a_string *string,
                                            unsigned long long value,
                                            unsigned min_digits, char padding)
{
  edg_string_append_base(string, value, 8, min_digits, padding);
}

#if defined(HOST_HAS_INT128_EXTENSIONS) && HOST_HAS_INT128_EXTENSIONS
static inline void edg_string_append_u128_base(a_string *string,
                                                __uint128_t value,
                                                unsigned base,
                                                unsigned min_digits,
                                                char padding)
{
  char digits[128];
  size_t len = 0;
  EDG_UTIL_ASSERT(base >= 2 && base <= 16);
  do {
    digits[len++] = "0123456789abcdef"[(unsigned)(value % base)];
    value /= base;
  } while (value != 0);
  if ((size_t)min_digits > len) {
    edg_string_append_repeat(string, padding, (size_t)min_digits - len);
  }
  if (edg_size_add(string->len, len) > string->cap) {
    edg_string_grow(string, string->len + len);
  }
  while (len != 0) edg_string_data(string)[string->len++] = digits[--len];
  edg_string_data(string)[string->len] = '\0';
}

static inline void edg_string_append_u128(a_string *string, __uint128_t value)
{
  edg_string_append_u128_base(string, value, 10, 0, '0');
}

static inline void edg_string_append_i128(a_string *string, __int128_t value)
{
  __uint128_t magnitude = (__uint128_t)value;
  if (value < 0) {
    edg_string_append_char(string, '-');
    magnitude = 0 - magnitude;
  }
  edg_string_append_u128(string, magnitude);
}

static inline void edg_string_append_hex128(a_string *string,
                                             __uint128_t value,
                                             unsigned min_digits, char padding)
{
  edg_string_append_u128_base(string, value, 16, min_digits, padding);
}
#endif

static inline int edg_snprintf(char *buffer, size_t capacity,
                                 const char *format, ...)
{
  va_list args;
  int count;
  EDG_UTIL_ASSERT(buffer != NULL && capacity != 0);
  va_start(args, format);
  count = vsnprintf(buffer, capacity, format, args);
  va_end(args);
  return count >= 0 && (size_t)count >= capacity ? -1 : count;
}

static inline void edg_string_append_vformat(a_string *string,
                                               const char *format, va_list args)
{
  va_list copy;
  int count;
  size_t available = edg_string_data(string) == NULL ? 0 :
                     string->cap - string->len + 1;
  size_t required, capacity, bytes;
  char *data;
  va_copy(copy, args);
  count = vsnprintf(available == 0 ? NULL :
                      edg_string_data(string) + string->len,
                    available, format, copy);
  va_end(copy);
  EDG_UTIL_ASSERT(count >= 0);
  if (count < 0) abort();
  if (available != 0 && (size_t)count < available) {
    string->len = edg_size_add(string->len, (size_t)count);
    return;
  }
  if (count == 0) return;
  if (available != 0) edg_string_data(string)[string->len] = '\0';
  required = edg_size_add(string->len, (size_t)count);
  capacity = string->cap == 0 ? 1 : edg_size_multiply(string->cap, 2);
  if (capacity < required) capacity = required;
  data = (char *)edg_alloc(string->allocator, edg_size_add(capacity, 1), &bytes);
  memcpy(data, edg_string_chars(string), string->len + 1);
  /* Keep the old storage live through formatting: arguments may refer to it. */
  va_copy(copy, args);
  count = vsnprintf(data + string->len, bytes - string->len, format, copy);
  va_end(copy);
  EDG_UTIL_ASSERT(count >= 0 && (size_t)count == required - string->len);
  if (count < 0 || (size_t)count != required - string->len) abort();
  if (string->data != string->inline_buffer) {
    edg_dealloc(string->allocator, string->data, string->allocation_bytes);
  }
  string->data = data;
  string->allocation_bytes = bytes;
  string->cap = bytes - 1;
  string->len = required;
}

static inline void edg_string_append_format(a_string *string,
                                              const char *format, ...)
{
  va_list args;
  va_start(args, format);
  edg_string_append_vformat(string, format, args);
  va_end(args);
}

static inline void edg_string_append_double(a_string *string, double value)
{
  edg_string_append_format(string, "%f", value);
}

static inline void edg_string_append_double_hex(a_string *string, double value)
{
  edg_string_append_format(string, "%a", value);
}

static inline void edg_string_append_long_double_hex(a_string *string,
                                                      long double value)
{
  edg_string_append_format(string, "%La", value);
}

static inline void edg_string_append_pointer(a_string *string, const void *value)
{
  edg_string_append_format(string, "%p", (void *)value);
}

static inline void edg_reverse_bytes(char *first, size_t len)
{
  size_t i;
  for (i = 0; i < len / 2; ++i) {
    char tmp = first[i];
    first[i] = first[len - i - 1];
    first[len - i - 1] = tmp;
  }
}

static inline void edg_string_insert_view(a_string *string, size_t index,
                                            a_string_view value)
{
  size_t original = string->len;
  char *data;
  EDG_UTIL_ASSERT(index <= original);
  edg_string_append_view(string, value);
  data = edg_string_data(string);
  if (value.len == 0) return;
  edg_reverse_bytes(data + index, original - index);
  edg_reverse_bytes(data + original, value.len);
  edg_reverse_bytes(data + index, original - index + value.len);
}

static inline void edg_string_append_padded(a_string *string, a_string_view value,
                                               size_t width, char padding,
                                               a_boolean pad_left)
{
  size_t original = string->len, count;
  EDG_UTIL_ASSERT(value.len <= width);
  count = value.len < width ? width - value.len : 0;
  edg_string_append_view(string, value);
  edg_string_append_repeat(string, padding, count);
  if (pad_left && count != 0 && value.len != 0) {
    char *data = edg_string_data(string);
    edg_reverse_bytes(data + original, value.len);
    edg_reverse_bytes(data + original + value.len, count);
    edg_reverse_bytes(data + original, value.len + count);
  }
}

static inline void edg_string_print(const a_string *string, FILE *stream,
                                      const char *end)
{
  fputs(edg_string_chars(string), stream);
  fputs(end, stream);
}

typedef struct a_path_handle {
  const char *ptr;
} a_path_handle;

#if !EDG_UTIL_STANDALONE
extern char *normalize_file_name(const char *file_name);
extern int f_compare_file_names(const char *file1, const char *file2,
                                a_boolean ignore_delimiters,
                                a_boolean is_partial_file_name,
                                a_boolean in_general_memory);

static inline a_boolean edg_path_handle_equal(const void *left,
                                               const void *right)
{
  const a_path_handle *a = (const a_path_handle *)left;
  const a_path_handle *b = (const a_path_handle *)right;
  if (a->ptr == NULL || b->ptr == NULL) return a->ptr == b->ptr;
  return f_compare_file_names(a->ptr, b->ptr, 0, 0, 1) == 0;
}

static inline size_t edg_path_handle_hash(const void *key)
{
  const a_path_handle *path = (const a_path_handle *)key;
  a_string_view view;
  if (path->ptr == NULL) return 0;
  view = edg_string_view_cstr(normalize_file_name(path->ptr));
  return edg_string_view_hash(&view);
}
#endif

/* Scalar math helpers retain the old call spelling where it is C-shaped. */
#define EDG_DECLARE_MINMAX(type, suffix) \
  static inline type edg_min_##suffix(type a, type b) { return b < a ? b : a; } \
  static inline type edg_max_##suffix(type a, type b) { return a < b ? b : a; } \
  static inline type *edg_min_ref_##suffix(type *a, type *b) \
    { return *b < *a ? b : a; } \
  static inline type *edg_max_ref_##suffix(type *a, type *b) \
    { return *a < *b ? b : a; }
EDG_DECLARE_MINMAX(char, char)
EDG_DECLARE_MINMAX(signed char, schar)
EDG_DECLARE_MINMAX(unsigned char, uchar)
EDG_DECLARE_MINMAX(short, short)
EDG_DECLARE_MINMAX(unsigned short, ushort)
EDG_DECLARE_MINMAX(int, int)
EDG_DECLARE_MINMAX(unsigned, uint)
EDG_DECLARE_MINMAX(long, long)
EDG_DECLARE_MINMAX(unsigned long, ulong)
EDG_DECLARE_MINMAX(long long, llong)
EDG_DECLARE_MINMAX(unsigned long long, ullong)
EDG_DECLARE_MINMAX(float, float)
EDG_DECLARE_MINMAX(double, double)
EDG_DECLARE_MINMAX(long double, ldouble)
#if defined(HOST_HAS_INT128_EXTENSIONS) && HOST_HAS_INT128_EXTENSIONS
EDG_DECLARE_MINMAX(__int128_t, i128)
EDG_DECLARE_MINMAX(__uint128_t, u128)
#define EDG_MINMAX_INT128_ASSOC(prefix) \
  , __int128_t: prefix##_i128, __uint128_t: prefix##_u128
#else
#define EDG_MINMAX_INT128_ASSOC(prefix)
#endif
#undef EDG_DECLARE_MINMAX
#define EDG_MINMAX_SELECT(value, prefix) \
  _Generic((value), char: prefix##_char, signed char: prefix##_schar, \
    unsigned char: prefix##_uchar, short: prefix##_short, \
    unsigned short: prefix##_ushort, int: prefix##_int, unsigned: prefix##_uint, \
    long: prefix##_long, unsigned long: prefix##_ulong, \
    long long: prefix##_llong, unsigned long long: prefix##_ullong, \
    float: prefix##_float, double: prefix##_double, long double: prefix##_ldouble \
    EDG_MINMAX_INT128_ASSOC(prefix))
#define min_val(a, b) EDG_MINMAX_SELECT(a, edg_min)(a, b)
#define max_val(a, b) EDG_MINMAX_SELECT(a, edg_max)(a, b)
#define min_ref(a, b) EDG_MINMAX_SELECT(*(a), edg_min_ref)(a, b)
#define max_ref(a, b) EDG_MINMAX_SELECT(*(a), edg_max_ref)(a, b)

static inline uint64_t edg_bitmask_of_width(unsigned width)
{
  return width >= 64 ? UINT64_MAX : (UINT64_C(1) << width) - 1;
}

#define bitmask_of_width(width) edg_bitmask_of_width(width)
#define copy_to_bitfield(src, dest, width) \
  ((dest) = (src) & bitmask_of_width(width))

static inline int edg_floor_log2_u64(uint64_t value)
{
  int result = 0;
  while ((value >>= 1) != 0) ++result;
  return result;
}

static inline unsigned next_pow2(uint64_t value)
{
  --value;
  value |= value >> 1;
  value |= value >> 2;
  value |= value >> 4;
  value |= value >> 8;
  value |= value >> 16;
  value |= value >> 32;
  return (unsigned)(value + 1);
}

static inline size_t edg_count_ones_u64(uint64_t value)
{
  size_t count = 0;
  while (value != 0) {
    value &= value - 1;
    ++count;
  }
  return count;
}

static inline size_t edg_integral_digits_u64(uint64_t value, unsigned base)
{
  size_t count = 1;
  EDG_UTIL_ASSERT(base >= 2);
  while ((value /= base) != 0) ++count;
  return count;
}

static inline size_t edg_integral_digits_i64(int64_t value, unsigned base)
{
  uint64_t magnitude = (uint64_t)value;
  if (value < 0) magnitude = 0 - magnitude;
  return edg_integral_digits_u64(magnitude, base);
}

static inline a_boolean edg_checked_add_u32(uint32_t *output,
                                             uint64_t a, uint64_t b)
{
  if (a > UINT32_MAX || b > UINT32_MAX - a) return 0;
  *output = (uint32_t)(a + b);
  return 1;
}

static inline a_boolean edg_checked_add_u64(uint64_t *output,
                                             uint64_t a, uint64_t b)
{
  if (b > UINT64_MAX - a) return 0;
  *output = a + b;
  return 1;
}

static inline a_boolean edg_checked_multiply_u32(uint32_t *output,
                                                  uint64_t a, uint64_t b)
{
  if (b != 0 && a > UINT32_MAX / b) return 0;
  *output = (uint32_t)(a * b);
  return 1;
}

static inline a_boolean edg_checked_multiply_u64(uint64_t *output,
                                                  uint64_t a, uint64_t b)
{
  if (b != 0 && a > UINT64_MAX / b) return 0;
  *output = a * b;
  return 1;
}

#if defined(HOST_HAS_INT128_EXTENSIONS) && HOST_HAS_INT128_EXTENSIONS
static inline int edg_floor_log2_u128(__uint128_t value)
{
  int result = 0;
  while ((value >>= 1) != 0) ++result;
  return result;
}

static inline size_t edg_count_ones_u128(__uint128_t value)
{
  size_t count = 0;
  while (value != 0) {
    value &= value - 1;
    ++count;
  }
  return count;
}

static inline size_t edg_integral_digits_u128(__uint128_t value, unsigned base)
{
  size_t count = 1;
  EDG_UTIL_ASSERT(base >= 2);
  while ((value /= base) != 0) ++count;
  return count;
}

static inline size_t edg_integral_digits_i128(__int128_t value, unsigned base)
{
  __uint128_t magnitude = (__uint128_t)value;
  if (value < 0) magnitude = 0 - magnitude;
  return edg_integral_digits_u128(magnitude, base);
}

static inline a_boolean edg_checked_add_u128(__uint128_t *output,
                                               uint64_t a, uint64_t b)
{
  *output = (__uint128_t)a + b;
  return 1;
}

static inline a_boolean edg_checked_multiply_u128(__uint128_t *output,
                                                    uint64_t a, uint64_t b)
{
  *output = (__uint128_t)a * b;
  return 1;
}

#define EDG_MATH_UINT128_ASSOC(fn) , __uint128_t: fn, __int128_t: fn
#define EDG_DIGITS_INT128_ASSOC \
  , __uint128_t: edg_integral_digits_u128, __int128_t: edg_integral_digits_i128
#define EDG_MAX_INT128_ASSOC \
  , __uint128_t: ~(__uint128_t)0, \
    __int128_t: (__int128_t)((~(__uint128_t)0) >> 1)
#define EDG_CHECKED_INT128_ASSOC(fn) , __uint128_t: fn
#else
#define EDG_MATH_UINT128_ASSOC(fn)
#define EDG_DIGITS_INT128_ASSOC
#define EDG_MAX_INT128_ASSOC
#define EDG_CHECKED_INT128_ASSOC(fn)
#endif

#define floor_log2(value) \
  _Generic((value), default: edg_floor_log2_u64 \
    EDG_MATH_UINT128_ASSOC(edg_floor_log2_u128))(value)
#define count_ones(value) \
  _Generic((value), default: edg_count_ones_u64 \
    EDG_MATH_UINT128_ASSOC(edg_count_ones_u128))(value)
#if CHAR_MIN < 0
#define EDG_CHAR_DIGITS edg_integral_digits_i64
#else
#define EDG_CHAR_DIGITS edg_integral_digits_u64
#endif
#define integral_digits(value, base) \
  _Generic((value), char: EDG_CHAR_DIGITS, signed char: edg_integral_digits_i64, \
    short: edg_integral_digits_i64, int: edg_integral_digits_i64, \
    long: edg_integral_digits_i64, long long: edg_integral_digits_i64, \
    default: edg_integral_digits_u64 EDG_DIGITS_INT128_ASSOC)(value, base)
#define EDG_MAX_INTEGRAL_VALUE(type) \
  _Generic((type)0, signed char: SCHAR_MAX, unsigned char: UCHAR_MAX, \
    char: CHAR_MAX, short: SHRT_MAX, unsigned short: USHRT_MAX, \
    int: INT_MAX, unsigned int: UINT_MAX, long: LONG_MAX, \
    unsigned long: ULONG_MAX, long long: LLONG_MAX, \
    unsigned long long: ULLONG_MAX EDG_MAX_INT128_ASSOC)
#define EDG_MAX_INTEGRAL_DIGITS(type) \
  integral_digits(EDG_MAX_INTEGRAL_VALUE(type), 10)
#define checked_addition(output, a, b) \
  _Generic(*(output), uint32_t: edg_checked_add_u32, \
    uint64_t: edg_checked_add_u64 \
    EDG_CHECKED_INT128_ASSOC(edg_checked_add_u128))(output, a, b)
#define checked_multiplication(output, a, b) \
  _Generic(*(output), uint32_t: edg_checked_multiply_u32, \
    uint64_t: edg_checked_multiply_u64 \
    EDG_CHECKED_INT128_ASSOC(edg_checked_multiply_u128))(output, a, b)

static inline uint32_t edg_rotate_left_u32(uint32_t value, unsigned count)
{
  count %= 32;
  return count == 0 ? value : (value << count) | (value >> (32 - count));
}

static inline uint32_t edg_rotate_right_u32(uint32_t value, unsigned count)
{
  count %= 32;
  return count == 0 ? value : (value >> count) | (value << (32 - count));
}

static inline uint64_t edg_rotate_left_u64(uint64_t value, unsigned count)
{
  count %= 64;
  return count == 0 ? value : (value << count) | (value >> (64 - count));
}

static inline uint64_t edg_rotate_right_u64(uint64_t value, unsigned count)
{
  count %= 64;
  return count == 0 ? value : (value >> count) | (value << (64 - count));
}

static inline uint8_t edg_rotate_left_u8(uint8_t value, unsigned count)
{
  count %= 8;
  return count == 0 ? value :
         (uint8_t)(((uint32_t)value << count) | ((uint32_t)value >> (8 - count)));
}

static inline uint8_t edg_rotate_right_u8(uint8_t value, unsigned count)
{
  count %= 8;
  return count == 0 ? value :
         (uint8_t)(((uint32_t)value >> count) | ((uint32_t)value << (8 - count)));
}

static inline uint16_t edg_rotate_left_u16(uint16_t value, unsigned count)
{
  count %= 16;
  return count == 0 ? value :
         (uint16_t)(((uint32_t)value << count) | ((uint32_t)value >> (16 - count)));
}

static inline uint16_t edg_rotate_right_u16(uint16_t value, unsigned count)
{
  count %= 16;
  return count == 0 ? value :
         (uint16_t)(((uint32_t)value >> count) | ((uint32_t)value << (16 - count)));
}

#if defined(HOST_HAS_INT128_EXTENSIONS) && HOST_HAS_INT128_EXTENSIONS
static inline __uint128_t edg_rotate_left_u128(__uint128_t value, unsigned count)
{
  count %= 128;
  return count == 0 ? value : (value << count) | (value >> (128 - count));
}

static inline __uint128_t edg_rotate_right_u128(__uint128_t value, unsigned count)
{
  count %= 128;
  return count == 0 ? value : (value >> count) | (value << (128 - count));
}
#define EDG_ROTATE_INT128_ASSOC(fn) , __uint128_t: fn
#else
#define EDG_ROTATE_INT128_ASSOC(fn)
#endif

#define bit_rotate_left(value, count) \
  _Generic((value), uint8_t: edg_rotate_left_u8, uint16_t: edg_rotate_left_u16, \
    uint32_t: edg_rotate_left_u32, uint64_t: edg_rotate_left_u64 \
    EDG_ROTATE_INT128_ASSOC(edg_rotate_left_u128))(value, count)
#define bit_rotate_right(value, count) \
  _Generic((value), uint8_t: edg_rotate_right_u8, uint16_t: edg_rotate_right_u16, \
    uint32_t: edg_rotate_right_u32, uint64_t: edg_rotate_right_u64 \
    EDG_ROTATE_INT128_ASSOC(edg_rotate_right_u128))(value, count)

static inline void edg_append_big_endian(unsigned char *dest, uint64_t value,
                                           size_t bytes)
{
  size_t i;
  EDG_UTIL_ASSERT(bytes <= sizeof(value));
  for (i = 0; i < bytes; ++i) dest[bytes - i - 1] = (unsigned char)(value >> (i * 8));
}

#if defined(HOST_HAS_INT128_EXTENSIONS) && HOST_HAS_INT128_EXTENSIONS
static inline void edg_append_big_endian_u128(unsigned char *dest,
                                                __uint128_t value, size_t bytes)
{
  size_t i;
  EDG_UTIL_ASSERT(bytes <= sizeof(value));
  for (i = 0; i < bytes; ++i) dest[bytes - i - 1] = (unsigned char)(value >> (i * 8));
}
#define EDG_BIG_ENDIAN_INT128_ASSOC \
  , __uint128_t: edg_append_big_endian_u128, __int128_t: edg_append_big_endian_u128
#else
#define EDG_BIG_ENDIAN_INT128_ASSOC
#endif

#define append_as_big_endian(dest, value) \
  _Generic((value), default: edg_append_big_endian EDG_BIG_ENDIAN_INT128_ASSOC) \
    ((unsigned char *)(dest), value, sizeof(value))

/* List field access through memcpy avoids aliasing typed pointer fields as
   void **.  Offsets are supplied with offsetof(the_actual_type, next/prev). */
static inline void *edg_list_link_get(const void *link)
{
  void *ptr;
  memcpy(&ptr, link, sizeof(ptr));
  return ptr;
}

static inline void edg_list_link_set(void *link, void *ptr)
{
  memcpy(link, &ptr, sizeof(ptr));
}

static inline void *edg_reverse_simple_list(void *head, size_t next_offset)
{
  void *result = NULL;
  while (head != NULL) {
    void *link = (char *)head + next_offset;
    void *next = edg_list_link_get(link);
    edg_list_link_set(link, result);
    result = head;
    head = next;
  }
  return result;
}

static inline void *edg_last_simple_list_link(void *head_link,
                                                 size_t next_offset)
{
  void *head;
  while ((head = edg_list_link_get(head_link)) != NULL) {
    head_link = (char *)head + next_offset;
  }
  return head_link;
}

static inline a_boolean edg_simple_list_has_cycle(const void *head,
                                                    size_t next_offset)
{
  const void *slow = head, *fast = head;
  while (fast != NULL) {
    fast = edg_list_link_get((const char *)fast + next_offset);
    if (fast == NULL) return 0;
    fast = edg_list_link_get((const char *)fast + next_offset);
    slow = edg_list_link_get((const char *)slow + next_offset);
    if (fast == slow) return 1;
  }
  return 0;
}

static inline size_t edg_count_list_elements_if(const void *head,
                                                  size_t next_offset,
                                                  Edg_predicate_fn predicate,
                                                  void *context)
{
  size_t count = 0;
  for (; head != NULL; head = edg_list_link_get((const char *)head + next_offset)) {
    if (predicate == NULL || predicate(head, context)) ++count;
  }
  return count;
}

static inline size_t edg_count_list_elements(const void *head,
                                                size_t next_offset)
{
  return edg_count_list_elements_if(head, next_offset, NULL, NULL);
}

static inline void edg_delete_from_list_if(void *head_link, void *tail_link,
                                              size_t next_offset,
                                              size_t prev_offset,
                                              Edg_predicate_fn predicate,
                                              void *context,
                                              Edg_destroy_fn delete_element)
{
  void *cursor = edg_list_link_get(head_link), *head = NULL, *tail = NULL;
  while (cursor != NULL) {
    void *next = edg_list_link_get((char *)cursor + next_offset);
    if (predicate(cursor, context)) {
      delete_element(cursor);
    } else {
      if (head == NULL) head = cursor;
      else edg_list_link_set((char *)tail + next_offset, cursor);
      if (prev_offset != SIZE_MAX) {
        edg_list_link_set((char *)cursor + prev_offset, tail);
      }
      tail = cursor;
    }
    cursor = next;
  }
  if (tail != NULL) edg_list_link_set((char *)tail + next_offset, NULL);
  edg_list_link_set(head_link, head);
  edg_list_link_set(tail_link, tail);
}

static inline void edg_linked_list_to_array(Edg_array *out, const void *head,
                                              size_t next_offset,
                                              a_boolean pointers)
{
  for (; head != NULL; head = edg_list_link_get((const char *)head + next_offset)) {
    edg_array_push(out, pointers ? (const void *)&head : head);
  }
}

static inline void edg_swap_at(void *left, void *right, size_t size)
{
  unsigned char *a = (unsigned char *)left, *b = (unsigned char *)right;
  size_t i;
  if (left == right) return;
  for (i = 0; i < size; ++i) {
    unsigned char tmp = a[i];
    a[i] = b[i];
    b[i] = tmp;
  }
}

static inline void edg_reverse_array(void *data, size_t count, size_t elem_size)
{
  size_t i;
  for (i = 0; i < count / 2; ++i) {
    edg_swap_at((char *)data + i * elem_size,
                 (char *)data + (count - i - 1) * elem_size, elem_size);
  }
}

static inline size_t edg_rotate(void *data, size_t count, size_t middle,
                                  size_t elem_size)
{
  EDG_UTIL_ASSERT(middle <= count);
  if (middle == 0) return count;
  if (middle == count) return 0;
  edg_reverse_array(data, middle, elem_size);
  edg_reverse_array((char *)data + middle * elem_size, count - middle, elem_size);
  edg_reverse_array(data, count, elem_size);
  return count - middle;
}

typedef const void *(*Edg_value_fn)(size_t index, void *context);

static inline ptrdiff_t edg_low_bound(size_t count, const void *value,
                                        Edg_value_fn element_at,
                                        Edg_compare_fn compare, void *context)
{
  size_t first = 0, len = count;
  while (len != 0) {
    size_t half = len / 2, index = first + half;
    if (compare(element_at(index, context), value, context) < 0) {
      first = index + 1;
      len -= half + 1;
    } else {
      len = half;
    }
  }
  return first == count ? -1 : (ptrdiff_t)first;
}

static inline ptrdiff_t edg_bin_search(size_t count, const void *value,
                                         Edg_value_fn element_at,
                                         Edg_compare_fn compare, void *context)
{
  ptrdiff_t index = edg_low_bound(count, value, element_at, compare, context);
  if (index >= 0 &&
      compare(element_at((size_t)index, context), value, context) != 0) return -1;
  return index;
}

static inline ptrdiff_t edg_array_bin_search(const void *data, size_t count,
                                                size_t elem_size,
                                                const void *value,
                                                Edg_compare_fn compare,
                                                void *context)
{
  size_t first = 0, len = count;
  while (len != 0) {
    size_t half = len / 2, index = first + half;
    if (compare((const char *)data + index * elem_size, value, context) < 0) {
      first = index + 1;
      len -= half + 1;
    } else len = half;
  }
  if (first == count ||
      compare((const char *)data + first * elem_size, value, context) != 0) return -1;
  return (ptrdiff_t)first;
}

static inline size_t edg_distance(const void *first, const void *last,
                                     size_t elem_size)
{
  if (first == last) return 0;
  return (size_t)((const char *)last - (const char *)first) / elem_size;
}

static inline void edg_heapify(unsigned char *data, size_t count, size_t root,
                                  size_t elem_size, Edg_compare_fn compare,
                                  void *context)
{
  for (;;) {
    size_t child;
    if (root >= count / 2) return;
    child = root * 2 + 1;
    if (child + 1 < count &&
        compare(data + child * elem_size, data + (child + 1) * elem_size,
                  context) < 0) ++child;
    if (compare(data + root * elem_size, data + child * elem_size, context) >= 0) {
      return;
    }
    edg_swap_at(data + root * elem_size, data + child * elem_size, elem_size);
    root = child;
  }
}

static inline void edg_heap_sort(void *data, size_t count, size_t elem_size,
                                    Edg_compare_fn compare, void *context)
{
  size_t i;
  unsigned char *bytes = (unsigned char *)data;
  for (i = count / 2; i != 0; --i) {
    edg_heapify(bytes, count, i - 1, elem_size, compare, context);
  }
  for (i = count; i > 1; --i) {
    edg_swap_at(bytes, bytes + (i - 1) * elem_size, elem_size);
    edg_heapify(bytes, i - 1, 0, elem_size, compare, context);
  }
}

static inline a_boolean edg_insertion_sort(void *data, size_t count,
                                             size_t elem_size,
                                             Edg_compare_fn compare,
                                             void *context, size_t moves_limit)
{
  size_t i, moves = 0;
  unsigned char *bytes = (unsigned char *)data;
  max_align_t temporary[(elem_size + sizeof(max_align_t) - 1) /
                          sizeof(max_align_t) + 1];
  for (i = 1; i < count; ++i) {
    size_t hole = i;
    if (compare(bytes + i * elem_size, bytes + (i - 1) * elem_size, context) >= 0) {
      continue;
    }
    memcpy(temporary, bytes + i * elem_size, elem_size);
    do {
      memcpy(bytes + hole * elem_size, bytes + (hole - 1) * elem_size, elem_size);
      --hole;
      ++moves;
    } while (hole != 0 &&
             compare(temporary, bytes + (hole - 1) * elem_size, context) < 0);
    memcpy(bytes + hole * elem_size, temporary, elem_size);
    if (moves_limit != 0 && moves > moves_limit) return 0;
  }
  return 1;
}

static inline void edg_sort_loop(unsigned char *data, size_t count,
                                    size_t elem_size, Edg_compare_fn compare,
                                    void *context, unsigned depth)
{
  max_align_t pivot[(elem_size + sizeof(max_align_t) - 1) /
                      sizeof(max_align_t) + 1];
  while (count > 15) {
    size_t mid = count / 2, left = 0, right = count, scan = 0;
    if (depth == 0) {
      edg_heap_sort(data, count, elem_size, compare, context);
      return;
    }
    --depth;
    if (compare(data + mid * elem_size, data, context) < 0) {
      edg_swap_at(data + mid * elem_size, data, elem_size);
    }
    if (compare(data + (count - 1) * elem_size, data + mid * elem_size,
                  context) < 0) {
      edg_swap_at(data + (count - 1) * elem_size, data + mid * elem_size, elem_size);
      if (compare(data + mid * elem_size, data, context) < 0) {
        edg_swap_at(data + mid * elem_size, data, elem_size);
      }
    }
    memcpy(pivot, data + mid * elem_size, elem_size);
    while (scan < right) {
      int order = compare(data + scan * elem_size, pivot, context);
      if (order < 0) {
        edg_swap_at(data + scan * elem_size, data + left * elem_size, elem_size);
        ++scan;
        ++left;
      } else if (order > 0) {
        --right;
        edg_swap_at(data + scan * elem_size, data + right * elem_size, elem_size);
      } else ++scan;
    }
    /* Recurse on the smaller partition: stack depth remains logarithmic. */
    if (left < count - right) {
      edg_sort_loop(data, left, elem_size, compare, context, depth);
      data += right * elem_size;
      count -= right;
    } else {
      edg_sort_loop(data + right * elem_size, count - right, elem_size,
                      compare, context, depth);
      count = left;
    }
  }
  (void)edg_insertion_sort(data, count, elem_size, compare, context, 0);
}

static inline void edg_sort(void *data, size_t count, size_t elem_size,
                               Edg_compare_fn compare, void *context)
{
  size_t i;
  a_boolean ascending = 1, descending = 1;
  unsigned char *bytes = (unsigned char *)data;
  if (count < 2) return;
  (void)edg_size_multiply(count, elem_size);
  for (i = 1; i < count && (ascending || descending); ++i) {
    int order = compare(bytes + (i - 1) * elem_size, bytes + i * elem_size, context);
    if (order > 0) ascending = 0;
    if (order < 0) descending = 0;
  }
  if (ascending) return;
  if (descending) {
    edg_reverse_array(data, count, elem_size);
    return;
  }
  if (edg_insertion_sort(data, count, elem_size, compare, context, 8)) return;
  edg_sort_loop(bytes, count, elem_size, compare, context,
                  (unsigned)edg_floor_log2_u64(count) * 2);
}

static inline void edg_array_sort(Edg_array *array, Edg_compare_fn compare,
                                     void *context)
{
  EDG_UTIL_ASSERT(array->relocate == NULL);
  edg_sort(array->data, array->len, array->elem_size, compare, context);
}

/* Sets use the same efficient table, with no associated value. */
typedef Edg_map Edg_set;

static inline void edg_set_init(Edg_set *set, size_t key_size,
                                  Edg_allocator allocator, unsigned mask_width,
                                  Edg_hash_fn hash, Edg_equal_fn equal,
                                  Edg_copy_fn copy, Edg_destroy_fn destroy)
{
  edg_map_init(set, key_size, 0, allocator, mask_width, hash, equal, copy, destroy,
                 NULL, NULL);
}

static inline a_boolean edg_set_contains(const Edg_set *set, const void *key)
{
  return edg_map_get(set, key) != NULL;
}

static inline void edg_set_add(Edg_set *set, const void *key)
{
  (void)edg_map_set(set, key, NULL);
}

static inline a_boolean edg_set_remove(Edg_set *set, const void *key)
{
  return edg_map_remove(set, key);
}

static inline void edg_set_clear(Edg_set *set) { edg_map_clear(set); }
static inline void edg_set_destroy(Edg_set *set) { edg_map_destroy(set); }

typedef struct Edg_multi_map_values {
  Edg_array values;
  size_t allocation_bytes;
} Edg_multi_map_values;

typedef struct Edg_multi_map {
  Edg_map map;
  size_t elem_size, inline_capacity;
  Edg_allocator object_allocator, element_allocator;
  Edg_copy_fn copy;
  Edg_destroy_fn destroy;
} Edg_multi_map;

EDG_CALLBACK_ACCESSORS(multi_map, Edg_multi_map, copy, copy, Edg_copy_fn)
EDG_CALLBACK_ACCESSORS(multi_map, Edg_multi_map, destroy, destroy, Edg_destroy_fn)

static inline void edg_multi_map_init(Edg_multi_map *map, size_t key_size,
                                         size_t elem_size,
                                         Edg_allocator map_allocator,
                                         Edg_allocator object_allocator,
                                         Edg_allocator element_allocator,
                                         unsigned mask_width,
                                         size_t inline_capacity,
                                         Edg_hash_fn hash, Edg_equal_fn equal,
                                         Edg_copy_fn key_copy,
                                         Edg_destroy_fn key_destroy,
                                         Edg_copy_fn copy,
                                         Edg_destroy_fn destroy)
{
  edg_map_init(&map->map, key_size, sizeof(Edg_multi_map_values *), map_allocator,
                 mask_width, hash, equal, key_copy, key_destroy, NULL, NULL);
  map->elem_size = elem_size;
  map->inline_capacity = inline_capacity;
  map->object_allocator = object_allocator;
  map->element_allocator = element_allocator;
  edg_multi_map_set_copy(map, copy);
  edg_multi_map_set_destroy(map, destroy);
}

static inline Edg_array *edg_multi_map_get(const Edg_multi_map *map,
                                             const void *key)
{
  Edg_multi_map_values **value =
    (Edg_multi_map_values **)edg_map_get(&map->map, key);
  return value == NULL ? NULL : &(*value)->values;
}

static inline Edg_array *edg_multi_map_get_or_alloc(Edg_multi_map *map,
                                                      const void *key)
{
  Edg_array *result = edg_multi_map_get(map, key);
  if (result == NULL) {
    size_t offset = edg_align_size(sizeof(Edg_multi_map_values)), bytes;
    size_t size = edg_size_add(offset,
                                edg_size_multiply(map->elem_size,
                                                    map->inline_capacity));
    Edg_multi_map_values *value =
      (Edg_multi_map_values *)edg_alloc(map->object_allocator, size, &bytes);
    value->allocation_bytes = bytes;
    edg_array_init_buffer(&value->values, map->elem_size, map->element_allocator,
                             0, edg_multi_map_get_copy(map),
                             edg_multi_map_get_destroy(map),
                             map->inline_capacity == 0 ? NULL :
                               (void *)((char *)value + offset),
                             map->inline_capacity);
    (void)edg_map_set(&map->map, key, &value);
    result = &value->values;
  }
  return result;
}

static inline a_boolean edg_multi_map_remove(Edg_multi_map *map,
                                                const void *key)
{
  Edg_multi_map_values **entry =
    (Edg_multi_map_values **)edg_map_get(&map->map, key);
  Edg_multi_map_values *value;
  if (entry == NULL) return 0;
  value = *entry;
  edg_array_destroy(&value->values);
  edg_dealloc(map->object_allocator, value, value->allocation_bytes);
  return edg_map_remove(&map->map, key);
}

static inline void edg_multi_map_take(Edg_multi_map *map, const void *key,
                                         Edg_array *out)
{
  Edg_array *value = edg_multi_map_get(map, key);
  EDG_UTIL_ASSERT(value != NULL);
  edg_array_copy(out, value);
  (void)edg_multi_map_remove(map, key);
}

static inline void edg_multi_map_clear(Edg_multi_map *map)
{
  size_t cursor = 0;
  void *entry;
  while (edg_map_next(&map->map, &cursor, NULL, &entry)) {
    Edg_multi_map_values *value = *(Edg_multi_map_values **)entry;
    edg_array_destroy(&value->values);
    edg_dealloc(map->object_allocator, value, value->allocation_bytes);
  }
  edg_map_clear(&map->map);
}

static inline void edg_multi_map_destroy(Edg_multi_map *map)
{
  edg_multi_map_clear(map);
  edg_map_destroy(&map->map);
}

typedef void (*Edg_diff_fn)(const void *left, const void *right, void *context);

typedef struct Edg_sequence_comparator {
  const void *left, *right;
  size_t left_len, right_len, left_size, right_size;
  size_t *table;
  size_t allocation_bytes;
  Edg_allocator allocator;
  Edg_equal_fn equal;
} Edg_sequence_comparator;

EDG_CALLBACK_ACCESSORS(sequence_comparator, Edg_sequence_comparator, equal,
                        equal, Edg_equal_fn)
#undef EDG_CALLBACK_ACCESSORS

static inline void edg_sequence_comparator_init(Edg_sequence_comparator *seq,
                                                   const void *left,
                                                   size_t left_len,
                                                   size_t left_size,
                                                   const void *right,
                                                   size_t right_len,
                                                   size_t right_size,
                                                   Edg_equal_fn equal,
                                                   Edg_allocator allocator)
{
  size_t a, b, columns = edg_size_add(right_len, 1);
  size_t entries = edg_size_multiply(edg_size_add(left_len, 1), columns);
  seq->left = left;
  seq->right = right;
  seq->left_len = left_len;
  seq->right_len = right_len;
  seq->left_size = left_size;
  seq->right_size = right_size;
  edg_sequence_comparator_set_equal(seq, equal);
  seq->allocator = allocator;
  seq->table = (size_t *)edg_alloc(allocator,
                                     edg_size_multiply(entries, sizeof(size_t)),
                                     &seq->allocation_bytes);
  for (a = 0; a <= left_len; ++a) seq->table[a * columns + right_len] = 0;
  for (b = 0; b <= right_len; ++b) seq->table[left_len * columns + b] = 0;
  for (a = left_len; a != 0; --a) {
    for (b = right_len; b != 0; --b) {
      size_t i = a - 1, j = b - 1;
      if (equal((const char *)left + i * left_size,
                  (const char *)right + j * right_size)) {
        seq->table[i * columns + j] = seq->table[a * columns + b] + 1;
      } else {
        size_t deleted = seq->table[a * columns + j];
        size_t inserted = seq->table[i * columns + b];
        seq->table[i * columns + j] = deleted > inserted ? deleted : inserted;
      }
    }
  }
}

static inline void edg_sequence_comparator_diff(const Edg_sequence_comparator *seq,
                                                    Edg_diff_fn consume,
                                                    void *context)
{
  size_t a = 0, b = 0, columns = seq->right_len + 1;
  while (a < seq->left_len || b < seq->right_len) {
    const void *left = a < seq->left_len ?
                        (const char *)seq->left + a * seq->left_size : NULL;
    const void *right = b < seq->right_len ?
                         (const char *)seq->right + b * seq->right_size : NULL;
    if (a < seq->left_len && b < seq->right_len &&
        edg_sequence_comparator_get_equal(seq)(left, right)) {
      consume(left, right, context);
      ++a;
      ++b;
    } else if (b < seq->right_len &&
               (a == seq->left_len ||
                seq->table[a * columns + b + 1] >=
                  seq->table[(a + 1) * columns + b])) {
      consume(NULL, right, context);
      ++b;
    } else {
      consume(left, NULL, context);
      ++a;
    }
  }
}

static inline void edg_sequence_comparator_destroy(Edg_sequence_comparator *seq)
{
  edg_dealloc(seq->allocator, seq->table, seq->allocation_bytes);
  seq->table = NULL;
  seq->allocation_bytes = 0;
}

#define EDG_SHA256_DIGEST_BYTES 32
#define EDG_SHA256_CHUNK_BYTES 64

typedef struct a_sha256_digest {
  unsigned char bytes[EDG_SHA256_DIGEST_BYTES];
} a_sha256_digest;

typedef struct a_sha256_hash {
  unsigned char chunk_bytes[EDG_SHA256_CHUNK_BYTES];
  uint32_t chunk_len;
  uint64_t num_bits_of_data;
  uint32_t state[8];
} a_sha256_hash;

static inline void edg_sha256_init(a_sha256_hash *hash)
{
  static const uint32_t initial[8] = {
    UINT32_C(0x6a09e667), UINT32_C(0xbb67ae85), UINT32_C(0x3c6ef372),
    UINT32_C(0xa54ff53a), UINT32_C(0x510e527f), UINT32_C(0x9b05688c),
    UINT32_C(0x1f83d9ab), UINT32_C(0x5be0cd19)
  };
  memset(hash, 0, sizeof(*hash));
  memcpy(hash->state, initial, sizeof(initial));
}

static inline void edg_sha256_transform(a_sha256_hash *hash)
{
  static const uint32_t constants[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
  };
  uint32_t schedule[64], state[8];
  size_t i;
  EDG_UTIL_ASSERT(hash->chunk_len == EDG_SHA256_CHUNK_BYTES);
  for (i = 0; i < 16; ++i) {
    const unsigned char *bytes = hash->chunk_bytes + i * 4;
    schedule[i] = ((uint32_t)bytes[0] << 24) |
                    ((uint32_t)bytes[1] << 16) |
                    ((uint32_t)bytes[2] << 8) | bytes[3];
  }
  for (i = 16; i < 64; ++i) {
    uint32_t a = schedule[i - 15], b = schedule[i - 2];
    uint32_t s0 = edg_rotate_right_u32(a, 7) ^ edg_rotate_right_u32(a, 18) ^ (a >> 3);
    uint32_t s1 = edg_rotate_right_u32(b, 17) ^ edg_rotate_right_u32(b, 19) ^ (b >> 10);
    schedule[i] = schedule[i - 16] + s0 + schedule[i - 7] + s1;
  }
  memcpy(state, hash->state, sizeof(state));
  for (i = 0; i < 64; ++i) {
    uint32_t s1 = edg_rotate_right_u32(state[4], 6) ^
                    edg_rotate_right_u32(state[4], 11) ^
                    edg_rotate_right_u32(state[4], 25);
    uint32_t ch = (state[4] & state[5]) ^ (~state[4] & state[6]);
    uint32_t t1 = state[7] + s1 + ch + constants[i] + schedule[i];
    uint32_t s0 = edg_rotate_right_u32(state[0], 2) ^
                    edg_rotate_right_u32(state[0], 13) ^
                    edg_rotate_right_u32(state[0], 22);
    uint32_t maj = (state[0] & state[1]) ^ (state[0] & state[2]) ^ (state[1] & state[2]);
    uint32_t t2 = s0 + maj;
    state[7] = state[6];
    state[6] = state[5];
    state[5] = state[4];
    state[4] = state[3] + t1;
    state[3] = state[2];
    state[2] = state[1];
    state[1] = state[0];
    state[0] = t1 + t2;
  }
  for (i = 0; i < 8; ++i) hash->state[i] += state[i];
  hash->chunk_len = 0;
}

static inline void edg_sha256_update(a_sha256_hash *hash, const void *data,
                                        size_t len)
{
  const unsigned char *bytes = (const unsigned char *)data;
  hash->num_bits_of_data += (uint64_t)len * 8;
  while (len != 0) {
    size_t count = EDG_SHA256_CHUNK_BYTES - hash->chunk_len;
    if (count > len) count = len;
    memcpy(hash->chunk_bytes + hash->chunk_len, bytes, count);
    hash->chunk_len += (uint32_t)count;
    bytes += count;
    len -= count;
    if (hash->chunk_len == EDG_SHA256_CHUNK_BYTES) edg_sha256_transform(hash);
  }
}

static inline a_sha256_digest edg_sha256_compute_digest(a_sha256_hash *hash)
{
  a_sha256_digest digest;
  size_t i;
  hash->chunk_bytes[hash->chunk_len++] = 0x80;
  if (hash->chunk_len > EDG_SHA256_CHUNK_BYTES - sizeof(uint64_t)) {
    memset(hash->chunk_bytes + hash->chunk_len, 0,
             EDG_SHA256_CHUNK_BYTES - hash->chunk_len);
    hash->chunk_len = EDG_SHA256_CHUNK_BYTES;
    edg_sha256_transform(hash);
  }
  memset(hash->chunk_bytes + hash->chunk_len, 0,
           EDG_SHA256_CHUNK_BYTES - sizeof(uint64_t) - hash->chunk_len);
  edg_append_big_endian(hash->chunk_bytes + EDG_SHA256_CHUNK_BYTES -
                         sizeof(uint64_t), hash->num_bits_of_data, sizeof(uint64_t));
  hash->chunk_len = EDG_SHA256_CHUNK_BYTES;
  edg_sha256_transform(hash);
  for (i = 0; i < 8; ++i) edg_append_big_endian(digest.bytes + i * 4, hash->state[i], 4);
  return digest;
}

#endif /* EDG_UTIL_H */
