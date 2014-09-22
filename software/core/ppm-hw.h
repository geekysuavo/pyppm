
/* ppm-hw.h: header for low-level hardware comms with ppm devices.
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
#ifndef __PPM_HW_H__
#define __PPM_HW_H__

/* include the PPM header. */
#include "ppm.h"

/* define the number of bytes used in version queries. */
#define PPM_BIGMSG_BYTES  8192
#define PPM_SIZMSG_BYTES  4
#define PPM_VERMSG_BYTES  3

/* define a quick conversion from index to time. */
#define SAMPLE_TIME(idx) \
  ((double) idx * parms->acquire_ovf * 62.5e-9)

/* define a quick conversion from word to voltage. */
#define SAMPLE_VOLT(msb, lsb) \
  (((double) (WORD (msb, lsb)) * 5.0 / 65536.0) - 2.5)

/* define helper functions for 16-bit words. */
#define WORD(msb, lsb) (((msb << 8) & 0xff00) | (lsb & 0x00ff))
#define LSB(n) (n & 0xff)
#define MSB(n) ((n >> 8) & 0xff)

/* define helper functions for 32-bit words. */
#define BYTE0(n) LSB(n)
#define BYTE1(n) MSB(n)
#define BYTE2(n) ((n >> 16) & 0xff)
#define BYTE3(n) ((n >> 24) & 0xff)
#define WORD32(b3, b2, b1, b0) \
  ((((int32_t) b3 << 24) & 0xff000000) | \
   (((int32_t) b2 << 16) & 0x00ff0000) | \
    (((int32_t) b1 << 8) & 0x0000ff00) | \
           ((int32_t) b0 & 0x000000ff))

/* define the PPM device filename. */
#ifdef _WIN32
#error "inadequate operating system!"
#else
#if __APPLE__ & __MACH__
#define PPM_DEVICE_FILENAME "/dev/cu.usbmodemPyPPM"
#else
#define PPM_DEVICE_FILENAME "/dev/ttyACM0"
#endif
#endif

/* function delarations. */

int ppm_device_open (const char *fname);

void ppm_device_close (int fd);

int ppm_chk (const char *fname);

int ppm_rst (const char *fname);

int ppm_ver_fd (int fd, int *ver, int *rev);

unsigned int ppm_szpp_fd (int fd);

int ppm_wpp_fd (int fd, ppm_prog *pp);

int ppm_rpp_fd (int fd, ppm_prog *pp);

int ppm_zg_fd (int fd, ppm_prog *pp, ppm_data *acq);

int ppm_ver (const char *fname, int *ver, int *rev);

int ppm_wpp (const char *fname, ppm_prog *pp);

int ppm_rpp (const char *fname, ppm_prog *pp);

int ppm_zg (const char *fname, ppm_prog *pp, ppm_data *acq);

/* ensure proper inclusion. */
#endif
