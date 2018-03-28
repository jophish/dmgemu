#include <GLFW/glfw3.h>
#include "test.h"
#include "string.h"

void testGLFW(void) {
  int num = 3*300*300;

  int width, height;
  uint8_t window_RGBData[num];
  for (int i = 0; i < num; i += 3) {
    window_RGBData[i] = i & 0xff;
    window_RGBData[i+1] = (i*i*i*i + 30)  & 0xff;
    window_RGBData[i+2] = (i + 60*i)  & 0xff;
  }

  glfwInit();
  GLFWwindow* window = glfwCreateWindow(300,300, "My Title", NULL, NULL);
  glfwMakeContextCurrent(window);

  glfwGetFramebufferSize(window, &width, &height);
  //ratio = width / (float) height;
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT);

  glDrawPixels(300,300, GL_RGB, GL_UNSIGNED_BYTE, window_RGBData);
  glfwSwapBuffers(window);
  return;
}
