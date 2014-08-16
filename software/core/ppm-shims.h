
/* ppm-shims.h: header for handling ppm device shim values.
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
#ifndef __PPM_SHIMS_H__
#define __PPM_SHIMS_H__

/* include the PPM header. */
#include "ppm.h"

/* define a structure for holding shim values. */
typedef struct ppm_shims_t {
  /* shim current set-point values. */
  uint16_t x, y, z;

  /* human-readable versions of the above parameters. */
  double f_x, f_y, f_z;  /* [-1, +1] */
} ppm_shims;

/* function delarations. */

int ppm_shims_humanize (ppm_shims *shims);

int ppm_shims_dehumanize (ppm_shims *shims);

/* ensure proper inclusion. */
#endif
