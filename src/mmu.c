#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "mmu.h"
#include "emu.h"


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


uint8_t read_8(emu *gb_emu_p, mem_addr addr) {
  switch (get_mem_region(addr)) {
    // These cases can safely fall through
    case (REGION_RES_INT_VEC) :
    case (REGION_CART_HEADER) :
    case (REGION_CART_ROM_BANK_0) :
      return gb_emu_p->gb_rom.data[addr];
    case (REGION_CART_ROM_BANK_SWITCH) :
      break;
    case (REGION_CHAR_RAM) :
      break;
    case (REGION_BG_MAP_DATA_1) :
      break;
    case (REGION_BG_MAP_DATA_2) :
      break;
    case (REGION_CART_RAM) :
      break;
    // On the DMG, the internal RAM doesn't have a switchable bank
    case (REGION_INTERNAL_RAM_0):
    case (REGION_INTERNAL_RAM_SWITCH) :
      break;
    case (REGION_ECHO_RAM) :
      break;
    case (REGION_OAM) :
      break;
    case (REGION_RESERVED) :
      break;
    case (REGION_HW_IO_REGS) :
      break;
    case (REGION_ZERO_PAGE) :
      break;
    case (REGION_INT_ENABLE_FLAG) :
      break;
    case (ERR_INVALID_ADDRESS) :
      printf("Error when trying to read from address.\n");
  }
  return ERR_INVALID_ADDRESS;
}


uint16_t read_16(emu *gb_emu_p, mem_addr addr) {
  return (((uint16_t)(gb_emu_p->gb_rom.data[addr+1])) << 8) | gb_emu_p->gb_rom.data[addr];
}


void  write_8(emu *gb_emu_p, mem_addr addr, uint8_t val) {
  uint16_t buf_offset;
  switch (get_mem_region(addr)) {
    // These cases can safely fall through
    case (REGION_RES_INT_VEC) :
    case (REGION_CART_HEADER) :
    case (REGION_CART_ROM_BANK_0) :
      // Illegal write here  Can't write to ROM
    case (REGION_CART_ROM_BANK_SWITCH) :
      break;
    case (REGION_CHAR_RAM) :
      break;
    case (REGION_BG_MAP_DATA_1) :
      break;
    case (REGION_BG_MAP_DATA_2) :
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
      break;
    case (REGION_OAM) :
      break;
    case (REGION_RESERVED) :
      break;
    case (REGION_HW_IO_REGS) :
      printf("Writing value %02x to HW IO register at address %04x\n", val, addr);
      buf_offset = addr - HW_IO_REGS_START;
      gb_emu_p->gb_mmu.hw_io_regs[buf_offset] = val;
      break;
    case (REGION_ZERO_PAGE) :
      printf("Writing value %02x to zero page at address %04x\n", val, addr);
      buf_offset = addr - ZERO_PAGE_START;
      gb_emu_p->gb_mmu.zero_page[buf_offset] = val;
      break;
    case (REGION_INT_ENABLE_FLAG) :
      printf("Writing value %02x to IE flag\n", MASK_IE_FLAG & val);
      gb_emu_p->gb_mmu.ie_flag = MASK_IE_FLAG & val;
      break;
    case (ERR_INVALID_ADDRESS) :
      printf("Error when trying to write to address.\n");
  }

}

void write_16(emu *gb_emu_p, mem_addr addr, uint16_t val) {
  // Dummy interface for now.
  return;
}

void init_mmu(mmu *mmu_p) {
  uint8_t *ram_buf = malloc(SZ_INTERNAL_RAM);
  mmu_p->ram = ram_buf;
  uint8_t *io_buf = malloc(SZ_HW_IO_REGS);
  mmu_p->hw_io_regs = io_buf;
  uint8_t *zero_page_buf = malloc(SZ_ZERO_PAGE);
  mmu_p->zero_page = zero_page_buf;
}
