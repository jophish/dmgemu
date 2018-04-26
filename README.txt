This is a work-in-progress gameboy classic (DMG) emulator written in C.

To compile, simply run `make` from the root directory. A binary will be created in `bin/dmgemu`. A ROM compatible with the DMG is required to run the emulator. To run the emualtor with a binary in `roms/tetris.gb`, simply run `./bin/dmgemu roms/tetris.gb`.

A debug build can be made by running `make debug`, which will generate a binary that prompts the user after every instruction with a debugger prompt. The debugger currently supports a variety of features, such as breakpoints, stepping n instructions, listing arbitrary memory locations (0x0000 - 0xFFFF), listing processor register contents, a simple dissasembler, and the like. Typing `help` at the debug prompt will list the available commands.



TODO:

- Window for BG map 
- Window for tileset
- Add functions in GPU for:
  - Getting tileset with pallette AND desired width (tiles per row) [2]
  - Getting BG with pallette [3]
  - Getting sprites
- Add utility function for taking an N x M array and scaling it to 2N x 2M, 3N x3M, etc etc [1]

- Fix background rendering issues

- Implement MBC1 (and others)
