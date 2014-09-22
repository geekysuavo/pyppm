
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

/* include the message handling source code. */
#include "../lib/msg.c"

/* main: firmware execution entry point. */
int main (void) {
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
  tcnt_init ();
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
  msg_loop ();

  /* execution never reaches this point. */
  return 0;
}
