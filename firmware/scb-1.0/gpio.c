
/* gpio.c: SCB firmware general purpose input/output source code.
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

/* gpio_init: initialize general purpose input/output pins. */
void gpio_init (void) {
  /* initialize all PORTB GPIO to hi-z. */
  DDRB = 0;
  PORTB = 0;

  /* initialize all PORTC GPIO to hi-z. */
  DDRC = 0;
  PORTC = 0;

  /* initialize all PORTD GPIO to hi-z. */
  DDRD = 0;
  PORTD = 0;

  /* initialize the spi digital-to-analog converter. */
  gpio_dac_init ();

  /* initialize the high current regulator. */
  gpio_ldo_init ();

  /* initialize the led indicator. */
  gpio_led_init ();

  /* signal that general purpose i/o has initialized by turning on the
   * indicator led.
   */
  gpio_led_comm_on ();
}
