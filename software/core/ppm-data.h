
/* ppm-data.h: header for passing around acquisition data.
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
#ifndef __PPM_DATA_H__
#define __PPM_DATA_H__

/* include the PPM header. */
#include "ppm.h"

/* define the number of characters used in line parsing. */
#define N_BUF 1024

/* define a structure for holding acquired data. */
typedef struct ppm_data_t {
  /* store the length of the arrays. */
  unsigned int n;

  /* store the time/frequency and voltage values. */
  double *x, *v;
} ppm_data;

/* function delarations. */

int ppm_data_alloc (ppm_data *acq, unsigned int n);

void ppm_data_empty (ppm_data *acq);

int ppm_data_read (ppm_data *acq, const char *fname);

int ppm_data_write (ppm_data *acq, const char *fname);

#ifdef HAVE_SNDFILE
int ppm_data_write_audio (ppm_data *acq, const char *fname);
#endif

/* ensure proper inclusion. */
#endif
