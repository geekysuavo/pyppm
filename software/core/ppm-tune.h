
/* ppm-tune.h: header for managing capacitive tuning functions.
 * Copyright (C) 2014  Bradley Worley  <geekysuavo@gmail.com>.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to:
 *
 *   Free Software Foundation, Inc.
 *   51 Franklin Street, Fifth Floor
 *   Boston, MA  02110-1301, USA.
 */

/* ensure proper inclusion. */
#ifndef __PPM_TUNE_H__
#define __PPM_TUNE_H__

/* include the PPM header. */
#include "ppm.h"

/* function delarations. */

double ppm_tune_capacitance (uint16_t wd);

uint16_t ppm_tune_optimize (double f0, double L);

/* ensure proper inclusion. */
#endif
