
/* main.c: SCB firmware main source code.
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

/* main: firmware execution entry point. */
int main (void) {
  /* declare a byte for reading command bytes. */
  uint8_t host_command = 0x00;

  /* disable the watchdog timer. */
  MCUSR = 0x00;
  wdt_disable ();

  /* disable interrupts during initialization. the usb_init() function
   * will re-enable interrupts once it has enabled the usb controller.
   */
  cli ();

  /* initialize SCB components. the usb_init() function re-enables global
   * interrupts, so no sei() statement is necessary after the init block.
   */
  clk_init ();
  gpio_init ();
  spi_init ();
  usb_init ();

  /* wait for the usb host to configure the device. */
  while (!usb_configured ());

  /* enable the analog signal chain power supply. */
  gpio_ldo_enable ();

  /* turn off the led. */
  gpio_leda_off ();

  /* flash the led while delaying for a second. */
  _delay_ms (200.0);
  gpio_leda_on ();
  _delay_ms (200.0);
  gpio_leda_off ();
  _delay_ms (200.0);
  gpio_leda_on ();
  _delay_ms (200.0);
  gpio_leda_off ();
  _delay_ms (200.0);
  gpio_leda_on ();
  _delay_ms (200.0);
  gpio_leda_off ();
  
  /* enter the main idle loop. */
  while (1) {
    /* wait for the user to open the device node with a terminal emulator
     * that sets DTR to indicate its ready to receive data.
     */
    while (!usb_cdc_control_line_dtr ());

    /* discard bytes received prior to receipt of the DTR signal. */
    usb_cdc_flush_input ();

    /* turn on the led to show connection. */
    gpio_leda_on ();

    /* now sit in a wait state for the next byte. */
    while (1) {
      /* try to read a byte from the host. */
      host_command = usb_cdc_getchar ();

      /* was a byte read from the host? */
      if (host_command != -1) {
        /* act based on the host command. */
        switch (host_command) {
          /* read shims. */
          case SCB_MSG_HOST_READ_SHIMS:
            scb_send_shims ();
            break;

          /* write shims. */
          case SCB_MSG_HOST_WRITE_SHIMS:
            scb_receive_shims ();
            break;

          /* reset device. */
          case SCB_MSG_HOST_RESET:
            scb_reset ();
            break;

          /* read parameters. */
          default:
            break;
        }
      }

      /* see if the usb has been de-configured or if the user disconnected. */
      if (!usb_configured () || !usb_cdc_control_line_dtr ()) break;
    }

    /* turn off the "connection" led. */
    _delay_ms (50.0);
    gpio_leda_off ();
  }

  /* execution never reaches this point. */
  return 0;
}
