#ifndef SF_H
#define SF_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

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

typedef struct SFS8 {
  Byte *data;
  Size size;
} SFS8;

#define SF_SIZE(a) (Size)(sizeof(a) / sizeof(0 [a]))
#define SF_S8(s) (SFS8){ .data = &(s)[0], .size = SF_SIZE(s) }

typedef struct SFQueue {
  struct SFQueue *prev;
  struct SFQueue *next;
} SFQueue;

#define SF_OFFSET_OF(ty, f) (ptrdiff_T)(&((ty *)(NULL))->f)
#define SF_CONTAINER_OF(ty, p, f) (ty *)((char *)(p) - SF_OFFSET_OF(ty, f))

#define SF_QUEUE_FOREACH(q, h)                                                 \
  for ((q) = (h)->next; (q) != (h); (q) = (q)->next)

#define SF_QUEUE_INSERT_HEAD(q, h)                                             \
  do {                                                                         \
    (q)->next = (h)->next;                                                     \
    (q)->prev = (h);                                                           \
    (q)->next->prev = (q);                                                     \
    (h)->next = (q);                                                           \
  } while (0)

#define SF_QUEUE_REMOVE(q)                                                     \
  do {                                                                         \
    (q)->prev->next = (q)->next;                                               \
    (q)->next->prev = (q)->prev;                                               \
  } while (0)

typedef struct SFArena {
  Byte *data;
  U64 position;
  U64 capacity;
  U64 alignment;
} SFArena;

SF_EXPORT U64 sfAlignU64(U64 size, U64 alignment);

SF_EXPORT void *sfAllocateMemory(U64 size);
SF_EXPORT void sfFreeMemory(void *data);

SF_EXPORT SFArena sfAllocateArena(U64 capactiy, U64 alignment);
SF_EXPORT void sfFreeArena(SFArena *arena);

SF_EXPORT void *sfAllocate(SFArena *arena, U64 size);

SF_EXPORT SFS8 sfLoadFileToS8(SFArena *arena, SFS8 path);
SF_EXPORT SFS8 sfEnsureNullTerminatedString(SFArena *arena, SFS8 s);

#ifdef SF_IMPLEMENTATION

SF_EXPORT U64 sfAlignU64(U64 value, U64 alignment) {
  return (value + alignment - 1) & ~(alignment - 1);
}

SF_EXPORT void *sfAllocateMemory(U64 size) { return malloc(size); }
SF_EXPORT void sfFreeMemory(void *data) { free(data); }

SF_EXPORT SFArena sfAllocateArena(U64 capactiy, U64 alignment) {
  SFArena arena = {0};

  arena.data = sfAllocateMemory(capactiy);
  if (arena.data)
    arena.alignment = alignment;

  return arena;
}

SF_EXPORT void sfFreeArena(SFArena *arena) {
  sfFreeMemory(arena->data);
  arena->data = NULL;
  arena->position = 0;
  arena->capacity = 0;
  arena->alignment = 0;
}


SF_EXPORT void *sfAllocate(SFArena *arena, U64 size) {
  U64 reqSize = arena->position + size;
  if (reqSize > arena->capacity)
    return NULL;

  U64 alignedPosition = sfAlignU64(reqSize, arena->alignment);
  Byte *data = &arena->data[arena->position]; 
  arena->position = alignedPosition;

  for (U64 i = 0; i < size; ++i)
    data[i] = 0;

  return data;
}

SF_EXPORT SFS8 sfLoadFileToS8(SFArena *arena, SFS8 path) {
  FILE *fd = NULL;
  SFS8 result = {0};

  SFS8 nullTerminatedPath = sfEnsureNullTerminatedString(arena, path);
  if (!nullTerminatedPath.data)
    goto cleanup;
  
  fd = fopen(nullTerminatedPath.data, "r");
  if (!fd)
    goto cleanup;

  if (0 != fseek(fd, 0L, SEEK_END))
    goto cleanup;

  long bufferSize = ftell(fd);
  if (-1 == bufferSize)
    goto cleanup;

  Byte *data = sfAllocate(arena, bufferSize);
  if (!data)
    goto cleanup;

  if (0 != fseek(fd, 0L, SEEK_SET))
    goto cleanup;

  size_t requiredSize = fread(data, sizeof(char), bufferSize, fd);
  if (ferror(fd))
    goto cleanup;

  result.data = data;
  result.size = requiredSize;
  
cleanup:
  if (fd)
    fclose(fd);
  
  return result;
}

SF_EXPORT SFS8 sfEnsureNullTerminatedString(SFArena *arena, SFS8 s) {
  SFS8 copy = {0};

  copy.data = sfAllocate(arena, s.size + 1);
  if (copy.data) {
    for (Size i = 0; i < s.size; ++i)
      copy.data[i] = s.data[i];

    copy.data[s.size] = '\0';
  }

  return copy;
}

#endif // SF_IMPLEMENTATION

#endif // SF_H
