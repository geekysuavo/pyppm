
/* gpio.h: PPM firmware general purpose input/output header.
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
#ifndef __PPM_GPIO_H__
#define __PPM_GPIO_H__

/* * * * GPIO HARDWARE CONNECTIONS * * * */

/* define connections for: light-emitting diodes. */
#define GPIO_LED_DDR  DDRC
#define GPIO_LED_PORT PORTC
#define GPIO_LED_PINA PORTC4
#define GPIO_LED_PINB PORTC5
#define GPIO_LED_PINC PORTC6

/* define connections for: constant current sink. */
#define GPIO_CCSEN_DDR  DDRC
#define GPIO_CCSEN_PORT PORTC
#define GPIO_CCSEN_PIN  PORTC2

/* define connections for: polarization relay. */
#define GPIO_RELAY_DDR  DDRD
#define GPIO_RELAY_PORT PORTD
#define GPIO_RELAY_PIN  PORTD3

/* define connections for: analog signal chain power. */
#define GPIO_LDOEN_DDR  DDRB
#define GPIO_LDOEN_PORT PORTB
#define GPIO_LDOEN_PIN  PORTB6

/* define connections for: hardware spi analog-to-digital converter. */
#define GPIO_HWSPI_DDR  DDRB
#define GPIO_HWSPI_SS   DDB0
#define GPIO_HWSPI_SCK  DDB1
#define GPIO_HWSPI_MOSI DDB2
/* GPIO_HWSPI_MISO need not be defined: master mode forces it as input. */

/* define connections for: analog-to-digital converter chip-select. */
#define GPIO_ADCSEL_DDR  DDRD
#define GPIO_ADCSEL_PORT PORTD
#define GPIO_ADCSEL_PIN  PORTD6

/* * * * GPIO SOFTWARE FUNCTIONS: LEDs * * * */

/* gpio_led_init: initialize the state of the led outputs. */
#define gpio_led_init() \
  GPIO_LED_DDR |= ((1 << GPIO_LED_PINA) | \
                   (1 << GPIO_LED_PINB) | \
                   (1 << GPIO_LED_PINC)); \
  gpio_leda_off (); \
  gpio_ledb_off (); \
  gpio_ledc_off ();

/* gpio_leda_on: turn on the first led output. */
#define gpio_leda_on() \
  GPIO_LED_PORT |= (1 << GPIO_LED_PINA);

/* gpio_leda_off: turn off the first led output. */
#define gpio_leda_off() \
  GPIO_LED_PORT &= ~(1 << GPIO_LED_PINA);

/* gpio_ledb_on: turn on the second led output. */
#define gpio_ledb_on() \
  GPIO_LED_PORT |= (1 << GPIO_LED_PINB);

/* gpio_ledb_off: turn off the second led output. */
#define gpio_ledb_off() \
  GPIO_LED_PORT &= ~(1 << GPIO_LED_PINB);

/* gpio_ledc_on: turn on the third led output. */
#define gpio_ledc_on() \
  GPIO_LED_PORT |= (1 << GPIO_LED_PINC);

/* gpio_ledc_off: turn off the third led output. */
#define gpio_ledc_off() \
  GPIO_LED_PORT &= ~(1 << GPIO_LED_PINC);

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

/* * * * GPIO SOFTWARE FUNCTIONS: LDO * * * */

/* gpio_ldo_init: initializes the state of the ASC power supply. */
#define gpio_ldo_init() \
  GPIO_LDOEN_DDR |= (1 << GPIO_LDOEN_PIN); \
  gpio_ldo_disable ();

/* gpio_ldo_enable: enables the analog signal chain power supply by
 * sending the LDO_EN pin high.
 */
#define gpio_ldo_enable() \
  GPIO_LDOEN_PORT |= (1 << GPIO_LDOEN_PIN);

/* gpio_ldo_disable: disables the analog signal chain power supply by
 * sending the LDO_EN pin low.
 */
#define gpio_ldo_disable() \
  GPIO_LDOEN_PORT &= ~(1 << GPIO_LDOEN_PIN);

/* * * * GPIO SOFTWARE FUNCTIONS: HW SPI * * * */

/* gpio_hwspi_init: initializes the hardware spi gpio ports. */
#define gpio_hwspi_init() \
  GPIO_HWSPI_DDR |= ((1 << GPIO_HWSPI_SS) | \
                     (1 << GPIO_HWSPI_SCK) | \
                     (1 << GPIO_HWSPI_MOSI));

/* * * * GPIO SOFTWARE FUNCTIONS: ADC /CS * * * */

/* gpio_adc_init: initialize the ADC /CS line state. */
#define gpio_adc_init() \
  gpio_hwspi_init (); \
  GPIO_ADCSEL_DDR |= (1 << GPIO_ADCSEL_PIN); \
  gpio_adc_deselect ();

/* gpio_adc_select: sends the ADC /CS line low to begin a conversion. */
#define gpio_adc_select() \
  GPIO_ADCSEL_PORT &= ~(1 << GPIO_ADCSEL_PIN);

/* gpio_adc_deselect: sends the ADC /CS line low to end a conversion. */
#define gpio_adc_deselect() \
  GPIO_ADCSEL_PORT |= (1 << GPIO_ADCSEL_PIN);

/* begin function declarations: */

void gpio_init (void);

#endif /* __PPM_GPIO_H__ */
