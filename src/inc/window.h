#ifndef WINDOW_H
#define WINDOW_H
#include <GLFW/glfw3.h>
#include "gpu.h"
#include "emu.h"

#define PX_SIZE 1
#define WINDOW_HEIGHT LCD_HEIGHT*PX_SIZE
#define WINDOW_WIDTH LCD_WIDTH*PX_SIZE

GLFWwindow *init_window(void);

void render(emu *gb_emu_p, GLFWwindow *window);

#endif /* WINDOW_H */
