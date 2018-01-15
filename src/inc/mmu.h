#ifndef MMU_H
#define MMU_H

#include <stdint.h>
#include "emu.h"

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

#endif /* MMU_H */
