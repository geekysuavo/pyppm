
/* tcnt.c: CCU firmware timer/counter peripherals source code.
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

/* include the main header. */
#include "main.h"

/* define all possible CCU operating states. */
#define CCU_IDLE     0x00
#define CCU_SAMPLE   0x01
#define CCU_POLARIZE 0x02

/* declare a global variable to store the current CCU state. */
volatile uint8_t ccu_state = CCU_IDLE;

/* declare a global variable for polarization timing.
 *  - t_POL = 0xff / (16MHz/1024) * ovf = 16.384 ms * ovf
 *  - default value of 62 counts provides a 1.016 s delay
 */
volatile uint16_t ccu_polarize_ovf = 62;
volatile uint16_t ccu_polarize_idx = 0;

/* declare a global variable for acquisition timing.
 *  - t_DW = ovf / 16MHz = 62.5 ns * ovf
 *  - default value of 800 provides a 50 us delay (20 kS/s)
 *  - handled via hardware overflow of timer1
 *  - the global variable exists for simplicity
 */
volatile uint16_t ccu_acquire_ovf = 800;

/* declare a global variable for acquisition counting.
 *  - t_ACQ = count * t_DW
 *  - default value of 10,000 provides a 1.0 s acquisition
 */
volatile uint16_t ccu_acquire_count = 10000;
volatile uint16_t ccu_acquire_idx = 0;

/* ISR (TIMER0_OVF_vect): timer0 overflow interrupt. */
ISR (TIMER0_OVF_vect) {
  /* increment the polarization overflow counter. */
  if (++ccu_polarize_idx > ccu_polarize_ovf) {
    /* disable timer0 interrupts. */
    TIMSK0 &= ~(1 << TOIE0);

    /* reset the polarization overflow counter and enter sampling mode. */
    ccu_polarize_idx = 0;
    ccu_state = CCU_SAMPLE;

    /* NOTE: depolarize the sample coil and delay a few milliseconds. */

    /* NOTE: switch the relay to the amplifier. */

    /* enable timer1 interrupts. */
    TIMSK1 |= (1 << OCIE1A);
  }
}

/* ISR (TIMER1_COMPA_vect): timer1 compare match A interrupt. */
ISR (TIMER1_COMPA_vect) {
  /* declare a variable to store the read ADC sample. */
  uint16_t adc = 0;

  /* sample the ADC. */
  adc = spi_read_adc ();

  /* transmit the ADC sample to the usb host. */
  usb_cdc_putchar (LSB (adc));
  usb_cdc_putchar (MSB (adc));

  /* increment the acquisition counter. */
  if (++ccu_acquire_idx > ccu_acquire_count) {
    /* disable timer1 interrupts. */
    TIMSK1 &= ~(1 << OCIE1A);

    /* reset the acquisition counter and enter idle mode. */
    ccu_acquire_idx = 0;
    ccu_state = CCU_IDLE;
  }
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
void tcnt1_start (void) {
  /* set the compare match registers. */
  OCR1AH = MSB (ccu_acquire_ovf);
  OCR1AL = LSB (ccu_acquire_ovf);

  /* zero the timer registers. */
  TCNT1H = 0x00;
  TCNT1L = 0x00;

  /* enable the timer. */
  TIMSK1 |= (1 << OCIE1A);
}
