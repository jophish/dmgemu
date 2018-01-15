#include "mmu.h"

uint8_t read_8(emu *gb_emu_p, mem_addr addr) {
  return gb_emu_p->gb_rom.data[addr];
}

uint16_t read_16(emu *gb_emu_p, mem_addr addr) {
  return (((uint16_t)(gb_emu_p->gb_rom.data[addr+1])) << 8) | gb_emu_p->gb_rom.data[addr];
}

void write_8(emu *gb_emu_p, mem_addr addr, uint8_t val) {
  // Dummy interface for now.
  return;
}

void write_16(emu *gb_emu_p, mem_addr addr, uint16_t val) {
  // Dummy interface for now.
  return;
}
