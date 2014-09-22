
/* scb.h: SCB firmware main controller header.
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
#ifndef __PPM_SCB_H__
#define __PPM_SCB_H__

/* begin function declarations: */

void ppm_done (void);

void ppm_error (void);

void ppm_version (void);

void ppm_reset (void);

void ppm_execute (void);

void ppm_pulprog (void);

#endif /* __PPM_SCB_H__ */
