#ifndef WGPU_RENDERER_H
#define WGPU_RENDERER_H

#include <Common/SF.h>

#include <webgpu/webgpu.h>

typedef struct WGPURenderer {
  WGPUInstance instance;
  WGPUAdapter adapter;
  WGPUDevice device;
  B32 hasDeviceRequestFinished;
  B32 hasAdapterRequestFinished;
  SFQueue errorMessageQueue;
} WGPURenderer;

void createWGPURenderer(SFArena *arena, WGPURenderer *r);
void destroyWGPURenderer(WGPURenderer *r);

#endif // WGPU_RENDERER_H
