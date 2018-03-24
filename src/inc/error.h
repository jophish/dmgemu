#ifndef ERR_H
#define ERR_H

// Enum for handling errors in a nice-ish way
enum error {
  ERR_INVALID_ADDRESS = -1,
  ERR_NOT_WRITABLE = -2,
  ERR_OP_INVALID_OR_NOT_IMPLEMENTED = -3,
  ERR_READ_LINE = -4,
  ERR_INVALID_TOKEN = -5,
  ERR_MAX_BREAKPOINTS = -6,
  ERR_DUPLICATE_BP = -7,
  ERR_INVALID_BP = -8,
  ERR_BUF_LEN = -9,
  ERR_INVALID_DD_PAIR = -10,
  ERR_INVALID_REG_CODE = -11,
};

#endif /* ERR_H */
