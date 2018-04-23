#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "mmu.h"
#include "emu.h"
#include "gpu.h"

int get_mem_region(mem_addr addr) {
  if (addr < RES_INT_VEC_END) {
    return REGION_RES_INT_VEC;
  } else if (addr < CART_HEADER_END) {
    return REGION_CART_HEADER;
  } else if (addr < CART_ROM_BANK_0_END) {
    return REGION_CART_ROM_BANK_0;
  } else if (addr < CART_ROM_BANK_SWITCH_END) {
    return REGION_CART_ROM_BANK_SWITCH;
  } else if (addr < CHAR_RAM_END) {
    return REGION_CHAR_RAM;
  } else if (addr < BG_MAP_DATA_1_END) {
    return REGION_BG_MAP_DATA_1;
  } else if (addr < BG_MAP_DATA_2_END) {
    return REGION_BG_MAP_DATA_2;
  } else if (addr < CART_RAM_END) {
    return REGION_CART_RAM;
  } else if (addr < INTERNAL_RAM_0_END) {
    return REGION_INTERNAL_RAM_0;
  } else if (addr < INTERNAL_RAM_SWITCH_END) {
    return REGION_INTERNAL_RAM_SWITCH;
  } else if (addr < ECHO_RAM_END) {
    return REGION_ECHO_RAM;
  } else if (addr < OAM_END) {
    return REGION_OAM;
  } else if (addr < RESERVED_END) {
    return REGION_RESERVED;
  } else if (addr < HW_IO_REGS_END) {
    return REGION_HW_IO_REGS;
  } else if (addr < ZERO_PAGE_END) {
    return REGION_ZERO_PAGE;
  } else if (addr == INT_ENABLE_FLAG) {
    return REGION_INT_ENABLE_FLAG;
  } else {
    return ERR_INVALID_ADDRESS;
  }
}

int get_hw_io_region(mem_addr addr) {
  if (addr >= REG_LCDC && addr <= REG_WX) {
    return REGION_IO_GPU;
  } else if (addr == REG_JOYP) {
    return REGION_JOYP;
  } else {
    return ERR_INVALID_ADDRESS;
  }
}

int read_8(emu *gb_emu_p, mem_addr addr) {
  uint16_t buf_offset;
  switch (get_mem_region(addr)) {
    // These cases can safely fall through
    case (REGION_RES_INT_VEC) :
    case (REGION_CART_HEADER) :
    case (REGION_CART_ROM_BANK_0) :
    case (REGION_CART_ROM_BANK_SWITCH) :
      return gb_emu_p->gb_rom.data[addr];
      break;
    case (REGION_CHAR_RAM) :
      buf_offset = addr - CHAR_RAM_START;
      return gb_emu_p->gb_mmu.char_ram[buf_offset];
    case (REGION_BG_MAP_DATA_1) :
      buf_offset = addr - BG_MAP_DATA_1_START;
      return gb_emu_p->gb_mmu.bg_map_data_1[buf_offset];
    case (REGION_BG_MAP_DATA_2) :
      buf_offset = addr - BG_MAP_DATA_2_START;
      return gb_emu_p->gb_mmu.bg_map_data_2[buf_offset];
    case (REGION_CART_RAM) :
      break;
    // On the DMG, the internal RAM doesn't have a switchable bank
    case (REGION_INTERNAL_RAM_0):
    case (REGION_INTERNAL_RAM_SWITCH) :
      buf_offset = addr - INTERNAL_RAM_0_START;
      return gb_emu_p->gb_mmu.ram[buf_offset];
    case (REGION_ECHO_RAM) :
      buf_offset = addr - ECHO_RAM_START;
      return gb_emu_p->gb_mmu.ram[buf_offset];
    case (REGION_OAM) :
      buf_offset = addr - OAM_START;
      return gb_emu_p->gb_mmu.ram[buf_offset];
    case (REGION_RESERVED) :
      break;
    case (REGION_HW_IO_REGS) :
      // We have seperate handlers for the IO regs, since they belong
      // to various different subsystems
      switch (get_hw_io_region(addr)) {
	case (REGION_IO_GPU) :
	  ;
	  gpu *gb_gpu_p = &(gb_emu_p->gb_gpu);
	  return read_gpu_reg(gb_gpu_p, addr);
	case (REGION_JOYP) :
	  buf_offset = addr - HW_IO_REGS_START;
	  uint8_t joyp_select = gb_emu_p->gb_mmu.hw_io_regs[buf_offset];
	  if ((joyp_select & 0b100000) == 0) {
	    // select appropriate inputs based off of select bits
	    return (joyp_select & 0xf0) | (gb_emu_p->gb_mmu.reg_joyp & 0xf);
	  } else {
	    return (joyp_select & 0xf0) | ((gb_emu_p->gb_mmu.reg_joyp >> 4) & 0xf);
	  }
	default :
	  buf_offset = addr - HW_IO_REGS_START;
	  return gb_emu_p->gb_mmu.hw_io_regs[buf_offset];
      }
      break;
    case (REGION_ZERO_PAGE) :
      buf_offset = addr - ZERO_PAGE_START;
      return gb_emu_p->gb_mmu.zero_page[buf_offset];
    case (REGION_INT_ENABLE_FLAG) :
      return gb_emu_p->gb_mmu.ie_flag;
    case (ERR_INVALID_ADDRESS) :
      break;
  }
  return ERR_INVALID_ADDRESS;
}


uint16_t read_16(emu *gb_emu_p, mem_addr addr) {
  int high_byte = read_8(gb_emu_p, addr+1);
  int low_byte = read_8(gb_emu_p, addr);
  return (((uint16_t)(high_byte)) << 8) | low_byte;
}


int write_8(emu *gb_emu_p, mem_addr addr, uint8_t val) {
  uint16_t buf_offset;
  int err;
  //gpu *gb_gpu_p = &(gb_emu_p->gb_gpu);
  switch (get_mem_region(addr)) {
    // These cases can safely fall through
    case (REGION_RES_INT_VEC) :
    case (REGION_CART_HEADER) :
    case (REGION_CART_ROM_BANK_0) :
    case (REGION_CART_ROM_BANK_SWITCH) :
      // Illegal write here  Can't write to ROM
      break;
    case (REGION_CHAR_RAM) :
      //printf("writing to char ram at: 0x%04x with val: 0x%02x\n", addr, val);
      buf_offset = addr - CHAR_RAM_START;
      gb_emu_p->gb_mmu.char_ram[buf_offset] = val;
      update_tileset(gb_emu_p, addr, val);
      break;
    case (REGION_BG_MAP_DATA_1) :
      ;
      buf_offset = addr - BG_MAP_DATA_1_START;
      gb_emu_p->gb_mmu.bg_map_data_1[buf_offset] = val;
      break;
    case (REGION_BG_MAP_DATA_2) :
      buf_offset = addr - BG_MAP_DATA_2_START;
      gb_emu_p->gb_mmu.bg_map_data_2[buf_offset] = val;
      break;
    case (REGION_CART_RAM) :
      break;
    // On the DMG, the internal RAM doesn't have a switchable bank
    case (REGION_INTERNAL_RAM_0):
    case (REGION_INTERNAL_RAM_SWITCH) :
      // This semicolon actually needs to be here because C is insane
      ;
      buf_offset = addr - INTERNAL_RAM_0_START;
      gb_emu_p->gb_mmu.ram[buf_offset] = val;
      break;
    case (REGION_ECHO_RAM) :
      ;
      buf_offset = addr - ECHO_RAM_START;
      gb_emu_p->gb_mmu.ram[buf_offset] = val;
      break;
    case (REGION_OAM) :
      // write to actual OAM buf, but also update sprite list in GPU. write_oam accomplishes both
      write_oam(gb_emu_p, addr, val);
      break;
    case (REGION_RESERVED) :
      break;
    case (REGION_HW_IO_REGS) :
      // We have seperate handlers for the IO regs, since they belong
      // to various different subsystems
      switch (get_hw_io_region(addr)) {
	case (REGION_IO_GPU) :
	  if ((err = write_gpu_reg(gb_emu_p, addr, val)) == ERR_INVALID_ADDRESS) {
	    return err;
	  }
	  break;
	case (REGION_JOYP):
	  ;
	  buf_offset = addr - HW_IO_REGS_START;
	  // We use the actual index in the io register buffer to hold the joypad select data,
	  // but the actual button data is held in reg_joyp
	  gb_emu_p->gb_mmu.hw_io_regs[buf_offset] = (gb_emu_p->gb_mmu.hw_io_regs[buf_offset]  & ~MASK_JOYP_WRITE) | (val & MASK_JOYP_WRITE);
	  break;
	default :
	  buf_offset = addr - HW_IO_REGS_START;
	  gb_emu_p->gb_mmu.hw_io_regs[buf_offset] = val;
	  }
      break;
    case (REGION_ZERO_PAGE) :
      buf_offset = addr - ZERO_PAGE_START;
      gb_emu_p->gb_mmu.zero_page[buf_offset] = val;
      break;
    case (REGION_INT_ENABLE_FLAG) :
      gb_emu_p->gb_mmu.ie_flag = MASK_IE_FLAG & val;
      break;
    case (ERR_INVALID_ADDRESS) :
      printf("Error when trying to write to address.\n");
  }
  return get_mem_region(addr);
}

void init_mmu(mmu *mmu_p) {
  uint8_t *ram_buf = malloc(SZ_INTERNAL_RAM);
  mmu_p->ram = ram_buf;
  memset(ram_buf, 0, SZ_INTERNAL_RAM);
  uint8_t *io_buf = malloc(SZ_HW_IO_REGS);
  memset(io_buf, 0, SZ_HW_IO_REGS);
  mmu_p->hw_io_regs = io_buf;
  mmu_p->reg_joyp = 0xff;
  uint8_t *zero_page_buf = malloc(SZ_ZERO_PAGE);
  mmu_p->zero_page = zero_page_buf;
  memset(zero_page_buf, 0, SZ_ZERO_PAGE);
  uint8_t *bg_data_1_buf = malloc(SZ_BG_MAP_DATA_1);
  mmu_p->bg_map_data_1 = bg_data_1_buf;
  memset(bg_data_1_buf, 0, SZ_BG_MAP_DATA_1);
  uint8_t *bg_data_2_buf = malloc(SZ_BG_MAP_DATA_2);
  mmu_p->bg_map_data_2 = bg_data_2_buf;
  memset(bg_data_2_buf, 0, SZ_BG_MAP_DATA_2);
  uint8_t *char_ram_buf = malloc(SZ_CHAR_RAM);
  mmu_p->char_ram = char_ram_buf;
  memset(char_ram_buf, 0, SZ_CHAR_RAM);
  uint8_t *oam_buf = malloc(SZ_OAM);
  mmu_p->oam = oam_buf;
  memset(oam_buf, 0, SZ_OAM);
}
