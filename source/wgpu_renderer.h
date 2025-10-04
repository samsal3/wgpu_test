#ifndef WGPU_RENDERER_H
#define WGPU_RENDERER_H

#include <sf.h>

#include <webgpu/webgpu.h>

struct wgpu_frame {
   WGPUCommandEncoder encoder;
   WGPUCommandBuffer buffer;
};

struct wgpu_renderer {
   WGPUInstance instance;
   WGPUAdapter adapter;
   WGPUDevice device;
   WGPUQueue queue;
   WGPUCommandEncoder command_encoder;
   WGPUCommandBuffer current_command_buffer;

   struct wgpu_frame frames[2];
   u32 current_frame_index;

   struct sf_queue error_message_queue;
};

void wgpu_renderer_init(struct sf_arena *arena, struct wgpu_renderer *r);
b32 wgpu_renderer_validate(struct wgpu_renderer *r);

void wgpu_renderer_deinit(struct wgpu_renderer *r);

void wgpu_renderer_begin_frame(struct wgpu_renderer *r);
void wgpu_renderer_end_frame(struct wgpu_renderer *r);

#endif // WGPU_RENDERER_H
