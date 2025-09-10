#include <webgpu/webgpu.h>

#include <SF.h>
#include <Rendering/WGPURenderer.h>

int main(void) {
  WGPURenderer renderer = {0};
  SFArena arena  = {0};

  sfAllocateArena(1024 * 1024, 16, &arena);
  if (!arena.data)
    return 0;

  createWGPURenderer(&arena, &renderer);
  assert(sfIsQueueEmpty(&renderer.errorMessageQueue));

  sfFreeArena(&arena);
}
