
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
#define PPM_PULPROG_HAVE_TX        1
#define PPM_PULPROG_HAVE_TUNE      1
// #define PPM_PULPROG_HAVE_SHIM_X
// #define PPM_PULPROG_HAVE_SHIM_Y
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

/* ppm_ccs_enable: enables or disables the constant current sink. */
void ppm_ccs_enable (uint8_t en) {
  /* determine which state the constant current sink should be in. */
  if (en) {
    /* enable the sink. */
    gpio_ccs_enable ();
  }
  else {
    /* disable the sink. */
    gpio_ccs_disable ();
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
  /* run the timed delay element. */
  return tcnt0_run_interp (dt, dac, 0x0000, 0x00);
}

/* ppm_acquire: runs a single acquisition function. */
inline void ppm_acquire (uint32_t n, uint16_t ovf) {
  /* enable the acquisition led indicator. */
  gpio_led_adc_on ();

  /* run a timed acquisition element. */
  tcnt1_run_acquire (n, ovf);

  /* disable the acquisition led indicator. */
  gpio_led_adc_off ();
}

/* ppm_txrise: runs a polarization rising edge function. */
inline uint16_t ppm_txrise (uint16_t n, uint16_t ampl, uint16_t dac) {
  /* enable the dac output led indicator. */
  gpio_led_dac_on ();

  /* run a timed interpolation element. */
  uint16_t dacout = 0x0000;
  dacout = tcnt0_run_interp (n, dac, ampl, 0x01);

  /* disable the dac output led indicator and return. */
  gpio_led_dac_off ();
  return dacout;
}

/* ppm_txfall: runs a polarization falling edge function. */
inline uint16_t ppm_txfall (uint16_t n, uint16_t ampl, uint16_t dac) {
  /* enable the dac output led indicator. */
  gpio_led_dac_on ();

  /* run a timed interpolation element. */
  uint16_t dacout = 0x0000;
  dacout = tcnt0_run_interp (n, dac, ampl, 0x02);

  /* disable the dac output led indicator and return. */
  gpio_led_dac_off ();
  return dacout;
}

/* ppm_txpulse: runs a sinusoidal pulse synthesis function. */
inline void ppm_txpulse (uint32_t n, uint32_t freq, uint8_t ampl) {
  /* enable the dac output led indicator. */
  gpio_led_dac_on ();

  /* run a timed interpolation element. */
  tcnt0_run_pulse (n, freq, ampl);

  /* disable the dac output led indicator. */
  gpio_led_dac_off ();
}

/* ppm_settune: changes the switch configuration of the capacitor bank. */
inline void ppm_settune (uint16_t tun) {
  /* write the tuning word to the switch-bank spi bus. */
  spi_write_tune (tun);
}

/* ppm_setshim_z: changes the value of the shim dac output. */
inline void ppm_setshim_z (uint16_t shm) {
  /* enable the dac output led indicator. */
  gpio_led_dac_on ();

  /* write the shimming word to the dac spi bus. */
  spi_write_dac_b (shm);

  /* disable the dac output led indicator. */
  gpio_led_dac_on ();
}

