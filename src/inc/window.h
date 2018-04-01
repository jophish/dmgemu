#ifndef WINDOW_H
#define WINDOW_H
#include <GLFW/glfw3.h>
#include "gpu.h"
#include "emu.h"

#define PX_SIZE 3
#define WINDOW_HEIGHT LCD_HEIGHT*PX_SIZE
#define WINDOW_WIDTH LCD_WIDTH*PX_SIZE

GLFWwindow *init_window(emu *gb_emu_p);

void render(emu *gb_emu_p, GLFWwindow *window);

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

#endif /* WINDOW_H */
