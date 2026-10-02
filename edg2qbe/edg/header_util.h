/*
Part of the EDG Compiler Project, under the Apache License v2.0 with LLVM
Exceptions.
See https://edgcpp.org/LICENSE.txt for license information.
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
*/

#ifndef EDG_HEADER_UTIL_H
#define EDG_HEADER_UTIL_H 1

#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* Optional values have explicit lifetimes.  A consumer destroys value, when
   has_value is set, before clearing or replacing an owning optional. */
#define EDG_DECLARE_OPTIONAL(name, type) \
  typedef struct name { a_boolean has_value; type value; } name

#define EDG_DECLARE_PTR_WITH_FLAG(name, type) \
  typedef struct name { type ptr; a_boolean flag; } name

/* Save into a typed local and restore explicitly on every exit path. */
#define EDG_SAVE_VALUE(type, saved, variable) type saved = (variable)
#define EDG_RESTORE_VALUE(variable, saved) ((variable) = (saved))

typedef struct a_string_view {
  const char *data;
  size_t len;
} a_string_view;

static inline a_string_view edg_string_view(const char *data, size_t len)
{
  a_string_view result;
  result.data = data;
  result.len = len;
  return result;
}

static inline a_string_view edg_string_view_cstr(const char *data)
{
  return edg_string_view(data, strlen(data));
}

static inline a_boolean edg_string_view_equal(const void *left,
                                               const void *right)
{
  const a_string_view *a = (const a_string_view *)left;
  const a_string_view *b = (const a_string_view *)right;
  return a->len == b->len &&
         (a->len == 0 || memcmp(a->data, b->data, a->len) == 0);
}

static inline int edg_string_view_compare(a_string_view a, a_string_view b)
{
  size_t len = a.len < b.len ? a.len : b.len;
  int result = len == 0 ? 0 : memcmp(a.data, b.data, len);
  if (result != 0) return result;
  return (a.len > b.len) - (a.len < b.len);
}

/* Keep the original string hash (a 32-bit, zero-seeded polynomial). */
static inline size_t edg_string_view_hash(const void *key)
{
  const a_string_view *view = (const a_string_view *)key;
  uint32_t hash = 0;
  size_t i;
  for (i = 0; i < view->len; ++i) {
    hash = hash * UINT32_C(33) + (uint32_t)view->data[i];
  }
  return (size_t)hash;
}

#endif /* EDG_HEADER_UTIL_H */
