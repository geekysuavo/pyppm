
/* scb.c: SCB firmware main controller source code.
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

/* declare global variables for gradient current setpoints.
 *  - i_POL = (Vcc / Rshim) / 0xfff * val
 */
volatile uint16_t scb_shim_x_val = 0;
volatile uint16_t scb_shim_y_val = 0;
volatile uint16_t scb_shim_z_val = 0;

/* scb_reset: resets the device using the watchdog timer. */
void scb_reset (void) {
  /* declare required variables. */
  uint8_t i;

  /* flash the led a couple times. */
  for (i = 0; i < 10; i++) {
    /* turn off the led. */
    gpio_leda_off ();
    _delay_ms (100.0);

    /* turn on the led. */
    gpio_leda_on ();
    _delay_ms (100.0);
  }

  /* enable the watchdog timer. */
  wdt_enable (WDTO_120MS);

  /* loop and wait for the timer to overflow. */
  while (1);
}

/* scb_send_shims: sends the current shim set to the host. */
void scb_send_shims (void) {
  /* send the x-gradient current set-point. */
  usb_cdc_putchar (MSB (scb_shim_x_val));
  usb_cdc_putchar (LSB (scb_shim_x_val));

  /* send the y-gradient current set-point. */
  usb_cdc_putchar (MSB (scb_shim_y_val));
  usb_cdc_putchar (LSB (scb_shim_y_val));

  /* send the z-gradient current set-point. */
  usb_cdc_putchar (MSB (scb_shim_z_val));
  usb_cdc_putchar (LSB (scb_shim_z_val));

  /* send the done message. */
  usb_cdc_putchar (SCB_MSG_DEVICE_DONE);
}

/* scb_receive_shims: reads a new shim set from the host. */
void scb_receive_shims (void) {
  /* declare variables to hold read bytes. */
  int16_t b0, b1;

  /* read the x-gradient current set-point. */
  b0 = usb_cdc_getchar ();
  b1 = usb_cdc_getchar ();
  if (b0 != -1 && b1 != -1)
    scb_shim_x_val = WORD (b0, b1);

  /* read the y-gradient current set-point. */
  b0 = usb_cdc_getchar ();
  b1 = usb_cdc_getchar ();
  if (b0 != -1 && b1 != -1)
    scb_shim_y_val = WORD (b0, b1);

  /* read the z-gradient current set-point. */
  b0 = usb_cdc_getchar ();
  b1 = usb_cdc_getchar ();
  if (b0 != -1 && b1 != -1)
    scb_shim_z_val = WORD (b0, b1);

  /* update the dac output values. */
  spi_write_dac (scb_shim_x_val, scb_shim_y_val, scb_shim_z_val);

  /* send an acknowledgement that the shims were set. */
  usb_cdc_putchar (SCB_MSG_DEVICE_DONE);
}
