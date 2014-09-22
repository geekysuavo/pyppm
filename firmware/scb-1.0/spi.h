
/* spi.h: SCB firmware serial peripheral interface header.
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

/* ensure proper inclusion. */
#ifndef __PPM_SPI_H__
#define __PPM_SPI_H__

/* define address bits for addressing dac outputs. */
#define SPI_DAC_ADDR0  (1 << 12)
#define SPI_DAC_ADDR1  (1 << 13)
#define SPI_DAC_ADDR2  (1 << 14)

/* define addresses for each dac output. */
#define SPI_DAC_OUTPUT_A   0x00
#define SPI_DAC_OUTPUT_B   SPI_DAC_ADDR0
#define SPI_DAC_OUTPUT_C   SPI_DAC_ADDR1
#define SPI_DAC_OUTPUT_D  (SPI_DAC_ADDR1 | SPI_DAC_ADDR0)
#define SPI_DAC_OUTPUT_E   SPI_DAC_ADDR2
#define SPI_DAC_OUTPUT_F  (SPI_DAC_ADDR2 | SPI_DAC_ADDR0)
#define SPI_DAC_OUTPUT_G  (SPI_DAC_ADDR2 | SPI_DAC_ADDR1)
#define SPI_DAC_OUTPUT_H  (SPI_DAC_ADDR2 | SPI_DAC_ADDR1 | SPI_DAC_ADDR0)

/* begin function declarations: */

void spi_init (void);

void spi_write_dac_pair (uint16_t addr0, uint16_t addr1, uint16_t v);

#endif /* __PPM_SPI_H__ */
