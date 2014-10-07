
/* spi.c: PPM firmware serial peripheral interfaces source code.
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

/* forward function declarations. */
void spi_write_dac_1x8 (uint8_t byte);
void spi_write_dac_3x8 (uint8_t lo, uint8_t mid, uint8_t hi);

/* spi_init_dac: initialize the serial peripheral interface to the DAC. */
void spi_init_dac (void) {
  /* select the output range of all dac units to +/- 5V. */
  spi_write_dac_3x8 (SPI_DAC_REGISTER_RANGE | SPI_DAC_ADDRESS_AB, 0x00,
                     SPI_DAC_RANGE_BI_5);

  /* power up all the dac outputs. */
  spi_write_dac_3x8 (SPI_DAC_REGISTER_PWR, 0x00,
                     SPI_DAC_PWR_UNIT_A |
                     SPI_DAC_PWR_UNIT_B);

  /* place all the dac outputs on the ground rail. */
  spi_write_dac (SPI_DAC_ADDRESS_AB, 0x0000);
}

/* spi_init_tune: initialize the serial peripheral interface to the switch. */
void spi_init_tune (void) {
  /* turn off all switches to disable capacitive tuning. */
  spi_write_tune (0x0000);
}

/* spi_init: initialize the ADC, DAC and switch serial peripheral interfaces.
 */
void spi_init (void) {
  /* turn off the adc led. */
  _delay_ms (50.0);
  gpio_led_adc_off ();

  /* initialize the dac. */
  spi_init_dac ();

  /* turn off the dac led. */
  _delay_ms (50.0);
  gpio_led_dac_off ();

  /* initialize the switch. */
  spi_init_tune ();
}

/* spi_write_dac_1x8: writes a byte to the AD5752 SPI DAC. */
void spi_write_dac_1x8 (uint8_t byte) {
  /* declare required variables. */
  uint8_t i, out;

  /* loop through the byte. */
  out = byte;
  for (i = 0; i < 8; i++) {
    /* determine the bit value. */
    if (out & 0x80) {
      /* set the bit high. */
      gpio_dac_mosi_hi ();
    }
    else {
      /* set the bit low. */
      gpio_dac_mosi_lo ();
    }

    /* clock the bit out and shift the byte. */
    gpio_dac_clock ();
    out <<= 1;
  }
}

/* spi_write_dac_3x8: writes a set of three bytes to the AD5752 SPI DAC. */
void spi_write_dac_3x8 (uint8_t lo, uint8_t mid, uint8_t hi) {
  /* select the dac. */
  gpio_dac_select ();

  /* write the bytes. */
  spi_write_dac_1x8 (hi);
  spi_write_dac_1x8 (mid);
  spi_write_dac_1x8 (lo);

  /* deselect the dac. */
  gpio_dac_deselect ();
}

/* spi_write_dac: transfers a 16-bit word to the AD5752 SPI DAC. */
void spi_write_dac (uint8_t output, uint16_t value) {
  /* program the dac register:
   * 00000 (A2) (A1) (A0)  MSB  LSB
   */
  spi_write_dac_3x8 (
    output & (SPI_DAC_A2 | SPI_DAC_A1 | SPI_DAC_A0),
    MSB (value), LSB (value));
}

/* spi_write_tune_1x8: writes a byte to the ADG714 switches. */
void spi_write_tune_1x8 (uint8_t byte) {
  /* declare required variables. */
  uint8_t i, out;

  /* loop through the byte. */
  out = byte;
  for (i = 0; i < 8; i++) {
    /* determine the bit value. */
    if (out & 0x80) {
      /* set the bit high. */
      gpio_tune_mosi_hi ();
    }
    else {
      /* set the bit low. */
      gpio_tune_mosi_lo ();
    }

    /* clock the bit out and shift the byte. */
    gpio_tune_clock ();
    out <<= 1;
  }
}

/* spi_write_tune: transfers a 16-bit configuration to the ADG714 switches. */
void spi_write_tune (uint16_t value) {
  /* select the switches. */
  gpio_tune_select ();

  /* write the two bytes. */
  spi_write_tune_1x8 (MSB (value));
  spi_write_tune_1x8 (LSB (value));

  /* deselect the switches. */
  gpio_tune_deselect ();
}

