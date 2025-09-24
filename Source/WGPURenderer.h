#ifndef WGPU_RENDERER_H
#define WGPU_RENDERER_H

#include <SF.h>

#include <webgpu/webgpu.h>

typedef struct WGPUFrame {
  WGPUCommandEncoder encoder;
  WGPUCommandBuffer buffer;
} WGPUFrame;

typedef struct WGPURenderer {
  WGPUInstance instance;
  WGPUAdapter adapter;
  WGPUDevice device;
  WGPUQueue queue;
  WGPUCommandEncoder commandEncoder;
  WGPUCommandBuffer currentCommandBuffer;

  WGPUFrame frames[2];
  U32 currentFrameIndex;

  SFQueue errorMessageQueue;
} WGPURenderer;

void createWGPURenderer(SFArena *arena, WGPURenderer *r);
B32 validateWGPURenderer(WGPURenderer *r);

void destroyWGPURenderer(WGPURenderer *r);

void beginWGPURendererFrame(WGPURenderer *r);
void endWGPURendererFrame(WGPURenderer *r);

#endif // WGPU_RENDERER_H
