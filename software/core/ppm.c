
/* ppm.c: core source file for interfacing to ppm devices.
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

/* debugf_fn: core function to print a failure/debug message.
 */
void debugf_fn (const char *f, const unsigned int l,
                const char *format, ...) {
  /* declare required variables. */
  va_list vl;

  /* initialize the arguments list. */
  va_start (vl, format);

  /* print the final output string. */
#ifdef PPM_CORE_VERBOSE
  char str[1024];
  vsnprintf (str, 1024, format, vl);
  fprintf (stderr, "%s:%u: %s\n", f, l, str);
  fflush (stderr);
#endif

  /* de-initialize the arguments list. */
  va_end (vl);
}

