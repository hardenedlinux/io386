#ifndef _IO386_H_
#define _IO386_H_

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif

#include <stdint.h>
#include <stdbool.h>
enum width {
  BYTE,
  SHORT,
  INT,
  INVALID
};

enum base {
  DEC,
  OCT,
  HEX
};

enum mode {
  UNINIT,
  INPUT,
  OUTPUT
};

typedef struct op_mode {
  uint8_t width;
  uint8_t base;
  uint8_t mode;
} op_mode;

#define INVALID_OP_MODE (struct op_mode){INVALID, INVALID, UNINIT}

#ifdef __cplusplus
#if 0
{
#endif
}
#endif

#endif
