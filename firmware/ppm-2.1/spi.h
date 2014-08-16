
/* spi.h: PPM firmware serial peripheral interfaces header.
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

/* ensure proper inclusion. */
#ifndef __PPM_SPI_H__
#define __PPM_SPI_H__

/* declare dac shift register bit fields. */
#define SPI_DAC_RW          0x80
#define SPI_DAC_REG2        0x20
#define SPI_DAC_REG1        0x10
#define SPI_DAC_REG0        0x08
#define SPI_DAC_A2          0x04
#define SPI_DAC_A1          0x02
#define SPI_DAC_A0          0x01

/* declare dac address bit masks. */
#define SPI_DAC_ADDRESS_A   0x00
#define SPI_DAC_ADDRESS_B   SPI_DAC_A1
#define SPI_DAC_ADDRESS_AB  SPI_DAC_A2

/* begin function declarations: */

void spi_init (void);

void spi_write_dac (uint8_t output, uint16_t value);

#define spi_write_dac_a(v) \
  spi_write_dac (SPI_DAC_ADDRESS_A, (v));

#define spi_write_dac_b(v) \
  spi_write_dac (SPI_DAC_ADDRESS_B, (v));

void spi_write_tune (uint16_t value);

#endif /* __PPM_SPI_H__ */
