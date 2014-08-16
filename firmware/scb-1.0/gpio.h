
/* gpio.h: SCB firmware general purpose input/output header.
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

/* define connections for: light-emitting diode. */
#define GPIO_LED_DDR  DDRC
#define GPIO_LED_PORT PORTC
#define GPIO_LED_PINA PORTC5

/* define connections for: high-current regulator power. */
#define GPIO_LDOEN_DDR  DDRD
#define GPIO_LDOEN_PORT PORTD
#define GPIO_LDOEN_PIN  PORTD0

/* define connections for: hardware spi digital-to-analog converter. */
#define GPIO_HWSPI_DDR  DDRB
#define GPIO_HWSPI_SS   DDB0
#define GPIO_HWSPI_SCK  DDB1
#define GPIO_HWSPI_MOSI DDB2
/* GPIO_HWSPI_MISO need not be defined: master mode forces it as input. */

/* define connections for: digital-to-analog converter chip-select. */
#define GPIO_DACSEL_DDR  DDRB
#define GPIO_DACSEL_PORT PORTB
#define GPIO_DACSEL_PIN  PORTB0

/* * * * GPIO SOFTWARE FUNCTIONS: LEDs * * * */

/* gpio_led_init: initialize the state of the led outputs. */
#define gpio_led_init() \
  GPIO_LED_DDR |= (1 << GPIO_LED_PINA); \
  gpio_leda_off (); \

/* gpio_leda_on: turn on the first led output. */
#define gpio_leda_on() \
  GPIO_LED_PORT |= (1 << GPIO_LED_PINA);

/* gpio_leda_off: turn off the first led output. */
#define gpio_leda_off() \
  GPIO_LED_PORT &= ~(1 << GPIO_LED_PINA);

/* * * * GPIO SOFTWARE FUNCTIONS: LDO * * * */

/* gpio_ldo_init: initializes the state of the high-current supply. */
#define gpio_ldo_init() \
  GPIO_LDOEN_DDR |= (1 << GPIO_LDOEN_PIN); \
  gpio_ldo_disable ();

/* gpio_ldo_enable: enables the high-current regulator supply by
 * sending the LDO_EN pin high.
 */
#define gpio_ldo_enable() \
  GPIO_LDOEN_PORT |= (1 << GPIO_LDOEN_PIN);

/* gpio_ldo_disable: disables the high-current regulator supply by
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

/* * * * GPIO SOFTWARE FUNCTIONS: DAC /CS * * * */

/* gpio_dac_init: initialize the DAC /CS line state. */
#define gpio_dac_init() \
  gpio_hwspi_init (); \
  GPIO_DACSEL_DDR |= (1 << GPIO_DACSEL_PIN); \
  gpio_dac_deselect ();

/* gpio_dac_select: sends the DAC /CS line low to begin a conversion. */
#define gpio_dac_select() \
  GPIO_DACSEL_PORT &= ~(1 << GPIO_DACSEL_PIN);

/* gpio_adc_deselect: sends the DAC /CS line low to end a conversion. */
#define gpio_dac_deselect() \
  GPIO_DACSEL_PORT |= (1 << GPIO_DACSEL_PIN);

/* begin function declarations: */

void gpio_init (void);

#endif /* __PPM_GPIO_H__ */
