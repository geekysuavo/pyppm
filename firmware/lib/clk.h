
/* clk.h: PPM firmware clocking header.
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

/* include required avr headers. */
#include <avr/io.h>

/* ensure proper inclusion. */
#ifndef __PPM_CLK_H__
#define __PPM_CLK_H__

/* redefine the PINDIV bit in PLLCSR to match the m32u2 datasheet.
 * m32u2 datasheet, sec 8.11.6, page 40.
 * m32u4 datasheet, sec 6.10.2, page 40.
 */
#ifndef _AVR_IOM32U4_H_
#define PINDIV PLLP0
#endif

/* begin function declarations: */

void clk_init (void);

void pll_init (void);

#endif /* __PPM_CLK_H__ */
