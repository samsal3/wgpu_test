#include "WGPURenderer.h"
#include "Error.h"
#include "SF.h"

#include <webgpu/webgpu.h>

static void defaultInitWGPURenderer(WGPURenderer *r) {
  r->instance = NULL;
  r->adapter = NULL;
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

#ifdef __EMSCRIPTEN__
#endif

void awaitAdapterRequest(WGPURenderer *r) {
#ifdef __EMSCRIPTEN__
  while (!r->hasAdapterRequestFinished)
    emscripten_sleep(100);
#endif
}

void awaitDeviceRequest(WGPURenderer *r) {
#ifdef __EMSCRIPTEN__
  while (!r->hasDeviceRequestFinished)
    emscripten_sleep(100);
#endif
}

typedef struct DeviceInformation {
  WGPUFeatureName *features;
  U32 featureCount;
  WGPUSupportedLimits limits;
} DeviceInformation;


void createWGPURenderer(SFArena *arena, WGPURenderer *r) {
  defaultInitWGPURenderer(r);

  {
    WGPUInstanceDescriptor desc = {0};
    desc.nextInChain = NULL;

    r->instance = wgpuCreateInstance(&desc);
    if (!r->instance)
      goto error;
  }

  {
    WGPURequestAdapterOptions options = {0};
    options.nextInChain = NULL;

    wgpuInstanceRequestAdapter(r->instance, &options, setWGPUAdapter, r);
    awaitAdapterRequest(r);
    if (!r->adapter)
      goto error;
  }

  {
    WGPUDeviceDescriptor desc = {0};
    desc.nextInChain = NULL;
    desc.label = "Device1";
    desc.requiredFeatureCount = 0;
    desc.requiredLimits = NULL;
    desc.defaultQueue.nextInChain = NULL;
    desc.defaultQueue.label = "Queue1";
    desc.deviceLostCallback = NULL;

    wgpuAdapterRequestDevice(r->adapter, &desc, setWGPUDevice, r); 
    awaitDeviceRequest(r); 
    if (!r->device)
      goto error;
  }

  return;

error:
  destroyWGPURenderer(r);
}

void destroyWGPURenderer(WGPURenderer *r) {
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
