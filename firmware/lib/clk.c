
/* clk.c: PPM firmware clocking source code.
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

/* include the clocking header. */
#include "clk.h"

/* clk_init: initialize the clock sources and prescalers. */
void clk_init (void) {
  /* configure the system clock prescaler for full frequency operation. */
  CLKPR = (1 << CLKPCE);
  CLKPR = 0;
}

/* pll_init: initialize the usb clock phase-locked loop. */
void pll_init (void) {
  /* configure the PLL prescaler to generate an 8.0 MHz clock. */
  PLLCSR |= (1 << PINDIV);

  /* enable the PLL. */
  PLLCSR |= (1 << PLLE);

  /* wait for the PLL to achieve lock. */
  while (!(PLLCSR & (1 << PLOCK)));
}
