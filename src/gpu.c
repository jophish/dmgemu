#include <string.h>
#include <stdio.h>
#include "gpu.h"
#include "emu.h"
#include "error.h"

int step_gpu(emu *gb_emu_p) {
  gpu *gb_gpu_p = &(gb_emu_p->gb_gpu);
  mmu *gb_mmu_p = &(gb_emu_p->gb_mmu);
  cpu *z80_p = &(gb_emu_p->z80);

  gb_gpu_p->gpu_clock += z80_p->clk.prev_cpu_cycles;
  switch (gb_gpu_p->mode) {
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
      }
      break;
  }
  return 0;
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
      gb_gpu_p->gb_gpu_regs.reg_lcdc = val & ~MASK_STAT_WRITE;
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
      return ERR_INVALID_ADDRESS;
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

