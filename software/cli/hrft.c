
/* hrft.c: source file for the command-line high-res fourier transform routine.
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

/* main: application entry point. */
int main (int argc, char **argv) {
  /* declare required variables. */
  ppm_data tacq, facq;
  double f1, f2;

  /* ensure the proper number of arguments was given. */
  if (argc != 3) {
    /* output an error message and return an error. */
    fprintf (stderr, "error: invalid argument count (%d != 3)\n", argc);
    return 1;
  }

  /* read in the frequency arguments. */
  f1 = atof (argv[1]);
  f2 = atof (argv[2]);

  /* open the fid file. */
  if (!ppm_data_read (&tacq, "fid")) {
    /* output an error. */
    fprintf (stderr, "error: failed to open fid\n");
    return 1;
  }

  /* frequency-transform the data. */
  if (!ppm_hrft (&tacq, &facq, f1, f2)) {
    /* output an error. */
    fprintf (stderr, "error: failed to frequency-transform data\n");

    /* return an error. */
    return 1;
  }

  /* write the transformed data to the default filename. */
  if (!ppm_data_write (&facq, "spect")) {
    /* output an error and return an error. */
    fprintf (stderr, "error: write failed\n");
    return 1;
  }

  /* free the data allocated. */
  ppm_data_empty (&tacq);
  ppm_data_empty (&facq);

  /* return success. */
  return 0;
}
