
/* rparh.c: source file for the command-line humanized parms read routine.
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

/* declare the output format string. */
#define RPARH_FORMAT "\n\
  Polarization time\t%.3lf s\n\
  Acquisition rate\t%.3lf kS/s\n\
  Sample points\t\t%u\n\
  Polarization deadtime\t%.3lf ms\n\
  Acquisition deadtime\t%.3lf ms\n\
  Polarization current\t%.3lf A\n\
"

/* main: application entry point. */
int main (int argc, char **argv) {
  /* declare required variables. */
  ppm_parms parms;

  /* read the acquisition parms. */
  if (!ppm_rpar (NULL, &parms)) {
    /* output an error message. */
    fprintf (stderr, "error: rpar failed\n");

    /* return an error. */
    return 1;
  }

  /* humanize the parameters. */
  if (!ppm_parms_humanize (&parms)) {
    /* output an error message. */
    fprintf (stderr, "error: humanize failed\n");

    /* return an error. */
    return 1;
  }

  /* output human-readable parameters. */
  fprintf (stdout, RPARH_FORMAT,
    parms.f_polarize_ovf,
    parms.f_acquire_ovf,
    parms.acquire_count,
    parms.f_deadtime_pol,
    parms.f_deadtime_acq,
    parms.f_ccs_value);

  /* return success. */
  return 0;
}
