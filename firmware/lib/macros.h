
/* macros.h: PPM firmware macro function header.
 * Copyright (C) 2014  Bradley Worley  <geekysuavo@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* definition of the hexadecimal firmware version. */
#define PPM_FW_VERSION \
  ((PPM_FW_VER_MAJOR << 4) | PPM_FW_VER_MINOR)

/* convenience functions for splitting 16-bit words by byte. */
#define LSB(n) (n & 0xff)
#define MSB(n) ((n >> 8) & 0xff)

/* convenience functions for splitting 32-bit words by byte. */
#define BYTE0(n) LSB(n)
#define BYTE1(n) MSB(n)
#define BYTE2(n) ((n >> 16) & 0xff)
#define BYTE3(n) ((n >> 24) & 0xff)

/* convenience function for joining bytes into a 16-bit word. */
#define WORD(msb, lsb) (((msb << 8) & 0xff00) | (lsb & 0x00ff))

/* convenience function for joining bytes into a 32-bit word. */
#define WORD32(b3, b2, b1, b0) \
  ((((int32_t) b3 << 24) & 0xff000000) | \
   (((int32_t) b2 << 16) & 0x00ff0000) | \
    (((int32_t) b1 << 8) & 0x0000ff00) | \
           ((int32_t) b0 & 0x000000ff))

/* macro function to suspend interrupts into a temporary variable. */
#define sreg_freeze() \
  uint8_t frozen_interrupt_state = SREG; \
  cli ();

/* macro function that exists because i'm too lazy to change my ways. */
#define sreg_refreeze() \
  frozen_interrupt_state = SREG; \
  cli ();

/* macro function that un-suspends interrupts. */
#define sreg_unfreeze() \
  SREG = frozen_interrupt_state;

