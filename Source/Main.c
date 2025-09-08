#include <webgpu/webgpu.h>

int main(void) {
  WGPUInstanceDescriptor desc = {0};
  desc.nextInChain = NULL;

  WGPUInstance instance = wgpuCreateInstance(&desc);
  wgpuInstanceRelease(instance);
}
