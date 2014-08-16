
/* ppm-parms.h: header for handling ppm device parameters.
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
#ifndef __PPM_PARMS_H__
#define __PPM_PARMS_H__

/* include the PPM header. */
#include "ppm.h"

/* declare acquisition parameter string representations. */
#define PPM_ACQPARM_NS   "ns"
#define PPM_ACQPARM_TP   "tp"
#define PPM_ACQPARM_TA   "ta"
#define PPM_ACQPARM_SR   "sr"
#define PPM_ACQPARM_SN   "sn"
#define PPM_ACQPARM_DTP  "dtp"
#define PPM_ACQPARM_DTA  "dta"
#define PPM_ACQPARM_IP   "ip"

/* define a structure for holding parameters. */
typedef struct ppm_parms_t {
  /* number of scans. managed by host. */
  int ns;

  /* number of polarization ticks before overflow. */
  uint16_t polarize_ovf;

  /* acquisition timer compare-match threshold. */
  uint16_t acquire_ovf;

  /* number of acquisition ticks before overflow. */
  uint32_t acquire_count;

  /* dead time before polarization and acquisition. */
  uint16_t deadtime_pol;
  uint16_t deadtime_acq;

  /* current sink voltage set-point value. */
  uint16_t ccs_value;

  /* human-readable versions of the above parameters. */
  double f_polarize_ovf;   /* s */
  double f_acquire_ovf;    /* kS/s */
  double f_deadtime_pol;   /* ms */
  double f_deadtime_acq;   /* ms */
  double f_ccs_value;      /* A */
} ppm_parms;

/* function delarations. */

int ppm_parms_humanize (ppm_parms *parms);

int ppm_parms_dehumanize (ppm_parms *parms);

/* ensure proper inclusion. */
#endif
