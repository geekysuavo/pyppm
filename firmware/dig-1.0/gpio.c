
/* gpio.c: CCU firmware general purpose input/output source code.
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

/* gpio_init: initialize general purpose input/output pins. */
void gpio_init (void) {
  /* initialize the PORTB data direction register. */
  DDRB |= ((1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB4));
  DDRB &= ~((1 << DDB3) | (1 << DDB5) | (1 << DDB6) | (1 << DDB7));

  /* initialize the PORTB outputs: ADC /CS pin is set high. */
  PORTB = (1 << PORTB4);

  /* initialize all PORTC GPIO to hi-z. */
  DDRC = 0;
  PORTC = 0;

  /* initialize all PORTD GPIO to hi-z. */
  DDRD = 0;
  PORTD = 0;
}
