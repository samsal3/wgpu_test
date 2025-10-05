#include "plataform.h"
#include "wgpu_renderer.h"

#include <stdio.h>

#include <sf.h>
#include <webgpu/webgpu.h>

int main(void) {
	plataform plataform = NIL_PLATAFORM;
	struct wgpu_renderer renderer = {0};
	struct sf_arena arena = {0};

	sf_arena_init(1024 * 1024, 16, &arena);
	if (!arena.data)
		goto cleanup;

	plataform = plataform_init(300, 300);
	if (!plataform)
		goto cleanup;

	wgpu_renderer_init(plataform, &arena, &renderer);
	if (!wgpu_renderer_validate(&renderer))
		goto cleanup;

	while (plataform_poll_events(plataform)) {
	}

cleanup:
	wgpu_renderer_deinit(&renderer);
	plataform_deinit(plataform);
	sf_arena_deinit(&arena);
}
