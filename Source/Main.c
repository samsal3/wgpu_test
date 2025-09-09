#include <webgpu/webgpu.h>

#include "SF.h"

int main(void) {
  SFArena arena = sfAllocateArena(1024 * 1024, 16);

  SFS8 testFile = sfLoadFileToS8(&arena, SF_S8("test.txt"));
  (void)testFile;

  SFS8 s = SF_S8("test");
  (void)s;

  WGPUInstanceDescriptor desc = {0};
  desc.nextInChain = NULL;

  WGPUInstance instance = wgpuCreateInstance(&desc);
  wgpuInstanceRelease(instance);

  sfFreeArena(&arena);
}
