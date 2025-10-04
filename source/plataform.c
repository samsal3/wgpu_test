#include "plataform.h"

#include "glfw_plataform.h"

struct glfw_plataform plataform;

b32 plataform_init(i32 w, i32 h) {
  return glfw_plataform_init(w, h, &plataform);
}

void plataform_deinit(void) {
  glfw_plataform_deinit(&plataform);
}

b32 plataform_poll_events(void) {
  return glfw_plataform_poll_events(&plataform);
}
