#ifndef MMU_H
#define MMU_H


#include <stdint.h>
#include "error.h"

// We forward declare the emulator typedef here to avoid a circular include
typedef struct emu emu;

typedef uint16_t mem_addr;

// Reads 8 bits from the byte designated by addr
uint8_t read_8(emu *gb_emu_p, mem_addr addr);

// Reads 16 bits starting at the byte designated by addr. Note that
// the z80 is little-endian. The returned value will contain the value at
// address "addr" in its lower 8 bits, and "addr+1" in its upper 8 bits in order
// to account for the endianness of the architecture.
uint16_t read_16(emu *gb_emu_p, mem_addr addr);

// Writes a byte of data to the byte designated by addr
void write_8(emu *gb_emu_p, mem_addr addr, uint8_t val);

// Writes two bytes of data starting at the byte designated by addr. Again,
// since the z80 is little-endian, this writes "val" in such a way that if
// read_16 were called on addr after the write, it would return "val" exactly.
void write_16(emu *gb_emu_p, mem_addr addr, uint16_t val);

// Given an address, returns the mem_region enum representing the
// region of memory the address resides in
int get_mem_region(mem_addr addr);

// The MMU struct contains internal RAM banks and memory-mapped registers
// that are accessed through the same memory-mapped interface as the cartridge
// ROM. Eventually, this will be used to implement bank switching and MBC-x
// cartridge features, as well as cartridge RAM.
typedef struct mmu {
  uint8_t IE_flag; // 0xFFFF - interrupt enable
  uint8_t *ram;
} mmu;

// Initializes buffers in the MMU
void init_mmu(mmu *mmu_p);

// Enum for distinct regions of memory with different behaviors
enum mem_region {
  REGION_RES_INT_VEC,
  REGION_CART_HEADER,
  REGION_CART_ROM_BANK_0,
  REGION_CART_ROM_BANK_SWITCH,
  REGION_CHAR_RAM,
  REGION_BG_MAP_DATA_1,
  REGION_BG_MAP_DATA_2,
  REGION_CART_RAM,
  REGION_INTERNAL_RAM_0,
  REGION_INTERNAL_RAM_SWITCH,
  REGION_ECHO_RAM,
  REGION_OAM,
  REGION_RESERVED,
  REGION_HW_IO_REGS,
  REGION_ZERO_PAGE,
  REGION_INT_ENABLE_FLAG,
};

// Definitions for borders in memory map
// Memory mapped sections include the START address, but NOT the END address
#define RES_INT_VEC_START 0x0000
#define RES_INT_VEC_END   0x0100
#define CART_HEADER_START 0x0100
#define CART_HEADER_END   0x0150
#define CART_ROM_BANK_0_START 0x0150
#define CART_ROM_BANK_0_END 0x4000
#define CART_ROM_BANK_SWITCH_START 0x4000
#define CART_ROM_BANK_SWITCH_END 0x8000
#define CHAR_RAM_START 0x8000
#define CHAR_RAM_END 0x9800
#define BG_MAP_DATA_1_START 0x9800
#define BG_MAP_DATA_1_END 0x9C00
#define BG_MAP_DATA_2_START 0x9C00
#define BG_MAP_DATA_2_END 0xA000
#define CART_RAM_START 0xA000
#define CART_RAM_END 0xC000
#define INTERNAL_RAM_0_START 0xC000
#define INTERNAL_RAM_0_END 0xD000
#define INTERNAL_RAM_SWITCH_START 0xD000
#define INTERNAL_RAM_SWITCH_END 0xE000
#define ECHO_RAM_START 0xE000
#define ECHO_RAM_END 0xFE00
#define OAM_START 0xFE00
#define OAM_END 0xFEA0
#define RESERVED_START 0xFEA0
#define RESERVED_END 0xFF00
#define HW_IO_REGS_START 0xFF00
#define HW_IO_REGS_END 0xFF80
#define ZERO_PAGE_START 0xFF80
#define ZERO_PAGE_END 0xFFFF
#define INT_ENABLE_FLAG 0xFFFF

// Size of internal RAM buffer (8kb)
#define SZ_INTERNAL_RAM 0x1FFF

#endif /* MMU_H */
