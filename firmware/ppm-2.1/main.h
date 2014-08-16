
/* main.h: PPM firmware main header.
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
#include <avr/wdt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

/* include required PPM headers. */
#include "ppm.h"
#include "../lib/clk.h"
#include "../lib/usb.h"
#include "../lib/macros.h"
#include "../lib/msg.h"
#include "tcnt.h"
#include "gpio.h"
#include "spi.h"
