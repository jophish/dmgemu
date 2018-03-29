#ifndef GPU_H
#define GPU_H

#include <stdint.h>

// Pixel lengths of screen and background
#define LCD_WIDTH 160
#define LCD_HEIGHT 144
#define BG_WIDTH 256
#define BG_HEIGHT 256

// Number of background blocks per section
#define BG_BLOCKS 1024
#define BG_BLOCKS_PER_ROW 32

// Masks for flag selection
#define BG_CODE_AREA_SELECT_FLAG 0x1000
#define BG_CHAR_DATA_SELECT_FLAG 0x10000

// Tile size
#define TILE_SIZE 0x10

// Pixels per tile
#define PX_PER_TILE 64
#define PX_PER_ROW 8

// 'gpu.h' and 'gpu.c' contain methods and definitions relating to the GPU and LCD/display
// features of the emulator. The framebuffer contained in the gpu struct consists of exactly
// as many pixels as there are in the GB display. Each element in the array denotes the color
// of a single pixel. A '0' corresponds to the lightest shade, and a '3' corresponds to the darkest
// shade. These mappings are fixed, regardless of whatever palette is selected internally, in order
// to decouple the internals and the rendering.

// Forward declaration of emu
typedef struct emu emu;

// Simple tile struct. 8x8 pixels.
typedef struct tile {
  uint8_t pixels[PX_PER_TILE];
} tile;

typedef struct gpu_regs {
  uint8_t reg_lcdc,
    reg_stat,
    reg_scy,
    reg_scx,
    reg_ly,
    reg_lyc,
    reg_wy,
    reg_wx,
    reg_bgp,
    reg_obp0,
    reg_obp1,
    reg_dma;
} gpu_regs;

typedef struct gpu {
  uint8_t mode;
  uint32_t gpu_clock;
  uint16_t line;
  gpu_regs gb_gpu_regs;
  uint8_t framebuffer[LCD_WIDTH*LCD_HEIGHT];
} gpu;

// Steps the GPU
int step_gpu(emu *gb_emu_p);

// Initializes the GPU with default values
void init_gpu(gpu *gb_gpu_p);

// Writes val to the GPU register at the given address. If the given address
// doesn't correspond to a valid GPU register, returns ERR_INVALID_ADDRESS.
int write_gpu_reg(gpu *gb_gpu_p, uint16_t addr, uint8_t val);

// Reads val from the GPU register at the given address. If the given address doesn't
// correspond to a valid GPU register, returns ERR_INVALID_ADDRESS.
int read_gpu_reg(gpu *gb_gpu_p, uint16_t addr);

// Updates the LCD buffer after end of VRAM read mode.
int update_buffer(emu *gb_emu_p);

// Given raw tile data, fills a tile struct with the correctly parsed values
int tile_data_to_tile(uint8_t *tile_data_p, tile *tile_p);

// Given raw tile data and a pixel number (counting from top left to bottom right),
// returns the parsed value
int tile_data_to_px(uint8_t *tile_data_p, uint8_t px_num);

// Given a tile and a palette, translates the pixels in the tile according to the palette
int translate_tile_palette(tile *tile_p, uint8_t bgp);

#endif /* GPU_H */
