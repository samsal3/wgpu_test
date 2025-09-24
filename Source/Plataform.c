#include "Plataform.h"

#include "GLFWPlataform.h"

GLFWPlataform plataform;

B32 initPlataform(I32 w, I32 h) {
  return createGLFWPlataform(&plataform, w, h);
}

void deinitPlataform(void) { destroyGLFWPlataform(&plataform); }

B32 pollPlataformEvents(void) { return pollGLFWEvents(&plataform); }
