
/* ppm-tune.c: source file for managing capacitive tuning functions.
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

/* define the array of tuning capacitance values, as specified in the
 * schematic and bill of materials in: designs/ppm-2.1
 */
static const double Cv[16] = {
  150.0e-12, 330.0e-12, 680.0e-12,   1.5e-9,
    3.3e-9,    6.8e-9,   15.0e-9,   33.0e-9,
   68.0e-9,  100.0e-9,  220.0e-9,  330.0e-9,
  680.0e-9,    1.0e-6,    2.2e-6,    4.7e-6
};

/* ppm_tune_capacitance: computes the total capacitance of the tuning bank
 * when a given tuning word is provided to the switches.
 */
double ppm_tune_capacitance (uint16_t wd) {
  /* declare required variables. */
  double C;
  int i;

  /* initialize the output capacitance. */
  C = 0.0;

  /* loop through the tuning word bits, adding the indexed capacitance
   * to the total whenever the bit is flipped high in the tuning word.
   */
  for (i = 0; i < 16; i++)
    if (wd & (1 << i))
      C += Cv[i];

  /* return the total capacitance. */
  return C;
}

/* ppm_tune_optimize: performs a binary search over the available tuning
 * word values to identify the tuning word that most nearly yields the
 * desired resonance frequency.
 */
uint16_t ppm_tune_optimize (double f0, double L) {
  /* declare required variables. */
  double C, C0;
  uint16_t wd;
  int i;

  /* initialize the output word. */
  wd = 0x0000;

  /* return a zero output word (all switches open) if a zero frequency was
   * provided.
   */
  if (f0 == 0.0)
    return wd;

  /* compute the desired capacitance value. */
  C0 = 1.0 / (4.0 * M_PI * M_PI * f0 * f0 * L);

  /* loop backwards through the bits of the word. */
  for (i = 15; i >= 0; i--) {
    /* compute the capacitance. */
    C = ppm_tune_capacitance (wd | (1 << i));

    /* check if the current bit should be flipped. */
    if (C <= C0)
      wd |= (1 << i);
  }

  /* return the optimized word. */
  return wd;
}

