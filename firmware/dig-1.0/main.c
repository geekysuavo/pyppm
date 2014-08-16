
/* main.c: CCU firmware main source code.
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
  /* disable interrupts during initialization. the usb_init() function
   * will re-enable interrupts once it has enabled the usb controller.
   */
  cli ();

  /* initialize CCU components. */
  clk_init ();
  tcnt_init ();
  gpio_init ();
  spi_init ();
  usb_init ();

  /* wait for the usb host to configure the device. */
  while (!usb_configured ());

  /* delay for a second. */
  _delay_ms(1000);
  
  /* enter the main idle loop. */
  while (1) {
    /* wait for the user to open the device node with a terminal emulator
     * that sets DTR to indicate its ready to receive data.
     */
    while (!usb_cdc_control_line_dtr ());

    /* discard bytes received prior to receipt of the DTR signal. */
    usb_cdc_flush_input ();

    /* start a capture round. */
    tcnt1_start ();

    /* now sit in a wait state. */
    while (1) {
      /* see if the usb has been de-configured or if the user disconnected. */
      if (!usb_configured () || !usb_cdc_control_line_dtr ()) break;
    }
  }

  /* execution never reaches this point. */
  return 0;
}
