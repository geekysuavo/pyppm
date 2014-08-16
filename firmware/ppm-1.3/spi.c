
/* spi.c: PPM firmware serial peripheral interface source code.
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

/* spi_init: initialize the serial peripheral interface to the ADC. */
void spi_init (void) {
  /* disable SPI interrupts. */
  SPCR &= ~(1 << SPIE);

  /* set the SPI clock rate to GCLK/8. */
  SPSR |= (1 << SPI2X);
  SPCR |= (1 << SPR0);
  SPCR &= ~(1 << SPR1);

  /* set SPI falling edges as leading edges, MISO falling-edge sampling. */
  SPCR |= (1 << CPOL);
  SPCR &= ~(1 << CPHA);

  /* set SPI data ordering to MSB first. */
  SPCR &= ~(1 << DORD);

  /* enable SPI in master mode. */
  SPCR |= ((1 << MSTR) | (1 << SPE));
}

/* spi_read_adc: transfers a 16-bit word from the AD7680 SPI ADC. */
uint16_t spi_read_adc (void) {
  /* declare a storage variable for the ADC value. */
  uint8_t adc_hi, adc_mid, adc_lo;
  uint16_t adc;

  /* assert the ADC /CS pin to begin a conversion. */
  gpio_adc_select ();

  /* shift the first byte from the ADC. */
  SPDR = 0;
  while (!(SPSR & (1 << SPIF)));
  adc_hi = SPDR;

  /* shift the second byte from the ADC. */
  SPDR = 0;
  while (!(SPSR & (1 << SPIF)));
  adc_mid = SPDR;

  /* shift the third byte from the ADC. */
  SPDR = 0;
  while (!(SPSR & (1 << SPIF)));
  adc_lo = SPDR;

  /* de-assert the ADC /CS pin to end the conversion. */
  gpio_adc_deselect ();

  /* build the final adc sample word. */
  adc = ((adc_hi << 12) | (adc_mid << 4) | (adc_lo >> 4));

  /* return the sampled value. */
  return adc;
}
