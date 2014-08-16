
/* backspin: A simple GTK3 graphical interface to the PPM device.
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
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <sndfile.h>

/* declare known commands. */
#define BS_COMMAND_QUIT   "quit"
#define BS_COMMAND_EXIT   "exit"
#define BS_COMMAND_FID    "fid"
#define BS_COMMAND_SPECT  "spect"
#define BS_COMMAND_WFALL  "wfall"
#define BS_COMMAND_PARMS  "par"
#define BS_COMMAND_EXPT   "expt"
#define BS_COMMAND_RST    "rst"
#define BS_COMMAND_II     "ii"
#define BS_COMMAND_ZG     "zg"
#define BS_COMMAND_FT     "ft"
#define BS_COMMAND_WFP    "wfp"
#define BS_COMMAND_PICK   "pick"
#define BS_COMMAND_SAVE   "save"
#define BS_COMMAND_AUDIO  "audio"
#define BS_COMMAND_COLOR  "color"

/* declare drawable drawing modes. */
#define BS_DRAWABLE_MODE_INIT   0x00
#define BS_DRAWABLE_MODE_PARMS  0x01
#define BS_DRAWABLE_MODE_FID    0x02
#define BS_DRAWABLE_MODE_SPECT  0x04
#define BS_DRAWABLE_MODE_WFALL  0x08

/* declare the drawable padding. */
#define PAD 50.0

/* define a macro to show informational dialogs. */
#define bs_info_dialog(...) \
  dialog = gtk_message_dialog_new (GTK_WINDOW (window), \
             GTK_DIALOG_DESTROY_WITH_PARENT, \
             GTK_MESSAGE_INFO, GTK_BUTTONS_OK, \
             __VA_ARGS__); \
  gtk_dialog_run (GTK_DIALOG (dialog)); \
  gtk_widget_destroy (dialog);

/* define a macro to show error dialogs. */
#define bs_error_dialog(...) \
  dialog = gtk_message_dialog_new (GTK_WINDOW (window), \
             GTK_DIALOG_DESTROY_WITH_PARENT, \
             GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, \
             __VA_ARGS__); \
  gtk_dialog_run (GTK_DIALOG (dialog)); \
  gtk_widget_destroy (dialog);

/* declare required persistent PPM data elements. */
ppm_data tacq, facq;
ppm_fall wfall;
ppm_parms parms;

/* declare required graphics elements. */
GtkWidget *window, *dialog, *grid, *entry, *drawable;
guint drawable_mode = BS_DRAWABLE_MODE_INIT;
double trcolor_r, trcolor_g, trcolor_b;

/* window_deleted: window 'delete-event' callback function. */
static gboolean window_deleted (GtkWidget *widget,
                                GdkEvent *event,
                                gpointer data) {
  /* end program execution. */
  return FALSE;
}

/* window_destroyed: window 'destroy' callback function. */
static void window_destroyed (GtkWidget *widget, gpointer data) {
  /* end program execution, for reals this time. */
  gtk_main_quit ();
}

/* drawable_set_font: sets the current font parameters. */
static void drawable_set_font (cairo_t *cr, double size,
                               gboolean italic, gboolean bold) {
  /* select the font face. */
  cairo_select_font_face (cr, "Sans",
    italic ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_NORMAL,
    bold ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL);

  /* set the font size. */
  cairo_set_font_size (cr, size);
}

/* drawable_draw_parm: draws a parameter to the drawing surface. */
static void drawable_draw_parm (cairo_t *cr, double x, double y,
                                const char *name, const char *format, ...) {
  /* declare required variables. */
  va_list vl;
  char str[128];

  /* build the output value string. */
  va_start (vl, format);
  vsnprintf (str, 128, format, vl);
  va_end (vl);

  /* select a bold, large font face and size. */
  drawable_set_font (cr, 12.0, FALSE, TRUE);

  /* write the heading text. */
  cairo_move_to (cr, x, y);
  cairo_show_text (cr, name);

  /* select a normal font face and size. */
  drawable_set_font (cr, 10.0, FALSE, FALSE);

  /* write the value text. */
  cairo_move_to (cr, x, y + 15.0);
  cairo_show_text (cr, str);
}

/* drawable_draw_allparms: draws the machine-readable parameter set. */
static void drawable_draw_allparms (cairo_t *cr) {
  /* declare required variables. */
  char str[256];

  /* select a bold, large font face and size. */
  drawable_set_font (cr, 12.0, FALSE, TRUE);

  /* write the heading text. */
  cairo_move_to (cr, 10.0, 42.0);
  cairo_show_text (cr, "rpar:");

  /* select a normal font face and size. */
  drawable_set_font (cr, 10.0, FALSE, FALSE);

  /* build the value string. */
  snprintf (str, 256, "%u %u %u %u %u %u",
    parms.polarize_ovf,
    parms.acquire_ovf,
    parms.acquire_count,
    parms.deadtime_pol,
    parms.deadtime_acq,
    parms.ccs_value);

  /* write the value string. */
  cairo_move_to (cr, 52.0, 42.0);
  cairo_show_text (cr, str);
}

/* drawable_draw_title: draws a title for the display. */
static void drawable_draw_title (cairo_t *cr, const char *title) {
  /* select the font face and size. */
  drawable_set_font (cr, 18.0, TRUE, TRUE);

  /* draw a title. */
  cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 1.0);
  cairo_move_to (cr, 10.0, 24.0);
  cairo_show_text (cr, title);
}

/* drawable_draw_axes: draws axes and labels, etc. */
static void drawable_draw_axes (cairo_t *cr, double wf, double hf,
                                const char *xlabel,
                                const char *ylabel) {
  /* declare required variables. */
  double x, xmin, xmax, y, ymin, ymax;
  cairo_text_extents_t extents;
  unsigned int i;
  char str[64];

  /* set a thicker black line. */
  cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 1.0);
  cairo_set_line_width (cr, 2.0);

  /* draw the bounding box. */
  cairo_rectangle (cr, PAD, PAD, wf - 2.0 * PAD, hf - 2.0 * PAD);
  cairo_stroke (cr);

  /* select a normal font face and size. */
  drawable_set_font (cr, 10.0, FALSE, FALSE);

  /* draw ticks based on the current draw mode. */
  switch (drawable_mode) {
    /* fid. */
    case BS_DRAWABLE_MODE_FID:
      /* get the extents. */
      xmin = tacq.x[0];
      xmax = tacq.x[tacq.n - 1];
      ymin = -2.5;
      ymax = 2.5;

      /* loop through the x-axis ticks. */
      for (x = xmin; x < xmax; x += (xmax - xmin) / 10.0) {
        /* move to the first point. */
        cairo_move_to (cr,
          PAD + (wf - 2.0 * PAD) * (x - xmin) / (xmax - xmin),
          hf - PAD);

        /* draw the line. */
        cairo_line_to (cr,
          PAD + (wf - 2.0 * PAD) * (x - xmin) / (xmax - xmin),
          hf - PAD + 5.0);

        /* draw the tick value. */
        snprintf (str, 64, "%.0lf", x * 1.0e3);
        cairo_text_extents (cr, str, &extents);
        cairo_move_to (cr,
          PAD + (wf - 2.0 * PAD) * (x - xmin) / (xmax - xmin)
              - extents.width / 2.0,
          hf - PAD + 5.0 + 1.5 * extents.height);
        cairo_show_text (cr, str);
      }

      /* loop through the y-axis ticks. */
      for (y = ymin; y <= ymax; y += 1.0) {
        /* move to the first point. */
        cairo_move_to (cr, PAD,
          PAD + (hf - 2.0 * PAD) * (y - ymin) / (ymax - ymin));

        /* draw the line. */
        cairo_line_to (cr, PAD - 5.0,
          PAD + (hf - 2.0 * PAD) * (y - ymin) / (ymax - ymin));

        /* draw the tick value. */
        snprintf (str, 64, "%.1lf", y);
        cairo_text_extents (cr, str, &extents);
        cairo_move_to (cr, PAD - 5.0 - 1.5 * extents.width,
          PAD + (hf - 2.0 * PAD) * (y - ymin) / (ymax - ymin)
              + 0.5 * extents.height);
        cairo_show_text (cr, str);
      }

      /* done. */
      break;

    /* spectrum. */
    case BS_DRAWABLE_MODE_SPECT:
      /* get the extents. */
      xmin = facq.x[0];
      xmax = facq.x[facq.n - 1];
      ymin = 0.0;
      ymax = -1.0e6;

      /* find the y-maximum. */
      for (i = 0; i < facq.n; i++)
        if (facq.v[i] > ymax)
          ymax = facq.v[i];

      /* loop through the x-axis ticks. */
      for (x = xmin; x <= xmax; x += 1.0e3) {
        /* move to the first point. */
        cairo_move_to (cr,
          PAD + (wf - 2.0 * PAD) * (x - xmin) / (xmax - xmin),
          hf - PAD);

        /* draw the line. */
        cairo_line_to (cr,
          PAD + (wf - 2.0 * PAD) * (x - xmin) / (xmax - xmin),
          hf - PAD + 5.0);

        /* draw the tick value. */
        snprintf (str, 64, "%.0lf", x);
        cairo_text_extents (cr, str, &extents);
        cairo_move_to (cr,
          PAD + (wf - 2.0 * PAD) * (x - xmin) / (xmax - xmin)
              - extents.width / 2.0,
          hf - PAD + 5.0 + 1.5 * extents.height);
        cairo_show_text (cr, str);
      }

      /* loop through the y-axis ticks. */
      for (y = ymin; y <= ymax; y += (ymax - ymin) / 5.0) {
        /* move to the first point. */
        cairo_move_to (cr, PAD,
          hf - 2.0 * PAD - 0.75 * (y - ymin) / (ymax - ymin)
             * (hf - 2.0 * PAD));

        /* draw the line. */
        cairo_line_to (cr, PAD - 5.0,
          hf - 2.0 * PAD - 0.75 * (y - ymin) / (ymax - ymin)
             * (hf - 2.0 * PAD));

        /* draw the tick value. */
        snprintf (str, 64, "%.1lf", y);
        cairo_text_extents (cr, str, &extents);
        cairo_move_to (cr, PAD - 5.0 - 1.5 * extents.width,
          hf - 2.0 * PAD - 0.75 * (y - ymin) / (ymax - ymin)
             * (hf - 2.0 * PAD) + 0.5 * extents.height);
        cairo_show_text (cr, str);
      }

      /* done. */
      break;

    /* waterfall. */
    case BS_DRAWABLE_MODE_WFALL:
      /* get the extents. */
      xmin = wfall.f[0];
      xmax = wfall.f[wfall.nfreq - 1];
      ymin = wfall.t[0];
      ymax = wfall.t[wfall.ntime - 1];

      /* loop through the x-axis ticks. */
      for (x = xmin; x <= xmax; x += 1.0e3) {
        /* move to the first point. */
        cairo_move_to (cr,
          PAD + (wf - 2.0 * PAD) * (x - xmin) / (xmax - xmin),
          hf - PAD);

        /* draw the line. */
        cairo_line_to (cr,
          PAD + (wf - 2.0 * PAD) * (x - xmin) / (xmax - xmin),
          hf - PAD + 5.0);

        /* draw the tick value. */
        snprintf (str, 64, "%.0lf", x);
        cairo_text_extents (cr, str, &extents);
        cairo_move_to (cr,
          PAD + (wf - 2.0 * PAD) * (x - xmin) / (xmax - xmin)
              - extents.width / 2.0,
          hf - PAD + 5.0 + 1.5 * extents.height);
        cairo_show_text (cr, str);
      }

      /* loop through the y-axis ticks. */
      for (y = ymin; y <= ymax; y += (ymax - ymin) / 5.0) {
        /* move to the first point. */
        cairo_move_to (cr, PAD,
          hf - PAD - (hf - 2.0 * PAD) * (y - ymin) / (ymax - ymin));

        /* draw the line. */
        cairo_line_to (cr, PAD - 5.0,
          hf - PAD - (hf - 2.0 * PAD) * (y - ymin) / (ymax - ymin));

        /* draw the tick value. */
        snprintf (str, 64, "%.0lf", y * 1.0e3);
        cairo_text_extents (cr, str, &extents);
        cairo_move_to (cr, PAD - 9.0 - extents.width,
          hf - PAD - (hf - 2.0 * PAD) * (y - ymin) / (ymax - ymin)
             + 0.5 * extents.height);
        cairo_show_text (cr, str);
      }

      /* done. */
      break;

    /* other. */
    default:
      break;
  }

  /* finish the axis ticks. */
  cairo_stroke (cr);

  /* select a bold, large font face and size. */
  drawable_set_font (cr, 12.0, FALSE, TRUE);

  /* write the x-axis label. */
  cairo_text_extents (cr, xlabel, &extents);
  cairo_move_to (cr, PAD, hf - 1.2 * extents.height);
  cairo_show_text (cr, xlabel);

  /* set up the transformation matrix for vertical text. */
  cairo_move_to (cr, 0.0, 0.0);
  cairo_rotate (cr, -M_PI / 2.0);

  /* write the y-axis label. */
  cairo_text_extents (cr, ylabel, &extents);
  cairo_move_to (cr, -hf + PAD, 1.2 * extents.height);
  cairo_show_text (cr, ylabel);

  /* reset the transformation matrix. */
  cairo_identity_matrix (cr);
}

/* drawable_draw: drawable 'draw' callback function. */
static gboolean drawable_draw (GtkWidget *widget, cairo_t *cr, gpointer data) {
  /* declare required variables. */
  double wf, hf, v, vmax, x, dx, y, dy;
  unsigned int i, idx1, idx2, j;
  guint w, h;

  /* get the drawing area width and height. */
  w = gtk_widget_get_allocated_width (widget);
  h = gtk_widget_get_allocated_height (widget);
  wf = (double) w;
  hf = (double) h;

  /* set the drawing color to opaque white. */
  cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 1.0);

  /* fill in the background color. */
  cairo_paint (cr);
  cairo_fill (cr);

  /* switch on the drawable mode. */
  switch (drawable_mode) {
    /* initial. */
    case BS_DRAWABLE_MODE_INIT:
      /* draw a title. */
      drawable_draw_title (cr, "Uninitialized");

      /* select a bold, large font face and size. */
      drawable_set_font (cr, 12.0, FALSE, TRUE);

      /* write the heading text. */
      cairo_move_to (cr, 10.0, 42.0);
      cairo_show_text (cr, "Run 'ii' to connect to the device");

      /* done. */
      break;

    /* parameters. */
    case BS_DRAWABLE_MODE_PARMS:
      /* draw a title. */
      drawable_draw_title (cr, "Experiment Parameters");

      /* set a thinner transparent red line. */
      cairo_set_source_rgba (cr, trcolor_r, trcolor_g, trcolor_b, 0.5);
      cairo_set_line_width (cr, 1.0);

      /* draw a FID to signify acquisition. */
      cairo_move_to (cr, 450.0, 300.0);
      for (i = 0; i < 1024; i++) {
        cairo_line_to (cr,
          (double) i / 1024.0 * 300.0 + 450.0,
          300.0 + 40.0 * sin (0.6 * (double) i) * exp (-(double) i / 150.0));
      }

      /* draw the fake FID. */
      cairo_stroke (cr);

      /* set a really thick black line. */
      cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 1.0);
      cairo_set_line_width (cr, 4.0);

      /* draw the main trace. */
      cairo_move_to (cr, 50.0, 300.0);
      cairo_line_to (cr, 100.0, 300.0);
      cairo_line_to (cr, 100.0, 250.0);
      cairo_line_to (cr, 300.0, 250.0);
      cairo_line_to (cr, 300.0, 300.0);
      cairo_line_to (cr, 750.0, 300.0);
      cairo_move_to (cr, 50.0, 295.0);
      cairo_line_to (cr, 50.0, 305.0);
      cairo_move_to (cr, 350.0, 295.0);
      cairo_line_to (cr, 350.0, 305.0);
      cairo_move_to (cr, 450.0, 295.0);
      cairo_line_to (cr, 450.0, 305.0);
      cairo_move_to (cr, 750.0, 295.0);
      cairo_line_to (cr, 750.0, 305.0);
      cairo_stroke (cr);

      /* make the trace thin again. */
      cairo_set_line_width (cr, 2.0);

      /* draw a loop indicator. */
      cairo_move_to (cr, 450.0, 245.0);
      cairo_line_to (cr, 460.0, 235.0);
      cairo_line_to (cr, 740.0, 235.0);
      cairo_line_to (cr, 750.0, 245.0);
      cairo_stroke (cr);

      /* draw a divider between 'sn' and 'sr'. */
      cairo_move_to (cr, 590.0, 335.0);
      cairo_line_to (cr, 600.0, 310.0);
      cairo_stroke (cr);

      /* draw an equals between 'sr' and 'ta'. */
      cairo_move_to (cr, 685.0, 328.0);
      cairo_line_to (cr, 695.0, 328.0);
      cairo_move_to (cr, 685.0, 322.0);
      cairo_line_to (cr, 695.0, 322.0);
      cairo_stroke (cr);

      /* write some heading text. */
      drawable_draw_parm (cr, 60.0, 320.0, PPM_ACQPARM_DTP,
        "%.2lf ms", parms.f_deadtime_pol);
      drawable_draw_parm (cr, 190.0, 320.0, PPM_ACQPARM_TP,
        "%.3lf s", parms.f_polarize_ovf);
      drawable_draw_parm (cr, 310.0, 320.0, PPM_ACQPARM_DTP,
        "%.2lf ms", parms.f_deadtime_pol);
      drawable_draw_parm (cr, 385.0, 320.0, PPM_ACQPARM_DTA,
        "%.2lf ms", parms.f_deadtime_acq);
      drawable_draw_parm (cr, 540.0, 320.0, PPM_ACQPARM_SN,
        "%u", parms.acquire_count);
      drawable_draw_parm (cr, 620.0, 320.0, PPM_ACQPARM_SR,
        "%.2lf kS/s", parms.f_acquire_ovf);
      drawable_draw_parm (cr, 110.0, 275.0, PPM_ACQPARM_IP,
        "%.3lf A", parms.f_ccs_value);
      drawable_draw_parm (cr, 590.0, 210.0, PPM_ACQPARM_NS,
        "%d", parms.ns);
      drawable_draw_parm (cr, 710.0, 320.0, PPM_ACQPARM_TA, "%.3lf s",
        1.0e-3 * (double) parms.acquire_count / parms.f_acquire_ovf);

      /* draw the 'rpar' line. */
      drawable_draw_allparms (cr);

      /* done. */
      break;

    /* free induction decay. */
    case BS_DRAWABLE_MODE_FID:
      /* draw a title. */
      drawable_draw_title (cr, "Free Induction Decay");


      /* fail out silently if no fid exists. */
      if (!tacq.n) break;

      /* set the plotting indices. */
      idx1 = 0;
      idx2 = tacq.n - 1;

      /* set a thinner red line. */
      cairo_set_source_rgba (cr, trcolor_r, trcolor_g, trcolor_b, 1.0);
      cairo_set_line_width (cr, 1.0);
      cairo_move_to (cr, PAD, hf / 2.0);

      /* loop through the time-domain data points. */
      for (i = idx1; i <= idx2; i++) {
        /* draw the line. */
        cairo_line_to (cr,
          tacq.x[i] / (tacq.x[idx2] - tacq.x[idx1])
                    * (wf - 2.0 * PAD) + PAD,
          tacq.v[i] / 5.0 * (hf - 2.0 * PAD) + (hf / 2.0));
      }

      /* draw the time-domain data points. */
      cairo_line_to (cr, wf - PAD, hf / 2.0);
      cairo_stroke (cr);

      /* draw the axis ticks and labels and the bounding box. */
      drawable_draw_axes (cr, wf, hf, "Time (ms)", "Amplitude (V)");

      /* done. */
      break;

    /* spectrum. */
    case BS_DRAWABLE_MODE_SPECT:
      /* draw a title. */
      drawable_draw_title (cr, "Spectrum");

      /* fail out silently if no spectrum exists. */
      if (!facq.n) break;

      /* set the plotting indices. */
      idx1 = 0;
      idx2 = facq.n - 1;

      /* set a thinner red line. */
      cairo_set_source_rgba (cr, trcolor_r, trcolor_g, trcolor_b, 1.0);
      cairo_set_line_width (cr, 1.0);
      cairo_move_to (cr, PAD, hf / 1.2);

      /* find the maximum intensity. */
      for (vmax = -1.0e+6, i = 0; i < facq.n; i++)
          if (facq.v[i] > vmax)
            vmax = facq.v[i];

      /* loop through the frequency-domain data points. */
      for (i = idx1; i <= idx2; i++) {
        /* get the new x-coordinate. */
        x = (facq.x[i] - facq.x[idx1]) / (facq.x[idx2] - facq.x[idx1])
                      * (wf - 2.0 * PAD) + PAD;
 
        /* get the new y-coordinate. */
        y = hf - 2.0 * PAD - 0.75 * facq.v[i] / vmax * (hf - 2.0 * PAD);

        /* draw the line. */
        cairo_line_to (cr, x, y);
      }

      /* draw the time-domain data points. */
      cairo_line_to (cr, wf - PAD, hf / 1.2);
      cairo_stroke (cr);

      /* draw the axis ticks and labels and the bounding box. */
      drawable_draw_axes (cr, wf, hf, "Frequency (Hz)", "Amplitude (V)");

      /* done. */
      break;

    /* waterfall. */
    case BS_DRAWABLE_MODE_WFALL:
      /* draw a title. */
      drawable_draw_title (cr, "Waterfall");

      /* fail out silently if no waterfall exists. */
      if (!wfall.ntime || !wfall.nfreq) break;

      /* calculate the width of the waterfall squares. */
      dx = 0.5 * (wf - 2.0 * PAD) / (double) wfall.nfreq;
      dy = 0.5 * (hf - 2.0 * PAD) / (double) wfall.ntime;

      /* find the maximum intensity. */
      for (vmax = -1.0e+6, i = 0; i < wfall.ntime; i++)
        for (j = 0; j < wfall.nfreq; j++)
          if (wfall.v[i][j] > vmax)
            vmax = wfall.v[i][j];

      /* loop through the time values. */
      for (i = 0; i < wfall.ntime; i++) {
        /* and through the frequency values. */
        for (j = 0; j < wfall.nfreq; j++) {
          /* calculate the scaled intensity. */
          v = wfall.v[i][j] / vmax;

          /* set a red fill, intensity shaded. */
          cairo_set_source_rgba (cr,
            1.0 - v * (1.0 - trcolor_r),
            1.0 - v * (1.0 - trcolor_g),
            1.0 - v * (1.0 - trcolor_b), 1.0);

          /* get the location of the square. */
          x = PAD + dx + (wf - 2.0 * PAD) * (double) j / (double) wfall.nfreq;
          y = hf - PAD - dy - (hf - 2.0 * PAD)
            * (double) i / (double) wfall.ntime;

          /* plot the waterfall square. */
          cairo_rectangle (cr, x - dx, y - dy, 2.0 * dx, 2.0 * dy);
          cairo_fill (cr);
        }
      }

      /* draw the axis ticks and labels and the bounding box. */
      drawable_draw_axes (cr, wf, hf, "Frequency (Hz)", "Time (ms)");

      /* done. */
      break;

    /* other. */
    default:
      break;
  }

  /* exit the callback. */
  return FALSE;
}

/* entry_activated: entry 'activate' callback function. */
static void entry_activated (GtkWidget *widget, gpointer data) {
  /* declare required variables. */
  const gchar *entryStr;
  gchar **entryStrv;

  /* get the contents of the entry. */
  entryStr = gtk_entry_get_text (GTK_ENTRY (widget));

  /* split the contents of the entry. */
  entryStrv = g_strsplit (entryStr, " ", 0);

  /* some commands work without a device connected. */
  if (drawable_mode == BS_DRAWABLE_MODE_INIT) {
    /* test only device-disconnected commands. */
    if (g_strcmp0 (entryStrv[0], BS_COMMAND_QUIT) == 0 ||
        g_strcmp0 (entryStrv[0], BS_COMMAND_EXIT) == 0) {
      /* quit. */
      gtk_main_quit ();
    }
    else if (g_strcmp0 (entryStrv[0], BS_COMMAND_II) == 0) {
      /* try to read the parameters. */
      if (ppm_rpar (NULL, &parms) && ppm_parms_humanize (&parms)) {
        /* set the next drawing mode. */
        drawable_mode = BS_DRAWABLE_MODE_PARMS;
        gtk_widget_queue_draw (drawable);
      }
      else {
        /* complain. */
        bs_error_dialog ("Error: Device init failed");
      }
    }
    else {
      /* complain. */
      bs_error_dialog ("Error: Uninitialized");
    }
  }
  else {
    /* test device-connected commands. */
    if (g_strcmp0 (entryStrv[0], BS_COMMAND_QUIT) == 0 ||
        g_strcmp0 (entryStrv[0], BS_COMMAND_EXIT) == 0) {
      /* quit. */
      gtk_main_quit ();
    }
    else if (g_strcmp0 (entryStrv[0], BS_COMMAND_FID) == 0) {
      /* make sure a fid exists. */
      if (!tacq.n) {
        /* complain. */
        bs_error_dialog ("Error: No acquisition data");
      }
      else {
        /* set the drawable mode and initiate a redraw. */
        drawable_mode = BS_DRAWABLE_MODE_FID;
        gtk_widget_queue_draw (drawable);
      }
    }
    else if (g_strcmp0 (entryStrv[0], BS_COMMAND_SPECT) == 0) {
      /* make sure a spectrum exists. */
      if (!facq.n) {
        /* complain. */
        bs_error_dialog ("Error: No spectrum data");
      }
      else {
        /* set the drawable mode and initiate a redraw. */
        drawable_mode = BS_DRAWABLE_MODE_SPECT;
        gtk_widget_queue_draw (drawable);
      }
    }
    else if (g_strcmp0 (entryStrv[0], BS_COMMAND_WFALL) == 0) {
      /* make sure a waterfall exists. */
      if (!wfall.ntime || !wfall.nfreq) {
        /* complain. */
        bs_error_dialog ("Error: No waterfall data");
      }
      else {
        /* set the drawable mode and initiate a redraw. */
        drawable_mode = BS_DRAWABLE_MODE_WFALL;
        gtk_widget_queue_draw (drawable);
      }
    }
    else if (g_strcmp0 (entryStrv[0], BS_COMMAND_PARMS) == 0) {
      /* attempt to read the device parameters. */
      if (!ppm_rpar (NULL, &parms) || !ppm_parms_humanize (&parms)) {
        /* complain. */
        bs_error_dialog ("Error: Read parameters failed");
      }

      /* set the drawable mode and initiate a redraw. */
      drawable_mode = BS_DRAWABLE_MODE_PARMS;
      gtk_widget_queue_draw (drawable);
    }
    else if (g_strcmp0 (entryStrv[0], BS_COMMAND_EXPT) == 0) {
      /* attempt to read the device parameters. */
      if (!ppm_rpar (NULL, &parms) || !ppm_parms_humanize (&parms)) {
        /* complain. */
        bs_error_dialog ("Error: Read parameters failed");
      }

      /* inform. */
      bs_info_dialog ("Experiment time: %.3lf s",
        1.0e-3 * (double) parms.ns
               * (double) parms.acquire_count
               / parms.f_acquire_ovf);
    }
    else if (g_strcmp0 (entryStrv[0], BS_COMMAND_RST) == 0) {
      /* attempt to reset the device. */
      if (!ppm_rst (NULL)) {
        /* complain. */
        bs_error_dialog ("Error: Device reset failed");
      }
    }
    else if (g_strcmp0 (entryStrv[0], BS_COMMAND_ZG) == 0) {
      /* attempt to initiate an acquisition. */
      if (!ppm_zg (NULL, &parms, &tacq)) {
        /* complain. */
        bs_error_dialog ("Error: Acquisition failed");
      }

      /* empty the transforms. */
      ppm_data_empty (&facq);
      ppm_fall_empty (&wfall);

      /* set the drawable mode and initiate a redraw. */
      drawable_mode = BS_DRAWABLE_MODE_FID;
      gtk_widget_queue_draw (drawable);
    }
    else if (g_strcmp0 (entryStrv[0], BS_COMMAND_FT) == 0) {
      /* make sure a fid exists. */
      if (!tacq.n) {
        /* complain. */
        bs_error_dialog ("Error: No acquisition data");
      }
      else {
        /* attempt to fourier-transform the time-domain data. */
        if (!ppm_fft (&tacq, &facq)) {
          /* complain. */
          bs_error_dialog ("Error: Fourier transform failed");
        }

        /* set the drawable mode and initiate a redraw. */
        drawable_mode = BS_DRAWABLE_MODE_SPECT;
        gtk_widget_queue_draw (drawable);
      }
    }
    else if (g_strcmp0 (entryStrv[0], BS_COMMAND_WFP) == 0) {
      /* make sure a fid exists. */
      if (!tacq.n) {
        /* complain. */
        bs_error_dialog ("Error: No acquisition data");
      }
      else {
        /* attempt to fourier-transform the time-domain data. */
        if (!ppm_wfall (&tacq, &wfall)) {
          /* complain. */
          bs_error_dialog ("Error: Waterfall transform failed");
        }

        /* set the drawable mode and initiate a redraw. */
        drawable_mode = BS_DRAWABLE_MODE_WFALL;
        gtk_widget_queue_draw (drawable);
      }
    }
    else if (g_strcmp0 (entryStrv[0], BS_COMMAND_SAVE) == 0 &&
             g_strv_length (entryStrv) >= 2) {
      /* see what the user wishes to save. */
      if (g_strcmp0 (entryStrv[1], BS_COMMAND_FID) == 0) {
        /* make sure a fid exists. */
        if (tacq.n) {
          /* try to save the fid. */
          if (!ppm_data_write (&tacq, entryStrv[2] ? entryStrv[2] : "fid")) {
            /* complain. */
            bs_error_dialog ("Error: Failed to write FID");
          }
        }
        else {
          /* complain. */
          bs_error_dialog ("Error: No FID data to save");
        }
      }
      else if (g_strcmp0 (entryStrv[1], BS_COMMAND_SPECT) == 0) {
        /* make sure a spectrum exists. */
        if (facq.n) {
          /* try to save the spectrum. */
          if (!ppm_data_write (&facq, entryStrv[2] ? entryStrv[2] : "spect")) {
            /* complain. */
            bs_error_dialog ("Error: Failed to write spectrum");
          }
        }
        else {
          /* complain. */
          bs_error_dialog ("Error: No spectrum data to save");
        }
      }
      else if (g_strcmp0 (entryStrv[1], BS_COMMAND_WFALL) == 0) {
        /* make sure a waterfall exists. */
        if (wfall.ntime && wfall.nfreq) {
          /* try to save the waterfall. */
          if (!ppm_fall_write (&wfall, entryStrv[2] ? entryStrv[2] : "wf")) {
            /* complain. */
            bs_error_dialog ("Error: Failed to write waterfall");
          }
        }
        else {
          /* complain. */
          bs_error_dialog ("Error: No waterfall data to save");
        }
      }
      else if (g_strcmp0 (entryStrv[1], BS_COMMAND_AUDIO) == 0) {
        /* make sure a fid exists. */
        if (tacq.n) {
          /* try to save the fid as an audio file. */
          if (!ppm_data_write_audio (&tacq,
               entryStrv[2] ? entryStrv[2] : "fid.wav")) {
            /* complain. */
            bs_error_dialog ("Error: Failed to save FID as audio");
          }
        }
        else {
          /* complain. */
          bs_error_dialog ("Error: No FID data to save");
        }
      }
      else {
        /* complain. */
        bs_error_dialog ("Error: Invalid save-data type");
      }
    }
    else if (g_strcmp0 (entryStrv[0], BS_COMMAND_COLOR) == 0 &&
             g_strv_length (entryStrv) == 4) {
      /* store the new color values. */
      trcolor_r = atof (entryStrv[1]);
      trcolor_g = atof (entryStrv[2]);
      trcolor_b = atof (entryStrv[3]);

      /* lower bound the color values. */
      trcolor_r = trcolor_r >= 0.0 ? trcolor_r : 0.0;
      trcolor_g = trcolor_g >= 0.0 ? trcolor_g : 0.0;
      trcolor_b = trcolor_b >= 0.0 ? trcolor_b : 0.0;

      /* upper bound the color values. */
      trcolor_r = trcolor_r <= 1.0 ? trcolor_r : 1.0;
      trcolor_g = trcolor_g <= 1.0 ? trcolor_g : 1.0;
      trcolor_b = trcolor_b <= 1.0 ? trcolor_b : 1.0;

      /* update the drawable. */
      gtk_widget_queue_draw (drawable);
    }
    else if (g_strcmp0 (entryStrv[0], PPM_ACQPARM_NS) == 0 &&
             g_strv_length (entryStrv) == 2) {
      /* parse and store the parameter. */
      int ns = atoi (entryStrv[1]);
      parms.ns = ns > 0 ? ns : 1;

      /* check if the parms screen needs updating. */
      if (drawable_mode == BS_DRAWABLE_MODE_PARMS)
        gtk_widget_queue_draw (drawable);
    }
    else if (g_strv_length (entryStrv) == 2) {
      /* read the current parameters. */
      if (!ppm_rpar (NULL, &parms) || !ppm_parms_humanize (&parms)) {
        /* complain. */
        bs_error_dialog ("Error: Read parameters failed");
      }

      /* wait for the device to breathe. */
      usleep (200000);

      /* parse the parameter update command. */
      if (g_strcmp0 (entryStrv[0], PPM_ACQPARM_TP) == 0) {
        /* parse the new parameter. */
        parms.f_polarize_ovf = atof (entryStrv[1]);
      }
      else if (g_strcmp0 (entryStrv[0], PPM_ACQPARM_SR) == 0) {
        /* parse the new parameter. */
        parms.f_acquire_ovf = atof (entryStrv[1]);
      }
      else if (g_strcmp0 (entryStrv[0], PPM_ACQPARM_SN) == 0) {
        /* parse the new parameter. */
        parms.acquire_count = (uint32_t) round (atof (entryStrv[1]));
      }
      else if (g_strcmp0 (entryStrv[0], PPM_ACQPARM_DTP) == 0) {
        /* parse the new parameter. */
        parms.f_deadtime_pol = atof (entryStrv[1]);
      }
      else if (g_strcmp0 (entryStrv[0], PPM_ACQPARM_DTA) == 0) {
        /* parse the new parameter. */
        parms.f_deadtime_acq = atof (entryStrv[1]);
      }
      else if (g_strcmp0 (entryStrv[0], PPM_ACQPARM_IP) == 0) {
        /* parse the new parameter. */
        parms.f_ccs_value = atof (entryStrv[1]);
      }
      else {
        /* complain. */
        bs_error_dialog ("Error: invalid parameter '%s'", entryStrv[0]);
      }

      /* dehumanize the parameters. */
      if (!ppm_parms_dehumanize (&parms)) {
        /* complain. */
        bs_error_dialog ("Error: Encode parameters failed");
      }

      /* write out the current parameters. */
      if (!ppm_wpar (NULL, &parms)) {
        /* complain. */
        bs_error_dialog ("Error: Write parameters failed");
      }

      /* wait for the device to breathe. */
      usleep (200000);

      /* read the current parameters. */
      if (!ppm_rpar (NULL, &parms) || !ppm_parms_humanize (&parms)) {
        /* complain. */
        bs_error_dialog ("Error: Read parameters failed");
      }

      /* check if the parms screen needs updating. */
      if (drawable_mode == BS_DRAWABLE_MODE_PARMS)
        gtk_widget_queue_draw (drawable);
    }
    else {
      /* complain. */
      bs_error_dialog ("Error: Invalid command '%s'", entryStr);
    }
  }

  /* clear the textbox. */
  gtk_entry_set_text (GTK_ENTRY (entry), "");
}

/* main: application entry point. */
int main (int argc, char **argv) {
  /* cheat. */
  parms.ns = 1;
  trcolor_r = 1.0;
  trcolor_g = 0.0;
  trcolor_b = 0.0;

  /* initialize the gtk subsystem. */
  gtk_init (&argc, &argv);

  /* create the main window. */
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  /* set the default main window size. */
  gtk_window_set_default_size (GTK_WINDOW (window), 800, 480);
  gtk_window_set_resizable (GTK_WINDOW (window), FALSE);

  /* connect the window 'delete-event' signal to its callback function. */
  g_signal_connect (window, "delete-event",
    G_CALLBACK (window_deleted), NULL);

  /* connect the window 'destroy' signal to its callback function. */
  g_signal_connect (window, "destroy",
    G_CALLBACK (window_destroyed), NULL);

  /* create the main grid. */
  grid = gtk_grid_new ();

  /* attach the main grid to the window. */
  gtk_container_add (GTK_CONTAINER (window), grid);

  /* create the drawing area. */
  drawable = gtk_drawing_area_new ();

  /* set the requested size of the drawing area. */
  gtk_widget_set_size_request (drawable, 800, 480);

  /* connect the drawable 'draw' signal to its callback function. */
  g_signal_connect (drawable, "draw",
    G_CALLBACK (drawable_draw), NULL);

  /* add the drawable to the main grid. */
  gtk_grid_attach (GTK_GRID (grid), drawable, 0, 0, 1, 1);

  /* show the drawable. */
  gtk_widget_show (drawable);

  /* create the main command entry bar. */
  entry = gtk_entry_new ();

  /* connect the entry 'changed' signal to its callback function. */
  g_signal_connect (entry, "activate",
    G_CALLBACK (entry_activated), NULL);

  /* add the entry bar to the main grid. */
  gtk_grid_attach_next_to (GTK_GRID (grid), entry, drawable,
    GTK_POS_BOTTOM, 1, 1);

  /* show the entry bar. */
  gtk_widget_show (entry);

  /* show the grid and window. */
  gtk_widget_show (grid);
  gtk_widget_show (window);

  /* begin the main graphics loop. */
  gtk_main ();

  /* return successfully. */
  return 0;
}
