#include "window.h"
#include "string.h"
#include "stdio.h"


GLFWwindow *init_window(emu *gb_emu_p) {
  glfwInit();
  GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "dmgemu", NULL, NULL);
  glfwMakeContextCurrent(window);
  glfwSetWindowUserPointer(window, gb_emu_p);
  glfwSetKeyCallback(window, key_callback);
  return window;
}

GLFWwindow *init_tile_window(emu *gb_emu_p) {
  glfwInit();
  GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "tileset", NULL, NULL);
  glfwMakeContextCurrent(window);
  glfwSetWindowUserPointer(window, gb_emu_p);
  return window;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  emu *gb_emu_p = glfwGetWindowUserPointer(window);
  mmu *mmu_p = &(gb_emu_p->gb_mmu);

  if (action == GLFW_RELEASE) {
    switch (key) {
      case (73): mmu_p->reg_joyp |= 0b1000000; break; // up
      case (74): mmu_p->reg_joyp |= 0b100000; break;  // left
      case (75): mmu_p->reg_joyp |= 0b10000000; break; // down
      case (76): mmu_p->reg_joyp |= 0b10000; break; // right
      case (65): mmu_p->reg_joyp |= 0b10; break;  // b
      case (83): mmu_p->reg_joyp |= 0b1; break;  // a
      case (46): mmu_p->reg_joyp |= 0b100; break; // select
      case (47): mmu_p->reg_joyp |= 0b1000; break; // start
    }
  } else {
    switch (key) {
      case (73): mmu_p->reg_joyp &= ~0b1000000; break;
      case (74): mmu_p->reg_joyp &= ~0b100000; break;
      case (75): mmu_p->reg_joyp &= ~0b10000000; break;
      case (76): mmu_p->reg_joyp &= ~0b10000; break;
      case (65): mmu_p->reg_joyp &= ~0b10; break;
      case (83): mmu_p->reg_joyp &= ~0b1; break;
      case (46): mmu_p->reg_joyp &= ~0b100; break;
      case (47): mmu_p->reg_joyp &= ~0b1000; break;
    }
  }
  return;
}

void render(emu *gb_emu_p, GLFWwindow *window) {
  glfwMakeContextCurrent(window);
  int width, height;
  gpu gb_gpu;
  if (gb_emu_p != NULL)
    gb_gpu = gb_emu_p->gb_gpu;
  else
    return;
  uint8_t window_RGBData[WINDOW_HEIGHT][WINDOW_WIDTH*3];

  uint8_t palette[4] = {0xFF, 0xAA, 0x55, 0x00};
  for (int i = LCD_HEIGHT-1; i >= 0 ; i--) {
    for (int j = 0; j < LCD_WIDTH; j++) {
      for (int k1 = 0; k1 < PX_SIZE; k1++) {
	for (int k2 = 0; k2 < PX_SIZE; k2++) {
	  window_RGBData[(LCD_HEIGHT - 1 -i)*PX_SIZE + k1][3*j*PX_SIZE + k2*PX_SIZE] = palette[gb_gpu.framebuffer[i][j]];
	  window_RGBData[(LCD_HEIGHT -1 -i)*PX_SIZE + k1][j*3*PX_SIZE + k2*PX_SIZE+1] = palette[gb_gpu.framebuffer[i][j]];
	  window_RGBData[(LCD_HEIGHT -1 -i)*PX_SIZE + k1][j*3*PX_SIZE + k2*PX_SIZE +2] = palette[gb_gpu.framebuffer[i][j]];
	}
      }
    }
  }
  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
  glfwGetFramebufferSize(window, &width, &height);
  glClear(GL_COLOR_BUFFER_BIT);

  glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, window_RGBData);

  glfwSwapBuffers(window);

  return;
}

void render_tileset(emu *gb_emu_p, GLFWwindow *window) {
  glfwMakeContextCurrent(window);
  int width, height;
  gpu gb_gpu;
  if (gb_emu_p != NULL)
    gb_gpu = gb_emu_p->gb_gpu;
  else
    return;
  uint8_t window_RGBData[8][8];
    for (int i = 0; i < PX_PER_ROW; i++) {
      for (int j = 0; j < PX_PER_ROW; j++) {
	window_RGBData[i][j] = gb_gpu.tileset[0][i][j]*0x3F;
      }
    }

  
  glViewport(0, 0, 16*PX_PER_ROW, 16*PX_PER_ROW);
  glfwGetFramebufferSize(window, &width, &height);
  glClear(GL_COLOR_BUFFER_BIT);

  glDrawPixels(8, 8, GL_RED, GL_UNSIGNED_BYTE, window_RGBData);

  glfwSwapBuffers(window);

  return;
}
