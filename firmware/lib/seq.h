
/* seq.h: PPM firmware pulse program sequencer header.
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
#ifndef __PPM_SEQ_H__
#define __PPM_SEQ_H__

/* begin function declarations: global sequencing functions, defined in the
 * lib/seq.c source file.
 */

void ppm_done (void);

void ppm_error (void);

void ppm_version (void);

void ppm_reset (void);

void ppm_execute (void);

void ppm_pulprog_size (void);

void ppm_pulprog_recv (void);

void ppm_pulprog_send (void);

/* begin function definitions: pulse sequence elements, defined in each
 * individual firmware source tree.
 */

void ppm_deadtime (uint16_t dt);

void ppm_ccs_enable (uint8_t en);

void ppm_relay_enable (uint8_t en);

uint16_t ppm_delay (uint16_t dt, uint16_t dac);

void ppm_acquire (uint32_t n, uint16_t ovf);

uint16_t ppm_txrise (uint16_t n, uint16_t ampl, uint16_t dac);

uint16_t ppm_txfall (uint16_t n, uint16_t ampl, uint16_t dac);

void ppm_txpulse (uint32_t n, uint32_t freq, uint8_t ampl);

void ppm_settune (uint16_t tun);

void ppm_setshim_x (uint16_t shm);

void ppm_setshim_y (uint16_t shm);

void ppm_setshim_z (uint16_t shm);

#endif /* __PPM_SEQ_H__ */
