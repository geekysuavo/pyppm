
/* msg.h: PPM/SCB firmware host-device messages header.
 * Copyright (C) 2014  Bradley Worley  <geekysuavo@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* ensure proper inclusion. */
#ifndef __PPM_MSG_H__
#define __PPM_MSG_H__

/* define all host-sent messages for the PPM. */
#define PPM_MSG_HOST_PP_READ      0x52  /* 'R' */
#define PPM_MSG_HOST_PP_SIZE      0x53  /* 'S' */
#define PPM_MSG_HOST_PP_WRITE     0x57  /* 'W' */
#define PPM_MSG_HOST_VERSION      0x56  /* 'V' */
#define PPM_MSG_HOST_EXECUTE      0x78  /* 'x' */
#define PPM_MSG_HOST_RESET        0x7e  /* '~' */

/* define all device-sent messages for the PPM. */
#define PPM_MSG_DEVICE_WAIT     0x21  /* '!' */
#define PPM_MSG_DEVICE_DONE     0x2e  /* '.' */
#define PPM_MSG_DEVICE_BEGIN    0x3e  /* '>' */
#define PPM_MSG_DEVICE_ERROR    0x3f  /* '?' */
#define PPM_MSG_DEVICE_PP_DATA  0x70  /* 'p' */
#define PPM_MSG_DEVICE_PP_SIZE  0x73  /* 's' */
#define PPM_MSG_DEVICE_VERSION  0x76  /* 'v' */

/* define all pulse program element codes. */
#define PPM_PULPROG_DEADTIME  0x01
#define PPM_PULPROG_DELAY     0x02
#define PPM_PULPROG_POLARIZE  0x03
#define PPM_PULPROG_RELAY     0x04
#define PPM_PULPROG_ACQUIRE   0x05
#define PPM_PULPROG_TXRISE    0x06
#define PPM_PULPROG_TXFALL    0x07
#define PPM_PULPROG_TXPULSE   0x08
#define PPM_PULPROG_TUNE      0x09
#define PPM_PULPROG_SHIM_X    0x0a
#define PPM_PULPROG_SHIM_Y    0x0b
#define PPM_PULPROG_SHIM_Z    0x0c
#define PPM_PULPROG_END       0x00

#endif /* __PPM_MSG_H__ */
