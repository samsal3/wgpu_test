#include "WGPURenderer.h"
#include "Plataform.h"

#include <stdio.h>

#include <SF.h>
#include <webgpu/webgpu.h>

int main(void) {
  WGPURenderer renderer = {0};
  SFArena arena  = {0};

  sfAllocateArena(1024 * 1024, 16, &arena);
  if (!arena.data)
    goto cleanup;

  if (!initPlataform(300, 300))
    goto cleanup;

  createWGPURenderer(&arena, &renderer);
  if (!validateWGPURenderer(&renderer))
    goto cleanup;

  while (pollPlataformEvents()) {}

cleanup:
  destroyWGPURenderer(&renderer);
  deinitPlataform();
  sfFreeArena(&arena);
}
