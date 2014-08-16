
/* ppm-fall.h: header for passing around waterfall-plot data.
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
#ifndef __PPM_FALL_H__
#define __PPM_FALL_H__

/* include the PPM header. */
#include "ppm.h"

/* define a structure for holding waterfall data. */
typedef struct ppm_fall_t {
  /* store the size of the matrix. */
  unsigned int nfreq, ntime;

  /* store the time, frequency, and voltage values. */
  double *t, *f, **v;
} ppm_fall;

/* function delarations. */

int ppm_fall_alloc (ppm_fall *wfall, unsigned int nr, unsigned int nc);

void ppm_fall_empty (ppm_fall *wfall);

int ppm_fall_write (ppm_fall *wfall, const char *fname);

/* ensure proper inclusion. */
#endif
