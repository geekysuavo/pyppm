
/* msg.c: PPM/SCB firmware host-device messages source code.
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

/* msg_loop: main message handling loop for all pyppm-compatible devices. */
void msg_loop (void) {
  /* declare a byte for reading command bytes. */
  int16_t host_command = 0;

  while (1) {
    /* wait for the user to open the device node with a terminal emulator
     * that sets DTR to indicate its ready to receive data.
     */
    while (!usb_cdc_control_line_dtr ());

    /* discard bytes received prior to receipt of the DTR signal. */
    usb_cdc_flush_input ();

    /* turn on the first led to show connection. */
    gpio_led_comm_on ();

    /* now sit in a wait state for the next byte. */
    while (1) {
      /* try to read a byte from the host. */
      host_command = usb_cdc_getchar ();

      /* was a byte read from the host? */
      if (host_command != -1) {
        /* act based on the host command. */
        switch (LSB (host_command)) {
          /* read pulse program size. */
          case PPM_MSG_HOST_PP_SIZE:
            ppm_pulprog_size ();
            break;

          /* transfer pulse program (host -> device). */
          case PPM_MSG_HOST_PP_WRITE:
            ppm_pulprog_recv ();
            break;

          /* transfer pulse program (device -> host). */
          case PPM_MSG_HOST_PP_READ:
            ppm_pulprog_send ();
            break;

          /* execute pulse program. */
          case PPM_MSG_HOST_EXECUTE:
            ppm_execute ();
            break;

          /* read version. */
          case PPM_MSG_HOST_VERSION:
            ppm_version ();
            break;

          /* reset device. */
          case PPM_MSG_HOST_RESET:
            ppm_reset ();
            break;

          /* read parameters. */
          default:
            break;
        }
      }

      /* see if the usb has been de-configured or if the user disconnected. */
      if (!usb_configured () || !usb_cdc_control_line_dtr ()) break;
    }

    /* turn off the first "connection" led. */
    _delay_ms (50.0);
    gpio_led_comm_off ();
  }
}

