
/* wpp.c: source file for the command-line pulprog write routine.
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
  ppm_prog pp;
  FILE *fh;

  /* initialize the program size. */
  pp.n = 0;

  /* determine if an input file was supplied. */
  if (argc >= 2) {
    /* open the input file. */
    fh = fopen (argv[1], "rb");

    /* check if the open was successful. */
    if (!fh) {
      /* output an error and return failure. */
      fprintf (stderr, "error: failed to open '%s'\n", argv[1]);
      return 1;
    }

    /* load the hex contents of the pulse program from the file. */
    ppm_prog_read (fh, &pp);

    /* close the input file. */
    fclose (fh);
  }
  else {
    /* load the hex contents of the pulse program from standard input. */
    ppm_prog_read (stdin, &pp);
  }

  /* write the pulse program to the device. */
  if (!ppm_wpp (NULL, &pp)) {
    /* output an error. */
    fprintf (stderr, "error: wpp failed\n");

    /* return an error. */
    return 1;
  }

  /* return success. */
  return 0;
}
