
/* ppm-hw.c: source file for low-level hardware comms with ppm devices.
 * Copyright (C) 2014  Bradley Worley  <geekysuavo@gmail.com>.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to:
 *
 *   Free Software Foundation, Inc.
 *   51 Franklin Street, Fifth Floor
 *   Boston, MA  02110-1301, USA.
 */

/* include the PPM header. */
#include "ppm.h"

/* ppm_device_open: opens the PPM device file. */
int ppm_device_open (const char *fname) {
  /* declare required variables. */
  const char *filename;
  struct termios tios;
  int fd;

  /* determine the device filename to open. */
  if (fname == NULL)
    filename = PPM_DEVICE_FILENAME;
  else
    filename = fname;

  /* open the device file. */
  fd = open (filename, O_RDWR | O_NOCTTY);

  /* ensure the file was opened. */
  if (fd == -1) {
    /* output an error. */
    debugf ("failed to open '%s'", filename);

    /* return an error. */
    return -1;
  }

  /* configure the serial port settings. */
  tcgetattr (fd, &tios);
  cfmakeraw (&tios);
  tcsetattr (fd, TCSANOW, &tios);

  /* return success. */
  return fd;
}

/* ppm_device_close: closes the PPM device file. */
void ppm_device_close (int fd) {
  /* declare required variables. */
  int status;

  /* set the DTR line low on the device.
   * this may be absolutely unneccessary.
   */
  ioctl (fd, TIOCMGET, &status);
  status &= ~TIOCM_DTR;
  ioctl (fd, TIOCMSET, &status);

  /* close the device file. */
  close (fd);
}

/* ppm_chk: opens and closes the device file. */
int ppm_chk (const char *fname) {
  /* declare required variables. */
  int fd;

  /* open the device file. */
  if ((fd = ppm_device_open (fname)) == -1) {
    /* return failure. */
    return 0;
  }

  /* close the device file. */
  ppm_device_close (fd);

  /* return success. */
  return 1;
}

/* ppm_rst: resets the device. */
int ppm_rst (const char *fname) {
  /* declare required variables. */
  uint8_t buf[PPM_VERMSG_BYTES];
  int i, fd;

  /* zero the buffer. */
  for (i = 0; i < PPM_VERMSG_BYTES; i++)
    buf[i] = 0x00;

  /* open the device file. */
  if ((fd = ppm_device_open (fname)) == -1) {
    /* output an error. */
    debugf ("failed to open device file");

    /* return failure. */
    return 0;
  }

  /* set up the message buffer. */
  buf[0] = PPM_MSG_HOST_RESET;

  /* write the message. */
  if (write (fd, buf, 1) != 1) {
    /* output an error. */
    debugf ("failed to write '%02x'", buf[0]);

    /* close the device file. */
    ppm_device_close (fd);

    /* return an error. */
    return 0;
  }

  /* close the device file. */
  ppm_device_close (fd);

  /* return success. */
  return 1;
}

/* ppm_ver_fd: reads version values from the device. */
int ppm_ver_fd (int fd, int *ver, int *rev) {
  /* declare required variables. */
  uint8_t buf[PPM_VERMSG_BYTES];
  int i, n;

  /* zero the buffer. */
  for (i = 0; i < PPM_VERMSG_BYTES; i++)
    buf[i] = 0x00;

  /* set up the message buffer. */
  buf[0] = PPM_MSG_HOST_VERSION;

  /* write the message. */
  if (write (fd, buf, 1) != 1) {
    /* output an error. */
    debugf ("failed to write '%02x'", buf[0]);
    return 0;
  }

  /* read back the device response. */
  n = read (fd, buf, PPM_VERMSG_BYTES);

  /* see if the number of expected bytes was read. */
  if (n != PPM_VERMSG_BYTES) {
    /* output an error. */
    debugf ("failed to read version (%d != %d)",
            n, PPM_VERMSG_BYTES);

    /* return an error. */
    return 0;
  }

  /* check the last byte. */
  if (buf[PPM_VERMSG_BYTES - 1] != PPM_MSG_DEVICE_DONE) {
    /* output an error message. */
    debugf ("invalid buffer end '%02x'",
            buf[PPM_VERMSG_BYTES - 1]);

    /* return an error. */
    return 0;
  }

  /* store the read version information. */
  *ver = (buf[1] & 0xf0) >> 4;
  *rev = buf[1] & 0x0f;

  /* return success. */
  return 1;
}

/* ppm_szpp_fd: pulse program size request routine. */
unsigned int ppm_szpp_fd (int fd) {
  /* declare required variables. */
  uint8_t buf[PPM_SIZMSG_BYTES];
  unsigned int sz;
  int n;

  /* set up the pulprog size request message header. */
  buf[0] = PPM_MSG_HOST_PP_SIZE;

  /* write the size request message. */
  if (write (fd, buf, 1) != 1) {
    /* output an error. */
    debugf ("failed to write '%02x'", buf[0]);
    return 0;
  }

  /* read back the device response. */
  n = read (fd, buf, PPM_SIZMSG_BYTES);

  /* check the size. */
  if (n != 4) {
    /* output an error. */
    debugf ("failed to read size (%d != %d)",
            n, PPM_SIZMSG_BYTES);

    /* return failure. */
    return 0;
  }

  /* get the number of bytes. */
  sz = WORD (buf[1], buf[2]);

  /* return success. */
  return sz;
}

/* ppm_wpp_fd: pulse program host -> device transfer routine. */
int ppm_wpp_fd (int fd, ppm_prog *pp) {
  /* declare required variables. */
  uint8_t buf[PPM_BIGMSG_BYTES];
  int i, n, n_buf;

  /* set up the message header. */
  buf[0] = PPM_MSG_HOST_PP_WRITE;

  /* write the number of bytes. */
  buf[1] = MSB (pp->n);
  buf[2] = LSB (pp->n);

  /* set the buffer contents. */
  for (i = 0; i < pp->n; i++)
    buf[i + 3] = pp->bytes[i];

  /* write the message. */
  n_buf = pp->n + 3;
  if (write (fd, buf, n_buf) != n_buf) {
    /* output an error. */
    debugf ("failed to write '%02x'", buf[0]);
    return 0;
  }

  /* read back the device response. */
  n = read (fd, buf, 1);

  /* check the last byte. */
  if (n != 1 || buf[0] != PPM_MSG_DEVICE_DONE) {
    /* output an error message and return failure. */
    debugf ("invalid buffer end '%02x'", buf[0]);
    return 0;
  }

  /* return success. */
  return 1;
}

/* ppm_rpp_fd: pulse program device -> host transfer routine. */
int ppm_rpp_fd (int fd, ppm_prog *pp) {
  /* declare required variables. */
  uint8_t buf[PPM_BIGMSG_BYTES];
  int i, n, n_read, n_bytes;

  /* request the size of the pulse program array. */
  n_bytes = ppm_szpp_fd (fd);

  /* ensure the request was fulfilled. */
  if (!n_bytes) {
    /* output an error. */
    debugf ("failed to get pulprog size");
    return 0;
  }

  /* allocate the local array memory. */
  if (!ppm_prog_alloc (pp, n_bytes)) {
    /* output an error. */
    debugf ("failed to allocate pulse program array");
    return 0;
  }

  /* zero the buffer. */
  for (i = 0; i < PPM_BIGMSG_BYTES; i++)
    buf[i] = 0x00;

  /* set up the message buffer. */
  buf[0] = PPM_MSG_HOST_PP_READ;

  /* write the message. */
  if (write (fd, buf, 1) != 1) {
    /* output an error. */
    debugf ("failed to write '%02x'", buf[0]);
    return 0;
  }

  /* attempt to read in the expected number of bytes. */
  for (n_read = 0; n_read < n_bytes + 2;) {
    /* read the next chunk of available bytes. */
    n = read (fd, buf + n_read, n_bytes + 2 - n_read);

    /* accumulate the number of bytes read. */
    n_read += n;
  }

  /* check the last byte. */
  if (buf[n_bytes + 1] != PPM_MSG_DEVICE_DONE) {
    /* output an error message. */
    debugf ("invalid buffer end '%02x'",
            buf[n_bytes + 1]);

    /* return an error. */
    return 0;
  }

  /* write the read bytes into the pulprog structure. */
  for (i = 0; i < n_bytes; i++) {
    /* store the pulse program byte. */
    pp->bytes[i] = buf[i + 1];
  }

  /* return success. */
  return 1;
}

/* ppm_zg_fd: data acquisition routine. */
int ppm_zg_fd (int fd, ppm_prog *pp, ppm_data *acq) {
  /* declare required variables. */
  uint8_t buf[PPM_VERMSG_BYTES], *bytes;
  int i, n, n_read, n_bytes, n_samples;

  /* zero the buffer. */
  for (i = 0; i < PPM_VERMSG_BYTES; i++)
    buf[i] = 0x00;

  /* set up the message buffer. */
  buf[0] = PPM_MSG_HOST_EXECUTE;

  /* compute the number of samples we should expect from the pulse program.
   * the number of read bytes will be twice this value.
   */
  n_samples = ppm_prog_samples (pp);
  n_bytes = 2 * n_samples;

  /* check if the sample count is nonzero. */
  if (n_bytes) {
    /* yes. allocate a temporary storage location. */
    bytes = (uint8_t *) calloc (n_bytes, sizeof (uint8_t));

    /* ensure the data array was allocated. */
    if (!bytes) {
      /* output an error. */
      debugf ("failed to allocate sample array");
      return 0;
    }

    /* allocate memory for the final acquired values. */
    if (!ppm_data_alloc (acq, n_samples)) {
      /* output an error. */
      debugf ("failed to allocate acquisition structure");
      return 0;
    }
  }
  else {
    /* no. null out the storage location. */
    bytes = NULL;
  }

  /* write the execute message to the device. */
  if (write (fd, buf, 1) != 1) {
    /* output an error. */
    debugf ("failed to send command '%02x'", buf[0]);
    return 0;
  }

  /* attempt to read in the expected number of bytes. */
  for (n_read = 0; n_read < n_bytes;) {
    /* read the next chunk of available bytes. */
    n = read (fd, bytes + n_read, n_bytes - n_read);

    /* accumulate the number of bytes read. */
    n_read += n;
  }

  /* read the last byte from the device. */
  n = read (fd, buf, 1);

  /* check that it was a success byte. */
  if (n != 1 || buf[0] != PPM_MSG_DEVICE_DONE) {
    /* output an error. */
    debugf ("failed to run pulse program (%02x)", buf[0]);
    return 0;
  }

  /* check if acquired samples are ready to be dumped into the acquisition
   * structure.
   */
  if (n_samples) {
    /* loop through the samples, building the output values. */
    for (i = 0; i < n_samples; i++) {
      /* build the voltage value. */
      acq->v[i] = SAMPLE_VOLT (bytes[2 * i], bytes[2 * i + 1]);
    }

    /* build the time values in the acquisition structure. */
    ppm_prog_timings (pp, acq);

    /* free the temporary array. */
    free (bytes);
    bytes = NULL;
  }

  /* return success. */
  return 1;
}

/* ppm_ver: read version information from the device. */
int ppm_ver (const char *fname, int *ver, int *rev) {
  /* declare required variables. */
  int fd, ret;

  /* open the device file. */
  if ((fd = ppm_device_open (fname)) == -1) {
    /* output an error. */
    debugf ("failed to open device file");

    /* return failure. */
    return 0;
  }

  /* run the version request code on the file descriptor. */
  ret = ppm_ver_fd (fd, ver, rev);

  /* close the device file. */
  ppm_device_close (fd);

  /* output a message. */
  if (ret)
    debugf ("VER: %d.%d OK", *ver, *rev);
  else
    debugf ("VER: ERR");

  /* return the result. */
  return ret;
}

/* ppm_wpp: pulse program host -> device transfer routine. */
int ppm_wpp (const char *fname, ppm_prog *pp) {
  /* declare required variables. */
  int fd, ret;

  /* open the device file. */
  if ((fd = ppm_device_open (fname)) == -1) {
    /* output an error. */
    debugf ("failed to open device file");

    /* return failure. */
    return 0;
  }

  /* run the transfer code on the file descriptor. */
  ret = ppm_wpp_fd (fd, pp);

  /* close the device file. */
  ppm_device_close (fd);

  /* output a message. */
  if (ret)
    debugf ("WPP: %u OK", pp->n);
  else
    debugf ("WPP: ERR");

  /* return the result. */
  return ret;
}

/* ppm_rpp: pulse program device -> host transfer routine. */
int ppm_rpp (const char *fname, ppm_prog *pp) {
  /* declare required variables. */
  int fd, ret;

  /* open the device file. */
  if ((fd = ppm_device_open (fname)) == -1) {
    /* output an error. */
    debugf ("failed to open device file");

    /* return failure. */
    return 0;
  }

  /* run the transfer code on the file descriptor. */
  ret = ppm_rpp_fd (fd, pp);

  /* close the device file. */
  ppm_device_close (fd);

  /* output a message. */
  if (ret)
    debugf ("RPP: %u OK", pp->n);
  else
    debugf ("RPP ERR");

  /* return the result. */
  return ret;
}

/* ppm_zg: data acquisition routine. */
int ppm_zg (const char *fname, ppm_prog *pp, ppm_data *acq) {
  /* declare required variables. */
  int fd, ret;

  /* open the device file. */
  if ((fd = ppm_device_open (fname)) == -1) {
    /* output an error. */
    debugf ("failed to open device file");

    /* return failure. */
    return 0;
  }

  /* run the acquisition code on the file descriptor. */
  ret = ppm_zg_fd (fd, pp, acq);

  /* close the device file. */
  ppm_device_close (fd);

  /* output a message. */
  if (ret)
    debugf ("ZG: %u OK", acq->n);
  else
    debugf ("ZG: ERR");

  /* return the result. */
  return ret;
}
