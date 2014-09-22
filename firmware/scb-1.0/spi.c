
/* spi.c: SCB firmware serial peripheral interface source code.
 * Copyright (C) 2013  Bradley Worley  <geekysuavo@gmail.com>
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

/* spi_wait: waits for a transmit to end on the SPI bus.
 */
#define spi_wait() \
  while (!(SPSR & (1 << SPIF)));

/* spi_write: writes a byte and waits for the bus to finish.
 * NOTE: this macro inverts the byte value to accomodate
 * the optoisolator connection.
 */
#define spi_write(byte) \
  SPDR = ((uint8_t) ~(byte)); \
  spi_wait ();

/* spi_init: initialize the serial peripheral interface to the DAC. */
void spi_init (void) {
  /* disable SPI interrupts. */
  SPCR &= ~(1 << SPIE);

  /* set the SPI clock rate to GCLK/128 (125 kHz). */
  SPCR |= ((1 << SPR1) | (1 << SPR0));

  /* set the SPI SCK line to idle high. (inverted via the optoisolator) */
  SPCR |= (1 << CPOL);

  /* set the SPI MOSI line for falling-edge sampling. */
  SPCR |= (1 << CPHA);

  /* set SPI data ordering to MSB first. */
  SPCR &= ~(1 << DORD);

  /* enable SPI in master mode. */
  SPCR |= ((1 << MSTR) | (1 << SPE));

  /* set the reference and gain modes.
   * WORD = b 1000 0000 0000 0011
   */
  gpio_dac_select ();
  spi_write (1 << 7);
  spi_write ((1 << 1) | (1 << 0));
  gpio_dac_deselect ();

  /* set the output latching mode.
   * WORD = b 1010 0000 0000 0000
   */
  gpio_dac_select ();
  spi_write ((1 << 7) | (1 << 5));
  spi_write (0x00);
  gpio_dac_deselect ();

  /* power down the unused outputs.
   * WORD = b 1100 0000 1100 0000
   */
  gpio_dac_select ();
  spi_write ((1 << 7) | (1 << 6));
  spi_write ((1 << 7) | (1 << 6));
  gpio_dac_deselect ();
}

/* spi_write_dac_unit: sets a single output of the AD5328 SPI DAC. */
void spi_write_dac_unit (uint16_t addr, uint16_t value) {
  /* compute the output word to the device. */
  uint16_t word = addr | value;

  /* assert the DAC /CS pin to begin a conversion. */
  gpio_dac_select ();

  /* write the two-byte word to the device, MSB first. */
  spi_write (MSB (word));
  spi_write (LSB (word));

  /* de-assert the DAC /CS pin to end the conversion. */
  gpio_dac_deselect ();
}

/* spi_write_dac_pair: writes a single signed 16-bit word to the AD5328
 * SPI DAC.
 */
void spi_write_dac_pair (uint16_t addr0, uint16_t addr1, uint16_t v) {
  /* declare required variables. */
  uint16_t mv;
  uint8_t sv;

  /* compute the sign of the value. */
  sv = (v & (1 << 15)) >> 15;

  /* compute the magnitude of the value, to 12-bit precision. */
  mv = v & ~(1 << 15);
  mv = (mv >> 4) & 0x0fff;

  /* act based on the sign of the shim word. */
  if (sv) {
    /* negative: send a negative value into the right dac unit. */
    spi_write_dac_unit (addr0, 0);
    spi_write_dac_unit (addr1, mv);
  }
  else {
    /* positive: send a positive value into the left dac unit. */
    spi_write_dac_unit (addr0, mv);
    spi_write_dac_unit (addr1, 0);
  }
}

