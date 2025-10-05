#ifndef PLATAFORM_H
#define PLATAFORM_H

#include <sf.h>

struct wgpu_renderer;

typedef intptr_t plataform;

plataform plataform_init(i32 w, i32 h);
void plataform_deinit(plataform p);
b32 plataform_poll_events(plataform p);

void plataform_init_wgpu_surface(plataform p, struct wgpu_renderer *r);

#endif
