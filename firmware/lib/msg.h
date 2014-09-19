
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

/* define all possible PPM operating states. */
#define PPM_IDLE      0x00
#define PPM_SAMPLE    0x01
#define PPM_POLARIZE  0x02

/* define all host-sent messages for the PPM. */
#define PPM_MSG_HOST_TEST_DAC     0x44  /* 'D' */
#define PPM_MSG_HOST_PULPROG      0x50  /* 'P' */
#define PPM_MSG_HOST_RESHIM       0x53  /* 'S' */
#define PPM_MSG_HOST_RETUNE       0x54  /* 'T' */
#define PPM_MSG_HOST_VERSION      0x56  /* 'V' */
#define PPM_MSG_HOST_READ_PARMS   0x72  /* 'r' */
#define PPM_MSG_HOST_WRITE_PARMS  0x77  /* 'w' */
#define PPM_MSG_HOST_EXECUTE      0x78  /* 'x' */
#define PPM_MSG_HOST_RESET        0x7e  /* '~' */

/* define all host-sent messages for the SCB. */
#define SCB_MSG_HOST_READ_SHIMS   0x52  /* 'R' */
#define SCB_MSG_HOST_WRITE_SHIMS  0x57  /* 'W' */
#define SCB_MSG_HOST_RESET        0x7e  /* '~' */

/* define all device-sent messages for the PPM. */
#define PPM_MSG_DEVICE_WAIT     0x21  /* '!' */
#define PPM_MSG_DEVICE_DONE     0x2e  /* '.' */
#define PPM_MSG_DEVICE_BEGIN    0x3e  /* '>' */
#define PPM_MSG_DEVICE_ERROR    0x3f  /* '?' */
#define PPM_MSG_DEVICE_PARMS    0x70  /* 'p' */
#define PPM_MSG_DEVICE_SAMPLE   0x73  /* 's' */
#define PPM_MSG_DEVICE_VERSION  0x76  /* 'v' */

/* define all device-sent messages for the SCB. */
#define SCB_MSG_DEVICE_SHIMS  0x50  /* 'P' */
#define SCB_MSG_DEVICE_DONE   0x2e  /* '.' */

/* define all pulse program element codes. */
#define PPM_PULPROG_DEADTIME  0x01
#define PPM_PULPROG_DELAY     0x02
#define PPM_PULPROG_POLARIZE  0x03
#define PPM_PULPROG_RELAY     0x04
#define PPM_PULPROG_ACQUIRE   0x05
#define PPM_PULPROG_TXRISE    0x06
#define PPM_PULPROG_TXFALL    0x07
#define PPM_PULPROG_TXPULSE   0x08
#define PPM_PULPROG_END       0x00

#endif /* __PPM_MSG_H__ */
