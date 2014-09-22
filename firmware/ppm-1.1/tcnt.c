
/* tcnt.c: PPM firmware timer/counter peripherals source code.
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

/* include the main header. */
#include "main.h"

/* ISR (TIMER0_OVF_vect): timer0 overflow interrupt. */
ISR (TIMER0_OVF_vect) {
  /* handle delay timing in the PPM. */
  ppm_tick_del ();
}

/* ISR (TIMER1_COMPA_vect): timer1 compare match A interrupt. */
ISR (TIMER1_COMPA_vect) {
  /* handle acquisition timing in the PPM. */
  ppm_tick_acq ();
}

/* tcnt_init: initialize timer/counter peripherals. */
void tcnt_init (void) {
  /* disable the timer interrupts. */
  TIMSK0 &= ~(1 << TOIE0);
  TIMSK1 &= ~((1 << TOIE1) | (1 << OCIE1A));

  /* configure the timer0 prescaler mux for 64 us resolution. */
  TCCR0B |= ((1 << CS02) | (1 << CS00));
  TCCR0B &= ~(1 << CS01);

  /* configure timer1 to clear on output compare match. */
  TCCR1B |= (1 << WGM12);
  TCCR1B &= ~(1 << WGM13);
  TCCR1A &= ~((1 << WGM11) | (1 << WGM10));

  /* configure the timer1 prescaler mux for 62.5 ns resolution. */
  TCCR1B |= (1 << CS00);
  TCCR1B &= ~((1 << CS02) | (1 << CS01));
}

/* tcnt0_start: set timer/counter0 into motion. */
void tcnt0_start (void) {
  /* zero the timer register. */
  TCNT0 = 0x00;
  
  /* enable the timer. */
  TIMSK0 |= (1 << TOIE0);
}

/* tcnt1_start: set timer/counter1 into motion. */
void tcnt1_start (uint16_t ovf) {
  /* set the compare match registers. */
  OCR1AH = MSB (ovf);
  OCR1AL = LSB (ovf);

  /* zero the timer registers. */
  TCNT1H = 0x00;
  TCNT1L = 0x00;

  /* enable the timer. */
  TIMSK1 |= (1 << OCIE1A);
}

/* tcnt0_stop: end interrupts on timer/counter0. */
inline void tcnt0_stop (void) {
  /* disable overflow interrupts. */
  TIMSK0 &= ~(1 << TOIE0);
}

/* tcnt1_stop: end interrupts on timer/counter1. */
inline void tcnt1_stop (void) {
  /* disable compare match interrupts. */
  TIMSK1 &= ~(1 << OCIE1A);
}
