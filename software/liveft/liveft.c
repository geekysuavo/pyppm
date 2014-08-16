
/* liveft: A live spectrum analyzer for the PPM device.
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

/* include required external library headers. */
#include <GL/glut.h>
#include <GL/gl.h>

/* define a macro to bound a number within a range. */
#define BOUND(x,xmin,xmax) (x < xmin ? xmin : x > xmax ? xmax : x)

/* define the number of FFT averages. */
#define N_ACQ 2048
#define N_AVE 8

/* declare required persistent PPM data elements. */
ppm_data tacq, facq[N_AVE], fave, fmem;
ppm_parms parms;
int i_ave, fd;

/* declare required persistent GL data elements. */
float xmin, xmax, xmark, ymin, ymax, ymark, step;
int win;

/* liveft_gl_init: window init function for GLUT.
 */
void liveft_gl_init (void) {
  /* initialize the averaging index. */
  i_ave = 0;

  /* initialize the boundaries. */
  xmin = ymin = +1e9;
  xmax = ymax = -1e9;

  /* initialize the markers. */
  xmark = ymark = 0.0;
  step = 0.1;

  /* set a black background. */
  glClearColor (0.0f, 0.0f, 0.0f, 0.0f);

  /* enable clearing the depth buffer. */
  glClearDepth (1.0);

  /* enable depth testing. */
  glDepthFunc (GL_LESS);
  glEnable (GL_DEPTH_TEST);

  /* enable smooth color shading. */
  glShadeModel (GL_SMOOTH);

  /* reset the projection matrix. */
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();

  /* set up an orthographic projection. */
  glOrtho (0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);

  /* open the model view matrix. */
  glMatrixMode (GL_MODELVIEW);
}

/* liveft_gl_reshape: window resize callback for GLUT.
 * @width: new window width.
 * @height: new window height.
 */
void liveft_gl_reshape (int width, int height) {
  /* reset the viewport. */
  glViewport (0, 0, width, height);

  /* reset the projection matrix. */
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();

  /* set up an orthographic projection. */
  glOrtho (0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);

  /* open the model view matrix. */
  glMatrixMode (GL_MODELVIEW);
}

/* liveft_gl_draw: redraw callback for GLUT.
 */
void liveft_gl_draw (void) {
  /* declare required variables. */
  float x1, x2, y1, y2;
  unsigned int i, j;

  /* acquire a new packet of data. */
  if (!ppm_zg_fd (fd, &parms, &tacq))
    glutDestroyWindow (win);

  /* fourier transform the time-domain data. */
  if (!ppm_fft (&tacq, &facq[i_ave]))
    glutDestroyWindow (win);

  /* allocate the frequency-domain average data. */
  if (!ppm_data_alloc (&fave, facq[0].n))
    glutDestroyWindow (win);

  /* increment the averaging index. */
  i_ave++;
  if (i_ave >= N_AVE)
    i_ave = 0;

  /* loop through the points in the frequency-domain data. */
  for (i = 0; i < fave.n; i++) {
    /* initialize the average value. */
    fave.x[i] = facq[0].x[i];
    fave.v[i] = 0.0;

    /* loop through the samples in the average. */
    for (j = 0; j < N_AVE && facq[j].n; j++)
      fave.v[i] += facq[j].v[i];

    /* check the x boundary. */
    if (fave.x[i] < xmin) xmin = fave.x[i];
    if (fave.x[i] > xmax) xmax = fave.x[i];

    /* check the y boundary. */
    if (fave.v[i] < ymin) ymin = fave.v[i];
    if (fave.v[i] > ymax) ymax = fave.v[i];
  }

  /* clear the screen and reset the view. */
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity ();

  /* begin drawing points. */
  glBegin (GL_POINTS);
  glColor4f (1.0f, 0.0f, 1.0f, 0.0f);

  /* see if memory data exists. */
  if (fmem.n) {
    /* loop through the memory data. */
    glColor4f (0.0f, 1.0f, 0.0f, 0.5f);
    for (i = 0; i < fmem.n; i++) {
      /* compute the coordinates. */
      x1 = (fmem.x[i] - xmin) / (xmax - xmin);
      y1 = (fmem.v[i] - ymin) / (ymax - ymin);

      /* draw the point. */
      glVertex3f (x1, y1, 0.0f);
    }
  }

  /* end drawing points. */
  glEnd ();

  /* begin drawing lines. */
  glBegin (GL_LINES);
  glColor4f (0.0f, 1.0f, 0.0f, 0.0f);

  /* loop again through the points in the frequency-domain data. */
  for (i = 1; i < fave.n; i++) {
    /* compute the start vertex coordinates. */
    x1 = (fave.x[i - 1] - xmin) / (xmax - xmin);
    y1 = (fave.v[i - 1] - ymin) / (ymax - ymin);

    /* compute the vertex coordinates. */
    x2 = (fave.x[i] - xmin) / (xmax - xmin);
    y2 = (fave.v[i] - ymin) / (ymax - ymin);

    /* draw the line. */
    glVertex3f (x1, y1, 0.0f);
    glVertex3f (x2, y2, 0.0f);
  }

  /* end drawing. */
  glEnd ();

  /* begin drawing points. */
  glBegin (GL_POINTS);
  glColor4f (0.0f, 1.0f, 0.0f, 0.5f);

  /* draw the vertical marker points. */
  for (y1 = 0.0; y1 < 1.0; y1 += 0.005)
    glVertex3f (xmark, y1, 0.0f);

  /* draw the horizontal marker points. */
  for (x1 = 0.0; x1 < 1.0; x1 += 0.005)
    glVertex3f (x1, ymark, 0.0f);

  /* end drawing. */
  glEnd ();

  /* swap the buffers. */
  glutSwapBuffers ();
}

/* liveft_set_marker: sets the marker location based on screen coords.
 * @x: the x-coordinate.
 * @y: the y-coordinate.
 */
void liveft_set_marker (int x, int y) {
  /* declare required variables. */
  int w, h;

  /* get the window size. */
  w = glutGet (GLUT_WINDOW_WIDTH);
  h = glutGet (GLUT_WINDOW_HEIGHT);

  /* set the marker. */
  xmark = BOUND (((double) x) / ((double) w), 0.0, 1.0);
  ymark = BOUND (1.0 - ((double) y) / ((double) h), 0.0, 1.0);
}

/* liveft_gl_keyboard: keypress callback for GLUT.
 * @key: the character of the keypress.
 * @x: window x-position of the mouse.
 * @y: window y-position of the mouse.
 */
void liveft_gl_keyboard (unsigned char key, int x, int y) {
  /* declare required variables. */
  unsigned int i;

  /* debounce... lol. */
  usleep (100);

  /* check which key was pressed. */
  if (key == 'q') {
    /* close the window. */
    glutDestroyWindow (win);
  }
  else if (key == 'm') {
    /* set the marker. */
    liveft_set_marker (x, y);
  }
  else if (key == 'w') {
    /* move the horizontal marker up. */
    ymark += step;
  }
  else if (key == 's') {
    /* move the horizontal marker down. */
    ymark -= step;
  }
  else if (key == 'a') {
    /* move the vertical marker left. */
    xmark -= step;
  }
  else if (key == 'd') {
    /* move the vertical marker right. */
    xmark += step;
  }
  else if (key == '[') {
    /* decrease the marker step size. */
    step /= 2.0;
  }
  else if (key == ']') {
    /* increase the marker step size. */
    step *= 2.0;
  }
  else if (key == ' ') {
    /* ensure the data is allocated. */
    if (!ppm_data_alloc (&fmem, fave.n))
      return;

    /* copy over the current waveform into the memory area. */
    for (i = 0; i < fave.n; i++) {
      fmem.x[i] = fave.x[i];
      fmem.v[i] = fave.v[i];
    }
  }

  /* check the marker boundaries. */
  xmark = BOUND (xmark, 0.0, 1.0);
  ymark = BOUND (ymark, 0.0, 1.0);
}

/* liveft_gl_mouse: mouse-click callback function for GLUT.
 * @button: the button that was clicked.
 * @state: the new state of the button.
 * @x: the window x-position of the mouse.
 * @y: the window y-position of the mouse.
 */
void liveft_gl_mouse (int button, int state, int x, int y) {
  /* check the mouse state. */
  if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
    /* set the marker. */
    liveft_set_marker (x, y);
  }
}

/* main: application entry point. */
int main (int argc, char **argv) {
  /* try to access the device. */
  fd = ppm_device_open (NULL);
  if (fd < 0)
    return 1;

  /* set up the desired device parameters for continuous acquisition. */
  parms.acquire_count = N_ACQ;
  parms.f_polarize_ovf = 0.0;
  parms.f_acquire_ovf = 10.0;
  parms.f_deadtime_pol = 1.0;
  parms.f_deadtime_acq = 1.0;
  parms.f_ccs_value = 0.0;
  parms.ns = 1;

  /* dehumanize the device parameters. */
  if (!ppm_parms_dehumanize (&parms))
    return 1;

  /* try to write the device parameters. */
  if (!ppm_wpar_fd (fd, &parms))
    return 1;

  /* initialize the GLUT state. */
  glutInit (&argc, argv);

  /* select the GL display mode. */
  glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);

  /* get a window at the top left corner of the screen. */
  glutInitWindowSize (800, 600);
  glutInitWindowPosition (0, 0);

  /* open the GL window. */
  win = glutCreateWindow ("liveft");

  /* register the GL callback functions. */
  glutDisplayFunc (&liveft_gl_draw);
  glutIdleFunc (&liveft_gl_draw);
  glutReshapeFunc (&liveft_gl_reshape);
  glutKeyboardFunc (&liveft_gl_keyboard);
  glutMouseFunc (&liveft_gl_mouse);

  /* initialize the GL state. */
  glutFullScreen ();
  liveft_gl_init ();

  /* start processing GL events. */
  glutMainLoop ();

  /* close the device. */
  ppm_device_close (fd);

  /* return successfully. */
  return 0;
}
