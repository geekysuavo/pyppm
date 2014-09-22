
/* ppm-fall.c: source file for passing around waterfall-plot data.
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

/* ppm_fall_alloc: allocates waterfall arrays. */
int ppm_fall_alloc (ppm_fall *wfall, unsigned int nf, unsigned int nt) {
  /* declare required values. */
  unsigned int i;

  /* set the counts. */
  wfall->nfreq = nf;
  wfall->ntime = nt;

  /* allocate memory for the time and frequency arrays. */
  wfall->t = (double *) calloc (wfall->ntime, sizeof (double));
  wfall->f = (double *) calloc (wfall->nfreq, sizeof (double));

  /* allocate memory for the slices. */
  wfall->v = (double **) malloc (sizeof (double *) * wfall->ntime);

  /* ensure the arrays were allocated. */
  if (!wfall->t || !wfall->f || !wfall->v) {
    /* return failure. */
    return 0;
  }

  /* loop through the slices. */
  for (i = 0; i < wfall->ntime; i++) {
    /* allocate the slice. */
    wfall->v[i] = (double *) calloc (wfall->nfreq, sizeof (double));

    /* ensure the slice was allocated. */
    if (!wfall->v[i]) return 0;
  }

  /* return success. */
  return 1;
}

/* ppm_fall_empty: frees waterfall arrays. */
void ppm_fall_empty (ppm_fall *wfall) {
  /* declare required variables. */
  unsigned int i;

  /* free the time and frequency arrays. */
  free (wfall->t); wfall->t = NULL;
  free (wfall->f); wfall->f = NULL;

  /* free the voltage arrays. */
  for (i = 0; i < wfall->ntime; i++) {
    /* free the array. */
    free (wfall->v[i]);
    wfall->v[i] = NULL;
  }

  /* free the voltage rows. */
  free (wfall->v);
  wfall->v = NULL;

  /* zero the counts. */
  wfall->nfreq = wfall->ntime = 0;
}

/* ppm_fall_write: writes waterfall data to a text file. */
int ppm_fall_write (ppm_fall *wfall, const char *fname) {
  /* declare required variables. */
  unsigned int i, j;
  FILE *fh;

  /* open the file for writing. */
  fh = fopen (fname ? fname : "wf", "wb");

  /* ensure the file was opened. */
  if (!fh) {
    /* output an error. */
    debugf ("failed to open '%s' for writing",
            fname ? fname : "wf");

    /* return an error. */
    return 0;
  }

  /* output the lines of the file. */
  for (i = 0; i < wfall->ntime; i++)
    for (j = 0; j < wfall->nfreq; j++)
      fprintf (fh, "%le %le %le\n", wfall->t[i], wfall->f[j], wfall->v[i][j]);

  /* flush and close the output file. */
  fflush (fh);
  fclose (fh);

  /* return success. */
  return 1;
}
