
/* scb.h: SCB firmware main controller header.
 * Copyright (C) 2013  Bradley Worley  <geekysuavo@gmail.com>
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
#ifndef __PPM_SCB_H__
#define __PPM_SCB_H__

/* define all host-sent messages for the SCB. */
#define SCB_MSG_HOST_READ_SHIMS   0x52  /* 'R' */
#define SCB_MSG_HOST_WRITE_SHIMS  0x57  /* 'W' */
#define SCB_MSG_HOST_RESET        0x7e  /* '~' */

/* define all device-sent messages for the SCB. */
#define SCB_MSG_DEVICE_SHIMS   0x50  /* 'P' */
#define SCB_MSG_DEVICE_DONE    0x2e  /* '.' */

/* begin function declarations: */

void scb_reset (void);

void scb_send_shims (void);

void scb_receive_shims (void);

#endif /* __PPM_SCB_H__ */
