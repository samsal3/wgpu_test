#ifndef SF_H
#define SF_H

#include <stdint.h>
#include <stdlib.h>

#ifdef SF_IMPLEMENTATION
#define SF_EXPORT 
#else
#define SF_EXPORT 
#endif // SF_IMPLEMENTATIONf

typedef unsigned char Byte;

typedef uint64_t U64;
typedef uint32_t U32;

typedef int64_t S64;
typedef int32_t S32;

typedef struct SfArena {
  Byte *data;
  U64 position;
  U64 capacity;
  U64 alignment;
} SfArena;

SF_EXPORT void *sfAllocateMemory(U64 size);
SF_EXPORT void sfFreeMemory(void *data);

SF_EXPORT SfArena sfAllocateArena(U64 capactiy, U64 alignment);
SF_EXPORT void sfFreeArena(SfArena *arena);

#ifdef SF_IMPLEMENTATION

SF_EXPORT void *sfAllocateMemory(U64 size) {
  return malloc(size);
}

SF_EXPORT void sfFreeMemory(void *data) {
  free(data);
}

SF_EXPORT SfArena sfAllocateArena(U64 capactiy, U64 alignment) {
  SfArena arena = {0};
  
  arena.data = sfAllocateMemory(capactiy);
  if (arena.data) {
    arena.position = 0;
    arena.capacity = 0;
    arena.alignment = alignment;
  }

  return arena;
}
SF_EXPORT void sfFreeArena(SfArena *arena) {
  sfFreeMemory(arena->data);
  arena->data = NULL;
  arena->position = 0;
  arena->capacity = 0 ;
  arena->alignment = 0;

}

#endif // SF_IMPLEMENTATION

#endif // SF_H
