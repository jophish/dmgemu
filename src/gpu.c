#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "gpu.h"
#include "emu.h"
#include "util.h"
#include "error.h"
#include "int.h"

int step_gpu(emu *gb_emu_p) {
  gpu *gb_gpu_p = &(gb_emu_p->gb_gpu);
  mmu *gb_mmu_p = &(gb_emu_p->gb_mmu);
  cpu *z80_p = &(gb_emu_p->z80);
  int return_val = 0;
  gb_gpu_p->gpu_clock += z80_p->clk.prev_cpu_cycles;
  switch (gb_gpu_p->mode) {
    // H-Blank period
    case (0) :
      if (gb_gpu_p->gpu_clock >= 204) {

	// End of H-Blank for last scanline, render screen
	if (gb_gpu_p->gb_gpu_regs.reg_ly == 143) {
	  gb_gpu_p->mode = 1;
	  // set V-Blank interrupt request
	  gb_mmu_p->hw_io_regs[REG_IF-HW_IO_REGS_START] |= INT_V_BLANK;
	  if (gb_gpu_p->gb_gpu_regs.reg_stat & 0x10)
	    gb_mmu_p->hw_io_regs[REG_IF-HW_IO_REGS_START] |= INT_LCDC;

	  // Return a 1 to indicate a screen render is required
	  if ((gb_gpu_p->gb_gpu_regs.reg_lcdc & 0b10000000) != 0)
	    return_val = 1;
	} else {
	  gb_gpu_p->mode = 2;
	  if (gb_gpu_p->gb_gpu_regs.reg_stat & 0x20)
	    gb_mmu_p->hw_io_regs[REG_IF-HW_IO_REGS_START] |= INT_LCDC;
	}

	gb_gpu_p->gpu_clock = 0;
	gb_gpu_p->gb_gpu_regs.reg_ly++;
	if ((gb_gpu_p->gb_gpu_regs.reg_ly == gb_gpu_p->gb_gpu_regs.reg_lyc) && (gb_gpu_p->gb_gpu_regs.reg_stat & 0x40))
	    gb_mmu_p->hw_io_regs[REG_IF-HW_IO_REGS_START] |= INT_LCDC;
      }
      break;
    // V-Blank period
    case (1) :
      if (gb_gpu_p->gpu_clock >= 456) {
	gb_gpu_p->gpu_clock = 0;
	gb_gpu_p->gb_gpu_regs.reg_ly++;
	if ((gb_gpu_p->gb_gpu_regs.reg_ly == gb_gpu_p->gb_gpu_regs.reg_lyc) && (gb_gpu_p->gb_gpu_regs.reg_stat & 0x40))
	    gb_mmu_p->hw_io_regs[REG_IF-HW_IO_REGS_START] |= INT_LCDC;
	if (gb_gpu_p->gb_gpu_regs.reg_ly > 153) {
	  gb_gpu_p->gb_gpu_regs.reg_ly = 0;
	  if ((gb_gpu_p->gb_gpu_regs.reg_ly == gb_gpu_p->gb_gpu_regs.reg_lyc) && (gb_gpu_p->gb_gpu_regs.reg_stat & 0x40))
	    gb_mmu_p->hw_io_regs[REG_IF-HW_IO_REGS_START] |= INT_LCDC;
	  gb_gpu_p->mode = 2;
	  if (gb_gpu_p->gb_gpu_regs.reg_stat & 0x20)
	    gb_mmu_p->hw_io_regs[REG_IF-HW_IO_REGS_START] |= INT_LCDC;
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
	if (gb_gpu_p->gb_gpu_regs.reg_stat & 0x08)
	    gb_mmu_p->hw_io_regs[REG_IF-HW_IO_REGS_START] |= INT_LCDC;
	// We should eventually write a scanline to the framebuffer here
	if ((gb_gpu_p->gb_gpu_regs.reg_lcdc & 0b10000000) != 0)
	  draw_scanline(gb_emu_p);
      }
      break;
  }
  return return_val;
}

int draw_scanline(emu *gb_emu_p) {
  gpu *gb_gpu_p = &(gb_emu_p->gb_gpu);
  uint8_t framebuf_row = gb_gpu_p->gb_gpu_regs.reg_ly;
  uint8_t x_off = gb_gpu_p->gb_gpu_regs.reg_scx;
  uint8_t y_off = gb_gpu_p->gb_gpu_regs.reg_scy;
  uint8_t px;
  uint8_t bgp_val = gb_gpu_p->gb_gpu_regs.reg_bgp;

  // Draw background first
  for (int i = 0; i < LCD_WIDTH; i++) {
    //printf("pixel no: %d\n", i);
    px = translate_palette_px(bgp_val, coord_to_bg_pixel(gb_emu_p, ((x_off + i) % BG_WIDTH), ((y_off + framebuf_row) % BG_HEIGHT)));
    gb_gpu_p->framebuffer[framebuf_row][i] = px;
    //printf("0x%02x\n", px);
  }

  // Take care of sprites
  if (gb_gpu_p->gb_gpu_regs.reg_lcdc & 0x2) {

    // Check LCDC to see if sprites are 8x8 or 8x16
    uint8_t sprite_h = gb_gpu_p->gb_gpu_regs.reg_lcdc & 0x4 ? 16 : 8;
    oam_entry sprite;

    // List of pixels in sprites that intersect the current scanline. We need this in order to
    // sort by priority in the case where we have too many sprites per row.
    oam_update_entry pixels_to_write[NUM_SPRITES*PX_PER_ROW];

    // Count for how many pixels we've written to our pixel array
    uint16_t pixel_count = 0;
    for (int i = 0; i < NUM_SPRITES; i++) {
      sprite = gb_gpu_p->sprites[i];

      // Crazy black magic. Determines whether our sprite intersects the current scanline, and which
      // pixels in the sprite intersect where on the scanline.
      uint16_t coord_x_lo = imax(8 - sprite.x_pos, 0);
      uint16_t coord_x_hi = imin(LCD_WIDTH - sprite.x_pos, 7);
      int coord_y = framebuf_row + 16 - sprite.y_pos;
      uint16_t px_lo = imax(sprite.x_pos - 8, 0);
      uint16_t px_hi = imin(LCD_WIDTH - 1, sprite.x_pos - 1);

      // No intersections with our scanline, continue
      if (!((coord_x_lo <= coord_x_hi) && (px_lo <= px_hi) && (coord_y >= 0) && (coord_y < sprite_h))) {
	continue;
      }

      // Create an update entry for each intersecting pixel. Currently this does NOT account for 8x16 pixels

      // Indices into the tile to grab the pixel. We might have X/Y flips so we have to do some computation
      uint8_t x_index, y_index;
      uint8_t palette = (sprite.palette_no) ? gb_gpu_p->gb_gpu_regs.reg_obp1 : gb_gpu_p->gb_gpu_regs.reg_obp0;
      for (int j = 0; j <= (coord_x_hi - coord_x_lo); j++) {
	oam_update_entry tmp_px = pixels_to_write[pixel_count];
	tmp_px.line_x = px_lo + j;
	//printf("line_x: %d\n", tmp_px.line_x);
	tmp_px.obj_no = i;
	tmp_px.oam_x_coord = sprite.x_pos;
	x_index = sprite.y_flip ? (PX_PER_ROW - 1 - (coord_x_lo + i)) : (coord_x_lo + j);
	y_index = sprite.x_flip ? (sprite_h - 1 - coord_y) : (coord_y);
	uint8_t sprite_px = gb_gpu_p->tileset[sprite.tile_no][y_index][x_index];
	tmp_px.px_col = translate_palette_px(palette, sprite_px);
	tmp_px.priority = sprite.priority;
	pixels_to_write[pixel_count] = tmp_px;
	pixel_count++;
      }

    }

    // Render pixels
    for (int i = 0; i < pixel_count; i++) {
      oam_update_entry tmp_entry = pixels_to_write[i];
      //printf("line_x end: %d\n", tmp_entry.line_x);
      gb_gpu_p->framebuffer[framebuf_row][tmp_entry.line_x] = tmp_entry.px_col;
    }
  }
  return 0;
}

int coord_to_bg_pixel(emu *gb_emu_p, uint16_t x, uint16_t y) {
  gpu *gb_gpu_p = &(gb_emu_p->gb_gpu);
  // First, get tile number, then get row in tile, then get pixel in row

  // This tile number is the index in the background map where we find which tile we're getting the pixel from
  // If tile data table is at 8800-97ff, interpret the index in bg_map as 2c.
  //uint16_t tile_data_st_addr;
  uint16_t tile_no = BG_BLOCKS_PER_ROW*(y/PX_PER_ROW) + (x / PX_PER_ROW);

  uint8_t lcdc_val = gb_gpu_p->gb_gpu_regs.reg_lcdc;
  uint16_t map_data_st_addr;

  // Get the correct location
  if ((lcdc_val & BG_CODE_AREA_SELECT_FLAG) == 0)
     map_data_st_addr = BG_MAP_DATA_1_START;
  else
    map_data_st_addr = BG_MAP_DATA_2_START;

  uint8_t tile_index = read_8(gb_emu_p, map_data_st_addr + tile_no);


  //printf("tile no: 0x%02x\n", tile_no);
  //printf("tile index: 0x%02x\n", tile_index);
  uint8_t row_no = y % PX_PER_ROW;
  uint8_t px_no = x % PX_PER_ROW;
  return gb_gpu_p->tileset[tile_index][row_no][px_no];
}

int translate_palette_px(uint8_t pal_val, uint8_t px) {
  return (pal_val >> px*2) & 0b11;
}

int update_tileset(emu *gb_emu_p, uint16_t addr, uint8_t val) {
  // Each row of pixels is defined by two bytes. We need to get the
  // other byte that corresponds with the one we just updated. This
  // other byte may come before or after the byte we updated

  // If the pixel we updated is on an EVEN byte address, it's the first of the pair,
  // else it's the second.
  gpu *gb_gpu_p = &(gb_emu_p->gb_gpu);
  uint16_t tile_no = (addr - CHAR_RAM_START) / TILE_SIZE;
  uint8_t row_no = (addr & 0xF) / 0x2;
  uint8_t byte1, byte2;
  ((addr & 0x1) == 0x1) ? (byte1 = read_8(gb_emu_p, addr-1), byte2 = val) : (byte1 = val, byte2 = read_8(gb_emu_p, addr-1));


  // Now that we have the tile data for the updated row, parse the data into pixels
  uint8_t new_row[PX_PER_ROW];
  for (int i = 0; i < PX_PER_ROW; i++) {
    new_row[i] = (((byte2 >> (7 - i)) & 0x1) << 1) | ((byte1 >> (7 - i)) & 0x1);
    gb_gpu_p->tileset[tile_no][row_no][i] = new_row[i];
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

int write_oam(emu *gb_emu_p, uint16_t addr, uint8_t val) {
  uint16_t buf_offset = addr - OAM_START;
  gb_emu_p->gb_mmu.oam[buf_offset] = val;
  gpu *gb_gpu_p = &(gb_emu_p->gb_gpu);
  // Each OAM entry is four bytes long. We need to determine which number entry we've written to,
  // and which byte, in order to update the gpu's sprites struct accordingly.
  uint16_t sprite_index = buf_offset / SPRITE_SIZE;
  uint8_t byte_no = buf_offset % SPRITE_SIZE;
  switch (byte_no) {
    case (0):
      gb_gpu_p->sprites[sprite_index].y_pos = val;
      break;
    case (1):
      gb_gpu_p->sprites[sprite_index].x_pos = val;
      break;
    case (2):
      gb_gpu_p->sprites[sprite_index].tile_no = val;
      break;
    case (3):
      (val & 0x10) ? (gb_gpu_p->sprites[sprite_index].palette_no = 1) : (gb_gpu_p->sprites[sprite_index].palette_no = 0);
      (val & 0x20) ? (gb_gpu_p->sprites[sprite_index].x_flip = 1) : (gb_gpu_p->sprites[sprite_index].x_flip = 0);
      (val & 0x40) ? (gb_gpu_p->sprites[sprite_index].y_flip = 1) : (gb_gpu_p->sprites[sprite_index].y_flip = 0);
      (val & 0x80) ? (gb_gpu_p->sprites[sprite_index].priority = 1) : (gb_gpu_p->sprites[sprite_index].priority = 0);
      break;
  }
  return 0;
}

void init_gpu(gpu *gb_gpu_p) {
  gb_gpu_p->mode = 2;
  gb_gpu_p->gpu_clock = 0;
  gb_gpu_p->line = 0;
  memset(&(gb_gpu_p->gb_gpu_regs), 0, sizeof(gpu_regs));
  memset(&(gb_gpu_p->framebuffer), 0, sizeof(gb_gpu_p->framebuffer));
  memset(&(gb_gpu_p->tileset), 0, sizeof(gb_gpu_p->tileset));
  memset(&(gb_gpu_p->sprites), 0, sizeof(gb_gpu_p->sprites));
}

int write_gpu_reg(emu *gb_emu_p, uint16_t addr, uint8_t val) {
  gpu *gb_gpu_p = &(gb_emu_p->gb_gpu);
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
      gb_gpu_p->gb_gpu_regs.reg_dma = val;
      uint16_t dma_src = val * 0x100;
      for (int i = 0; i < SZ_OAM; i++)
	write_oam(gb_emu_p, OAM_START + i, read_8(gb_emu_p, dma_src + i));
      break;

      break;
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
      gb_gpu_p->gb_gpu_regs.reg_wy = val;
      break;
    case (REG_WX) :
      gb_gpu_p->gb_gpu_regs.reg_wx = val;
      break;
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
      return gb_gpu_p->gb_gpu_regs.reg_lyc;
    case (REG_DMA) :
      return gb_gpu_p->gb_gpu_regs.reg_dma;
    case (REG_BGP) :
      return gb_gpu_p->gb_gpu_regs.reg_bgp;
    case (REG_OBP0) :
      return gb_gpu_p->gb_gpu_regs.reg_obp0;
    case (REG_OBP1) :
      return gb_gpu_p->gb_gpu_regs.reg_obp1;
    case (REG_WY) :
      return gb_gpu_p->gb_gpu_regs.reg_wy;
    case (REG_WX) :
      return gb_gpu_p->gb_gpu_regs.reg_wx;
    default :
      return ERR_INVALID_ADDRESS;
  }
  return 0;
}

