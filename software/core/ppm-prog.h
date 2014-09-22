
/* ppm-prog.h: header for passing around pulse programs.
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
#ifndef __PPM_PROG_H__
#define __PPM_PROG_H__

/* include the PPM header. */
#include "ppm.h"

/* define a structure for holding pulse programs. */
typedef struct ppm_prog_t {
  /* underlying byte array that mirrors that of the device. */
  uint8_t *bytes;
  unsigned int n;
} ppm_prog;

/* function delarations. */

int ppm_prog_alloc (ppm_prog *pp, unsigned int n);

void ppm_prog_empty (ppm_prog *pp);

unsigned int ppm_prog_samples (ppm_prog *pp);

void ppm_prog_timings (ppm_prog *pp, ppm_data *acq);

void ppm_prog_read (FILE *fh, ppm_prog *pp);

void ppm_prog_write (FILE *fh, ppm_prog *pp);

/* ensure proper inclusion. */
#endif
