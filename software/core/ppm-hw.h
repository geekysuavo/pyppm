
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

/* define the number of bytes used in parameter transfers. */
#define PPM_PARMSG_BYTES 15
#define PPM_SHIMSG_BYTES 7
#define PPM_VERMSG_BYTES 3

/* define a quick conversion from index to time. */
#define SAMPLE_TIME(idx) \
  ((double) idx * parms->acquire_ovf * 62.5e-9)

/* define a quick conversion from word to voltage. */
#define SAMPLE_VOLT(msb, lsb) \
  (((double) (WORD (msb, lsb)) * 5.0 / 65536.0) - 2.5)

/* define helper functions for 16-bit words. */
#define WORD(msb, lsb) ((msb << 8) & 0xff00) | (lsb & 0x00ff)
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

/* define all host-sent messages for the PPM. */
//#define PPM_MSG_HOST_READ_PARMS   0x72  /* 'r' */
//#define PPM_MSG_HOST_READ_SHIMS   0x52  /* 'R' */
//#define PPM_MSG_HOST_WRITE_PARMS  0x77  /* 'w' */
//#define PPM_MSG_HOST_WRITE_SHIMS  0x57  /* 'W' */
//#define PPM_MSG_HOST_EXECUTE      0x78  /* 'x' */
//#define PPM_MSG_HOST_RESET        0x7e  /* '~' */
//#define PPM_MSG_HOST_TEST_DAC     0x44  /* 'D' */

/* define all device-sent messages for the PPM. */
//#define PPM_MSG_DEVICE_PARMS   0x70  /* 'p' */
//#define PPM_MSG_DEVICE_SHIMS   0x50  /* 'P' */
//#define PPM_MSG_DEVICE_WAIT    0x21  /* '!' */
//#define PPM_MSG_DEVICE_DONE    0x2e  /* '.' */
//#define PPM_MSG_DEVICE_BEGIN   0x3e  /* '>' */
//#define PPM_MSG_DEVICE_SAMPLE  0x73  /* 's' */

/* define the PPM device filename. */
#ifdef _WIN32
#error "inadequate operating system!"
#else
#if __APPLE__ & __MACH__
#define PPM_DEVICE_FILENAME "/dev/cu.usbmodemPPMv1r1"
#else
#define PPM_DEVICE_FILENAME "/dev/ttyACM0"
#endif
#endif

/* function delarations. */

int ppm_device_open (const char *fname);

void ppm_device_close (int fd);

int ppm_chk (const char *fname);

int ppm_rst (const char *fname);

int ppm_dactest (const char *fname);

int ppm_scbtest (const char *fname);

int ppm_ver_fd (int fd, int *ver, int *rev);

int ppm_rpar_fd (int fd, ppm_parms *parms);

int ppm_wpar_fd (int fd, ppm_parms *parms);

int ppm_rsh_fd (int fd, ppm_shims *shims);

int ppm_wsh_fd (int fd, ppm_shims *shims);

int ppm_zg_fd (int fd, ppm_parms *parms, ppm_data *acq);

int ppm_ver (const char *fname, int *ver, int *rev);

int ppm_rpar (const char *fname, ppm_parms *parms);

int ppm_wpar (const char *fname, ppm_parms *parms);

int ppm_rsh (const char *fname, ppm_shims *shims);

int ppm_wsh (const char *fname, ppm_shims *shims);

int ppm_zg (const char *fname, ppm_parms *parms, ppm_data *acq);

/* ensure proper inclusion. */
#endif
