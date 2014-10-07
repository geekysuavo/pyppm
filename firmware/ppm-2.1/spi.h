
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

/* NOTE: all dac unit nomenclature is based around the AD5752 dual-output
 * DAC, so output A resides on pin #3, and output B resides on pin #23.
 * the pin-compatible quad-output AD5754 calls pin #23 "output C", but
 * the bits line up with our "output B".
 */

/* declare dac shift register bit fields.
 * ad575{2,4} datasheet, pages 24-26.
 */
#define SPI_DAC_RW          (1 << 7)  /* 0x80 */
#define SPI_DAC_REG2        (1 << 5)  /* 0x20 */
#define SPI_DAC_REG1        (1 << 4)  /* 0x10 */
#define SPI_DAC_REG0        (1 << 3)  /* 0x08 */
#define SPI_DAC_A2          (1 << 2)  /* 0x04 */
#define SPI_DAC_A1          (1 << 1)  /* 0x02 */
#define SPI_DAC_A0          (1 << 0)  /* 0x01 */

/* declare dac range register bit fields.
 * ad575{2,4} datasheet, page 25.
 */
#define SPI_DAC_RANGE_R0  (1 << 0)  /* 0x01 */
#define SPI_DAC_RANGE_R1  (1 << 1)  /* 0x02 */
#define SPI_DAC_RANGE_R2  (1 << 2)  /* 0x04 */

/* declare dac power register bit fields.
 * ad575{2,4} datasheet, page 26.
 */
#define SPI_DAC_PWR_UNIT_A  (1 << 0)  /* 0x01 */
#define SPI_DAC_PWR_UNIT_B  (1 << 2)  /* 0x04 */

/* declare dac address bit masks.
 * ad575{2,4} datasheet, page 24.
 */
#define SPI_DAC_ADDRESS_A   0x00
#define SPI_DAC_ADDRESS_B   SPI_DAC_A1
#define SPI_DAC_ADDRESS_AB  SPI_DAC_A2

/* declare dac register bit masks.
 * ad575{2,4} datasheet, pages 24-26.
 */
#define SPI_DAC_REGISTER_DAC    0x00
#define SPI_DAC_REGISTER_RANGE  SPI_DAC_REG0
#define SPI_DAC_REGISTER_PWR    SPI_DAC_REG1

/* declare dac range register bit masks.
 * ad575{2,4} datasheet, page 25.
 */
#define SPI_DAC_RANGE_UNI_5    0x00
#define SPI_DAC_RANGE_UNI_10   SPI_DAC_RANGE_R0
#define SPI_DAC_RANGE_BI_5     (SPI_DAC_RANGE_R1 | SPI_DAC_RANGE_R0)
#define SPI_DAC_RANGE_BI_10    SPI_DAC_RANGE_R2

/* begin function declarations: */

void spi_init (void);

void spi_write_dac (uint8_t output, uint16_t value);

#define spi_write_dac_a(v) \
  spi_write_dac (SPI_DAC_ADDRESS_A, (v));

#define spi_write_dac_b(v) \
  spi_write_dac (SPI_DAC_ADDRESS_B, (v));

void spi_write_tune (uint16_t value);

#endif /* __PPM_SPI_H__ */
