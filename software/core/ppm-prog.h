
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

/* define all pulse program hardware <==> software conversion factors below.
 */

/* conversion factor: short, imprecise delay.
 *  - (factor is expressed in milliseconds)
 *  - t_DELAY = 4 * 100 / f_CPU * n
 *            = 4 * 100 / 16 MHz * n
 *            = 4 * 100 * 62.5 ns * n
 *            = 25.0 us * n
 */
#define PPM_PULPROG_F_DT_DEADTIME  25.0e-3

/* conversion factor: precise delay.
 *  - t_DELAY = 256 * n / f_TIMER
 *            = 256 * n / (f_CPU / k_TCNT0)
 *            = 256 * n / (16 MHz / 64)
 *            = 256 * n / 250 kHz
 *            = 1.024 ms * n
 */
#define PPM_PULPROG_F_DT_DELAY     1.024e-3

/* conversion factor: acquisition sample rate.
 *  - (n is the overflow register value)
 *  - (f_ACQ is expressed in kHz)
 *  - f_ACQ = f_CPU / n
 *          = 16 MHz / n
 *          = 16000 kHz / n
 */
#define PPM_PULPROG_F_DT_ACQUIRE   1.6e+4

/* conversion factor: trasmit edge dwell time.
 *  - (factor is expressed in milliseconds)
 *  - t_DELAY = n / f_TCNT0
 *            = n / (f_CPU / (k_TCNT0 * 0xff))
 *            = n / (16 MHz / (64 * 0xff))
 *            = 1.024 ms * n
 */
#define PPM_PULPROG_F_DT_TXEDGE    1.024

/* conversion factor: transmit edge gain factor.
 *  - (A may range from -1 to +1)
 *  - A_DEV = A * (2 ^ 15)
 */
#define PPM_PULPROG_F_AV_TXEDGE    32767.0

/* conversion factor: transmit pulse dwell time.
 *  - (n is expressed in seconds)
 *  - t_PULSE = ((k_TCNT0 * n_OVF) / f_CPU) * n
 *            = ((64 * 3) / 16 MHz) * n
 *            = 12.0 us * n
 */
#define PPM_PULPROG_F_DT_TXPULSE   12.0e-6

/* conversion factor: transmit pulse frequency tuning word.
 *  - (B represents the size of the phase accumulator)
 *  - (n represents the phase accumulation word)
 *  - f_OUT = f_TIMER / (2 ^ B) * n
 *          = (f_CPU / (k_TCNT0 * n_OVF)) / (2 ^ B) * n
 *          = (16 MHz / (64 * 3)) / (2 ^ 24) * n
 *          = 250 kHz / (3 * 16777216) * n
 *          = 250 kHz / 50331648 * n
 */
#define PPM_PULPROG_F_TW_TXPULSE \
  (250.0e+3 / 50331648.0)

/* conversion factor: transmit pulse gain factor.
 *  - (A is the human-readable gain factor, A in [0:1])
 *  - A_DEV = A * 0xff
 */
#define PPM_PULPROG_F_AV_TXPULSE 255.0

/* conversion factor: shim gain factor.
 *  - (A is the human-readable gain factor, A in [-1:1])
 *  - A_DEV = A * (2 ^ 15)
 */
#define PPM_PULPROG_F_AV_SHIM    32767.0

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

int ppm_prog_add_deadtime (ppm_prog *pp, unsigned int *idx, double ms);

int ppm_prog_add_delay (ppm_prog *pp, unsigned int *idx, double s);

int ppm_prog_add_polarize (ppm_prog *pp, unsigned int *idx, long en);

int ppm_prog_add_relay (ppm_prog *pp, unsigned int *idx, long en);

int ppm_prog_add_acquire (ppm_prog *pp, unsigned int *idx,
                          long n, double rate);

int ppm_prog_add_txedge (ppm_prog *pp, unsigned int *idx, uint8_t cmd,
                         double ms, double ampl);

int ppm_prog_add_txpulse (ppm_prog *pp, unsigned int *idx,
                          double t, double f, double ampl);

int ppm_prog_add_tune (ppm_prog *pp, unsigned int *idx, double f);

int ppm_prog_add_shim (ppm_prog *pp, unsigned int *idx, uint8_t cmd,
                       double s);

void ppm_prog_read (FILE *fh, ppm_prog *pp);

void ppm_prog_write (FILE *fh, ppm_prog *pp);

/* ensure proper inclusion. */
#endif
