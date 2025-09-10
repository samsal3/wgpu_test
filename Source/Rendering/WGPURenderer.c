#include <Rendering/WGPURenderer.h>

#include <Common/Error.h>

#include <webgpu/webgpu.h>

static void defaultInitWGPURenderer(WGPURenderer *r) {
  r->instance = NULL;
  r->adapter = NULL;
  r->device = NULL;
  r->queue = NULL;
  r->commandEncoder = NULL;
  r->currentCommandBuffer = NULL;

  r->hasDeviceRequestFinished = SF_FALSE;
  r->hasAdapterRequestFinished = SF_FALSE;
  sfDefaultInitQueue(&r->errorMessageQueue);
}

void setWGPUAdapter(WGPURequestAdapterStatus status, WGPUAdapter adapter,
                    char const *message, void *userData) {
  WGPURenderer *r = userData;

  (void)message;

  if (status == WGPURequestAdapterStatus_Success)
    r->adapter = adapter;

  r->hasAdapterRequestFinished = SF_TRUE;
}

void setWGPUDevice(WGPURequestDeviceStatus status, WGPUDevice device,
                   char const *message, void *userData) {
   WGPURenderer *r = userData;

  (void)message;

  if (status == WGPURequestAdapterStatus_Success)
    r->device = device;

  r->hasDeviceRequestFinished = SF_TRUE;
}

void unhandledWGPUError(WGPUErrorType type, char const* message, void* userData) {
  unused(type)
  unused(message)
  unused(userData)
  assert(0);
}

#ifdef __EMSCRIPTEN__
#endif

void awaitAdapterRequest(WGPURenderer *r) {
#ifdef __EMSCRIPTEN__
  while (!r->hasAdapterRequestFinished)
    emscripten_sleep(100);
#else
  unused(r);
#endif
}

void awaitDeviceRequest(WGPURenderer *r) {
#ifdef __EMSCRIPTEN__
  while (!r->hasDeviceRequestFinished)
    emscripten_sleep(100);
#else
  unused(r);
#endif
}

typedef struct DeviceInformation {
  WGPUFeatureName *features;
  U32 featureCount;
  WGPUSupportedLimits limits;
} DeviceInformation;


void createWGPURenderer(SFArena *arena, WGPURenderer *r) {
  defaultInitWGPURenderer(r);

  unused(arena);

  {
    WGPUInstanceDescriptor desc;
    desc.nextInChain = NULL;

    r->instance = wgpuCreateInstance(&desc);
    if (!r->instance)
      goto error;
  }

  {
    WGPURequestAdapterOptions options;

    options.nextInChain = NULL;
    options.compatibleSurface = NULL;
    options.powerPreference = WGPUPowerPreference_HighPerformance;
    options.backendType = WGPUBackendType_Null;
    options.forceFallbackAdapter = 0;

    wgpuInstanceRequestAdapter(r->instance, &options, setWGPUAdapter, r);
    awaitAdapterRequest(r);
    if (!r->adapter)
      goto error;
  }

  {
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

    wgpuAdapterRequestDevice(r->adapter, &desc, setWGPUDevice, r); 
    awaitDeviceRequest(r); 
    if (!r->device)
      goto error;

    r->queue = wgpuDeviceGetQueue(r->device);
  }

 
  {
    WGPUCommandEncoderDescriptor desc;

    desc.nextInChain = NULL;
    desc.label = "CommandEncoder1";

    r->commandEncoder = wgpuDeviceCreateCommandEncoder(r->device, &desc);
    if (!r->commandEncoder)
      goto error;
  }


  wgpuDeviceSetUncapturedErrorCallback(r->device, unhandledWGPUError, r);

  return;

error:
  destroyWGPURenderer(r);
}

void beginWGPURendererFrame(WGPURenderer *r) {
  unused(r);
}

void endWGPURendererFrame(WGPURenderer *r) {
  unused(r);
}

void destroyWGPURenderer(WGPURenderer *r) {
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
