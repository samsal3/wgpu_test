#ifndef GLFW_PLATAFORM_H
#define GLFW_PLATAFORM_H

#include <SF.h>

#include <GLFW/glfw3.h>

typedef struct GLFWPlataform {
  B32 hasGLFWBeenInitialized;
  GLFWwindow *window;
} GLFWPlataform;

B32 createGLFWPlataform(GLFWPlataform *p, I32 w, I32 h);
void destroyGLFWPlataform(GLFWPlataform *p);

B32 pollGLFWEvents(GLFWPlataform *p);

#endif
