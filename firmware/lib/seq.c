
/* seq.c: PPM firmware pulse program sequencer source code.
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

/* include the sequencer header. */
#include "seq.h"

/* define a macro to initialize interpreter variables. */
#define VAR_INIT(v) \
  (v) = 0; \
  (v) = (v);

/* define the maximum size of the pulse program array.
 * m32u4 devices are allocated more pulse program space due to their larger
 * flash ram size. nothing personal.
 */
#ifdef _AVR_IOM32U4_H_
#define SZ_PULPROG 1024
#else
#define SZ_PULPROG 512
#endif

/* declare a global array to hold downloaded pulse programs. */
volatile uint16_t n_pulprog = 0;
volatile uint16_t i_pulprog = 0;
volatile uint8_t pulprog[SZ_PULPROG];

/* ppm_done: sends a completion message to the host over the usb bus.
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

/* ppm_execute: runs the currently downloaded pulse program. */
void ppm_execute (void) {
  /* declare required variables. */
  uint32_t f32, n32;
  uint16_t a16, d16;
  uint8_t a8, en;

  /* declare the current value of the shim dac output. */
  uint16_t dac16 = 0x0000;

  /* initialize all interpreter variables. */
  VAR_INIT (n32);
  VAR_INIT (f32);
  VAR_INIT (d16);
  VAR_INIT (a16);
  VAR_INIT (a8);
  VAR_INIT (en);
  VAR_INIT (dac16);

  /* loop through the pulse program array. */
  for (i_pulprog = 0;
       i_pulprog < n_pulprog &&
       i_pulprog < SZ_PULPROG;
       i_pulprog++) {
    /* determine the pulse program command at the current index. */
    switch (pulprog[i_pulprog]) {
      /* short, imprecise delay. */
#ifdef PPM_PULPROG_HAVE_DEADTIME
      case PPM_PULPROG_DEADTIME:
        /* build the delay time word and run the dead time command. */
        d16 = WORD (pulprog[i_pulprog + 1], pulprog[i_pulprog + 2]);
        ppm_deadtime (d16);

        /* skip to the next pulse program command. */
        i_pulprog += 2;
        break;
#endif

      /* precise delay. */
#ifdef PPM_PULPROG_HAVE_DELAY
      case PPM_PULPROG_DELAY:
        /* build the delay time word and run the delay command. */
        d16 = WORD (pulprog[i_pulprog + 1], pulprog[i_pulprog + 2]);
        dac16 = ppm_delay (d16, dac16);

        /* skip to the next pulse program command. */
        i_pulprog += 2;
        break;
#endif

      /* polarization control. */
#ifdef PPM_PULPROG_HAVE_POLARIZE
      case PPM_PULPROG_POLARIZE:
        /* get the enable byte and run the control command. */
        en = pulprog[i_pulprog + 1];
        ppm_ccs_enable (en);

        /* skip to the next pulse program command. */
        i_pulprog += 1;
        break;
#endif

      /* relay control. */
#ifdef PPM_PULPROG_HAVE_RELAY
      case PPM_PULPROG_RELAY:
        /* get the enable byte and run the control command. */
        en = pulprog[i_pulprog + 1];
        ppm_relay_enable (en);

        /* skip to the next pulse program command. */
        i_pulprog += 1;
        break;
#endif

      /* acquisition. */
#ifdef PPM_PULPROG_HAVE_ACQUIRE
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
#endif

      /* adiabatic polarization rising edge. */
#ifdef PPM_PULPROG_HAVE_TX
      case PPM_PULPROG_TXRISE:
        /* build the sample count word. */
        d16 = WORD (pulprog[i_pulprog + 1], pulprog[i_pulprog + 2]);

        /* build the amplitude step word. */
        a16 = WORD (pulprog[i_pulprog + 3], pulprog[i_pulprog + 4]);

        /* run the rising edge command. */
        dac16 = ppm_txrise (d16, a16, dac16);

        /* skip to the next pulse program command. */
        i_pulprog += 4;
        break;
#endif

      /* adiabatic polarization falling edge. */
#ifdef PPM_PULPROG_HAVE_TX
      case PPM_PULPROG_TXFALL:
        /* build the sample count word. */
        d16 = WORD (pulprog[i_pulprog + 1], pulprog[i_pulprog + 2]);

        /* build the amplitude step word. */
        a16 = WORD (pulprog[i_pulprog + 3], pulprog[i_pulprog + 4]);

        /* run the falling edge command. */
        dac16 = ppm_txfall (d16, a16, dac16);

        /* skip to the next pulse program command. */
        i_pulprog += 4;
        break;
#endif

      /* sinusoidal pulse. */
#ifdef PPM_PULPROG_HAVE_TX
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

        /* return the transmit coil dac output to zero. */
        spi_write_dac_a (0x0000);

        /* skip to the next pulse program command. */
        i_pulprog += 9;
        break;
#endif

      /* capacitive tuning. */
#ifdef PPM_PULPROG_HAVE_TUNE
      case PPM_PULPROG_TUNE:
        /* build the tuning word. */
        a16 = WORD (pulprog[i_pulprog + 1], pulprog[i_pulprog + 2]);

        /* run the tuning command. */
        ppm_settune (a16);

        /* skip to the next pulse program command. */
        i_pulprog += 2;
        break;
#endif

      /* x-axis shim. */
#ifdef PPM_PULPROG_HAVE_SHIM_X
      case PPM_PULPROG_SHIM_X:
        /* build the shimming word. */
        a16 = WORD (pulprog[i_pulprog + 1], pulprog[i_pulprog + 2]);

        /* run the shimming command. */
        ppm_setshim_x (a16);

        /* skip to the next pulse program command. */
        i_pulprog += 2;
        break;
#endif

      /* y-axis shim. */
#ifdef PPM_PULPROG_HAVE_SHIM_Y
      case PPM_PULPROG_SHIM_Y:
        /* build the shimming word. */
        a16 = WORD (pulprog[i_pulprog + 1], pulprog[i_pulprog + 2]);

        /* run the shimming command. */
        ppm_setshim_y (a16);

        /* skip to the next pulse program command. */
        i_pulprog += 2;
        break;
#endif

      /* z-axis shim. */
#ifdef PPM_PULPROG_HAVE_SHIM_Z
      case PPM_PULPROG_SHIM_Z:
        /* build the shimming word. */
        a16 = WORD (pulprog[i_pulprog + 1], pulprog[i_pulprog + 2]);

        /* run the shimming command. */
        ppm_setshim_z (a16);

        /* skip to the next pulse program command. */
        i_pulprog += 2;
        break;
#endif

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

/* ppm_pulprog_size: sends the number of pulse program bytes to the host.
 */
void ppm_pulprog_size (void) {
  /* send the values. */
  usb_cdc_putchar (PPM_MSG_DEVICE_PP_SIZE);
  usb_cdc_putchar (MSB (SZ_PULPROG));
  usb_cdc_putchar (LSB (SZ_PULPROG));
  usb_cdc_putchar (PPM_MSG_DEVICE_DONE);
}

/* ppm_pulprog_recv: receives a new pulse program from the host.
 */
void ppm_pulprog_recv (void) {
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

  /* loop from the end of the received data to the end of the array. */
  for (i_pulprog = n_pulprog; i_pulprog < SZ_PULPROG; i_pulprog++) {
    /* zero the currently indexed byte. */
    pulprog[i_pulprog] = 0x00;
  }

  /* zero the pulse program index. */
  i_pulprog = 0;

  /* send an acknowledgement that the pulse program was set. */
  ppm_done ();
}

/* ppm_pulprog_send: sends the current pulse program to the host.
 */
void ppm_pulprog_send (void) {
  /* write the response byte. */
  usb_cdc_putchar (PPM_MSG_DEVICE_PP_DATA);

  /* loop through the pulse program array bytes. */
  for (i_pulprog = 0; i_pulprog < SZ_PULPROG; i_pulprog++) {
    /* write the next byte in the pulse program. */
    usb_cdc_putchar (pulprog[i_pulprog]);
  }

  /* zero the pulse program index. */
  i_pulprog = 0;

  /* send a final completion byte. */
  ppm_done ();
}

