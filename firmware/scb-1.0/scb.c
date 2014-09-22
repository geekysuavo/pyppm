
/* scb.c: SCB firmware main controller source code.
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

/* include the main header. */
#include "main.h"

/* define available pulse sequence elements on this device. */
#define PPM_PULPROG_HAVE_DEADTIME  1
// #define PPM_PULPROG_HAVE_DELAY
// #define PPM_PULPROG_HAVE_POLARIZE
// #define PPM_PULPROG_HAVE_RELAY
// #define PPM_PULPROG_HAVE_ACQUIRE
// #define PPM_PULPROG_HAVE_TX
// #define PPM_PULPROG_HAVE_TUNE
#define PPM_PULPROG_HAVE_SHIM_X    1
#define PPM_PULPROG_HAVE_SHIM_Y    1
#define PPM_PULPROG_HAVE_SHIM_Z    1

/* include the pulse sequencer source code. */
#include "../lib/seq.c"

/* ppm_deadtime: runs a deadtime delay function. */
void ppm_deadtime (uint16_t dt) {
  /* declare a counter variable. */
  uint8_t i;

  /* loop 100 times to make the delay effective. */
  for (i = 0; i < 100; i++)
    _delay_loop_2 (dt);
}

/* ppm_setshim_x: changes the value of the shim dac output. */
inline void ppm_setshim_x (uint16_t shm) {
  /* write the signed shimming word to the dac spi bus. */
  spi_write_dac_pair (SPI_DAC_OUTPUT_A, SPI_DAC_OUTPUT_B, shm);
}

/* ppm_setshim_y: changes the value of the shim dac output. */
inline void ppm_setshim_y (uint16_t shm) {
  /* write the signed shimming word to the dac spi bus. */
  spi_write_dac_pair (SPI_DAC_OUTPUT_C, SPI_DAC_OUTPUT_D, shm);
}

/* ppm_setshim_z: changes the value of the shim dac output. */
inline void ppm_setshim_z (uint16_t shm) {
  /* write the signed shimming word to the dac spi bus. */
  spi_write_dac_pair (SPI_DAC_OUTPUT_E, SPI_DAC_OUTPUT_F, shm);
}

