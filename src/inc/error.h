#ifndef ERR_H
#define ERR_H

// Enum for handling errors in a nice-ish way
enum error {
  ERR_INVALID_ADDRESS = -1,
  ERR_NOT_WRITABLE = -2,
  ERR_OP_INVALID_OR_NOT_IMPLEMENTED = -3,
};
#endif /* ERR_H */
