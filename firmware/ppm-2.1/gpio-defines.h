
/* gpio-defines.h: PPM firmware general purpose input/output header.
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
#ifndef __PPM_GPIO_DEFINES_H__
#define __PPM_GPIO_DEFINES_H__

/* * * * GPIO HARDWARE CONNECTIONS * * * */

/* define connections for: light-emitting diodes: communication. */
#define GPIO_LED_COMM_DDR  DDRE
#define GPIO_LED_COMM_PORT PORTE
#define GPIO_LED_COMM_PIN  PORTE6

/* define connections for: light-emitting diodes: adc operation. */
#define GPIO_LED_ADC_DDR  DDRC
#define GPIO_LED_ADC_PORT PORTC
#define GPIO_LED_ADC_PIN  PORTC6

/* define connections for: light-emitting diodes: dac operation. */
#define GPIO_LED_DAC_DDR  DDRD
#define GPIO_LED_DAC_PORT PORTD
#define GPIO_LED_DAC_PIN  PORTD5

/* define connections for: constant current sink. */
#define GPIO_CCSEN_DDR  DDRD
#define GPIO_CCSEN_PORT PORTD
#define GPIO_CCSEN_PIN  PORTD0

/* define connections for: polarization relay. */
#define GPIO_RELAY_DDR  DDRD
#define GPIO_RELAY_PORT PORTD
#define GPIO_RELAY_PIN  PORTD4

/* define connections for: analog signal chain power. */
#define GPIO_ASCEN_DDR  DDRF
#define GPIO_ASCEN_PORT PORTF
#define GPIO_ASCEN_PIN  PORTF0

/* define connections for: software spi analog-to-digital converter. */
#define GPIO_ADC_DDR      DDRB
#define GPIO_ADC_PORT     PORTB
#define GPIO_ADC_PIN      PINB
#define GPIO_ADC_DDR_CS   DDB4
#define GPIO_ADC_DDR_SCK  DDB6
#define GPIO_ADC_DDR_MISO DDB5
#define GPIO_ADC_PIN_CS   PORTB4
#define GPIO_ADC_PIN_SCK  PORTB6
#define GPIO_ADC_PIN_MISO PINB5

/* define connections for: software spi digital-to-analog converter. */
#define GPIO_DAC_DDR      DDRD
#define GPIO_DAC_PORT     PORTD
#define GPIO_DAC_DDR_CS   DDD3
#define GPIO_DAC_DDR_SCK  DDD2
#define GPIO_DAC_DDR_MOSI DDD1
#define GPIO_DAC_PIN_CS   PORTD3
#define GPIO_DAC_PIN_SCK  PORTD2
#define GPIO_DAC_PIN_MOSI PORTD1

/* define connections for: software spi tuning capacitor bank. */
#define GPIO_TUNE_DDR      DDRF
#define GPIO_TUNE_PORT     PORTF
#define GPIO_TUNE_DDR_CS   DDF5
#define GPIO_TUNE_DDR_SCK  DDF6
#define GPIO_TUNE_DDR_MOSI DDF7
#define GPIO_TUNE_PIN_CS   PORTF5
#define GPIO_TUNE_PIN_SCK  PORTF6
#define GPIO_TUNE_PIN_MOSI PORTF7

/* * * * GPIO SOFTWARE FUNCTIONS: LEDs * * * */

/* gpio_leds_init: initialize the state of the led outputs. */
#define gpio_leds_init() \
  gpio_led_comm_init (); \
  gpio_led_adc_init (); \
  gpio_led_dac_init ();

/* gpio_led_comm_init: initialize the state of the communication led. */
#define gpio_led_comm_init() \
  GPIO_LED_COMM_DDR |= (1 << GPIO_LED_COMM_PIN); \
  gpio_led_comm_off ();

/* gpio_led_adc_init: initialize the state of the adc led. */
#define gpio_led_adc_init() \
  GPIO_LED_ADC_DDR |= (1 << GPIO_LED_ADC_PIN); \
  gpio_led_adc_off ();

/* gpio_led_dac_init: initialize the state of the dac led. */
#define gpio_led_dac_init() \
  GPIO_LED_DAC_DDR |= (1 << GPIO_LED_DAC_PIN); \
  gpio_led_dac_off ();

/* gpio_led_comm_on: turn on the communication led output. */
#define gpio_led_comm_on() \
  GPIO_LED_COMM_PORT |= (1 << GPIO_LED_COMM_PIN);

/* gpio_led_comm_off: turn off the communication led output. */
#define gpio_led_comm_off() \
  GPIO_LED_COMM_PORT &= ~(1 << GPIO_LED_COMM_PIN);

/* gpio_led_adc_on: turn on the adc led output. */
#define gpio_led_adc_on() \
  GPIO_LED_ADC_PORT |= (1 << GPIO_LED_ADC_PIN);

/* gpio_led_adc_off: turn off the adc led output. */
#define gpio_led_adc_off() \
  GPIO_LED_ADC_PORT &= ~(1 << GPIO_LED_ADC_PIN);

/* gpio_led_dac_on: turn on the dac led output. */
#define gpio_led_dac_on() \
  GPIO_LED_DAC_PORT |= (1 << GPIO_LED_DAC_PIN);

/* gpio_led_dac_off: turn off the dac led output. */
#define gpio_led_dac_off() \
  GPIO_LED_DAC_PORT &= ~(1 << GPIO_LED_DAC_PIN);

/* * * * GPIO SOFTWARE FUNCTIONS: CCS * * * */

/* gpio_ccs_init: initializes the state of the current source. */
#define gpio_ccs_init() \
  GPIO_CCSEN_DDR |= (1 << GPIO_CCSEN_PIN); \
  gpio_ccs_disable ();

/* gpio_ccs_enable: disable the polarization current source. this function
 * ties the CCS_EN pin high to signal the MOSFET gate driver to drive the
 * FET gate high.
 */
#define gpio_ccs_enable() \
  GPIO_CCSEN_PORT |= (1 << GPIO_CCSEN_PIN);

/* gpio_ccs_disable: enable the polarization current source. this function
 * ties the CCS_EN pin low to signal the MOSFET gate driver to drive the
 * FET gate low.
 */
#define gpio_ccs_disable() \
  GPIO_CCSEN_PORT &= ~(1 << GPIO_CCSEN_PIN);

/* * * * GPIO SOFTWARE FUNCTIONS: RELAY * * * */

/* gpio_relay_init: initializes the state of the POL/ACQ relay. */
#define gpio_relay_init() \
  GPIO_RELAY_DDR |= (1 << GPIO_RELAY_PIN); \
  gpio_relay_acq ();

/* gpio_relay_pol: engages the POL/ACQ relay into the POL mode by sending
 * the RELAY_EN pin high. this software requires that the default relay
 * position is ACQ mode.
 */
#define gpio_relay_pol() \
  GPIO_RELAY_PORT |= (1 << GPIO_RELAY_PIN);

/* gpio_relay_acq: disengages the POL/ACQ relay into the ACQ mode by sending
 * the RELAY_EN pin low. this software requires that the default relay
 * position is ACQ mode.
 */
#define gpio_relay_acq() \
  GPIO_RELAY_PORT &= ~(1 << GPIO_RELAY_PIN);

/* * * * GPIO SOFTWARE FUNCTIONS: ASC PWR * * * */

/* gpio_asc_init: initializes the state of the ASC power supply. */
#define gpio_asc_init() \
  GPIO_ASCEN_DDR |= (1 << GPIO_ASCEN_PIN); \
  gpio_asc_disable ();

/* gpio_asc_enable: enables the analog signal chain power supply by
 * sending the ASC_EN pin high.
 */
#define gpio_asc_enable() \
  GPIO_ASCEN_PORT |= (1 << GPIO_ASCEN_PIN);

/* gpio_asc_disable: disables the analog signal chain power supply by
 * sending the ASC_EN pin low.
 */
#define gpio_asc_disable() \
  GPIO_ASCEN_PORT &= ~(1 << GPIO_ASCEN_PIN);

/* * * * GPIO SOFTWARE FUNCTIONS: SPI ADC * * * */

/* gpio_adc_init: initializes the software spi gpio ports. */
#define gpio_adc_init() \
  GPIO_ADC_DDR |= ((1 << GPIO_ADC_DDR_CS) | \
                   (1 << GPIO_ADC_DDR_SCK)); \
  GPIO_ADC_PORT |= (1 << GPIO_ADC_DDR_SCK); \
  gpio_adc_deselect ();

/* gpio_adc_select: sends the ADC /CS line low to begin a conversion. */
#define gpio_adc_select() \
  GPIO_ADC_PORT &= ~(1 << GPIO_ADC_PIN_CS);

/* gpio_adc_deselect: sends the ADC /CS line low to end a conversion. */
#define gpio_adc_deselect() \
  GPIO_ADC_PORT |= (1 << GPIO_ADC_PIN_CS);

/* * * * GPIO SOFTWARE FUNCTIONS: SPI DAC * * * */

/* gpio_dac_init: initializes the software spi dac gpio ports. */
#define gpio_dac_init() \
  GPIO_DAC_DDR |= ((1 << GPIO_DAC_DDR_CS) | \
                   (1 << GPIO_DAC_DDR_SCK) | \
                   (1 << GPIO_DAC_DDR_MOSI)); \
  gpio_dac_deselect (); \
  gpio_dac_mosi_lo (); \
  gpio_dac_clock ();

/* gpio_dac_select: sends the DAC /CS line low to begin a transfer. */
#define gpio_dac_select() \
  GPIO_DAC_PORT &= ~(1 << GPIO_DAC_PIN_CS);

/* gpio_dac_deselect: sends the DAC /CS line high to end a transfer. */
#define gpio_dac_deselect() \
  GPIO_DAC_PORT |= (1 << GPIO_DAC_PIN_CS);

/* gpio_dac_clock: sends the DAC SCK line high, then low to clock a bit. */
#define gpio_dac_clock() \
  GPIO_DAC_PORT |= (1 << GPIO_DAC_PIN_SCK); \
  GPIO_DAC_PORT &= ~(1 << GPIO_DAC_PIN_SCK);

/* gpio_dac_mosi_lo: sends the DAC MOSI line low. */
#define gpio_dac_mosi_lo() \
  GPIO_DAC_PORT &= ~(1 << GPIO_DAC_PIN_MOSI);

/* gpio_dac_mosi_hi: sends the DAC MOSI line high. */
#define gpio_dac_mosi_hi() \
  GPIO_DAC_PORT |= (1 << GPIO_DAC_PIN_MOSI);

/* * * * GPIO SOFTWARE FUNCTIONS: SPI tuning bank * * * */

/* gpio_tune_init: initializes the the software spi tuning gpio ports. */
#define gpio_tune_init() \
  GPIO_TUNE_DDR |= ((1 << GPIO_TUNE_DDR_CS) | \
                    (1 << GPIO_TUNE_DDR_SCK) | \
                    (1 << GPIO_TUNE_DDR_MOSI)); \
  gpio_tune_deselect (); \
  gpio_tune_mosi_lo (); \
  gpio_tune_clock ();

/* gpio_tune_select: sends the switch /CS line low (through an inverted
 * optoisolated output) to begin a transfer.
 */
#define gpio_tune_select() \
  GPIO_TUNE_PORT |= (1 << GPIO_TUNE_PIN_CS);

/* gpio_tune_deselect: sends the switch /CS line high (through an inverted
 * optoisolated output) to end a transfer.
 */
#define gpio_tune_deselect() \
  GPIO_TUNE_PORT &= ~(1 << GPIO_TUNE_PIN_CS);

/* gpio_tune_clock: sends the switch SCK line high, then low (through an
 * inverted optoisolated output) to clock a bit.
 */
#define gpio_tune_clock() \
  GPIO_TUNE_PORT &= ~(1 << GPIO_TUNE_PIN_SCK); \
  _delay_us (4.0); \
  GPIO_TUNE_PORT |= (1 << GPIO_TUNE_PIN_SCK); \
  _delay_us (4.0);

/* gpio_tune_mosi_lo: sends the switch MOSI line low (through an inverted
 * optoisolated output).
 */
#define gpio_tune_mosi_lo() \
  GPIO_TUNE_PORT |= (1 << GPIO_TUNE_PIN_MOSI);

/* gpio_tune_mosi_hi: sends the switch MOSI line high (through an inverted
 * optoisolated output).
 */
#define gpio_tune_mosi_hi() \
  GPIO_TUNE_PORT &= ~(1 << GPIO_TUNE_PIN_MOSI);

#endif /* __PPM_GPIO_DEFINES_H__ */
