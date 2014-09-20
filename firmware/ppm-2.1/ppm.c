
/* ppm.c: PPM firmware main controller source code.
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

/* define the maximum size of the pulse program array. */
#define SZ_PULPROG 1024

/* declare a global array to hold downloaded pulse programs. */
volatile uint16_t n_pulprog = 0;
volatile uint16_t i_pulprog = 0;
volatile uint8_t pulprog[SZ_PULPROG];

/* ppm_done: sets a completion message to the host over the usb bus.
 */
void ppm_done (void) {
  /* send the completion message byte. */
  usb_cdc_putchar (PPM_MSG_DEVICE_DONE);
}

/* ppm_error: sends an error message to the host over the usb bus.
 */
void ppm_error (void) {
  /* send the error message byte. */
  usb_cdc_putchar (PPM_MSG_DEVICE_ERROR);
}

/* ppm_version: returns the firmware version of the device. */
void ppm_version (void) {
  /* write the message header, version, and footer. */
  usb_cdc_putchar (PPM_MSG_DEVICE_VERSION);
  usb_cdc_putchar (PPM_FW_VERSION);
  usb_cdc_putchar (PPM_MSG_DEVICE_DONE);
}

/* ppm_reset: resets the device using the watchdog timer. */
void ppm_reset (void) {
  /* declare required variables. */
  uint8_t i;

  /* flash the leds a couple times. */
  for (i = 0; i < 10; i++) {
    /* turn off the leds. */
    gpio_led_comm_off ();
    _delay_ms (100.0);

    /* turn on the leds. */
    gpio_led_comm_on ();
    _delay_ms (100.0);
  }

  /* enable the watchdog timer. */
  wdt_enable (WDTO_120MS);

  /* loop and wait for the timer to overflow. */
  while (1);
}

/* ppm_deadtime: runs a deadtime delay function. */
void ppm_deadtime (uint16_t dt) {
  /* declare a counter variable. */
  uint8_t i;

  /* loop 100 times to make the delay effective. */
  for (i = 0; i < 100; i++)
    _delay_loop_2 (dt);
}

/* ppm_ccs_enable: enables or disables the constant current sink. */
void ppm_ccs_enable (uint8_t en) {
  /* determine which state the constant current sink should be in. */
  if (en) {
    /* enable the sink. */
    gpio_ccs_enable ();
  }
  else {
    /* disable the sink. */
    gpio_ccs_disable ();
  }
}

/* ppm_relay_enable: enables or disables the polarize/acquire relay. */
void ppm_relay_enable (uint8_t en) {
  /* determine which state the relay should be in. */
  if (en) {
    /* switch the relay on to enter polarization mode. */
    gpio_relay_pol ();
  }
  else {
    /* switch the relay off to enter acquisition mode. */
    gpio_relay_acq ();
  }
}

/* ppm_acquire: runs a single acquisition function. */
#define ppm_acquire(n, ovf) \
  gpio_led_adc_on (); \
  //FIXME tcnt1_run_acquire ((n), (ovf)); \
  gpio_led_adc_off ();

/* ppm_delay: runs a precision delay function. */
#define ppm_delay(n) \
  //FIXME dac16 = tcnt0_run_interp ((n), dac16, 0x0000, 0x00);

/* ppm_txrise: runs a polarization rising edge function. */
#define ppm_txrise(n, a) \
  gpio_led_dac_on (); \
  //FIXME dac16 = tcnt0_run_interp ((n), dac16, (a), 0x01); \
  gpio_led_dac_off ();

/* ppm_txfall: runs a polarization falling edge function. */
#define ppm_txfall(n, a) \
  gpio_led_dac_on (); \
  //FIXME dac16 = tcnt0_run_interp ((n), dac16, (a), 0x02); \
  gpio_led_dac_off ();

/* ppm_txpulse: runs a sinusoidal pulse synthesis function. */
#define ppm_txpulse(n, f, a) \
  gpio_led_dac_on (); \
  //FIXME tcnt0_run_pulse((n), (f), (a)); \
  gpio_led_dac_off ();

/* ppm_execute: runs the currently downloaded pulse program. */
void ppm_execute (void) {
  /* declare required variables. */
  uint32_t f32, n32;
  uint16_t a16, d16;
  uint8_t a8, en;

  /* declare the current value of the shim dac output. */
  uint16_t dac16 = 0x0000;

  /* loop through the pulse program array. */
  for (i_pulprog = 0;
       i_pulprog < n_pulprog &&
       i_pulprog < SZ_PULPROG;
       i_pulprog++) {
    /* determine the pulse program command at the current index. */
    switch (pulprog[i_pulprog]) {
      /* short, imprecise  delay. */
      case PPM_PULPROG_DEADTIME:
        /* build the delay time word and run the dead time command. */
        d16 = WORD (pulprog[i_pulprog + 1], pulprog[i_pulprog + 2]);
        ppm_deadtime (d16);

        /* skip to the next pulse program command. */
        i_pulprog += 2;
        break;

      /* precise delay. */
      case PPM_PULPROG_DELAY:
        /* build the delay time word and run the delay command. */
        d16 = WORD (pulprog[i_pulprog + 1], pulprog[i_pulprog + 2]);
        ppm_delay (d16);

        /* skip to the next pulse program command. */
        i_pulprog += 2;
        break;

      /* polarization control. */
      case PPM_PULPROG_POLARIZE:
        /* get the enable byte and run the control command. */
        en = pulprog[i_pulprog + 1];
        ppm_ccs_enable (en);

        /* skip to the next pulse program command. */
        i_pulprog += 1;
        break;

      /* relay control. */
      case PPM_PULPROG_RELAY:
        /* get the enable byte and run the control command. */
        en = pulprog[i_pulprog + 1];
        ppm_relay_enable (en);

        /* skip to the next pulse program command. */
        i_pulprog += 1;
        break;

      /* acquisition. */
      case PPM_PULPROG_ACQUIRE:
        /* build the acquisition sample count. */
        n32 = WORD32 (pulprog[i_pulprog + 1], pulprog[i_pulprog + 2],
                      pulprog[i_pulprog + 3], pulprog[i_pulprog + 4]);

        /* build the acquisition sample rate overflow. */
        d16 = WORD (pulprog[i_pulprog + 5], pulprog[i_pulprog + 6]);

        /* run the acquisition command. */
        ppm_acquire (n32, d16);

        /* skip to the next pulse program command. */
        i_pulprog += 6;
        break;

      /* adiabatic polarization rising edge. */
      case PPM_PULPROG_TXRISE:
        /* build the sample count word. */
        d16 = WORD (pulprog[i_pulprog + 1], pulprog[i_pulprog + 2]);

        /* build the amplitude step word. */
        a16 = WORD (pulprog[i_pulprog + 3], pulprog[i_pulprog + 4]);

        /* run the rising edge command. */
        ppm_txrise (d16, a16);

        /* skip to the next pulse program command. */
        i_pulprog += 4;
        break;

      /* adiabatic polarization falling edge. */
      case PPM_PULPROG_TXFALL:
        /* build the sample count word. */
        d16 = WORD (pulprog[i_pulprog + 1], pulprog[i_pulprog + 2]);

        /* build the amplitude step word. */
        a16 = WORD (pulprog[i_pulprog + 3], pulprog[i_pulprog + 4]);

        /* run the falling edge command. */
        ppm_txfall (d16, a16);

        /* skip to the next pulse program command. */
        i_pulprog += 4;
        break;

      /* sinusoidal pulse. */
      case PPM_PULPROG_TXPULSE:
        /* build the synthesis sample count. */
        n32 = WORD32 (pulprog[i_pulprog + 1], pulprog[i_pulprog + 2],
                      pulprog[i_pulprog + 3], pulprog[i_pulprog + 4]);

        /* build the frequency tuning word. */
        f32 = WORD32 (pulprog[i_pulprog + 5], pulprog[i_pulprog + 6],
                      pulprog[i_pulprog + 7], pulprog[i_pulprog + 7]);

        /* get the amplitude multiplier byte. */
        a8 = pulprog[i_pulprog + 9];

        /* run the pulse synthesis command. */
        ppm_txpulse (n32, f32, a8);

        /* skip to the next pulse program command. */
        i_pulprog += 9;
        break;

      /* end execution. */
      case PPM_PULPROG_END:
        /* send back a completion code and end execution. */
        ppm_done ();
        return;

      /* unrecognized command. */
      default:
        /* send back an error code and end execution. */
        ppm_error ();
        return;
    }
  }
}

/* ppm_retune: sets the tuning capacitor switch bank configuration.
 */
void ppm_retune (void) {
  /* declare variables to hold read bytes. */
  int16_t b0, b1;
  uint16_t w;

  /* read the tuning word. */
  b0 = usb_cdc_getchar ();
  b1 = usb_cdc_getchar ();
  w = WORD (b0, b1);

  /* if both read bytes are valid, write the tuning word to the switches. */
  if (b0 != -1 && b1 != -1) {
    /* write the word. */
    spi_write_tune (w);
    ppm_done ();
  }
  else {
    /* return failure. */
    ppm_error ();
  }
}

/* ppm_reshim: sets the shim output current.
 */
void ppm_reshim (void) {
  /* declare variables to hold read bytes. */
  int16_t b0, b1;
  uint16_t w;

  /* read the shimming word. */
  b0 = usb_cdc_getchar ();
  b1 = usb_cdc_getchar ();
  w = WORD (b0, b1);

  /* if both read bytes are valid, write the shimming word to the dac. */
  if (b0 != -1 && b1 != -1) {
    /* write the word. */
    spi_write_dac_b (w);
    ppm_done ();
  }
  else {
    /* return failure. */
    ppm_error ();
  }
}

/* ppm_pulprog: sets the pulse program array.
 */
void ppm_pulprog (void) {
  /* declare variables to hold read bytes. */
  int16_t b0, b1;

  /* read the pulse program length. */
  b0 = usb_cdc_getchar ();
  b1 = usb_cdc_getchar ();

  /* check for any errors. */
  if (b0 == -1 || b1 == -1) {
    /* return failure. */
    ppm_error ();
    return;
  }

  /* set the number of pulse program bytes. */
  n_pulprog = WORD (b0, b1);

  /* loop through the pulse program array bytes. */
  for (i_pulprog = 0;
       i_pulprog < n_pulprog &&
       i_pulprog < SZ_PULPROG;
       i_pulprog++) {
    /* read the next byte. */
    b0 = usb_cdc_getchar ();

    /* check if the byte is valid. */
    if (b0 != -1) {
      /* store the pulse program byte in the array. */
      pulprog[i_pulprog] = LSB (b0);
    }
    else {
      /* return failure. */
      ppm_error ();
      return;
    }
  }

  /* zero the pulse program index. */
  i_pulprog = 0;

  /* send an acknowledgement that the tuning was set. */
  ppm_done ();
}
