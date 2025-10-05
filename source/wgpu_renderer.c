#include "wgpu_renderer.h"

#include <webgpu/webgpu.h>

static void wgpu_renderer_default_init(struct wgpu_renderer *r) {
	r->plataform = NIL_PLATAFORM;
	r->instance = NULL;
	r->adapter = NULL;
	r->device = NULL;
	r->queue = NULL;

	r->surface_is_configured = SF_FALSE;
	r->framebuffer_width = 0;
	r->framebuffer_height = 0;
	r->current_command_encoder = NULL;
	r->current_command_buffer = NULL;
	r->current_surface_texture.texture = NULL;
	r->current_surface_texture.suboptimal = SF_FALSE;
	r->current_surface_texture.status = WGPUSurfaceGetCurrentTextureStatus_Success;
	r->current_surface_texture_view = NULL;
	r->current_render_pass_encoder = NULL;
}

void wgpu_renderer_set_adapter(WGPURequestAdapterStatus status, WGPUAdapter adapter, char const *message, void *user_data) {
	intptr_t *handle = user_data;

	(void)message;

	if (WGPURequestAdapterStatus_Success == status)
		*handle = (intptr_t)adapter;
	else
		*handle = -1;
}

void wgpu_renderer_set_device(WGPURequestDeviceStatus status, WGPUDevice device, char const *message, void *user_data) {
	intptr_t *handle = user_data;

	(void)message;

	if (WGPURequestDeviceStatus_Success == status)
		*handle = (intptr_t)device;
	else
		*handle = -1;
}

void wgpu_renderer_unhandled_error(WGPUErrorType type, char const *message, void *data) {
	UNUSED(type);
	UNUSED(message);
	UNUSED(data);
}

#ifdef __EMSCRIPTEN__
#endif

void wgpu_renderer_await_adapter_request(intptr_t *data) {
#ifdef __EMSCRIPTEN__
	while (!*data)
		emscripten_sleep(100);
#else
	UNUSED(data);
#endif
}

void wgpu_renderer_await_device_request(intptr_t *data) {
#ifdef __EMSCRIPTEN__
	while (!*data)
		emscripten_sleep(100);
#else
	UNUSED(data);
#endif
}

void wgpu_renderer_init(plataform p, struct sf_arena *arena, struct wgpu_renderer *r) {
	intptr_t adapter_handle = 0;
	intptr_t device_handle = 0;
	WGPUInstanceDescriptor instance_desc;
	WGPURequestAdapterOptions adapter_options;
	WGPUDeviceDescriptor device_desc;
	WGPUSurfaceConfiguration surface_config;

	wgpu_renderer_default_init(r);

	r->plataform = p;

	UNUSED(arena);

	instance_desc.nextInChain = NULL;

	r->instance = wgpuCreateInstance(&instance_desc);
	assert(r->instance);
	if (!r->instance)
		goto error;

	plataform_init_wgpu_surface(r->plataform, r);
	assert(r->surface);
	if (!r->surface)
		goto error;

	adapter_options.nextInChain = NULL;
	adapter_options.compatibleSurface = r->surface;
	adapter_options.powerPreference = WGPUPowerPreference_HighPerformance;
	adapter_options.backendType = WGPUBackendType_Undefined;
	adapter_options.forceFallbackAdapter = 0;

	wgpuInstanceRequestAdapter(r->instance, &adapter_options, wgpu_renderer_set_adapter, &adapter_handle);
	wgpu_renderer_await_adapter_request(&adapter_handle);
	assert(-1 != adapter_handle);
	if (-1 == adapter_handle)
		goto error;

	r->adapter = (WGPUAdapter)adapter_handle;

	device_desc.nextInChain = NULL;
	device_desc.label = "device_1";
	device_desc.requiredFeatureCount = 0;
	device_desc.requiredFeatures = NULL;
	device_desc.requiredLimits = NULL;
	device_desc.defaultQueue.nextInChain = NULL;
	device_desc.defaultQueue.label = "queue_1";
	device_desc.deviceLostCallback = NULL;
	device_desc.deviceLostUserdata = NULL;

	wgpuAdapterRequestDevice(r->adapter, &device_desc, wgpu_renderer_set_device, &device_handle);
	wgpu_renderer_await_device_request(&device_handle);
	assert(-1 != device_handle);
	if (-1 == device_handle)
		goto error;

	r->device = (WGPUDevice)device_handle;
	r->queue = wgpuDeviceGetQueue(r->device);

	plataform_window_dimensions(r->plataform, &r->framebuffer_width, &r->framebuffer_height);

	surface_config.nextInChain = NULL;
	surface_config.device = r->device;
	surface_config.format = wgpuSurfaceGetPreferredFormat(r->surface, r->adapter);
	surface_config.usage = WGPUTextureUsage_RenderAttachment;
	surface_config.viewFormatCount = 0;
	surface_config.viewFormats = 0;
	surface_config.alphaMode = WGPUCompositeAlphaMode_Auto;
	surface_config.width = r->framebuffer_width;
	surface_config.height = r->framebuffer_height;
	surface_config.presentMode = WGPUPresentMode_Fifo;


	wgpuSurfaceConfigure(r->surface, &surface_config);
	r->surface_is_configured = SF_TRUE;

	wgpuDeviceSetUncapturedErrorCallback(r->device, wgpu_renderer_unhandled_error, r);

	return;

error:
	wgpu_renderer_deinit(r);
}

b32 wgpu_renderer_validate(struct wgpu_renderer *r) {
	b32 result = SF_TRUE;

	UNUSED(r);

	return result;
}

void wgpu_renderer_begin_frame(struct wgpu_renderer *r) {
	WGPUTextureViewDescriptor texture_view_desc;
	WGPUCommandEncoderDescriptor command_encoder_desc;
	WGPUCommandBufferDescriptor command_buffer_desc;
	WGPURenderPassDescriptor render_pass_desc;
	WGPURenderPassColorAttachment color_attachment;

	wgpuSurfaceGetCurrentTexture(r->surface, &r->current_surface_texture);
	if (WGPUSurfaceGetCurrentTextureStatus_Success != r->current_surface_texture.status) {
		// TODO(samsal): Handle surface lost and timeout properly
		assert(0);
		return;
	}

	texture_view_desc.nextInChain = NULL;
	texture_view_desc.label = "current_surface_texture_view";
	texture_view_desc.format = wgpuTextureGetFormat(r->current_surface_texture.texture);
	texture_view_desc.dimension = WGPUTextureViewDimension_2D;
	texture_view_desc.baseMipLevel = 0;
	texture_view_desc.mipLevelCount = 1;
	texture_view_desc.baseArrayLayer = 0;
	texture_view_desc.arrayLayerCount = 1;
	texture_view_desc.aspect = WGPUTextureAspect_All;

	r->current_surface_texture_view = wgpuTextureCreateView(r->current_surface_texture.texture, &texture_view_desc);
	assert(r->current_surface_texture_view);
	if (!r->current_surface_texture_view)
		return;

	command_encoder_desc.nextInChain = NULL;
	command_encoder_desc.label = "command_encoder_1";

	r->current_command_encoder = wgpuDeviceCreateCommandEncoder(r->device, &command_encoder_desc);
	assert(r->current_command_encoder);
	if (!r->current_command_encoder)
		return;

	color_attachment.nextInChain = NULL;
	color_attachment.view = r->current_surface_texture_view;
	color_attachment.resolveTarget = NULL;
	color_attachment.loadOp =  WGPULoadOp_Clear;
	color_attachment.storeOp = WGPUStoreOp_Store;
	color_attachment.clearValue.r = 0.9;
	color_attachment.clearValue.g = 0.3;
	color_attachment.clearValue.b = 0.3;
	color_attachment.clearValue.a = 1.0;

	render_pass_desc.nextInChain = NULL;
	render_pass_desc.label = "render_pass_1";
	render_pass_desc.colorAttachmentCount = 1;
	render_pass_desc.colorAttachments = &color_attachment;
	render_pass_desc.depthStencilAttachment = NULL;
	render_pass_desc.occlusionQuerySet = NULL;
	render_pass_desc.timestampWrites = NULL;

	r->current_render_pass_encoder = wgpuCommandEncoderBeginRenderPass(r->current_command_encoder, &render_pass_desc);
	assert(r->current_render_pass_encoder);
	if (!r->current_render_pass_encoder)
		return;

	wgpuRenderPassEncoderEnd(r->current_render_pass_encoder);
	wgpuRenderPassEncoderRelease(r->current_render_pass_encoder);
	r->current_render_pass_encoder = NULL;

	command_buffer_desc.nextInChain = NULL;
	command_buffer_desc.label = "command_buffer_1";

	r->current_command_buffer = wgpuCommandEncoderFinish(r->current_command_encoder, &command_buffer_desc);
	assert(r->current_command_buffer);
	if (!r->current_command_buffer)
		return;

	wgpuCommandEncoderRelease(r->current_command_encoder);
	r->current_command_encoder = NULL;
}

void wgpu_renderer_end_frame(struct wgpu_renderer *r) {
	wgpuQueueSubmit(r->queue, 1, &r->current_command_buffer);

	wgpuCommandBufferRelease(r->current_command_buffer);
	r->current_command_buffer = NULL;

	wgpuTextureViewRelease(r->current_surface_texture_view);
	r->current_surface_texture_view = NULL;
	
	wgpuSurfacePresent(r->surface);

	wgpuTextureRelease(r->current_surface_texture.texture);
	r->current_surface_texture.texture = NULL;
}

void wgpu_renderer_deinit(struct wgpu_renderer *r) {
	if (r->current_command_encoder) {
		wgpuCommandEncoderRelease(r->current_command_encoder);
		r->current_command_encoder = NULL;
	}

	if (r->current_command_buffer) {
		wgpuCommandBufferRelease(r->current_command_buffer);
		r->current_command_buffer = NULL;
	}

	if (r->current_surface_texture.texture) {
		wgpuTextureRelease(r->current_surface_texture.texture);
		r->current_surface_texture.texture = NULL;
	}

	if (r->current_render_pass_encoder) {
		wgpuRenderPassEncoderRelease(r->current_render_pass_encoder);
		r->current_render_pass_encoder = NULL;
	}

	if (r->surface_is_configured) {
		wgpuSurfaceUnconfigure(r->surface);
		r->surface_is_configured = SF_FALSE;
	}

	if (r->queue) {
		wgpuQueueRelease(r->queue);
		r->queue = NULL;
	}

	if (r->device) {
		wgpuDeviceRelease(r->device);
		r->device = NULL;
	}

	if (r->adapter) {
		wgpuAdapterRelease(r->adapter);
		r->adapter = NULL;
	}

	if (r->surface) {
		wgpuSurfaceRelease(r->surface);
		r->surface = NULL;
	}

	if (r->instance) {
		wgpuInstanceRelease(r->instance);
		r->instance = NULL;
	}
}
