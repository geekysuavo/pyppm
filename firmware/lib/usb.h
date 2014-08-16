
/* usb.h: PPM firmware univeral serial bus header.
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

/* include required avr headers. */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

/* include the avr macro header. */
#include "macros.h"

/* ensure proper inclusion. */
#ifndef __PPM_USB_H__
#define __PPM_USB_H__

/* begin function declarations: */

void usb_init (void);

uint8_t usb_configured (void);

int16_t usb_cdc_getchar (void);

uint8_t usb_cdc_avail (void);

void usb_cdc_flush_input (void);

int8_t usb_cdc_putchar (uint8_t ch);

int8_t usb_cdc_putchar_immediate (uint8_t ch);

int8_t usb_cdc_write (const uint8_t *buf, uint16_t size);

void usb_cdc_flush_output (void);

uint8_t usb_cdc_control_line_dtr (void);

uint8_t usb_cdc_control_line_rts (void);

#endif /* __PPM_USB_H__ */
