/* 
 * io386.h
 * A command line tool wrapping around ioperm(2) iopl(2) outb(2), etc.
 * 
 *
 * The program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * The program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA 
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 *
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */

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
