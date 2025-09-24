#include "WGPURenderer.h"

#include "Error.h"

#include <webgpu/webgpu.h>

typedef struct RequestWGPUDeviceData {
  WGPUDevice device;
  B32 done;
} RequestWGPUDeviceData;

typedef struct RequestWGPUAdapterData {
  WGPUAdapter adapter;
  B32 done;
} RequestWGPUAdapterData;

static void defaultInitWGPURenderer(WGPURenderer *r) {
  r->instance = NULL;
  r->adapter = NULL;
  r->device = NULL;
  r->queue = NULL;
  r->commandEncoder = NULL;
  r->currentCommandBuffer = NULL;

  sfDefaultInitQueue(&r->errorMessageQueue);
}

void setWGPUAdapter(WGPURequestAdapterStatus status, WGPUAdapter adapter,
                    char const *message, void *userData) {
  RequestWGPUAdapterData *data = userData;

  (void)message;

  if (status == WGPURequestAdapterStatus_Success)
    data->adapter = adapter;

  data->done = SF_TRUE;
}

void setWGPUDevice(WGPURequestDeviceStatus status, WGPUDevice device,
                   char const *message, void *userData) {
  RequestWGPUDeviceData *data = userData;

  (void)message;

  if (status == WGPURequestAdapterStatus_Success)
    data->device = device;

  data->done = SF_TRUE;
}

void unhandledWGPUError(WGPUErrorType type, char const *message,
                        void *userData) {
  unused(type) unused(message) unused(userData) assert(0);
}

#ifdef __EMSCRIPTEN__
#endif

void awaitWGPUAdapterRequest(RequestWGPUAdapterData *data) {
#ifdef __EMSCRIPTEN__
  while (!data->done)
    emscripten_sleep(100);
#else
  unused(data);
#endif
}

void awaitWGPUDeviceRequest(RequestWGPUDeviceData *data) {
#ifdef __EMSCRIPTEN__
  while (!data->done)
    emscripten_sleep(100);
#else
  unused(data);
#endif
}

void createWGPURenderer(SFArena *arena, WGPURenderer *r) {
  defaultInitWGPURenderer(r);

  unused(arena);

  {
    WGPUInstanceDescriptor desc;
    desc.nextInChain = NULL;

    r->instance = wgpuCreateInstance(&desc);
    assert(r->instance);
    if (!r->instance)
      goto error;
  }

  {
    RequestWGPUAdapterData request;
    WGPURequestAdapterOptions options;

    options.nextInChain = NULL;
    options.compatibleSurface = NULL;
    options.powerPreference = WGPUPowerPreference_HighPerformance;
    options.backendType = WGPUBackendType_Undefined;
    options.forceFallbackAdapter = 0;

    request.adapter = NULL;
    request.done = SF_FALSE;

    wgpuInstanceRequestAdapter(r->instance, &options, setWGPUAdapter, &request);
    awaitWGPUAdapterRequest(&request);

    r->adapter = request.adapter;
    if (!r->adapter)
      goto error;
  }

  {
    RequestWGPUDeviceData request;
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

    request.device = NULL;
    request.done = SF_FALSE;

    wgpuAdapterRequestDevice(r->adapter, &desc, setWGPUDevice, &request);
    awaitWGPUDeviceRequest(&request);

    r->device = request.device;
    if (!r->device)
      goto error;

    r->queue = wgpuDeviceGetQueue(r->device);
  }

  {
    WGPUCommandEncoderDescriptor desc;

    desc.nextInChain = NULL;
    desc.label = "CommandEncoder1";

    r->commandEncoder = wgpuDeviceCreateCommandEncoder(r->device, &desc);
    assert(r->commandEncoder);
    if (!r->commandEncoder)
      goto error;
  }

  wgpuDeviceSetUncapturedErrorCallback(r->device, unhandledWGPUError, r);

  return;

error:
  destroyWGPURenderer(r);
}

B32 validateWGPURenderer(WGPURenderer *r) {
  B32 result = SF_TRUE;

  unused(r);

  assert(r->instance);
  assert(r->adapter);
  assert(r->device);
  assert(r->queue);
  assert(r->commandEncoder);

  // result = result && !!r->instance;
  // result = result && !!r->adapter;
  // result = result && !!r->device;
  // result = result && !!r->queue;
  // result = result && !!r->commandEncoder;

  return result;
}

void beginWGPURendererFrame(WGPURenderer *r) { unused(r); }

void endWGPURendererFrame(WGPURenderer *r) { unused(r); }

void destroyWGPURenderer(WGPURenderer *r) {
  if (r->commandEncoder) {
    wgpuCommandEncoderRelease(r->commandEncoder);
    r->commandEncoder = NULL;
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
