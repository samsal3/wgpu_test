#ifndef WGPU_RENDERER_H
#define WGPU_RENDERER_H

#include "plataform.h"

#include <sf.h>

#include <webgpu/webgpu.h>

struct wgpu_frame {
	WGPUCommandEncoder encoder;
	WGPUCommandBuffer buffer;
};

struct wgpu_renderer {
	plataform plataform;
	WGPUInstance instance;
	WGPUSurface surface;
	WGPUAdapter adapter;
	WGPUDevice device;
	WGPUQueue queue;

	b32 surface_is_configured;
	i32 framebuffer_width;
	i32 framebuffer_height;

	WGPUCommandEncoder current_command_encoder;
	WGPUCommandBuffer current_command_buffer;
	WGPUSurfaceTexture current_surface_texture;
	WGPUTextureView current_surface_texture_view;
	WGPURenderPassEncoder current_render_pass_encoder;

};

void wgpu_renderer_init(plataform p, struct sf_arena *arena, struct wgpu_renderer *r);
b32 wgpu_renderer_validate(struct wgpu_renderer *r);

void wgpu_renderer_deinit(struct wgpu_renderer *r);

void wgpu_renderer_begin_frame(struct wgpu_renderer *r);
void wgpu_renderer_end_frame(struct wgpu_renderer *r);

#endif // WGPU_RENDERER_H
