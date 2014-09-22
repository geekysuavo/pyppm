
/* ppm-data.c: source file for passing around acquisition data.
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

/* ppm_data_alloc: allocates acquisition data arrays. */
int ppm_data_alloc (ppm_data *acq, unsigned int n) {
  /* check if the data arrays are allocated to the right size. */
  if (acq->n == n) {
    /* zero the arrays. */
    memset (acq->x, 0, acq->n * sizeof (double));
    memset (acq->v, 0, acq->n * sizeof (double));
  }

  /* set the value count. */
  acq->n = n;

  /* allocate memory for the output values. */
  acq->x = (double *) calloc (acq->n, sizeof (double));
  acq->v = (double *) calloc (acq->n, sizeof (double));

  /* ensure the arrays were allocated. */
  if (!acq->x || !acq->v) {
    /* return failure. */
    return 0;
  }

  /* return success. */
  return 1;
}

/* ppm_data_empty: frees acquired data arrays. */
void ppm_data_empty (ppm_data *acq) {
  /* free the arrays. */
  if (acq->n && acq->x && acq->v) {
    /* only if they're allocated. */
    free (acq->x); acq->x = NULL;
    free (acq->v); acq->v = NULL;
  }

  /* zero the count. */
  acq->n = 0;
}

/* ppm_data_read: reads acquired data from a text file. */
int ppm_data_read (ppm_data *acq, const char *fname) {
  /* declare required variables. */
  char buf[N_BUF];
  double x, v;
  FILE *fh;

  /* open the file for input. */
  fh = fopen (fname ? fname : "data", "rb");

  /* ensure the file was opened. */
  if (!fh) {
    /* output an error message. */
    debugf ("failed to open '%s' for reading",
            fname ? fname : "data");

    /* return an error. */
    return 0;
  }

  /* forcefully and rudely empty the data structure. */
  ppm_data_empty (acq);

  /* read in the values from the file. */
  while (!feof (fh)) {
    /* read a new line from the file. */
    if (fgets (buf, N_BUF, fh)) {
      /* parse the read line into a pair of values. */
      if (sscanf (buf, "%le %le ", &x, &v) == 2) {
        /* increment the array size. */
        acq->n++;

        /* resize the arrays. */
        if (acq->n == 1) {
          /* allocate new arrays. */
          acq->x = (double *) malloc (sizeof (double));
          acq->v = (double *) malloc (sizeof (double));
        }
        else {
          /* reallocate the arrays. */
          acq->x = (double *) realloc (acq->x, sizeof (double) * acq->n);
          acq->v = (double *) realloc (acq->v, sizeof (double) * acq->n);
        }

        /* ensure the arrays were (re)allocated. */
        if (!acq->x || !acq->v) {
          /* output an error message. */
          debugf ("failed to allocate data matrices");
          return 0;
        }

        /* store the new pair of values. */
        acq->x[acq->n - 1] = x;
        acq->v[acq->n - 1] = v;
      }
    }
  }

  /* close the input file. */
  fclose (fh);

  /* return success. */
  return 1;
}

/* ppm_data_write: writes acquired data to a text file. */
int ppm_data_write (ppm_data *acq, const char *fname) {
  /* declare required variables. */
  unsigned int i;
  FILE *fh;

  /* open the file for output. */
  fh = fopen (fname ? fname : "data", "wb");

  /* ensure the file was opened. */
  if (!fh) {
    /* output an error. */
    debugf ("failed to open '%s' for writing",
            fname ? fname : "data");

    /* return an error. */
    return 0;
  }

  /* output the lines of the file. */
  for (i = 0; i < acq->n; i++)
    fprintf (fh, "%le %le\n", acq->x[i], acq->v[i]);

  /* flush and close the output file. */
  fflush (fh);
  fclose (fh);

  /* return success. */
  return 1;
}

/* ppm_data_write_audio: writes acquired data to a sound file. */
#ifdef HAVE_SNDFILE
int ppm_data_write_audio (ppm_data *acq, const char *fname) {
  /* declare required variables. */
  sf_count_t count, n;
  unsigned int i;
  SNDFILE *sfh;
  SF_INFO sfi;
  int srate;

  /* calculate the sample rate from the time points. */
  srate = 1.0 / (acq->x[1] - acq->x[0]);

  /* set the desired output sound file parameters. */
  sfi.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
  sfi.frames = 0x7FFFFFFF;
  sfi.samplerate = srate;
  sfi.channels = 1;

  /* open a sound file for writing. */
  sfh = sf_open (fname, SFM_WRITE, &sfi);

  /* see if the file was opened. */
  if (!sfh) {
    /* output an error message. */
    debugf ("failed to open '%s': %s",
            fname, sf_strerror (NULL));

    /* return failure. */
    return 0;
  }

  /* get the number of samples to write out. */
  n = acq->n;

  /* temporarily down-scale the fid data to different bounds. */
  for (i = 0; i < acq->n; i++) acq->v[i] /= 2.5;

  /* try to write the output data. */
  count = sf_write_double (sfh, acq->v, n);

  /* check if the number of output bytes matches the desired number. */
  if (count != n) {
    /* output an error message. */
    debugf ("failed to write all bytes (%ld != %ld)",
            (long) count, (long) n);

    /* re-scale the fid data to the original bounds. */
    for (i = 0; i < acq->n; i++) acq->v[i] *= 2.5;

    /* return an error. */
    return 0;
  }

  /* sync the output buffers to the file. */
  sf_write_sync (sfh);

  /* close the file. */
  sf_close (sfh);

  /* re-scale the fid data to the original bounds. */
  for (i = 0; i < acq->n; i++) acq->v[i] *= 2.5;

  /* return success. */
  return 1;
}
#endif
