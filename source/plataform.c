#include "plataform.h"

#include "glfw_plataform.h"

#include <assert.h>

struct glfw_plataform global_plataform;

plataform plataform_init(i32 w, i32 h) {
	if (glfw_plataform_init(w, h, &global_plataform))
		return (plataform)&global_plataform;
	else
		return NIL_PLATAFORM;
}

void plataform_deinit(plataform p) {
	assert(p == (intptr_t)&global_plataform);
	glfw_plataform_deinit(&global_plataform);
}

b32 plataform_poll_events(plataform p) {
	UNUSED(p);
	return glfw_plataform_poll_events(&global_plataform);
}

void plataform_window_dimensions(plataform p, i32 *w, i32 *h) {
	UNUSED(p);
	glfw_plataform_window_dimensions(&global_plataform, w, h);
}

void plataform_init_wgpu_surface(plataform p, struct wgpu_renderer *r) {
	UNUSED(p);
	assert(p == (intptr_t)&global_plataform);
	glfw_plataform_init_wgpu_surface(&global_plataform, r);
}
