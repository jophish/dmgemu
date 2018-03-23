This is a work-in-progress gameboy classic (DMG) emulator written in C.

To compile, simply run `make` from the root directory. A binary will be created in `bin/dmgemu`. A ROM compatible with the DMG is required to run the emulator. To run the emualtor with a binary in `roms/tetris.gb`, simply run `./bin/dmgemu roms/tetris.gb`.

A debug build can be made by running `make debug`, which will generate a binary that prompts the user after every instruction with a debugger prompt. The debugger currently supports a variety of features, such as breakpoints, stepping n instructions, listing arbitrary memory locations (0x0000 - 0xFFFF), listing processor register contents, a simple dissasembler, and the like. Typing `help` at the debug prompt will list the available commands.


notes to self: filter on opcodes based off the organization in the nintendo ggameboy programmming manual. the first two and last three bits in each instruction actually specify the type of opcode. if some instruction type requires 0b10---110 (where --- is a parameter), you can check if the opcode 'op' matches this pattern easily. first define OP_MASK = 0b11000111, then

`(op & OP_MASK) == 0b10000110`

will test whether or not `op` is the kind of instruction specified by 0b10---110


------------------------

Maintain a list of every individual opcode and CB extensions, but dispatch based on the opcode masks to handler functions.


Current problems to fix:

1. Opcode implementation is bad. Should have an opcode struct which contains (minimally):
   	  - Actual opcode (hex)
	  - mnemonic (ld a, ld b, st, mov, etc...)
	  - debug format string (how to print disassembled version of this op
	  - op length (including args)
	  - cycles (machine and cpu)
	  - op type enum (ld X, st X, xor X, etc...)
	  - FUNCTION pointer to dispatch fn
	  
All the opcode structs should be stored in one big array, where op_array[X] corresponds to the instruction with actual opcode X. For the CB prefixed opcodes, maintain another array (cb_op_array[] or something).

Maintain an enum for each op type (above).

2. 
