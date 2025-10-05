#ifndef GLFW_PLATAFORM_H
#define GLFW_PLATAFORM_H

#include <sf.h>

#include <GLFW/glfw3.h>

struct wgpu_renderer;

struct glfw_plataform {
	b32 has_glfw_been_initialized;
	GLFWwindow *window;
};

b32 glfw_plataform_init(i32 w, i32 h, struct glfw_plataform *p);
void glfw_plataform_deinit(struct glfw_plataform *p);

b32 glfw_plataform_poll_events(struct glfw_plataform *p);

void glfw_plataform_init_wgpu_surface(struct glfw_plataform *p, struct wgpu_renderer *r);

#endif
