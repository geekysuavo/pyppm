
/* ppm-math.h: header for mathematical routines used with ppm devices.
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

/* ensure proper inclusion. */
#ifndef __PPM_MATH_H__
#define __PPM_MATH_H__

/* include the PPM header. */
#include "ppm.h"

/* define the number of output values for the HRFT algorithm. */
#define PPM_HRFT_N 2048

/* define a swap function, mainly used in the fft code. */
#define SWAP(a,b) temp = (a); (a) = (b); (b) = temp;

/* define names for real/imaginary indices in 'complex' arrays. */
#define RE 0
#define IM 1

/* function delarations. */

int ppm_fft (ppm_data *tacq, ppm_data *facq);

int ppm_hrft (ppm_data *tacq, ppm_data *facq, double f1, double f2);

int ppm_wfall (ppm_data *tacq, ppm_fall *wfall);

/* ensure proper inclusion. */
#endif
