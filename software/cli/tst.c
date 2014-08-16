
/* tst.c: source file for the command-line general device test routine.
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

/* declare testmode strings. */
#define PPM_TESTSTRING_DAC  "dac"
#define PPM_TESTSTRING_CCS  "ccs"
#define PPM_TESTSTRING_SCB  "scb"

/* main: application entry point. */
int main (int argc, char **argv) {
  /* ensure the correct number of arguments was passed. */
  if (argc != 2) {
    /* output an error message and return an error. */
    fprintf (stderr, "error: invalid number of arguments\n");
    return 1;
  }

  /* determine which feature to run a test for. */
  if (strcmp (argv[1], PPM_TESTSTRING_DAC) == 0) {
    /* test the dac on the device. */
    if (!ppm_dactest (NULL)) {
      /* output an error message. */
      fprintf (stderr, "error: tst[%s] failed\n", argv[1]);
      return 1;
    }
  }
  if (strcmp (argv[1], PPM_TESTSTRING_SCB) == 0) {
    /* test the scb outputs on the device. */
    if (!ppm_scbtest (NULL)) {
      /* output an error message. */
      fprintf (stderr, "error: tst[%s] failed\n", argv[1]);
      return 1;
    }
  }
  else {
    /* output an error message. */
    fprintf (stderr, "error: tst failed with invalid mode '%s'\n", argv[1]);
    return 1;
  }

  /* return success. */
  return 0;
}
