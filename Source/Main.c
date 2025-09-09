#include <webgpu/webgpu.h>

#include <Common/SF.h>
#include <Rendering/WGPURenderer.h>

int main(void) {
  SFArena arena  = {0};
  sfAllocateArena(1024 * 1024, 16, &arena);
  if (arena.data) {
    WGPURenderer renderer = {0};
    createWGPURenderer(&arena, &renderer);
    assert(sfIsQueueEmpty(&renderer.errorMessageQueue));

    sfFreeArena(&arena);
  }

}
