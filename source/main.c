#include "plataform.h"
#include "wgpu_renderer.h"

#include <stdio.h>

#include <sf.h>
#include <webgpu/webgpu.h>

int main(void) {
   struct wgpu_renderer renderer = {0};
   struct sf_arena arena = {0};

   sf_allocate_arena(1024 * 1024, 16, &arena);
   if (!arena.data)
      goto cleanup;

   if (!plataform_init(300, 300))
      goto cleanup;

   wgpu_renderer_init(&arena, &renderer);
   if (!wgpu_renderer_validate(&renderer))
      goto cleanup;

   while (plataform_poll_events()) {
   }

cleanup:
   wgpu_renderer_deinit(&renderer);
   plataform_deinit();
   sf_free_arena(&arena);
}
