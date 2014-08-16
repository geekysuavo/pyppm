
/* tcnt.h: CCU firmware timer/counter peripherals header.
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

/* ensure proper inclusion. */
#ifndef __CCU_TCNT_H__
#define __CCU_TCNT_H__

/* begin function declarations: */

void tcnt_init (void);

void tcnt0_start (void);

void tcnt1_start (void);

#endif /* __CCU_TCNT_H__ */
