#include "window.h"
#include "string.h"
#include "debug.h"
#include "stdio.h"
#include "util.h"


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
  debugger *dbg_p = &(gb_emu_p->gb_debugger);

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
    case (81): dbg_p->run = false; // break out of run mode
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

  uint8_t pal[4][3]= {{0xFF, 0xE0, 0xC0},
		      {0xAA, 0xAA, 0xAA},
		      {0x55, 0x55, 0x55},
		      {0x00, 0x00, 0x00}};

  uint8_t window_RGBData[WINDOW_HEIGHT][WINDOW_WIDTH];
  resize_array(LCD_WIDTH, LCD_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT, gb_gpu.framebuffer, window_RGBData, true);
  uint8_t glfw_array[WINDOW_HEIGHT][3*WINDOW_WIDTH];
  array_to_glfw_rgb(WINDOW_WIDTH, WINDOW_HEIGHT, window_RGBData, glfw_array, pal);

  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
  glfwGetFramebufferSize(window, &width, &height);
  glClear(GL_COLOR_BUFFER_BIT);

  glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, glfw_array);

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

  uint8_t pal[4][3]= {{0xFF, 0xFF, 0xFF},
		      {0xAA, 0xAA, 0xAA},
		      {0x55, 0x55, 0x55},
		      {0x00, 0x00, 0x00}};

  uint8_t window_RGBData[WINDOW_HEIGHT][WINDOW_WIDTH];
  resize_array(LCD_WIDTH, LCD_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT, gb_gpu.framebuffer, window_RGBData, true);
  uint8_t glfw_array[WINDOW_HEIGHT][3*WINDOW_WIDTH];
  array_to_glfw_rgb(WINDOW_WIDTH, WINDOW_HEIGHT, window_RGBData, glfw_array, pal);

  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
  glfwGetFramebufferSize(window, &width, &height);
  glClear(GL_COLOR_BUFFER_BIT);

  glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, glfw_array);

  else
    return;
  uint8_t TILES_PER_ROW = 20;
  uint8_t window_RGBData[8*TILES_PER_ROW][8*TILES_PER_ROW];
  for (int k = 0; k < 192; k++) {
    for (int i = 0; i < PX_PER_ROW; i++) {
      for (int j = 0; j < PX_PER_ROW; j++) {
	window_RGBData[PX_PER_ROW*(k / TILES_PER_ROW) + i][PX_PER_ROW*(k % TILES_PER_ROW) + j] = gb_gpu.tileset[k][i][j]*0x3F;
      }
    }
  }

  glViewport(0, 0, 8*TILES_PER_ROW, 8*TILES_PER_ROW);
  glfwGetFramebufferSize(window, &width, &height);
  glClear(GL_COLOR_BUFFER_BIT);

  glDrawPixels(8*TILES_PER_ROW, 8*TILES_PER_ROW, GL_RED, GL_UNSIGNED_BYTE, window_RGBData);

  glfwSwapBuffers(window);

  return;
}


int array_to_glfw_rgb(uint16_t old_w, uint16_t old_h, uint8_t array[old_h][old_w], uint8_t resized_array[old_h][3*old_w], uint8_t pal[4][3]) {
  for (int i = 0; i < old_h; i++) {
    for (int j = 0;j < old_w; j++) {
      uint8_t index = array[i][j];
      resized_array[i][3*j + 0] = pal[index][0];
      resized_array[i][3*j + 1] = pal[index][1];
      resized_array[i][3*j + 2] = pal[index][2];
    }
  }
  return 0;
}
