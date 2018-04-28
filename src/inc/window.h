#ifndef WINDOW_H
#define WINDOW_H
#include <GLFW/glfw3.h>
#include "gpu.h"
#include "emu.h"

#define PX_SIZE 4
#define WINDOW_HEIGHT LCD_HEIGHT*PX_SIZE
#define WINDOW_WIDTH LCD_WIDTH*PX_SIZE

// A pallette consists of four RGB colors, represented by four three-byte values, where each byte
// corresponds to the R, G, B components respectively. These four colors in turn represent hues
// on the Gameboy LCD, from light to dark.

GLFWwindow *init_window(emu *gb_emu_p);
GLFWwindow *init_tile_window(emu *gb_emu_p);

void render(emu *gb_emu_p, GLFWwindow *window);
void render_tileset(emu *gb_emu_p, GLFWwindow *window);

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

int array_to_glfw_rgb(uint16_t old_w, uint16_t old_h, uint8_t array[old_h][old_w], uint8_t resized_array[old_h][3*old_w], uint8_t pal[4][3]);

#endif /* WINDOW_H */
