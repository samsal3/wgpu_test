#include "WGPURenderer.h"
#include "Error.h"
#include "SF.h"

#include <webgpu/webgpu.h>

static void defaultInitWGPURenderer(WGPURenderer *r) {
  r->instance = NULL;
  r->adapter = NULL;
  sfInitQueue(&r->errorMessageQueue);
}

void setWGPUAdapter(WGPURequestAdapterStatus status, WGPUAdapter adapter,
                    char const *message, void *userData) {

  WGPURenderer *r = userData;

  (void)message;

  if (status == WGPURequestAdapterStatus_Success)
    r->adapter = adapter;

  r->wasAdapterRequestFinished = SF_TRUE;
}

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
    if (!r->adapter)
      goto error;
  }

  return;

error:
  destroyWGPURenderer(r);
}

void destroyWGPURenderer(WGPURenderer *r) {
  if (r->adapter) {
    wgpuAdapterRelease(r->adapter);
    r->adapter = NULL;
  }

  if (r->instance) {
    wgpuInstanceRelease(r->instance);
    r->instance = NULL;
  }
}
