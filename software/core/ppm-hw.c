
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
    fprintf (stderr, "error: failed to open '%s'\n", filename);

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
  uint8_t buf[PPM_PARMSG_BYTES];
  int i, fd;

  /* zero the buffer. */
  for (i = 0; i < PPM_PARMSG_BYTES; i++)
    buf[i] = 0x00;

  /* open the device file. */
  if ((fd = ppm_device_open (fname)) == -1) {
    /* output an error. */
    fprintf (stderr, "error: failed to open device file.\n");

    /* return failure. */
    return 0;
  }

  /* set up the message buffer. */
  buf[0] = PPM_MSG_HOST_RESET;

  /* write the message. */
  if (write (fd, buf, 1) != 1) {
    /* output an error. */
    fprintf (stderr, "error: failed to write '%02x'\n", buf[0]);

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

/* ppm_dactest: tests the dac on the device. */
int ppm_dactest (const char *fname) {
  /* declare required variables. */
  uint8_t buf[PPM_PARMSG_BYTES];
  int i, fd;

  /* zero the buffer. */
  for (i = 0; i < PPM_PARMSG_BYTES; i++)
    buf[i] = 0x00;

  /* open the device file. */
  if ((fd = ppm_device_open (fname)) == -1) {
    /* output an error and return failure. */
    fprintf (stderr, "error: failed to open device file.\n");
    return 0;
  }

  /* set up the message buffer. */
  buf[0] = PPM_MSG_HOST_TEST_DAC;

  /* write the message. */
  if (write (fd, buf, 1) != 1) {
    /* output an error. */
    fprintf (stderr, "error: failed to write '%02x'\n", buf[0]);

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

/* ppm_scbtest: tests the scb dac outputs on the device. */
int ppm_scbtest (const char *fname) {
  /* declare required variables. */
  ppm_shims shims;
  int i, fd;

  /* open the device file. */
  if ((fd = ppm_device_open (fname)) == -1) {
    /* output an error and return failure. */
    fprintf (stderr, "error: failed to open device file.\n");
    return 0;
  }

  /* loop through the test indices. */
  for (i = 0; i <= 1536; i++) {
    /* compute the x-shim output value. */
    if (i <= 1024)
      shims.f_x = sin (2.0 * M_PI * ((double) i) / 1024.0);
    else
      shims.f_x = 0.0;

    /* compute the y-shim output value. */
    if (i >= 256 && i <= 1280)
      shims.f_y = sin (2.0 * M_PI * ((double) i) / 1024.0 + 256.0);
    else
      shims.f_y = 0.0;

    /* compute the z-shim output value. */
    if (i >= 512)
      shims.f_z = sin (2.0 * M_PI * ((double) i) / 1024.0 + 512.0);
    else
      shims.f_z = 0.0;

    /* dehumanize the shims. */
    if (!ppm_shims_dehumanize (&shims)) {
      /* output an error, close the device file and return failure. */
      fprintf (stderr, "error: failed to dehumanize shims.\n");
      ppm_device_close (fd);
      return 0;
    }

    /* send the new shim values to the device. */
    if (!ppm_wsh_fd (fd, &shims)) {
      /* output an error, close the device file and return failure. */
      fprintf (stderr, "error: failed to write new shims.\n");
      ppm_device_close (fd);
      return 0;
    }
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
    fprintf (stderr, "error: failed to write '%02x'\n", buf[0]);
    return 0;
  }

  /* read back the device response. */
  n = read (fd, buf, PPM_VERMSG_BYTES);

  /* see if the number of expected bytes was read. */
  if (n != PPM_VERMSG_BYTES) {
    /* output an error. */
    fprintf (stderr, "error: failed to read version (%d != %d)\n",
             n, PPM_VERMSG_BYTES);

    /* return an error. */
    return 0;
  }

  /* check the last byte. */
  if (buf[PPM_VERMSG_BYTES - 1] != PPM_MSG_DEVICE_DONE) {
    /* output an error message. */
    fprintf (stderr, "error: invalid buffer end '%02x'\n",
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

/* ppm_rpar_fd: reads parameters from the device. */
int ppm_rpar_fd (int fd, ppm_parms *parms) {
  /* declare required variables. */
  uint8_t buf[PPM_PARMSG_BYTES];
  int i, n;

  /* zero the buffer. */
  for (i = 0; i < PPM_PARMSG_BYTES; i++)
    buf[i] = 0x00;

  /* set up the message buffer. */
  buf[0] = PPM_MSG_HOST_READ_PARMS;

  /* write the message. */
  if (write (fd, buf, 1) != 1) {
    /* output an error. */
    fprintf (stderr, "error: failed to write '%02x'\n", buf[0]);
    return 0;
  }

  /* read back the device response. */
  n = read (fd, buf, PPM_PARMSG_BYTES);

  /* see if the number of expected bytes was read. */
  if (n != PPM_PARMSG_BYTES) {
    /* output an error. */
    fprintf (stderr, "error: failed to read parms (%d != %d)\n",
             n, PPM_PARMSG_BYTES);

    /* return an error. */
    return 0;
  }

  /* check the last byte. */
  if (buf[PPM_PARMSG_BYTES - 1] != PPM_MSG_DEVICE_DONE) {
    /* output an error message. */
    fprintf (stderr, "error: invalid buffer end '%02x'\n",
             buf[PPM_PARMSG_BYTES - 1]);

    /* return an error. */
    return 0;
  }

  /* store the read parameters. */
  parms->polarize_ovf = WORD (buf[0], buf[1]);
  parms->acquire_ovf = WORD (buf[2], buf[3]);
  parms->acquire_count = WORD32 (buf[4], buf[5], buf[6], buf[7]);
  parms->deadtime_pol = WORD (buf[8], buf[9]);
  parms->deadtime_acq = WORD (buf[10], buf[11]);
  parms->ccs_value = WORD (buf[12], buf[13]);

  /* return success. */
  return 1;
}

/* ppm_wpar_fd: write parameters to the device. */
int ppm_wpar_fd (int fd, ppm_parms *parms) {
  /* declare required variables. */
  uint8_t buf[PPM_PARMSG_BYTES];
  int i;

  /* zero the buffer. */
  for (i = 0; i < PPM_PARMSG_BYTES; i++)
    buf[i] = 0x00;

  /* set the message byte. */
  buf[0] = PPM_MSG_HOST_WRITE_PARMS;

  /* set the polarize overflow word. */
  buf[1] = MSB (parms->polarize_ovf);
  buf[2] = LSB (parms->polarize_ovf);

  /* set the acquire overflow word. */
  buf[3] = MSB (parms->acquire_ovf);
  buf[4] = LSB (parms->acquire_ovf);

  /* set the acquire count word. */
  buf[5] = BYTE3 (parms->acquire_count);
  buf[6] = BYTE2 (parms->acquire_count);
  buf[7] = BYTE1 (parms->acquire_count);
  buf[8] = BYTE0 (parms->acquire_count);

  /* set the polarize deadtime word. */
  buf[9] = MSB (parms->deadtime_pol);
  buf[10] = LSB (parms->deadtime_pol);

  /* set the acquire deadtime word. */
  buf[11] = MSB (parms->deadtime_acq);
  buf[12] = LSB (parms->deadtime_acq);

  /* set the current sink set-point. */
  buf[13] = MSB (parms->ccs_value);
  buf[14] = LSB (parms->ccs_value);

  /* send the command to the device. */
  if (write (fd, buf, PPM_PARMSG_BYTES) != PPM_PARMSG_BYTES) {
    /* output an error. */
    fprintf (stderr, "error: failed to send command '%02x'\n", buf[0]);
    return 0;
  }

  /* read in the response byte from the device. */
  if (read (fd, buf, 1) != 1 || buf[0] != PPM_MSG_DEVICE_DONE) {
    /* output an error. */
    fprintf (stderr, "error: device returned response '%02x'\n", buf[0]);

    /* return an error. */
    return 0;
  }

  /* return success. */
  return 1;
}

/* ppm_rsh_fd: read shims from the device. */
int ppm_rsh_fd (int fd, ppm_shims *shims) {
  /* declare required variables. */
  uint8_t buf[PPM_SHIMSG_BYTES];
  int i, n;

  /* zero the buffer. */
  for (i = 0; i < PPM_SHIMSG_BYTES; i++)
    buf[i] = 0x00;

  /* set up the message buffer. */
  buf[0] = PPM_MSG_HOST_READ_SHIMS;

  /* write the message. */
  if (write (fd, buf, 1) != 1) {
    /* output an error. */
    fprintf (stderr, "error: failed to write '%02x'\n", buf[0]);
    return 0;
  }

  /* read back the device response. */
  n = read (fd, buf, PPM_SHIMSG_BYTES);

  /* see if the number of expected bytes was read. */
  if (n != PPM_SHIMSG_BYTES) {
    /* output an error. */
    fprintf (stderr, "error: failed to read shims (%d != %d)\n",
             n, PPM_SHIMSG_BYTES);

    /* return an error. */
    return 0;
  }

  /* check the last byte. */
  if (buf[PPM_SHIMSG_BYTES - 1] != PPM_MSG_DEVICE_DONE) {
    /* output an error message. */
    fprintf (stderr, "error: invalid buffer end '%02x'\n",
             buf[PPM_SHIMSG_BYTES - 1]);

    /* return an error. */
    return 0;
  }

  /* store the read parameters. */
  shims->x = WORD (buf[0], buf[1]);
  shims->y = WORD (buf[2], buf[3]);
  shims->z = WORD (buf[4], buf[5]);

  /* return success. */
  return 1;
}

/* ppm_wsh_fd: write shims to the device. */
int ppm_wsh_fd (int fd, ppm_shims *shims) {
  /* declare required variables. */
  uint8_t buf[PPM_SHIMSG_BYTES];
  int i;

  /* zero the buffer. */
  for (i = 0; i < PPM_SHIMSG_BYTES; i++)
    buf[i] = 0x00;

  /* set the message byte. */
  buf[0] = PPM_MSG_HOST_WRITE_SHIMS;

  /* set the x-shim value. */
  buf[1] = MSB (shims->x);
  buf[2] = LSB (shims->x);

  /* set the y-shim value. */
  buf[3] = MSB (shims->y);
  buf[4] = LSB (shims->y);

  /* set the z-shim value. */
  buf[5] = MSB (shims->z);
  buf[6] = LSB (shims->z);

  /* send the command to the device. */
  if (write (fd, buf, PPM_SHIMSG_BYTES) != PPM_SHIMSG_BYTES) {
    /* output an error. */
    fprintf (stderr, "error: failed to send command '%02x'\n", buf[0]);
    return 0;
  }

  /* read the response byte from the device. */
  if (read (fd, buf, 1) != 1 || buf[0] != PPM_MSG_DEVICE_DONE) {
    /* output an error. */
    fprintf (stderr, "error: device returned response '%02x'\n", buf[0]);
    return 0;
  }

  /* return success. */
  return 1;
}

/* ppm_zg_fd: data acquisition routine. */
int ppm_zg_fd (int fd, ppm_parms *parms, ppm_data *acq) {
  /* declare required variables. */
  uint8_t buf[PPM_PARMSG_BYTES], *samples;
  int i, n, scan, n_total, n_samples;
  unsigned int d1;

  /* zero the buffer. */
  for (i = 0; i < PPM_PARMSG_BYTES; i++)
    buf[i] = 0x00;

  /* allocate memory for the acquired samples. */
  n_samples = 2 * parms->acquire_count;
  samples = (uint8_t *) calloc (n_samples, sizeof (uint8_t));

  /* make sure the arrays were allocated successfully. */
  if (!samples) {
    /* output an error. */
    fprintf (stderr, "error: failed to allocate sampled data arrays\n");
    return 0;
  }

  /* allocate memory for the final acquired values. */
  if (!ppm_data_alloc (acq, parms->acquire_count)) {
    /* output an error. */
    fprintf (stderr, "error: failed to allocate acquisition structure\n");
    return 0;
  }

  /* calculate the interscan delay. (minimum: 1 second) */
  if (parms->ccs_value) {
    /* only delay between scans when polarizing. */
    d1 = (unsigned int) round ((double) parms->polarize_ovf * 16.384e-3);
    if (!d1) d1 = 1;
  }
  else {
    /* no delay. */
    d1 = 0;
  }

  /* loop for the number of scans. */
  for (scan = 0; scan < parms->ns; scan++) {
    /* delay for the polarization time between scans. */
    if (scan && d1) sleep (d1);

    /* set the execute message. */
    buf[0] = PPM_MSG_HOST_EXECUTE;

    /* write the execute message to the device. */
    if (write (fd, buf, 1) != 1) {
      /* output an error. */
      fprintf (stderr, "error: failed to send command '%02x'\n", buf[0]);
      return 0;
    }

    /* read back the response from the device. */
    n = read (fd, &buf[0], 1);

    /* wait for the acquisition to begin. */
    while (buf[0] != PPM_MSG_DEVICE_BEGIN)
      n = read (fd, &buf[0], 1);

    /* attempt to read in the expected number of bytes. */
    for (n_total = 0; n_total < n_samples;) {
      /* read the next chunk of available bytes. */
      n = read (fd, samples + n_total, n_samples - n_total);

      /* accumulate the number of bytes read. */
      n_total += n;
    }

    /* loop through the sampled bytes, building the output values. */
    for (i = 0; i < parms->acquire_count; i++) {
      /* build the time and voltage values. */
      acq->x[i] = SAMPLE_TIME (i);
      acq->v[i] += SAMPLE_VOLT (samples[2 * i], samples[2 * i + 1]);
    }
  } /* scans */

  /* loop finally to scale the voltages back to the correct range. */
  for (i = 0; i < parms->acquire_count; i++)
    acq->v[i] /= (double) parms->ns;

  /* free the acquired sample memory. */
  free (samples);
  samples = NULL;

  /* return success. */
  return 1;
}

/* ppm_ver: read version information from the device. */
int ppm_ver (const char *fname, int *ver, int *rev) {
  /* declare required variables. */
  int fd, ret;

  /* output a message. */
  fprintf (stderr, "VER:");

  /* open the device file. */
  if ((fd = ppm_device_open (fname)) == -1) {
    /* output an error. */
    fprintf (stderr, "error: failed to open device file.\n");

    /* return failure. */
    return 0;
  }

  /* run the version request code on the file descriptor. */
  ret = ppm_ver_fd (fd, ver, rev);

  /* close the device file. */
  ppm_device_close (fd);

  /* output a message. */
  if (ret)
    fprintf (stdout, " %d.%d\n", *ver, *rev);
  else
    fprintf (stderr, " ERR\n");

  /* return the result. */
  return ret;
}

/* ppm_rpar: read parameters from the device. */
int ppm_rpar (const char *fname, ppm_parms *parms) {
  /* declare required variables. */
  int fd, ret;

  /* output a message. */
  fprintf (stderr, "RPAR:");

  /* open the device file. */
  if ((fd = ppm_device_open (fname)) == -1) {
    /* output an error. */
    fprintf (stderr, "error: failed to open device file.\n");

    /* return failure. */
    return 0;
  }

  /* run the parameter request code on the file descriptor. */
  ret = ppm_rpar_fd (fd, parms);

  /* close the device file. */
  ppm_device_close (fd);

  /* output a message. */
  if (ret) {
    /* show the status parameters.. */
    fprintf (stderr, " %u %u %u %u %u %u OK\n",
      parms->polarize_ovf,
      parms->acquire_ovf,
      parms->acquire_count,
      parms->deadtime_pol,
      parms->deadtime_acq,
      parms->ccs_value);
  }
  else
    fprintf (stderr, " ERR\n");

  /* return the result. */
  return ret;
}

/* ppm_wpar: write parameters to the device. */
int ppm_wpar (const char *fname, ppm_parms *parms) {
  /* declare required variables. */
  int fd, ret;

  /* output a message. */
  fprintf (stderr, "WPAR:");

  /* open the device file. */
  if ((fd = ppm_device_open (fname)) == -1) {
    /* output an error. */
    fprintf (stderr, "error: failed to open device file.\n");

    /* return failure. */
    return 0;
  }

  /* run the parameter send code on the file descriptor. */
  ret = ppm_wpar_fd (fd, parms);

  /* close the device file. */
  ppm_device_close (fd);

  /* output a message. */
  if (ret)
    fprintf (stderr, " OK\n");
  else
    fprintf (stderr, " ERR\n");

  /* return the result. */
  return ret;
}

/* ppm_rsh: read shims from the device. */
int ppm_rsh (const char *fname, ppm_shims *shims) {
  /* declare required variables. */
  int fd, ret;

  /* output a message. */
  fprintf (stderr, "RSH:");

  /* open the device file. */
  if ((fd = ppm_device_open (fname)) == -1) {
    /* output an error. */
    fprintf (stderr, "error: failed to open device file.\n");

    /* return failure. */
    return 0;
  }

  /* run the shim request code on the file descriptor. */
  ret = ppm_rsh_fd (fd, shims);

  /* close the device file. */
  ppm_device_close (fd);

  /* output a message. */
  if (ret) {
    /* show the status parameters.. */
    fprintf (stderr, " %u %u %u OK\n", shims->x, shims->y, shims->z);
  }
  else
    fprintf (stderr, " ERR\n");

  /* return the result. */
  return ret;
}

/* ppm_wsh: write shims to the device. */
int ppm_wsh (const char *fname, ppm_shims *shims) {
  /* declare required variables. */
  int fd, ret;

  /* output a message. */
  fprintf (stderr, "WSH:");

  /* open the device file. */
  if ((fd = ppm_device_open (fname)) == -1) {
    /* output an error. */
    fprintf (stderr, "error: failed to open device file.\n");

    /* return failure. */
    return 0;
  }

  /* run the shim send code on the file descriptor. */
  ret = ppm_wsh_fd (fd, shims);

  /* close the device file. */
  ppm_device_close (fd);

  /* output a message. */
  if (ret)
    fprintf (stderr, " OK\n");
  else
    fprintf (stderr, " ERR\n");

  /* return the result. */
  return ret;
}
/* ppm_zg: data acquisition routine. */
int ppm_zg (const char *fname, ppm_parms *parms, ppm_data *acq) {
  /* declare required variables. */
  int fd, ret;

  /* output a message. */
  fprintf (stderr, "ZG:");

  /* open the device file. */
  if ((fd = ppm_device_open (fname)) == -1) {
    /* output an error. */
    fprintf (stderr, "error: failed to open device file.\n");

    /* return failure. */
    return 0;
  }

  /* run the acquisition code on the file descriptor. */
  ret = ppm_zg_fd (fd, parms, acq);

  /* close the device file. */
  ppm_device_close (fd);

  /* output a message. */
  if (ret)
    fprintf (stderr, " OK\n");
  else
    fprintf (stderr, " ERR\n");

  /* return the result. */
  return ret;
}
