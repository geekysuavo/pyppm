
/* ppm-parms.c: source file for handling ppm device parameters.
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

/* include the PPM header. */
#include "ppm.h"

/* ppm_parms_humanize: translates a parameter struct to human-readable. */
int ppm_parms_humanize (ppm_parms *parms) {
  /* translate the values to floating point. */
  parms->f_polarize_ovf = (double) parms->polarize_ovf * 16.384e-3;
  parms->f_acquire_ovf = 1.0e-3 / ((double) parms->acquire_ovf * 62.5e-9);
  parms->f_deadtime_pol = (double) parms->deadtime_pol * 25.0e-3;
  parms->f_deadtime_acq = (double) parms->deadtime_acq * 25.0e-3;
  parms->f_ccs_value = (double) parms->ccs_value * 0.48828125e-3;

  /* return success. */
  return 1;
}

/* ppm_parms_dehumanize: translates a parameter struct to machine-readable. */
int ppm_parms_dehumanize (ppm_parms *parms) {
  /* translate the values to the proper integer sizes. */
  parms->polarize_ovf = (uint16_t) round (parms->f_polarize_ovf / 16.384e-3);
  parms->acquire_ovf = (uint16_t) round (1.6e4 / parms->f_acquire_ovf);
  parms->deadtime_pol = (uint16_t) round (parms->f_deadtime_pol / 25.0e-3);
  parms->deadtime_acq = (uint16_t) round (parms->f_deadtime_acq / 25.0e-3);
  parms->ccs_value = (uint16_t) round (parms->f_ccs_value / 0.48828125e-3);

  /* return success. */
  return 1;
}
