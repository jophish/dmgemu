#include <string.h>
#include <stdio.h>
#include "gpu.h"
#include "emu.h"
#include "util.h"
#include "error.h"

int step_gpu(emu *gb_emu_p) {
  gpu *gb_gpu_p = &(gb_emu_p->gb_gpu);
  mmu *gb_mmu_p = &(gb_emu_p->gb_mmu);
  cpu *z80_p = &(gb_emu_p->z80);

  gb_gpu_p->gpu_clock += z80_p->clk.prev_cpu_cycles;
  switch (gb_gpu_p->mode
) {
    // H-Blank period
    case (0) :
      if (gb_gpu_p->gpu_clock >= 204) {

	// End of H-Blank for last scanline, render scree
	if (gb_gpu_p->gb_gpu_regs.reg_ly == 143) {
	  gb_gpu_p->mode = 1;
	  // set V-Blank interrupt request

	  gb_mmu_p->hw_io_regs[REG_IF-HW_IO_REGS_START] |= 0x2;
	  // Render screen
	} else {
	  gb_gpu_p->mode = 2;
	}
	gb_gpu_p->gpu_clock = 0;
	gb_gpu_p->gb_gpu_regs.reg_ly++;
      }
      break;
    // V-Blank period
    case (1) :
      if (gb_gpu_p->gpu_clock >= 456) {
	gb_gpu_p->gpu_clock = 0;
	gb_gpu_p->gb_gpu_regs.reg_ly++;
	if (gb_gpu_p->gb_gpu_regs.reg_ly > 153) {
	  gb_gpu_p->gb_gpu_regs.reg_ly = 0;
	  gb_gpu_p->mode = 2;
	}
      }
      break;
    // OAM read mode
    case (2) :
      if (gb_gpu_p->gpu_clock >= 80) {
	gb_gpu_p->gpu_clock = 0;
	gb_gpu_p->mode = 3;
      }
      break;
    // VRAM read mode
    case (3) :
      if (gb_gpu_p->gpu_clock >= 172) {
	gb_gpu_p->gpu_clock = 0;
	gb_gpu_p->mode = 0;
	// We should eventually write a scanline to the framebuffer here
	update_buffer(gb_emu_p);
      }
      break;
  }
  return 0;
}

int update_buffer(emu *gb_emu_p) {

  // First, write background to buffer
  //    - check bit 3 of LCDC to determine where BG map data is stored
  //    - check bit 4 of LCDC to determine where tile data is stored
  //    - check SCY and SCX to know where (0,0) is
  //    - for all BG tiles:
  //       - get the corresponding tile from the right location (use b4 of LCDC above)
  //       - translate tile pixels using palette translation
  //       - update the 8x8 pixel region in a total background buffer
  //    - using SCY and SCX, fill framebuffer by grabbing from total BG buffer

  gpu *gb_gpu_p = &(gb_emu_p->gb_gpu);
  uint16_t map_data_st_addr, char_data_st_addr;
  uint8_t x_off, y_off;
  uint8_t lcdc_val = gb_gpu_p->gb_gpu_regs.reg_lcdc;
  uint8_t bg_buffer[BG_WIDTH*BG_HEIGHT];

  if ((lcdc_val & BG_CODE_AREA_SELECT_FLAG) == 0)
    map_data_st_addr = BG_MAP_DATA_1_START;
  else
    map_data_st_addr = BG_MAP_DATA_2_START;

  if ((lcdc_val & BG_CHAR_DATA_SELECT_FLAG) == 0)
    char_data_st_addr = CHAR_RAM_SEC_1_START;
  else
    char_data_st_addr = CHAR_RAM_SEC_0_START;

  x_off = gb_gpu_p->gb_gpu_regs.reg_scx;
  y_off = gb_gpu_p->gb_gpu_regs.reg_scy;

  uint8_t char_index;
  int tile_offset;
  uint8_t bgp_val = gb_gpu_p->gb_gpu_regs.reg_bgp;

  for (int i = 0; i < BG_BLOCKS; i++) {
    char_index = read_8(gb_emu_p, map_data_st_addr + i);

    // If we're using the second char ram section, tiles are
    // determined by 2c offset from 0x9000
    if (char_data_st_addr == CHAR_RAM_SEC_1_START)
      tile_offset = byte_to_2c(char_index);
    else
      tile_offset = char_index;

    // Load tile into tile_data
    uint8_t tile_data[TILE_SIZE];
    for (int j = 0; j < TILE_SIZE; j++) {
    tile_data[j] = read_8(gb_emu_p, char_data_st_addr + tile_offset*TILE_SIZE + j);
    }

    // Parse the esoteric format of the tile and make a tile struct
    tile cur_tile;
    tile_data_to_tile(tile_data, &cur_tile);

    // Now, convert the tile to the right palette
    translate_tile_palette(&cur_tile, bgp_val);

    // Use this tile struct to fill in bg_buffer
    uint8_t x_coord = i % BG_BLOCKS_PER_ROW;
    uint8_t y_coord = i / BG_BLOCKS_PER_ROW;
    for (int j = 0; j < PX_PER_ROW; j++) {
      for (int k = 0; k < PX_PER_ROW; k++) {
	bg_buffer[x_coord*PX_PER_ROW + (y_coord+j)*PX_PER_ROW + k] = cur_tile.pixels[PX_PER_ROW*j + k];
      }
    }
  }

  // Now, bg_buffer is full. Just get the right chunk and update the framebuffer.
  for (int j = 0; j < LCD_HEIGHT; j++) {
    for (int k = 0; k < LCD_WIDTH; k++) {
      gb_gpu_p->framebuffer[j*LCD_WIDTH + k] = bg_buffer[((y_off + j) % LCD_HEIGHT)*LCD_WIDTH + ((x_off + k) % LCD_WIDTH)];
    }
  }
  return 0;
}

int translate_tile_palette(tile *tile_p, uint8_t bgp) {
  for (int i = 0; i < PX_PER_TILE; i++) {
    tile_p->pixels[i] = (bgp >> tile_p->pixels[i]*2) & 0b11;
  }
  return 0;
}
int tile_data_to_tile(uint8_t *tile_data, tile *tile_p) {
  int px;
  for (int i = 0; i < PX_PER_TILE; i++) {
    if ((px = tile_data_to_px(tile_data, i)) < 0)
      return px;
    tile_p->pixels[i] = px;
  }
  return 0;
}

int tile_data_to_px(uint8_t *tile_data, uint8_t px_num) {
  if (px_num >= PX_PER_TILE)
    return ERR_INVALID_PX;
  uint8_t byte_offset = px_num / 0x08;
  uint8_t bit_offset =  7 - (px_num % 0x08);
  return (((tile_data[byte_offset*2 + 1] >> bit_offset) & 0x1) << 1) |
    (((tile_data[byte_offset*2] >> bit_offset) & 0x1));
}

void init_gpu(gpu *gb_gpu_p) {
  gb_gpu_p->mode = 2;
  gb_gpu_p->gpu_clock = 0;
  gb_gpu_p->line = 0;
  memset(&(gb_gpu_p->gb_gpu_regs), 0, sizeof(gpu_regs));
  
}

int write_gpu_reg(gpu *gb_gpu_p, uint16_t addr, uint8_t val) {
  switch (addr) {
    case (REG_LCDC) :
      // Bits 0-2 are read only
      gb_gpu_p->gb_gpu_regs.reg_lcdc = val;
      break;
    case (REG_STAT) :
      // Bit 7 is always 0. Bits 6-2 are R/W, and bits 1-0 are read only
      gb_gpu_p->gb_gpu_regs.reg_stat = val & ~MASK_STAT_WRITE;
      break;
    case (REG_SCY) :
      gb_gpu_p->gb_gpu_regs.reg_scy = val;
      break;
    case (REG_SCX) :
      gb_gpu_p->gb_gpu_regs.reg_scx = val;
      break;
    case (REG_LY) :
      // Register not writeable
      break;
    case (REG_LYC) :
      gb_gpu_p->gb_gpu_regs.reg_lyc = val;
      break;
    case (REG_DMA) :
      return ERR_INVALID_ADDRESS;
    case (REG_BGP) :
      gb_gpu_p->gb_gpu_regs.reg_bgp = val;
      break;
    case (REG_OBP0) :
      gb_gpu_p->gb_gpu_regs.reg_obp0 = val;
      break;
    case (REG_OBP1) :
      gb_gpu_p->gb_gpu_regs.reg_obp1 = val;
      break;
    case (REG_WY) :
      return ERR_INVALID_ADDRESS;
    case (REG_WX) :
      return ERR_INVALID_ADDRESS;
    default :
      return ERR_INVALID_ADDRESS;
  }
  return 0;
}

int read_gpu_reg(gpu *gb_gpu_p, uint16_t addr) {
  switch (addr) {
    case (REG_LCDC) :
      return gb_gpu_p->gb_gpu_regs.reg_lcdc;
    case (REG_STAT) :
      ;
      // Bit 2 is set if LY==LYC
      bool eq = gb_gpu_p->gb_gpu_regs.reg_ly == gb_gpu_p->gb_gpu_regs.reg_lyc;
      uint8_t co_flag = eq ? 1 : 0;
      return gb_gpu_p->gb_gpu_regs.reg_stat | (co_flag << 2) | (gb_gpu_p->mode & 0x3);
    case (REG_SCY) :
      return gb_gpu_p->gb_gpu_regs.reg_scy;
    case (REG_SCX) :
      return gb_gpu_p->gb_gpu_regs.reg_scx;
    case (REG_LY) :
      return gb_gpu_p->gb_gpu_regs.reg_ly;
    case (REG_LYC) :
      return ERR_INVALID_ADDRESS;
    case (REG_DMA) :
      return ERR_INVALID_ADDRESS;
    case (REG_BGP) :
      return gb_gpu_p->gb_gpu_regs.reg_bgp;
    case (REG_OBP0) :
      return gb_gpu_p->gb_gpu_regs.reg_obp0;
    case (REG_OBP1) :
      return gb_gpu_p->gb_gpu_regs.reg_obp1;
    case (REG_WY) :
      return ERR_INVALID_ADDRESS;
    case (REG_WX) :
      return ERR_INVALID_ADDRESS;
    default :
      return ERR_INVALID_ADDRESS;
  }
  return 0;
}

