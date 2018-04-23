#ifndef MMU_H
#define MMU_H


#include <stdint.h>
#include "error.h"

// We forward declare the emulator typedef here to avoid a circular include
typedef struct emu emu;

typedef uint16_t mem_addr;

// Reads 8 bits from the byte designated by addr. Returns ERR_INVALID_ADDRESS
// if the given address is not readable
int read_8(emu *gb_emu_p, mem_addr addr);

// Reads 16 bits starting at the byte designated by addr. Note that
// the z80 is little-endian. The returned value will contain the value at
// address "addr" in its lower 8 bits, and "addr+1" in its upper 8 bits in order
// to account for the endianness of the architecture.
uint16_t read_16(emu *gb_emu_p, mem_addr addr);

// Writes a byte of data to the byte designated by addr
int write_8(emu *gb_emu_p, mem_addr addr, uint8_t val);

// Writes two bytes of data starting at the byte designated by addr. Again,
// since the z80 is little-endian, this writes "val" in such a way that if
// read_16 were called on addr after the write, it would return "val" exactly.
void write_16(emu *gb_emu_p, mem_addr addr, uint16_t val);

// Given an address, returns the mem_region enum representing the
// region of memory the address resides in
int get_mem_region(mem_addr addr);

// Given an address in the HW_IO_REGION, returns the io_region enum
// representing the region of memory the address resides in
int get_hw_io_region(mem_addr addr);

// The MMU struct contains internal RAM banks and memory-mapped registers
// that are accessed through the same memory-mapped interface as the cartridge
// ROM. Eventually, this will be used to implement bank switching and MBC-x
// cartridge features, as well as cartridge RAM.
typedef struct mmu {
  uint8_t ie_flag; // 0xFFFF - interrupt enable
  uint8_t *ram;
  uint8_t *hw_io_regs;
  uint8_t *zero_page;
  uint8_t *bg_map_data_1;
  uint8_t *bg_map_data_2;
  uint8_t *char_ram;
  uint8_t *oam;
  uint8_t reg_joyp; // top four bits are arrow data, low 4 bits are everything else
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

enum io_region {
  REGION_IO_GPU,
  REGION_TIMER,
  REGION_JOYP,
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
#define CHAR_RAM_SEC_0_START 0x8000
#define CHAR_RAM_SEC_1_START 0x8800
#define CHAR_RAM_SEC_0_END 0x9000
#define CHAR_RAM_SEC_1_END 0x9800
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
#define SZ_INTERNAL_RAM 0x2000
#define SZ_HW_IO_REGS 0x80
#define SZ_ZERO_PAGE 0x7F
#define SZ_BG_MAP_DATA_1 0x400
#define SZ_BG_MAP_DATA_2 0x400
#define SZ_CHAR_RAM 0x1800
#define SZ_OAM 0xA0

// Masks
#define MASK_IE_FLAG 0xF
#define MASK_STAT_WRITE 0x83
#define MASK_JOYP_WRITE 0x30

// Register definitions
#define REG_IF 0xFF0F
#define REG_IE 0xFFFF
#define REG_JOYP 0xFF00

// GPU regs
#define REG_LCDC 0xFF40
#define REG_STAT 0xFF41
#define REG_SCY 0xFF42
#define REG_SCX 0xFF43
#define REG_LY 0xFF44
#define REG_LYC 0xFF45
#define REG_DMA 0xFF46
#define REG_BGP 0xFF47
#define REG_OBP0 0xFF48
#define REG_OBP1 0xFF49
#define REG_WY 0xFF4A
#define REG_WX 0xFF4B

// Timer regs
#define REG_DIV 0xFF04
#define REG_TIMA 0xFF05
#define REG_TMA 0xFF06
#define REG_TAC 0xFF07

#endif /* MMU_H */
