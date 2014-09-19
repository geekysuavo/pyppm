
/* ppm.c: PPM firmware main controller source code.
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

/* declare a global variable to store the current PPM state. */
volatile uint8_t ppm_state = PPM_IDLE;

/* declare a global variable for polarization timing.
 *  - t_POL = 0xff / (F_CLK/1024) * ovf
 *          = 0xff / (16MHz/1024) * ovf
 *          = (16.384 ms) * ovf
 *  - default value of 62 counts provides a 1.016 s delay
 */
volatile uint16_t ppm_polarize_ovf = 62;
volatile uint16_t ppm_polarize_idx = 0;

/* declare a global variable for acquisition timing.
 *  - t_DW = ovf / F_CLK
 *         = ovf / 16MHz
 *         = (62.5 ns) * ovf
 *  - default value of 800 provides a 50 us delay (20 kS/s)
 *  - handled via hardware overflow of timer1
 *  - the global variable exists for simplicity
 */
volatile uint16_t ppm_acquire_ovf = 800;

/* declare a global variable for acquisition counting.
 *  - t_ACQ = count * t_DW
 *  - default value of 16,384 provides a 819.2 ms acquisition
 */
volatile uint32_t ppm_acquire_count = 16384;
volatile uint32_t ppm_acquire_idx = 0;

/* declare global variables for dead time delays.
 *  - t_DELAY = 4 * 100 / F_CLK * dt
 *            = 4 * 100 / 16MHz * dt
 *            = 4 * 100 * (62.5 ns) * dt
 *            = (25 us) * dt
 *  - default values of 40, 400 provide 1 ms, 10 ms delays
 */
volatile uint16_t ppm_deadtime_pol = 40;
volatile uint16_t ppm_deadtime_acq = 400;

/* declare a global variable for current source setpoint.
 *  - i_POL = (Vcc / Rcs) / 0xfff * val
 *          = (5 V / 2.5 ohm) / 0xfff * val
 *          = 0.48828125 mA * val
 *  - default value of 256 provides 125 mA current
 */
volatile uint16_t ppm_ccs_val = 256;

/* ppm_deadtime: runs a deadtime delay function. */
inline void ppm_deadtime (uint16_t dt) {
  /* declare a counter variable. */
  uint8_t i;

  /* loop 100 times to make the delay effective. */
  for (i = 0; i < 100; i++)
    _delay_loop_2 (dt);
}

/* ppm_version: returns the firmware version of the device. */
void ppm_version (void) {
  /* write the message header, version, and footer. */
  usb_cdc_putchar (PPM_MSG_DEVICE_VERSION);
  usb_cdc_putchar (0x12);
  usb_cdc_putchar (PPM_MSG_DEVICE_DONE);
}

/* ppm_reset: resets the device using the watchdog timer. */
void ppm_reset (void) {
  /* declare required variables. */
  uint8_t i;

  /* flash the leds a couple times. */
  for (i = 0; i < 10; i++) {
    /* turn off the leds. */
    gpio_leda_off ();
    gpio_ledb_off ();
    gpio_ledc_off ();
    _delay_ms (100.0);

    /* turn on the leds. */
    gpio_leda_on ();
    gpio_ledb_on ();
    gpio_ledc_on ();
    _delay_ms (100.0);
  }

  /* enable the watchdog timer. */
  wdt_enable (WDTO_120MS);

  /* loop and wait for the timer to overflow. */
  while (1);
}

/* ppm_test_dac: ramps the current-sink dac up and back down once. */
void ppm_test_dac (void) {
  /* declare a loop counter. */
  uint16_t i;

  /* enable the current sink output. */
  gpio_ccs_enable ();

  /* loop the dac value up. */
  for (i = 0; i < 4096; i++)
    spi_write_dac (i);

  /* loop the dac value down. */
  for (i = 0; i < 4096; i++)
    spi_write_dac (4095 - i);

  /* disable the current sink output. */
  spi_write_dac (0);
  gpio_ccs_disable ();
}

/* ppm_execute: runs a single experiment with current timing settings. */
void ppm_execute (void) {
  /* initialize the values of the software overflow counters. */
  ppm_polarize_idx = 0;
  ppm_acquire_idx = 0;

  /* only switch the relay if polarization current is nonzero. */
  if (ppm_ccs_val > 0) {
    /* switch the relay to the current sink. */
    gpio_relay_pol ();

    /* delay for the polarization dead-time. */
    ppm_deadtime (ppm_deadtime_pol);

    /* turn on the third led to show polarization. */
    gpio_ledc_on ();

    /* polarize the sample coil. */
    gpio_ccs_enable ();
    spi_write_dac (ppm_ccs_val);

    /* begin timing the polarization. everything from here on out is
     * automatically managed by timers.
     */
    tcnt0_start ();
  }
  else {
    /* turn on the second led to show acquisition. */
    gpio_ledb_on ();

    /* send the acquisition beginning message. */
    usb_cdc_putchar (PPM_MSG_DEVICE_BEGIN);

    /* begin timing the acquisition without switching the relay over. */
    tcnt1_start ();
  }
}

/* ppm_tick_pol: inline function called from the timer/counter0 interrupt
 * service routine to handle any polarization timing, as well as switchover
 * from polarization to acquisition.
 */
inline void ppm_tick_pol (void) {
  /* increment the polarization overflow counter. */
  if (++ppm_polarize_idx > ppm_polarize_ovf) {
    /* disable timer0 interrupts. */
    tcnt0_stop ();

    /* reset the polarization overflow counter and enter sampling mode. */
    ppm_polarize_idx = 0;
    ppm_state = PPM_SAMPLE;

    /* depolarize the sample coil. */
    spi_write_dac (0);
    gpio_ccs_disable ();

    /* turn off the third led to show polarization completion. */
    gpio_ledc_off ();

    /* delay for the polarization dead-time. */
    ppm_deadtime (ppm_deadtime_pol);

    /* switch the relay to the amplifier. */
    gpio_relay_acq ();

    /* delay for the acquisition dead-time. */
    ppm_deadtime (ppm_deadtime_acq);

    /* turn on the second led to show acquisition. */
    gpio_ledb_on ();

    /* send the acquisition beginning message. */
    usb_cdc_putchar (PPM_MSG_DEVICE_BEGIN);

    /* enable timer1 interrupts. */
    tcnt1_start ();
    return;
  }

  /* ask the host to keep waiting for data. (usb keep-alive) */
  usb_cdc_putchar (PPM_MSG_DEVICE_WAIT);
}

/* ppm_tick_acq: inline function called from the timer/counter1 interrupt
 * service routine to handle any acquisition timing, as well as switchover
 * from acqisition to idle.
 */
inline void ppm_tick_acq (void) {
  /* declare a variable to store the read ADC sample. */
  uint16_t adc = 0;

  /* increment the acquisition counter. */
  if (++ppm_acquire_idx > ppm_acquire_count) {
    /* disable timer1 interrupts. */
    tcnt1_stop ();

    /* turn off the second "acquisition" led. */
    gpio_ledb_off ();

    /* send the execution complete message. */
    usb_cdc_putchar (PPM_MSG_DEVICE_DONE);

    /* reset the acquisition counter and enter idle mode. */
    ppm_acquire_idx = 0;
    ppm_state = PPM_IDLE;
    return;
  }

  /* sample the ADC. */
  adc = spi_read_adc ();

  /* transmit the ADC sample to the usb host. */
  usb_cdc_putchar (MSB (adc));
  usb_cdc_putchar (LSB (adc));
}

/* ppm_send_parameters: sends the current parameter set to the host. */
void ppm_send_parameters (void) {
  /* send the polarization overflow count. */
  usb_cdc_putchar (MSB (ppm_polarize_ovf));
  usb_cdc_putchar (LSB (ppm_polarize_ovf));

  /* send the acquisition overflow count. */
  usb_cdc_putchar (MSB (ppm_acquire_ovf));
  usb_cdc_putchar (LSB (ppm_acquire_ovf));

  /* send the acquisition sample count. */
  usb_cdc_putchar (BYTE3 (ppm_acquire_count));
  usb_cdc_putchar (BYTE2 (ppm_acquire_count));
  usb_cdc_putchar (BYTE1 (ppm_acquire_count));
  usb_cdc_putchar (BYTE0 (ppm_acquire_count));

  /* send the polarization dead time. */
  usb_cdc_putchar (MSB (ppm_deadtime_pol));
  usb_cdc_putchar (LSB (ppm_deadtime_pol));

  /* send the acquisition dead time. */
  usb_cdc_putchar (MSB (ppm_deadtime_acq));
  usb_cdc_putchar (LSB (ppm_deadtime_acq));

  /* send the current sink set-point. */
  usb_cdc_putchar (MSB (ppm_ccs_val));
  usb_cdc_putchar (LSB (ppm_ccs_val));

  /* send the done message. */
  usb_cdc_putchar (PPM_MSG_DEVICE_DONE);
}

/* ppm_receive_parameters: reads a new parameter set from the host. */
void ppm_receive_parameters (void) {
  /* declare variables to hold read bytes. */
  int16_t b0, b1, b2, b3;

  /* read the polarization overflow count. */
  b0 = usb_cdc_getchar ();
  b1 = usb_cdc_getchar ();
  if (b0 != -1 && b1 != -1)
    ppm_polarize_ovf = WORD (b0, b1);

  /* read the acquisition overflow count. */
  b0 = usb_cdc_getchar ();
  b1 = usb_cdc_getchar ();
  if (b0 != -1 && b1 != -1)
    ppm_acquire_ovf = WORD (b0, b1);

  /* read the acquisition sample count. */
  b0 = usb_cdc_getchar ();
  b1 = usb_cdc_getchar ();
  b2 = usb_cdc_getchar ();
  b3 = usb_cdc_getchar ();
  if (b0 != -1 && b1 != -1 && b2 != -1 && b3 != -1)
    ppm_acquire_count = WORD32 (b0, b1, b2, b3);

  /* read the polarization dead time. */
  b0 = usb_cdc_getchar ();
  b1 = usb_cdc_getchar ();
  if (b0 != -1 && b1 != -1)
    ppm_deadtime_pol = WORD (b0, b1);

  /* read the acquisition dead time. */
  b0 = usb_cdc_getchar ();
  b1 = usb_cdc_getchar ();
  if (b0 != -1 && b1 != -1)
    ppm_deadtime_acq = WORD (b0, b1);

  /* read the current sink set-point. */
  b0 = usb_cdc_getchar ();
  b1 = usb_cdc_getchar ();
  if (b0 != -1 && b1 != -1)
    ppm_ccs_val = WORD (b0, b1);

  /* send an acknowledgement that the parameters were set. */
  usb_cdc_putchar (PPM_MSG_DEVICE_DONE);
}

/* ppm_get_acquire_ovf: returns the current timer1 overflow value. */
uint16_t ppm_get_acquire_ovf (void) {
  /* return the value. */
  return ppm_acquire_ovf;
}
