
/* ppm-shims.c: source file for handling ppm device shim values.
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

/* ppm_shims_host_to_dev: translate a floating point value to an integer.
 * @host: floating-point value to convert.
 */
uint16_t ppm_shims_host_to_dev (double host) {
  /* declare required variables. */
  uint16_t mag, dev;
  uint8_t sign;

  /* determine the sign of the value. */
  if (host < 0.0) {
    /* negative. compute the sign and magnitude. */
    sign = 0x01;
    mag = (uint16_t) (4095.0 * -host);
  }
  else {
    /* positive. compute the sign and magnitude. */
    sign = 0x00;
    mag = (uint16_t) (4095.0 * +host);
  }

  /* join the sign and magnitude together and send them on their way. */
  dev = mag | (sign << 12);
  return dev;
}

/* ppm_shims_dev_to_host: translate an integer value to floating point.
 * @dev: integer value to convert.
 */
double ppm_shims_dev_to_host (uint16_t dev) {
  /* declare required variables. */
  uint16_t mag;
  uint8_t sign;
  double host;

  /* extract the sign and magnitude from the integer. */
  sign = (dev & (1 << 12)) >> 12;
  mag = dev & ~(1 << 12);

  /* determine the sign of the value. */
  if (sign) {
    /* negative. compute the magnitude. */
    host = -((double) mag) / 4095.0;
  }
  else {
    /* positive. compute the magnitude. */
    host = +((double) mag) / 4095.0;
  }

  /* return the converted value. */
  return host;
}

/* ppm_shims_humanize: translates a shim struct to human-readable. */
int ppm_shims_humanize (ppm_shims *shims) {
  /* translate the values to floating point. */
  shims->f_x = ppm_shims_dev_to_host (shims->x);
  shims->f_y = ppm_shims_dev_to_host (shims->y);
  shims->f_z = ppm_shims_dev_to_host (shims->z);

  /* return success. */
  return 1;
}

/* ppm_shims_dehumanize: translates a shim struct to machine-readable. */
int ppm_shims_dehumanize (ppm_shims *shims) {
  /* translate the values to the proper integer sizes. */
  shims->x = ppm_shims_host_to_dev (shims->f_x);
  shims->y = ppm_shims_host_to_dev (shims->f_y);
  shims->z = ppm_shims_host_to_dev (shims->f_z);

  /* return success. */
  return 1;
}
