#include "window.h"
#include "string.h"
#include "stdio.h"


GLFWwindow *init_window(void) {
  glfwInit();
  GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "dmgemu", NULL, NULL);
  glfwMakeContextCurrent(window);
  return window;
}

void render(emu *gb_emu_p, GLFWwindow *window) {
  int width, height;
  gpu gb_gpu;
  if (gb_emu_p != NULL)
    gb_gpu = gb_emu_p->gb_gpu;
  else
    return;
  uint8_t *framebuf = gb_gpu.framebuffer;
  uint8_t window_RGBData[WINDOW_WIDTH*WINDOW_HEIGHT*3];
  /* for (uint16_t i = 0; i < LCD_HEIGHT; i++) { */
  /*   for (uint16_t j = 0; j < LCD_WIDTH; j++) { */
  /*     for (uint8_t k1 = 0; k1 < PX_SIZE; k1++) { */
  /* 	for (uint8_t k2 = 0; k2 < PX_SIZE; k2++) { */
  /* 	  window_RGBData[(k2*WINDOW_WIDTH*i) + (3*j*PX_SIZE + k1)] = framebuf[i*LCD_HEIGHT + j]*0x3F; */
  /* 	  window_RGBData[(k2*WINDOW_WIDTH*i) + (3*j*PX_SIZE + k1+1)] = framebuf[i*LCD_HEIGHT + j]*0x3F; */
  /* 	  window_RGBData[(k2*WINDOW_WIDTH*i) + (3*j*PX_SIZE + k1+2)] = framebuf[i*LCD_HEIGHT + j]*0x3F; */
  /* 	} */
  /*     } */
  /*   } */
  /* } */
  for (uint16_t i = 0; i < LCD_HEIGHT*LCD_WIDTH; i++) {
    window_RGBData[i*3] = framebuf[i]*0x3F;
    window_RGBData[i*3+1] = framebuf[i]*0x3F;
    window_RGBData[i*3+2] = framebuf[i]*0x3F;
  }
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);

  glClear(GL_COLOR_BUFFER_BIT);

  glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, window_RGBData);

  glfwSwapBuffers(window);

  return;
}
