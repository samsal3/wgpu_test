#include "wgpu_renderer.h"

#include <webgpu/webgpu.h>

static void wgpu_renderer_default_init(struct wgpu_renderer *r) {
  r->instance = NULL;
  r->adapter = NULL;
  r->device = NULL;
  r->queue = NULL;
  r->command_encoder = NULL;
  r->current_command_buffer = NULL;

  SF_QUEUE_INIT(&r->error_message_queue);
}

void wgpu_renderer_set_adapter(WGPURequestAdapterStatus status, WGPUAdapter adapter,
                    char const *message, void *handle) {
  intptr_t *data = handle;

  (void)message;

  if (status == WGPURequestAdapterStatus_Success)
    *data = (intptr_t)adapter;

  *data = -1;
}

void wgpu_renderer_set_device(WGPURequestDeviceStatus status, WGPUDevice device,
                   char const *message, void *handle) {
  intptr_t  *data = handle;

  (void)message;

  if (status == WGPURequestDeviceStatus_Success)
    *data = (intptr_t)device;

  *data = -1;
}

void wgpu_renderer_unhandled_error(WGPUErrorType type, char const *message,
                        void *data) {
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

void wgpu_renderer_init(struct sf_arena *arena, struct wgpu_renderer *r) {
  wgpu_renderer_default_init(r);

  UNUSED(arena);

  {
    WGPUInstanceDescriptor desc;
    desc.nextInChain = NULL;

    r->instance = wgpuCreateInstance(&desc);
    assert(r->instance);
    if (!r->instance)
      goto error;
  }

  {
    intptr_t adapter_handle = 0;
    WGPURequestAdapterOptions options;

    options.nextInChain = NULL;
    options.compatibleSurface = NULL;
    options.powerPreference = WGPUPowerPreference_HighPerformance;
    options.backendType = WGPUBackendType_Undefined;
    options.forceFallbackAdapter = 0;

    wgpuInstanceRequestAdapter(r->instance, &options, wgpu_renderer_set_adapter, &adapter_handle);
    wgpu_renderer_await_adapter_request(&adapter_handle);
    if (-1 == adapter_handle)
      goto error;

    r->adapter = (WGPUAdapter)adapter_handle;
  }

  {
    intptr_t device_handle = 0;
    WGPUDeviceDescriptor desc;

    desc.nextInChain = NULL;
    desc.label = "Device1";
    desc.requiredFeatureCount = 0;
    desc.requiredFeatures = NULL;
    desc.requiredLimits = NULL;
    desc.defaultQueue.nextInChain = NULL;
    desc.defaultQueue.label = "Queue1";
    desc.deviceLostCallback = NULL;
    desc.deviceLostUserdata = NULL;


    wgpuAdapterRequestDevice(r->adapter, &desc, wgpu_renderer_set_device, &device_handle);
    wgpu_renderer_await_device_request(&device_handle);

    if (-1 == device_handle) 
      goto error;

    r->device = (WGPUDevice)device_handle;
    r->queue = wgpuDeviceGetQueue(r->device);
  }

  {
    WGPUCommandEncoderDescriptor desc;

    desc.nextInChain = NULL;
    desc.label = "command_encoder_1";

    r->command_encoder = wgpuDeviceCreateCommandEncoder(r->device, &desc);
    assert(r->command_encoder);
    if (!r->command_encoder)
      goto error;
  }

  wgpuDeviceSetUncapturedErrorCallback(r->device, wgpu_renderer_unhandled_error, r);

  return;

error:
  wgpu_renderer_deinit(r);
}

b32 wgpu_renderer_validate(struct wgpu_renderer *r) {
  b32 result = SF_TRUE;

  UNUSED(r);

  assert(r->instance);
  assert(r->adapter);
  assert(r->device);
  assert(r->queue);
  assert(r->command_encoder);

  // result = result && !!r->instance;
  // result = result && !!r->adapter;
  // result = result && !!r->device;
  // result = result && !!r->queue;
  // result = result && !!r->commandEncoder;

  return result;
}

void wgpu_renderer_begin_frame(struct wgpu_renderer *r) { UNUSED(r); }

void wgpu_renderer_end_frame(struct wgpu_renderer *r) { UNUSED(r); }

void wgpu_renderer_deinit(struct wgpu_renderer *r) {
  if (r->command_encoder) {
    wgpuCommandEncoderRelease(r->command_encoder);
    r->command_encoder = NULL;
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

  if (r->instance) {
    wgpuInstanceRelease(r->instance);
    r->instance = NULL;
  }
}
