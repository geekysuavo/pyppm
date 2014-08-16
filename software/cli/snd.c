
/* snd.c: source file for the command-line sound creator routine.
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
  ppm_data tacq;

  /* open the fid file. */
  if (!ppm_data_read (&tacq, "fid")) {
    /* output an error. */
    fprintf (stderr, "error: failed to open fid\n");
    return 1;
  }

  /* try to write the fid data to a sound file. */
  if (!ppm_data_write_audio (&tacq, "fid.wav")) {
    /* output an error. */
    fprintf (stderr, "error: failed to write fid.wav\n");
    return 1;
  }

  /* free the data allocated. */
  ppm_data_empty (&tacq);

  /* return success. */
  return 0;
}
