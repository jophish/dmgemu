#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "debug.h"
#include "util.h"
#include "error.h"
#include "emu.h"
#include "mmu.h"
#include "opcodes.h"

int debug_prompt(emu *gb_emu_p) {

  // Keeps track of whether or not we're done showing the prompt
  bool success = false;
  bool shown_inst = false;
  // If the prompt would be shown, first show the previous instruction
  // that was executed
  cpu *z80_p = &(gb_emu_p->z80);
  debugger *dbg_p = &(gb_emu_p->gb_debugger);
  // Don't show the prompt at all if we should be stepping
  if (dbg_p->n > 0) {
    dbg_p->n -= 1;
    success = true;
  }

  // If we've told the debugger to execute until we reach a breakpoint, don't show
  // the prompt unless we've actually reached a breakpoint
  if (dbg_p->run) {
    if (hit_breakpoint(z80_p, dbg_p)) {
      dbg_p->run = false;
    } else {
      success = true;
    }
  }


  while(!success) {
    int err;
    char * dbg_str;
    if (!shown_inst) {
      show_previous_inst(gb_emu_p);
      shown_inst = true;
    }
    show_prompt(z80_p);
    char line[MAX_USER_INPUT_LENGTH];
    err = get_user_input(line, sizeof(line));
    if(err == ERR_READ_LINE) {
      dbg_str = "";
    } else {
      dbg_str = strtok(line, TOK_DELIM);
    }
    char *endptr;
    uint16_t addr;
    int mem_val;
    uint32_t addr2_hi;
    switch (get_debug_tok(dbg_str)) {

      case (TOK_STEP_INST) :
	// This check makes sure "si" is the only token in the input string
	if ((dbg_str = get_next_tok()) != NULL) {
	  printf("%ssi takes no arguments\n", ERR_SPACE);
	  break;
	}
	success = true;
	break;

      case (TOK_STEP_N_INSTS) :
	if ((dbg_str = get_next_tok()) == NULL) {
	  printf("%ssn takes a single positive integer argument\n", ERR_SPACE);
	  break;
	}

	int num_steps = (int)strtoimax(dbg_str, &endptr, 10);
	if (num_steps <= 0) {
	  printf("%sPlease enter a positive integer number of steps\n", ERR_SPACE);
	  break;
	}
	if ((dbg_str = get_next_tok()) != NULL) {
	  printf("%ssn takes a single argument\n", ERR_SPACE);
	  break;
	}

	dbg_p->n = (uint16_t)num_steps;
	success = true;
	break;

      case (TOK_SET_BP) :
	if ((dbg_str = get_next_tok()) == NULL) {
	  printf("%sbp takes a single hexadecimal argument\n", ERR_SPACE);
	  break;
	}

	addr = (uint16_t)strtoimax(dbg_str, &endptr, 0);

	if (addr <= 0) {
	  printf("%sPlease enter a hexadecimal address (e.g., 0xFF12)\n", ERR_SPACE);
	  break;
	}

	if ((dbg_str = get_next_tok()) != NULL) {
	  printf("%sbp takes a single argument\n", ERR_SPACE);
	  break;
	}

	if ((err = add_breakpoint(dbg_p, addr)) == ERR_MAX_BREAKPOINTS) {
	  printf("%sMaximum number of breakpoints (%d) already set\n", ERR_SPACE, MAX_BREAKPOINTS);
	  break;
	}

	if (err == ERR_DUPLICATE_BP) {
	  printf("%sBreakpoint already exists for this address (0x%04x)\n", ERR_SPACE, addr);
	  break;
	}

	printf("%sBreakpoint set at address 0x%04x\n", ERR_SPACE, addr);
	break;

      case (TOK_RM_BP) :
	if ((dbg_str = get_next_tok()) == NULL) {
	  printf("%srbp takes a single hexadecimal argument\n", ERR_SPACE);
	  break;
	}

	addr = (uint16_t)strtoimax(dbg_str, &endptr, 0);

	if (addr <= 0) {
	  printf("%sPlease enter a hexadecimal address (e.g., 0xFF12)\n", ERR_SPACE);
	  break;
	}

	if ((dbg_str = get_next_tok()) != NULL) {
	  printf("%srbp takes a single argument\n", ERR_SPACE);
	  break;
	}

	if ((err = remove_breakpoint(dbg_p, addr)) == ERR_INVALID_BP) {
	  printf("%sNo breakpoint defined at address 0x%04x\n", ERR_SPACE, addr);
	  break;
	}

	printf("%sSuccessfully removed breakpoint at address 0x%04x\n", ERR_SPACE, addr);
	break;

      case (TOK_CONTINUE) :
	if ((dbg_str = get_next_tok()) != NULL) {
	  printf("%sc takes no arguments\n", ERR_SPACE);
	  break;
	}
	dbg_p->run = true;
	success = true;
	break;
      case (TOK_HELP) :
	if ((dbg_str = get_next_tok()) != NULL) {
	  printf("%shelp takes no arguments\n", ERR_SPACE);
	  break;
	}
	printf("%sstep a single instruction: si\n", ERR_SPACE);
	printf("%sstep n instructions: sn n (e.g., sn 10)\n", ERR_SPACE);
	printf("%sset a breakpoint at hex address n: bp n (e.g., bp 0x0150)\n", ERR_SPACE);
	printf("%sremove a breakpoint at hex address n: rbp n (e.g., rbp 0x0150)\n", ERR_SPACE);
	printf("%sshow all breakpoints: sbp\n", ERR_SPACE);
	printf("%scontinue execution until next breakpoint: c\n", ERR_SPACE);
	printf("%sread memory address at n: m n (e.g., m 0xFF00)\n", ERR_SPACE);
	printf("%sread memory range [n, m): mr n m (e.g., mr 0xFF00 0xFFFF)\n", ERR_SPACE);
	printf("%sread instruction at address n: mi n (e.g., mi 0x0150)\n", ERR_SPACE);
	printf("%sread instructions in memory range [n, m): mri n m (e.g., mri 0x150 0x160)\n", ERR_SPACE);
	printf("%sshow internal cpu registers: r\n", ERR_SPACE);
	printf("%sshow flag register f: f\n", ERR_SPACE);
	printf("%sshow this help menu: help\n", ERR_SPACE);
	break;

      case (TOK_SHOW_BP) :
	if ((dbg_str = get_next_tok()) != NULL) {
	  printf("%ssbp takes no arguments\n", ERR_SPACE);
	  break;
	}
	show_breakpoints(dbg_p);
	break;

      case (TOK_READ_MEM_ADDR) :
	if ((dbg_str = get_next_tok()) == NULL) {
	  printf("%sm takes a single hexadecimal argument\n", ERR_SPACE);
	  break;
	}

	addr = (uint16_t)strtoimax(dbg_str, &endptr, 0);

	if (addr <= 0) {
	  printf("%sPlease enter a hexadecimal address (e.g., 0xFF12)\n", ERR_SPACE);
	  break;
	}

	if ((dbg_str = get_next_tok()) != NULL) {
	  printf("%sm takes a single argument\n", ERR_SPACE);
	  break;
	}

	if ((mem_val = read_8(gb_emu_p, addr)) == ERR_INVALID_ADDRESS) {
	  printf("%sPlease enter a valid (readable) memory address\n", ERR_SPACE);
	  break;
	}

	printf("%s[0x%04x] %02x\n", ERR_SPACE, addr, mem_val);
	break;

      case (TOK_READ_MEM_RANGE) :
	if ((dbg_str = get_next_tok()) == NULL) {
	  printf("%smr takes two hexadecimal arguments\n", ERR_SPACE);
	  break;
	}
	addr = (uint16_t)strtoimax(dbg_str, &endptr, 0);

	if ((dbg_str = get_next_tok()) == NULL) {
	  printf("%smr takes two hexadecimal arguments\n", ERR_SPACE);
	  break;
	}

	addr2_hi = (uint32_t)strtoimax(dbg_str, &endptr, 0);

	if (addr <= 0 || addr2_hi <= 0) {
	  printf("%sPlease enter a hexadecimal address (e.g., 0xFF12)\n", ERR_SPACE);
	  break;
	}

	if (addr > addr2_hi) {
	  printf("%sPlease enter memory addresses from lowest to highest\n", ERR_SPACE);
	  break;
	}

	if (addr > INT_ENABLE_FLAG) {
	  printf("%sPlease enter a first address less than or equal to 0x%04x\n", ERR_SPACE, INT_ENABLE_FLAG);
	  break;
	}

	if (addr2_hi > INT_ENABLE_FLAG+1) {
	  printf("%sPlease enter a first address less than or equal to 0x%05x\n", ERR_SPACE, INT_ENABLE_FLAG+1);
	  break;
	}

	for (int i = 0; i < addr2_hi - addr; i++) {
	  mem_val = read_8(gb_emu_p, addr+i);
	  if (mem_val == ERR_INVALID_ADDRESS) {
	    printf("%s[0x%04x] ??\n", ERR_SPACE, addr);
	  } else {
	    printf("%s[0x%04x] %02x\n", ERR_SPACE, addr+i, mem_val);
	  }
	}
	break;

      case (TOK_READ_MEM_ADDR_INST) :
	if ((dbg_str = get_next_tok()) == NULL) {
	  printf("%smi takes a single hexadecimal argument\n", ERR_SPACE);
	  break;
	}

	addr = (uint16_t)strtoimax(dbg_str, &endptr, 0);

	if (addr <= 0) {
	  printf("%sPlease enter a hexadecimal address (e.g., 0x0150)\n", ERR_SPACE);
	  break;
	}

	if ((dbg_str = get_next_tok()) != NULL) {
	  printf("%smi takes a single argument\n", ERR_SPACE);
	  break;
	}

	if ((mem_val = read_8(gb_emu_p, addr)) == ERR_INVALID_ADDRESS) {
	  printf("%sPlease enter a valid (readable) memory address\n", ERR_SPACE);
	  break;
	}

	show_inst_at_addr(gb_emu_p, addr, NULL);
	break;

      case (TOK_READ_MEM_INST_RANGE) :
	if ((dbg_str = get_next_tok()) == NULL) {
	  printf("%smri takes two hexadecimal arguments\n", ERR_SPACE);
	  break;
	}
	addr = (uint16_t)strtoimax(dbg_str, &endptr, 0);

	if ((dbg_str = get_next_tok()) == NULL) {
	  printf("%smri takes two hexadecimal arguments\n", ERR_SPACE);
	  break;
	}

	addr2_hi = (uint32_t)strtoimax(dbg_str, &endptr, 0);

	if (addr <= 0 || addr2_hi <= 0) {
	  printf("%sPlease enter a hexadecimal address (e.g., 0xFF12)\n", ERR_SPACE);
	  break;
	}

	if (addr > addr2_hi) {
	  printf("%sPlease enter memory addresses from lowest to highest\n", ERR_SPACE);
	  break;
	}

	if (addr > INT_ENABLE_FLAG) {
	  printf("%sPlease enter a first address less than or equal to 0x%04x\n", ERR_SPACE, INT_ENABLE_FLAG);
	  break;
	}

	if (addr2_hi > INT_ENABLE_FLAG+1) {
	  printf("%sPlease enter a first address less than or equal to 0x%05x\n", ERR_SPACE, INT_ENABLE_FLAG+1);
	  break;
	}

	for (int i = 0; i < addr2_hi - addr; ) {
	  mem_val = read_8(gb_emu_p, addr+i);

	  if (mem_val == ERR_INVALID_ADDRESS) {
	    printf("%s[0x%04x] ??\n", ERR_SPACE, addr+i);
	    i += 1;
	  } else {
	    opcode op_struct;
	    show_inst_at_addr(gb_emu_p, addr+i, &op_struct);
	    i += op_struct.len;
	  }

	}
	break;

      case (TOK_SHOW_REGS) :
	if ((dbg_str = get_next_tok()) != NULL) {
	  printf("%sr takes no arguments\n", ERR_SPACE);
	  break;
	}
	printf("%sA: 0x%02x, F: 0x%02x\n", ERR_SPACE, z80_p->regs.a, z80_p->regs.f);
	printf("%sB: 0x%02x, C: 0x%02x\n", ERR_SPACE, z80_p->regs.b, z80_p->regs.c);
	printf("%sD: 0x%02x, E: 0x%02x\n", ERR_SPACE, z80_p->regs.d, z80_p->regs.e);
	printf("%sH: 0x%02x, L: 0x%02x\n", ERR_SPACE, z80_p->regs.h, z80_p->regs.l);
	printf("%sSP: 0x%04x\n", ERR_SPACE, get_SP(z80_p));
	printf("%sPC: 0x%04x\n", ERR_SPACE, get_PC(z80_p));
	printf("%sIME: %s\n", ERR_SPACE, z80_p->regs.ime ? "true" : "false");
	break;

      case (TOK_SHOW_FLAGS) :
	if ((dbg_str = get_next_tok()) != NULL) {
	  printf("%sf takes no arguments\n", ERR_SPACE);
	  break;
	}
	printf("%sZ: %d, N: %d, H: %d, C: %d\n", ERR_SPACE, get_flag_Z(z80_p), get_flag_N(z80_p), get_flag_H(z80_p), get_flag_C(z80_p));
	break;
      default :
	printf("%sPlease enter a valid command (type 'help' for a list)\n", ERR_SPACE);
	break;
    }
  }
  // We're about to step out of the debugger and execute, so set the previous instruction to the
  // current PC
  dbg_p->prev_inst_addr = get_PC(z80_p);
  return 0;
}



int get_debug_tok(char *buf) {
  if (strcmp(buf, DBG_STR_STEP_INST) == 0) {
    return TOK_STEP_INST;
  }
  if (strcmp(buf, DBG_STR_STEP_N_INSTS) == 0) {
    return TOK_STEP_N_INSTS;
  }
  if (strcmp(buf, DBG_STR_HELP) == 0) {
    return TOK_HELP;
  }
  if (strcmp(buf, DBG_STR_SET_BP) == 0) {
    return TOK_SET_BP;
  }
  if (strcmp(buf, DBG_STR_SHOW_BP) == 0) {
    return TOK_SHOW_BP;
  }
  if (strcmp(buf, DBG_STR_RM_BP) == 0) {
    return TOK_RM_BP;
  }
  if (strcmp(buf, DBG_STR_CONTINUE) == 0) {
    return TOK_CONTINUE;
  }
  if (strcmp(buf, DBG_STR_READ_MEM_ADDR) == 0) {
    return TOK_READ_MEM_ADDR;
  }
  if (strcmp(buf, DBG_STR_READ_MEM_RANGE) == 0) {
    return TOK_READ_MEM_RANGE;
  }
  if (strcmp(buf, DBG_STR_SHOW_REGS) == 0) {
    return TOK_SHOW_REGS;
  }
  if (strcmp(buf, DBG_STR_SHOW_FLAGS) == 0) {
    return TOK_SHOW_FLAGS;
  }
  if (strcmp(buf, DBG_STR_READ_MEM_ADDR_INST) == 0) {
    return TOK_READ_MEM_ADDR_INST;
  }
  if (strcmp(buf, DBG_STR_READ_MEM_INST_RANGE) == 0) {
    return TOK_READ_MEM_INST_RANGE;
  }
  return ERR_INVALID_TOKEN;
}


void show_prompt(cpu *z80_p) {
  printf("dmgdbg (0x%04x) > ", get_PC(z80_p));
}


char *get_next_tok(void) {
  return strtok(NULL, TOK_DELIM);
}


void init_dbg(debugger *dbg_p) {
  dbg_p->n = 0;
  dbg_p->run = false;
  dbg_p->prev_inst_addr = 0;
  dbg_p->breakpoints = malloc(MAX_BREAKPOINTS*sizeof(uint16_t));
  memset(dbg_p->breakpoints, 0, MAX_BREAKPOINTS*sizeof(uint16_t));
}


int add_breakpoint(debugger *dbg_p, uint16_t addr) {
  for (int i = 0; i < MAX_BREAKPOINTS; i++) {
    if (dbg_p->breakpoints[i] == addr) {
      return ERR_DUPLICATE_BP;
    }
  }
  for (int i = 0; i < MAX_BREAKPOINTS; i++) {
    if (dbg_p->breakpoints[i] == 0) {
      dbg_p->breakpoints[i] = addr;
      return 0;
    }
  }
  return ERR_MAX_BREAKPOINTS;
}


void show_breakpoints(debugger *dbg_p) {
  int present = 0;
  for (int i = 0; i < MAX_BREAKPOINTS; i++) {
    if (dbg_p->breakpoints[i] != 0) {
      present = 1;
      break;
    }
  }
  if (present == 0) {
    printf("%sNo breakpoints currently set\n", ERR_SPACE);
  } else {
    printf("%sBreakpoints at addresses:\n", ERR_SPACE);
    for (int i = 0; i < MAX_BREAKPOINTS; i++) {
      if (dbg_p->breakpoints[i] != 0) {
	printf("%s%s0x%04x\n", ERR_SPACE, ERR_SPACE, dbg_p->breakpoints[i]);
      }
    }
  }
}

int remove_breakpoint(debugger *dbg_p, uint16_t addr) {
  for (int i = 0; i < MAX_BREAKPOINTS; i++) {
    if (dbg_p->breakpoints[i] == addr) {
      dbg_p->breakpoints[i] = 0;
      return 0;
    }
  }
  return ERR_INVALID_BP;
}

bool hit_breakpoint(cpu *z80_p, debugger *dbg_p) {
  return check_breakpoint(dbg_p, get_PC(z80_p));
}

bool check_breakpoint(debugger *dbg_p, uint16_t addr) {
  if (!addr)
    return false;
  for (int i = 0; i < MAX_BREAKPOINTS; i++) {
    if (dbg_p->breakpoints[i] == addr) {
      return true;
    }
  }
  return false;
}

void show_inst_at_addr(emu *gb_emu_p, uint16_t addr, opcode *op_struct_p) {
  char buf[MAX_BUF_LEN];
  int err;
  opcode op_struct;

  if ((err = addr_to_op_str(gb_emu_p, addr, buf, MAX_BUF_LEN, &op_struct)) == ERR_OP_INVALID_OR_NOT_IMPLEMENTED) {
    printf("%sNo string data available for this op\n", ERR_SPACE);
    return;
  } else if (err == ERR_BUF_LEN) {
    printf("Debugger needs more buffer space to print this op\n");
    exit(0);
  }

  // Formatting
  uint16_t space_len = 20 - (int)strlen(buf);
  char space[space_len];
  for (int i = 0; i < space_len-1; i++) {
    space[i] = ' ';
  }
  space[space_len-1] = '\0';
  printf("%s[0x%04x] %s%s(", ERR_SPACE, addr, buf, space);
  for (int i = 0; i < op_struct.len; i++) {
    printf(" %02x", read_8(gb_emu_p, addr+i));
  }
  printf(" )\n");
  if (op_struct_p != NULL) {
    *op_struct_p = op_struct;
  }
  return;

}
void show_previous_inst(emu *gb_emu_p) {
  debugger *dbg_p = &(gb_emu_p->gb_debugger);
  uint16_t addr = dbg_p->prev_inst_addr;
  if (addr == 0) {
    return;
  } else {
    show_inst_at_addr(gb_emu_p, addr, NULL);
  }
}
