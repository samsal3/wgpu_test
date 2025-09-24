#include "GLFWPlataform.h"

B32 createGLFWPlataform(GLFWPlataform *p, I32 w, I32 h) {
  p->hasGLFWBeenInitialized = SF_FALSE;
  p->window = NULL;

  if (!glfwInit())
    goto error;

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  p->window = glfwCreateWindow(w, h, "WGPUTest", NULL, NULL);
  if (!p->window)
    goto error;


  return SF_TRUE;

error:
  destroyGLFWPlataform(p);
  return SF_FALSE;
}

void destroyGLFWPlataform(GLFWPlataform *p) {
  if (p->hasGLFWBeenInitialized)
    glfwTerminate(); // NOTE(samsal): This destroys all created windows

  p->window = NULL;
  p->hasGLFWBeenInitialized = SF_FALSE;
}

B32 pollGLFWEvents(GLFWPlataform *p) {
  glfwPollEvents();
  return !glfwWindowShouldClose(p->window);
}

