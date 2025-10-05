#include "glfw_plataform.h"

#include "wgpu_renderer.h"

#include <glfw3webgpu.h>

b32 glfw_plataform_init(i32 w, i32 h, struct glfw_plataform *p) {
	p->has_glfw_been_initialized = SF_FALSE;
	p->window = NULL;

	if (!glfwInit())
		goto error;

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	
	printf("asdasdsad\n");
	p->window = glfwCreateWindow(w, h, "WGPUTest", NULL, NULL);
	if (!p->window)
		goto error;

	return SF_TRUE;

error:
	glfw_plataform_deinit(p);
	return SF_FALSE;
}

void glfw_plataform_deinit(struct glfw_plataform *p) {
	if (p->has_glfw_been_initialized)
		glfwTerminate(); // NOTE(samsal): This destroys all created windows

	p->window = NULL;
	p->has_glfw_been_initialized = SF_FALSE;
}

b32 glfw_plataform_poll_events(struct glfw_plataform *p) {
	assert(p->window);
	glfwPollEvents();
	return !glfwWindowShouldClose(p->window);
}

void glfw_plataform_window_dimensions(struct glfw_plataform *p, i32 *w, i32 *h) {
	assert(p->window);
	glfwGetFramebufferSize(p->window, w, h);
}

void glfw_plataform_init_wgpu_surface(struct glfw_plataform *p, struct wgpu_renderer *r) {
	r->surface = glfwGetWGPUSurface(r->instance, p->window);
}
