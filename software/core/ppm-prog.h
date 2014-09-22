
/* ppm-prog.h: header for passing around pulse programs.
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
#ifndef __PPM_PROG_H__
#define __PPM_PROG_H__

/* include the PPM header. */
#include "ppm.h"

/* define all pulse program element strings. */
#define PPM_PULPROG_STR_DEADTIME  "deadtime"
#define PPM_PULPROG_STR_DELAY     "delay"
#define PPM_PULPROG_STR_POLARIZE  "polarize"
#define PPM_PULPROG_STR_RELAY     "relay"
#define PPM_PULPROG_STR_ACQUIRE   "acquire"
#define PPM_PULPROG_STR_TXRISE    "tx-rise"
#define PPM_PULPROG_STR_TXFALL    "tx-fall"
#define PPM_PULPROG_STR_TXPULSE   "tx-pulse"
#define PPM_PULPROG_STR_TUNE      "tune"
#define PPM_PULPROG_STR_SHIM_X    "shim-x"
#define PPM_PULPROG_STR_SHIM_Y    "shim-y"
#define PPM_PULPROG_STR_SHIM_Z    "shim-z"
#define PPM_PULPROG_STR_END       "end"

/* define a structure for holding pulse programs. */
typedef struct ppm_prog_t {
  /* underlying byte array that mirrors that of the device. */
  uint8_t *bytes;
  unsigned int n;
} ppm_prog;

/* define a pulse program iterator callback function. */
typedef void (* ppm_prog_iterator) (uint8_t cmd, uint8_t *args, void *pdata);

/* function delarations. */

int ppm_prog_alloc (ppm_prog *pp, unsigned int n);

void ppm_prog_empty (ppm_prog *pp);

void ppm_prog_zero (ppm_prog *pp);

unsigned int ppm_prog_id_from_string (const char *str);

void ppm_prog_iterate (ppm_prog *pp, ppm_prog_iterator fn, void *pdata);

unsigned int ppm_prog_samples (ppm_prog *pp);

void ppm_prog_timings (ppm_prog *pp, ppm_data *acq);

void ppm_prog_add_deadtime (ppm_prog *pp, unsigned int *idx, double ms);

void ppm_prog_add_delay (ppm_prog *pp, unsigned int *idx, double s);

void ppm_prog_add_polarize (ppm_prog *pp, unsigned int *idx, long en);

void ppm_prog_add_relay (ppm_prog *pp, unsigned int *idx, long en);

void ppm_prog_add_acquire (ppm_prog *pp, unsigned int *idx,
                           long n, double rate);

void ppm_prog_add_txedge (ppm_prog *pp, unsigned int *idx, uint8_t cmd,
                          double ms, double ampl);

void ppm_prog_add_txpulse (ppm_prog *pp, unsigned int *idx,
                           double t, double f, double ampl);

void ppm_prog_add_tune (ppm_prog *pp, unsigned int *idx, double f);

void ppm_prog_add_shim (ppm_prog *pp, unsigned int *idx, uint8_t cmd,
                        double s);

void ppm_prog_read (FILE *fh, ppm_prog *pp);

void ppm_prog_write (FILE *fh, ppm_prog *pp);

/* ensure proper inclusion. */
#endif
