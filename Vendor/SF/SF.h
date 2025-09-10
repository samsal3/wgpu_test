#ifndef SF_H
#define SF_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef SF_IMPLEMENTATION
#define SF_EXPORT
#else
#define SF_EXPORT
#endif // SF_IMPLEMENTATION

typedef char Byte;
typedef ptrdiff_t Size;

typedef uint64_t U64;
typedef uint32_t U32;

typedef int64_t I64;
typedef int32_t I32;

typedef int32_t B32;

#define SF_TRUE 1
#define SF_FALSE 0

#define SF_SIZE(a) (Size)(sizeof(a) / sizeof(0 [a]))
#define SF_MIN(a, b) ((a) < (b) ? (a) : (b))
#define SF_MAX(a, b) ((a) > (b) ? (a) : (b))

typedef struct SFString8 {
  char const *data;
  Size size;
} SFString8;

typedef struct SFQueue {
  struct SFQueue *prev;
  struct SFQueue *next;
} SFQueue;

typedef struct SFArena {
  Byte *data;
  U64 position;
  U64 capacity;
  U64 alignment;
} SFArena;

#define unused(e) (void)e;
#define sfOffsetOf(ty, f) (ptrdiff_t)(&((ty *)(NULL))->f)
#define sfContainerOf(ty, p, f) (ty *)((char *)(p)-SF_OFFSET_OF(ty, f))

#define sfDefaultInitQueue(q)                                                  \
  do {                                                                         \
    (q)->next = (q);                                                           \
    (q)->prev = (q);                                                           \
  } while (0)

#define sfForEachQueue(q, h) for ((q) = (h)->next; (q) != (h); (q) = (q)->next)

#define sfInsertIntoQueueHead(q, h)                                            \
  do {                                                                         \
    (q)->next = (h)->next;                                                     \
    (q)->prev = (h);                                                           \
    (q)->next->prev = (q);                                                     \
    (h)->next = (q);                                                           \
  } while (0)

#define sfRemoveFromQueue(q)                                                   \
  do {                                                                         \
    (q)->prev->next = (q)->next;                                               \
    (q)->next->prev = (q)->prev;                                               \
  } while (0)

#define sfIsQueueEmpty(q) ((q)->next == (q))

#define sfAsString8(s, out)                                                    \
  do {                                                                         \
    (out)->data = &(s)[0];                                                     \
    (out)->size = SF_SIZE(s);                                                  \
  } while (0)

#define sfDefaultInitString8(s)                                                \
  do {                                                                         \
    (s)->data = NULL;                                                          \
    (s)->size = 0;                                                             \
  } while (0)

SF_EXPORT U64 sfAlignU64(U64 size, U64 alignment);

SF_EXPORT void *sfAllocateMemory(U64 size);
SF_EXPORT void sfFreeMemory(void *data);

SF_EXPORT void sfAllocateArena(U64 capactiy, U64 alignment, SFArena *arena);
SF_EXPORT void sfFreeArena(SFArena *arena);

SF_EXPORT void *sfAllocate(SFArena *arena, U64 size);

SF_EXPORT void sfLoadFileToString8(SFArena *arena, SFString8 const *path,
                                   SFString8 *out);

SF_EXPORT B32 sfCompareString8(SFString8 const *lhs, SFString8 const *rhs);

SF_EXPORT void sfNullTerminateString8(SFArena *arena, SFString8 const *s,
                                      SFString8 *out);

#ifdef SF_IMPLEMENTATION

SF_EXPORT U64 sfAlignU64(U64 value, U64 alignment) {
  return (value + alignment - 1) & ~(alignment - 1);
}

SF_EXPORT void *sfAllocateMemory(U64 size) { return malloc(size); }

SF_EXPORT void sfFreeMemory(void *data) { free(data); }

SF_EXPORT void sfAllocateArena(U64 capacity, U64 alignment, SFArena *arena) {
  arena->data = sfAllocateMemory(capacity);
  if (arena->data) {
    arena->position = 0;
    arena->capacity = capacity;
    arena->alignment = alignment;
  } else {
    arena->position = 0;
    arena->capacity = 0;
    arena->alignment = 0;
  }
}

SF_EXPORT void sfFreeArena(SFArena *arena) {
  sfFreeMemory(arena->data);
  arena->data = NULL;
  arena->position = 0;
  arena->capacity = 0;
  arena->alignment = 0;
}

SF_EXPORT void *sfAllocate(SFArena *arena, U64 size) {
  U64 i = 0, previousPosition = 0;
  Byte *data = NULL;
  U64 reqSize = arena->position + size;

  if (reqSize > arena->capacity)
    return NULL;

  previousPosition = arena->position;
  arena->position = sfAlignU64(reqSize, arena->alignment);

  data = &arena->data[previousPosition];
  for (i = 0; i < size; ++i)
    data[i]  = 0xD1;

  return data;
}

SF_EXPORT void sfLoadFileToString8(SFArena *arena, SFString8 const *path,
                                   SFString8 *out) {
  FILE *fd = NULL;
  long bufferSize = 0;
  size_t reqSize = 0;
  Byte *data = 0;
  SFString8 nullTerminatedPath;

  sfDefaultInitString8(&nullTerminatedPath);
  sfDefaultInitString8(out);

  sfNullTerminateString8(arena, path, &nullTerminatedPath);
  if (!nullTerminatedPath.data)
    goto cleanup;

  fd = fopen(nullTerminatedPath.data, "r");
  if (!fd)
    goto cleanup;

  if (0 != fseek(fd, 0L, SEEK_END))
    goto cleanup;

  bufferSize = ftell(fd);
  if (-1 == bufferSize)
    goto cleanup;

  data = sfAllocate(arena, bufferSize);
  if (!data)
    goto cleanup;

  if (0 != fseek(fd, 0L, SEEK_SET))
    goto cleanup;

  reqSize = fread(data, sizeof(char), bufferSize, fd);
  if (ferror(fd))
    goto cleanup;

  out->data = data;
  out->size = reqSize;

cleanup:
  if (fd)
    fclose(fd);
}

SF_EXPORT B32 sfCompareString8(SFString8 const *lhs, SFString8 const *rhs) {
  Size i = 0;

  if (lhs->size != rhs->size)
    return SF_FALSE;

  for (i = 0; i < SF_MIN(lhs->size, rhs->size); ++i)
    if (lhs->data[i] != rhs->data[i])
      return SF_FALSE;

  return SF_TRUE;
}

SF_EXPORT void sfNullTerminateString8(SFArena *arena, SFString8 const *s,
                                      SFString8 *out) {
  Byte *data = NULL;

  sfDefaultInitString8(out);

  data = sfAllocate(arena, s->size + 1);
  if (data) {
    Size i = 0;

    for (i = 0; i < s->size; ++i)
      data[i] = s->data[i];

    data[s->size] = '\0';

    out->data = data;
    out->size = s->size;
  }
}

#endif // SF_IMPLEMENTATION

#endif // SF_H
