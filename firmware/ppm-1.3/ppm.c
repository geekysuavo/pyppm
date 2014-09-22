
/* ppm.c: PPM firmware main controller source code.
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
#define PPM_PULPROG_HAVE_DELAY     1
#define PPM_PULPROG_HAVE_POLARIZE  1
#define PPM_PULPROG_HAVE_RELAY     1
#define PPM_PULPROG_HAVE_ACQUIRE   1
// #define PPM_PULPROG_HAVE_TX
// #define PPM_PULPROG_HAVE_TUNE
// #define PPM_PULPROG_HAVE_SHIM_X
// #define PPM_PULPROG_HAVE_SHIM_Y
// #define PPM_PULPROG_HAVE_SHIM_Z

/* include the pulse sequencer source code. */
#include "../lib/seq.c"

/* define two global variables for timer/counter tick counting. */
volatile uint16_t ppm_delay_ticks;
volatile uint32_t ppm_acquire_ticks;

/* ppm_deadtime: runs a deadtime delay function. */
void ppm_deadtime (uint16_t dt) {
  /* declare a counter variable. */
  uint8_t i;

  /* loop 100 times to make the delay effective. */
  for (i = 0; i < 100; i++)
    _delay_loop_2 (dt);
}

/* ppm_ccs_enable: enables or disables the constant current sink. */
void ppm_ccs_enable (uint8_t en) {
  /* determine which state the constant current sink should be in. */
  if (en) {
    /* enable the sink. */
    gpio_ccs_enable ();
    gpio_led_ccs_on ();
  }
  else {
    /* disable the sink. */
    gpio_ccs_disable ();
    gpio_led_ccs_off ();
  }
}

/* ppm_relay_enable: enables or disables the polarize/acquire relay. */
void ppm_relay_enable (uint8_t en) {
  /* determine which state the relay should be in. */
  if (en) {
    /* switch the relay on to enter polarization mode. */
    gpio_relay_pol ();
  }
  else {
    /* switch the relay off to enter acquisition mode. */
    gpio_relay_acq ();
  }
}

/* ppm_delay: runs a precision delay function. */
inline uint16_t ppm_delay (uint16_t dt, uint16_t dac) {
  /* initialize the tick counter. */
  ppm_delay_ticks = 0;

  /* run a timed delay element. */
  tcnt0_start ();

  /* loop until the number of ticks meets the required value. */
  while (ppm_delay_ticks < dt);

  /* end the delay element. */
  tcnt0_stop ();

  /* this variable has no meaning for this firmware. return it verbatim.
   */
  return dac;
}

/* ppm_acquire: runs a single acquisition function. */
inline void ppm_acquire (uint32_t n, uint16_t ovf) {
  /* enable the acquisition led indicator. */
  gpio_led_adc_on ();

  /* initialize the tick counter. */
  ppm_acquire_ticks = 0;

  /* run a timed acquisition element. */
  tcnt1_start (ovf);

  /* loop until the number of ticks meets the required value. */
  while (ppm_acquire_ticks < n);

  /* end the acquisition element. */
  tcnt1_stop ();

  /* disable the acquisition led indicator. */
  gpio_led_adc_off ();
}

/* ppm_tick_del: inline function called from the timer/counter0 interrupt
 * service routine to handle any precision delay timing.
 */
inline void ppm_tick_del (void) {
  /* increment the delay tick counter. */
  ppm_delay_ticks++;
}

/* ppm_tick_acq: inline function called from the timer/counter1 interrupt
 * service routine to handle adc sample acquisition and transmission.
 */
inline void ppm_tick_acq (void) {
  /* declare a variable to store the read ADC sample. */
  uint16_t adc = 0;

  /* sample the ADC. */
  adc = spi_read_adc ();

  /* transmit the ADC sample to the usb host. */
  usb_cdc_putchar (MSB (adc));
  usb_cdc_putchar (LSB (adc));

  /* increment the acquisition tick counter. */
  ppm_acquire_ticks++;
}

