#ifndef WGPU_RENDERER_H
#define WGPU_RENDERER_H

#include "SF.h"

#include <webgpu/webgpu.h>

typedef struct WGPURenderer {
  WGPUInstance instance;
  WGPUAdapter adapter;
  B32 wasAdapterRequestFinished;
  SFQueue errorMessageQueue;
} WGPURenderer;

void createWGPURenderer(SFArena *arena, WGPURenderer *r);
void destroyWGPURenderer(WGPURenderer *r);

#endif // WGPU_RENDERER_H
