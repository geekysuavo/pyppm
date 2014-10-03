
/* ppm.h: core header for interfacing to ppm devices.
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

/* include libc and linux headers. */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <math.h>

/* include sndfile headers. */
#ifdef HAVE_SNDFILE
#include <sndfile.h>
#endif

/* include the globally defined messages and macros. */
#include "../../firmware/lib/msg.h"

/* include ppm headers. */
#include "ppm-hw.h"
#include "ppm-tune.h"
#include "ppm-prog.h"
#include "ppm-math.h"
#include "ppm-data.h"
#include "ppm-fall.h"

/* ensure proper inclusion. */
#ifndef __PPM_H__
#define __PPM_H__

/* define the proton gyromagnetic ratio. */
#define GAMMPROT 2.675222005e+8

/* define a debug function. */
#define debugf(...) \
  debugf_fn (__FILE__, __LINE__, __VA_ARGS__)

/* function delarations. */

void debugf_fn (const char *f, const unsigned int l,
                const char *format, ...)
  __attribute__ ((format (printf, 3, 4)));

/* ensure proper inclusion. */
#endif
