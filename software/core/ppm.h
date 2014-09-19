
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
#include "ppm-parms.h"
#include "ppm-shims.h"
#include "ppm-math.h"
#include "ppm-data.h"
#include "ppm-fall.h"

/* ensure proper inclusion. */
#ifndef __PPM_H__
#define __PPM_H__

/* define the proton gyromagnetic ratio. */
#define GAMMPROT 2.675222005e+8

/* function delarations. */

/* ensure proper inclusion. */
#endif
