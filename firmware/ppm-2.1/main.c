
/* main.c: PPM firmware main source code.
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

  /* initialize PPM components. the usb_init() function re-enables global
   * interrupts, so no sei() statement is necessary after the init block.
   */
  clk_init ();
//FIXME  tcnt_init ();
  gpio_init ();
  spi_init ();
  usb_init ();

  /* wait for the usb host to configure the device. */
  while (!usb_configured ());

  /* enable the analog signal chain power supply. */
  gpio_asc_enable ();

  /* flash leds while delaying for a second. */
  gpio_led_comm_off ();
  _delay_ms (200.0);
  gpio_led_comm_on ();
  _delay_ms (200.0);
  gpio_led_comm_off ();
  _delay_ms (200.0);
  gpio_led_comm_on ();
  _delay_ms (200.0);
  gpio_led_comm_off ();
  _delay_ms (200.0);
  gpio_led_comm_on ();
  _delay_ms (200.0);
  gpio_led_comm_off ();
  
  /* enter the main idle loop. */
  while (1) {
    /* wait for the user to open the device node with a terminal emulator
     * that sets DTR to indicate its ready to receive data.
     */
    while (!usb_cdc_control_line_dtr ());

    /* discard bytes received prior to receipt of the DTR signal. */
    usb_cdc_flush_input ();

    /* turn on the first led to show connection. */
    gpio_led_comm_on ();

    /* now sit in a wait state for the next byte. */
    while (1) {
      /* try to read a byte from the host. */
      host_command = usb_cdc_getchar ();

      /* was a byte read from the host? */
      if (host_command != -1) {
        /* act based on the host command. */
        switch (host_command) {
          /* transfer shimming word. */
          case PPM_MSG_HOST_RESHIM:
            ppm_reshim ();
            break;

          /* transfer tuning word. */
          case PPM_MSG_HOST_RETUNE:
            ppm_retune ();
            break;

          /* transfer pulse program. */
          case PPM_MSG_HOST_PULPROG:
            ppm_pulprog ();
            break;

          /* execute pulse program. */
          case PPM_MSG_HOST_EXECUTE:
            ppm_execute ();
            break;

          /* reset device. */
          case PPM_MSG_HOST_RESET:
            ppm_reset ();
            break;

          /* read parameters. */
          default:
            break;
        }
      }

      /* see if the usb has been de-configured or if the user disconnected. */
      if (!usb_configured () || !usb_cdc_control_line_dtr ()) break;
    }

    /* turn off the first "connection" led. */
    _delay_ms (50.0);
    gpio_led_comm_off ();
  }

  /* execution never reaches this point. */
  return 0;
}
