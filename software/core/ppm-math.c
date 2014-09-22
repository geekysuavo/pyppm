
/* ppm-math.c: source file for mathematical routines used with ppm devices.
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

/* fft: run a fast fourier transformation. */
void fft (double *x, unsigned int n) {
  /* declare required variables. */
  double *y, ctemp[2], w[2], resi[2], resik[2];
  int i, j, k, m, step;
  double theta, temp;

  /* allocate memory for the imaginary component. */
  y = (double *) calloc (n, sizeof (double));

  for (i = 0, j = 0; i < n - 1; i++) {
    if (j > i) {
      SWAP (x[j], x[i]);
      SWAP (y[j], y[i]);
    }

    m = n >> 1;

    while (m <= j) {
      j -= m;
      m >>= 1;
    }

    j += m;
  }

  /* initialize prior to recursion. */
  k = 1;

  do {
    step = 2 * k;

    for (m = 0; m < k; m++) {
      /* calculate the twiddle factor angle. */
      theta = M_PI * (double) m / (double) k;

      /* compute the twiddle factor. */
      w[RE] = cos (theta);
      w[IM] = sin (theta);

      for (i = m; i < n; i += step) {
        resik[RE] = x[i + k];
        resik[IM] = y[i + k];

        resi[RE] = x[i];
        resi[IM] = y[i];

        ctemp[RE] = w[RE] * resik[RE] - w[IM] * resik[IM];
        ctemp[IM] = w[IM] * resik[RE] + w[RE] * resik[IM];

        resik[RE] = resi[RE] - ctemp[RE];
        resik[IM] = resi[IM] - ctemp[IM];

        resi[RE] += ctemp[RE];
        resi[IM] += ctemp[IM];

        x[i + k] = resik[RE];
        y[i + k] = resik[IM];

        x[i] = resi[RE];
        y[i] = resi[IM];
      }
    }
    k = step;
  }
  while (k < n);

  /* calculate the absolute value of the complex data. */
  for (i = 0; i < n; i++)
    x[i] = 2.0 * sqrt (x[i] * x[i] + y[i] * y[i]) / (double) n;

  /* free the imaginary component vector. */
  free (y);
  y = NULL;
}

/* hrft: high-resolution fourier transform of real data. */
int hrft (double *x, unsigned int n, double *y, unsigned int nft,
          double dt, double f1, double f2) {
  /* declare required variables. */
  double c0, s0, ci, si, T0, T1, T2, U0, U1, U2;
  double f, df, w, dw;
  unsigned int i, j;
  double *re, *im;

  /* allocate memory for the temporary storage arrays. */
  re = (double *) calloc (nft, sizeof (double));
  im = (double *) calloc (nft, sizeof (double));

  /* ensure the arrays were allocated. */
  if (!re || !im) {
    /* print an error and return failure. */
    debugf ("failed to allocate hrft arrays");
    return 0;
  }

  /* calculate the frequency values. */
  f = f1;
  df = (f2 - f1) / ((double) (nft - 1));
  w = 2.0 * M_PI * f * dt;
  dw = 2.0 * M_PI * df * dt;

  /* loop through the frequencies of interest. */
  for (i = 0; i < nft; i++) {
    /* initialize the coefficients. */
    c0 = cos (w);
    s0 = sin (w);
    T0 = 1.0;
    T1 = c0;
    U0 = 1.0;
    U1 = 2.0 * c0;

    /* calculate the first two coefficients. */
    re[i] = x[0] + x[1] * T1;
    im[i] = x[1] * s0 * U0;

    /* loop through the remaining points. */
    c0 *= 2.0;
    for (j = 2; j < n; j++) {
      /* calculate the next type 1 coefficients. */
      T2 = c0 * T1 - T0;
      T0 = T1;
      T1 = T2;

      /* and the next type 2 coefficients. */
      U2 = c0 * U1 - U0;
      U0 = U1;
      U1 = U2;

      /* calculate the next sin/cos factors. */
      ci = T2;
      si = s0 * U0;

      /* calculate the next transformed points. */
      re[i] += x[j] * ci;
      im[i] += x[j] * si;
    }

    /* increment the angular frequency. */
    w += dw;
  }

  /* loop through the transformed points and store the values. */
  for (i = 0; i < nft; i++)
    y[i] = 4.0 * sqrt (re[i] * re[i] + im[i] * im[i]) / (double) n;

  /* free the temporary arrays. */
  free (re); re = NULL;
  free (im); im = NULL;

  /* return success. */
  return 1;
}

/* ppm_fft: fast fourier transform an acquired time-domain signal. */
int ppm_fft (ppm_data *tacq, ppm_data *facq) {
  /* declare required variables. */
  double *re;
  int i, N;

  /* loop until we find the next power of two. */
  for (N = 2; N < tacq->n; N *= 2);

  /* allocate memory for the final calculated values. */
  if (!ppm_data_alloc (facq, N >> 1)) {
    /* output an error. */
    debugf ("failed to allocate acquisition structure");

    /* return an error. */
    return 0;
  }

  /* allocate temporary arrays for the complex fft data. */
  re = (double *) malloc (sizeof (double) * N);

  /* ensure the temporary array was created. */
  if (!re) {
    /* output an error. */
    debugf ("failed to allocate temporary fft array");

    /* free the final calculated values. */
    ppm_data_empty (facq);

    /* return an error. */
    return 0;
  }

  /* copy the input data over into the arrays. */
  for (i = 0; i < N; i++) {
    /* copy over the real component, zero-padding the end. */
    if (i < tacq->n)
      re[i] = tacq->v[i];
    else
      re[i] = 0.0;
  }

  /* fourier transform the real data. */
  fft (re, N);

  /* loop through the output values. */
  for (i = 0; i < facq->n; i++) {
    /* calculate the frequency axis value. */
    facq->x[i] = (((double) i) / ((double) facq->n))
               / (2.0 * (tacq->x[1] - tacq->x[0]));

    /* calculate the intensity value. */
    facq->v[i] = re[i];
  }

  /* zero out the dc component point. */
  facq->x[0] = facq->v[0] = 0.0;

  /* free the temporary array. */
  free (re); re = NULL;

  /* return success. */
  return 1;
}

/* ppm_hrft: high-res fourier transform a frequency region of TD data. */
int ppm_hrft (ppm_data *tacq, ppm_data *facq, double f1, double f2) {
  /* declare required variables. */
  unsigned int i, nft;
  double w, *wx;

  /* calculate the number of output values. */
  nft = tacq->n < PPM_HRFT_N ? tacq->n : PPM_HRFT_N;

  /* allocate memory for the final calculated values. */
  if (!ppm_data_alloc (facq, nft)) {
    /* output an error. */
    debugf ("failed to allocate acquisition structure");

    /* return an error. */
    return 0;
  }

  /* allocate a temporary array for weighted time-domain data. */
  wx = (double *) calloc (tacq->n, sizeof (double));

  /* ensure the temp array was allocated. */
  if (!wx) {
    /* output an error and fail. */
    debugf ("failed to allocated temporary array");
    return 0;
  }

  /* copy the time-domain data into the temporary array. */
  for (i = 0; i < tacq->n; i++) {
    /* calculate the current blackman weight value. */
    w = 0.42 - 0.5 * cos (2.0 * M_PI * (double) i / (double) tacq->n)
             + 0.08 * cos (4.0 * M_PI * (double) i / (double) tacq->n);

    /* copy the currently indexed voltage. */
    wx[i] = w * tacq->v[i];
  }

  /* calculate the frequency values. */
  for (i = 0; i < nft; i++)
    facq->x[i] = f1 + (f2 - f1) * (((double) (i + 1)) / ((double) nft));

  /* perform the fourier transform. */
  hrft (wx, tacq->n, facq->v, facq->n, tacq->x[1] - tacq->x[0], f1, f2);

  /* return success. */
  return 1;
}

/* ppm_wfall: generates a waterfall-style matrix of time-domain data. */
int ppm_wfall (ppm_data *tacq, ppm_fall *wfall) {
  /* declare required variables. */
  int i, offs, j, nf, nt, incr;
  double *re, *w;

  /* set some 'constants'. */
  nf = 1024;
  incr = 64;

  /* calculate the number of time slices in the waterfall. */
  nt = (unsigned int)
    floor (((double) tacq->n - 2.0 * (double) nf) / (double) incr);

  /* allocate memory for the final calculated values. */
  if (!ppm_fall_alloc (wfall, nf, nt)) {
    /* output an error. */
    debugf ("failed to allocate waterfall structure");

    /* return an error. */
    return 0;
  }

  /* allocate temporary arrays for the complex fft data. */
  re = (double *) malloc (sizeof (double) * 2 * nf);
  w = (double *) malloc (sizeof (double) * 2 * nf);

  /* ensure the temporary array was created. */
  if (!re || !w) {
    /* output an error. */
    debugf ("failed to allocate temporary fft array");

    /* free the final calculated values. */
    ppm_fall_empty (wfall);

    /* return an error. */
    return 0;
  }

  /* copy the input data over into the weight array. */
  for (i = 0; i < 2 * nf; i++) {
    /* set the weight value. */
    w[i] = 0.42 - 0.5 * cos (M_PI * (double) i / (double) nf)
                + 0.08 * cos (2.0 * M_PI * (double) i / (double) nf);
  }

  /* loop through the time slices. */
  for (i = 0, offs = 0; i < nt; i++, offs += incr) {
    /* and through the frequency slices. */
    for (j = 0; j < 2 * nf; j++)
      re[j] = w[j] * tacq->v[offs + j];

    /* fourier transform the slice. */
    fft (re, 2 * nf);

    /* store the frequency slice. */
    for (j = 0; j < nf; j++)
      wfall->v[i][j] = re[j];
  }

  /* calculate the time values. */
  for (i = 0; i < nt; i++)
    wfall->t[i] = tacq->x[tacq->n - 1] * (double) i / (double) nt;

  /* calculate the frequency values. */
  for (i = 0; i < nf; i++)
    wfall->f[i] = (((double) i) / ((double) nf))
                / (2.0 * (tacq->x[1] - tacq->x[0]));

  /* free the temporary arrays. */
  free (re); re = NULL;
  free (w); w = NULL;

  /* return success. */
  return 1;
}
