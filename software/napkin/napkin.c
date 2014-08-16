
/* napkin.c: proton precession magnetometer coil parameter calculator.
 * Copyright (C) 2013 Bradley Worley.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the
 *
 *     Free Software Foundation, Inc.
 *     59 Temple Place, Suite 330
 *     Boston, MA 02111-1307 USA
 */

/* include required headers. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

/* declare physical constants, etc. */
#define MU_0  (4.0 * M_PI * 1.0e-07)
#define K_B   (1.3806488e-23)
#define CHI_V (9.035e-06)

/* declare fit constants for the water longitudinal relaxation time. */
#define T1_FIT_A  0.000290700941881018
#define T1_FIT_B -0.0866514470383879
#define T1_FIT_C  3.4453680758948
#define T1_FIT(T) \
  (T1_FIT_A * (T) * (T) + T1_FIT_B * (T) + T1_FIT_C)

/* declare fit constants for the filling factor. */
#define ETA_FIT_A  0.840959591801484
#define ETA_FIT_B  0.486492874184007
#define ETA_FIT_C  0.165933086274473
#define ETA_FIT(alpha) \
  (ETA_FIT_A * exp (-(alpha) / ETA_FIT_B) + ETA_FIT_C)

/* declare number formatting conventions. */
#define SFMT ": %.2lf%s\n"
#define FMT(n) \
 (n >= 1e+00 ? n : \
  n >= 1e-03 ? n * 1e+03 : \
  n >= 1e-06 ? n * 1e+06 : \
  n >= 1e-09 ? n * 1e+09 : \
  n >= 1e-12 ? n * 1e+12 : \
  n >= 1e-15 ? n * 1e+15 : n), \
 (n >= 1e+00 ? "" : \
  n >= 1e-03 ? "m" : \
  n >= 1e-06 ? "u" : \
  n >= 1e-09 ? "n" : \
  n >= 1e-12 ? "p" : \
  n >= 1e-15 ? "f" : "")

/* readstring: reads a prompted string response from standard input.
 * @prompt: the prompt string to print for the user.
 */
char *readstring (const char *prompt) {
  /* declare required variables. */
  char buf[256], *ret;
  int i;

  /* print the prompt. */
  fprintf (stdout, "%s: ", prompt);
  
  /* read in the response. */
  if (!fgets (buf, sizeof (buf), stdin))
    return NULL;

  /* chop newline characters off the end of the response. */
  for (i = strlen (buf) - 1; i >= 0 && buf[i] == '\n'; i--)
    buf[i] = '\0';

  /* chop characters after a hash character. */
  for (i = 0; i < strlen (buf); i++) {
    /* check if the current character is a hash. */
    if (buf[i] == '#') {
      /* truncate the string and break the loop. */
      buf[i] = '\0';
      break;
    }
  }

  /* chop whitespace characters off the end of the response. */
  for (i = strlen (buf) - 1; i >= 0 && buf[i] == ' '; i--)
    buf[i] = '\0';

  /* print the response if it was read from a text file. */
  if (!isatty (fileno (stdin)))
    printf ("%s\n", buf);

  /* return the newly duplicated string response. */
  ret = strdup (buf);
  return ret;
}

/* readnumber: reads a double-precision prompted response from stdin.
 * @prompt: the prompt string to print for the user.
 */
double readnumber (const char *prompt) {
  /* declare required variables. */
  double val;
  char *ret;

  /* read in the response string. */
  ret = readstring (prompt);
  if (!ret) return NAN;

  /* convert the first part of the response string to a number. */
  val = atof (ret);

  /* see if the number was suffixed with a metric prefix. */
  switch (ret[strlen (ret) - 1]) {
    /* exa. */
    case 'E':
      val *= 1e+18;
      break;

    /* peta. */
    case 'P':
      val *= 1e+15;
      break;

    /* tera. */
    case 'T':
      val *= 1e+12;
      break;

    /* giga. */
    case 'G':
      val *= 1e+09;
      break;

    /* mega. */
    case 'M':
      val *= 1e+06;
      break;

    /* kilo. */
    case 'k':
      val *= 1e+03;
      break;

    /* centi. */
    case 'c':
      val *= 1e-02;
      break;

    /* milli. */
    case 'm':
      val *= 1e-03;
      break;

    /* micro. */
    case 'u':
      val *= 1e-06;
      break;

    /* nano. */
    case 'n':
      val *= 1e-09;
      break;

    /* pico. */
    case 'p':
      val *= 1e-12;
      break;

    /* femto. */
    case 'f':
      val *= 1e-15;
      break;

    /* atto. */
    case 'a':
      val *= 1e-18;
      break;

    /* anything else... */
    default:
      break;
  }

  /* free the response string and return the number. */
  free (ret);
  return val;
}

/* main: application entry point.
 * @argc: number of commandline arguments.
 * @argv: commandline argument string array.
 */
int main (int argc, char **argv) {
  /* generic required variables. */
  double dl, reml, r_coil;
  char *poliv;
  int i;
  
  /* base coil parameters. */
  double g_wire, d_wire, A_wire, l_wire, rho_wire, mu_wire;
  double d_coil, l_coil, vol_coil, eta_coil, L_coil;
  double r_coil_inner, r_coil_outer;
  double n_turns, n_layers, n_turns_per_layer;
  double R_coil_dc, R_coil_ac_min, R_coil_ac_max;
  double Q_min, Q_max;
  
  /* frequency parameters. */
  double f_min, f_max, bw, bw_adj, w_min, w_max, delta_min, delta_max;
  
  /* voltage parameters. */
  double v_noise_bw, v_noise, v_sig_min, v_sig_max, SNR_min, SNR_max;
  
  /* global parameters. */
  double T, tau1, V_pol, I_pol, P_pol, t_pol, E_pol, frac_pol;
  
  /* get the temperature. */
  T = readnumber ("temperature / K");

  /* get the coil wire gauge. */
  g_wire = readnumber ("coil wire gauge / AWG");
  
  /* calculate the coil wire diameter. */
  d_wire = 1e-03 * exp (2.1104 - 0.1159 * g_wire);
  fprintf (stdout, "coil wire diameter / m" SFMT, FMT (d_wire));
  
  /* calculate the coil cross-sectional area. */
  A_wire = M_PI * d_wire * d_wire / 4.0;
  fprintf (stdout, "coil wire area / m^2: %.4le\n", A_wire);
  
  /* get the coil wire resistivity. */
  rho_wire = readnumber ("coil wire resistivity / ohm-meter");
  
  /* get the coil wire magnetic permeability. */
  mu_wire = readnumber ("coil wire relative magnetic permeability");
  mu_wire *= MU_0;
  
  /* get the coil dimensions. */
  d_coil = readnumber ("coil diameter / m");
  l_coil = readnumber ("coil length / m");
  
  /* calculate the coil volume. */
  vol_coil = M_PI * l_coil * d_coil * d_coil / 4.0;
  fprintf (stdout, "coil inner volume / m^3: %.4le\n", vol_coil);
  
  /* calculate the filling factor. */
  eta_coil = ETA_FIT (d_coil / (2.0 * l_coil));
  fprintf (stdout, "coil filling factor: %.4le\n", eta_coil);
  
  /* get the coil turns number. */
  n_turns = readnumber ("coil turns");
  
  /* calculate the number of turns per layer. */
  n_turns_per_layer = floor (l_coil / d_wire);
  fprintf (stdout, "coil turns per layer: %.2lf\n", n_turns_per_layer);
  
  /* calculate the number of layers. */
  n_layers = n_turns / n_turns_per_layer;
  fprintf (stdout, "coil layers: %.2lf\n", n_layers);

  /* calculate the coil wire length. */
  for (reml = l_wire = 0.0, i = 1; reml < n_layers; reml += dl, i++) {
    /* calculate the relative length of the current layer. */
    dl = n_layers - reml >= 1.0 ? 1.0 : n_layers - reml;
    
    /* calculate the coil radius on this layer. */
    r_coil = (d_coil / 2.0) + d_wire * ((double) i - 0.5);
    
    /* add to the total length of wire. */
    l_wire += (2.0 * M_PI * r_coil * n_turns_per_layer * dl);
  }

  /* print the total length of wire. */
  fprintf (stdout, "coil wire length / m" SFMT, FMT (l_wire));

  /* calculate the coil inner and outer radii. */
  r_coil_inner = d_coil / 2.0;
  r_coil_outer = r_coil_inner + (double) n_layers * d_wire;

  /* calculate the coil inductance. */
  L_coil = (31.6 * r_coil_inner * r_coil_inner
    * (double) n_turns * (double) n_turns)
    / (6.0 * r_coil_inner + 9.0 * l_coil
    + 10.0 * (r_coil_outer - r_coil_inner));
  L_coil *= 1.0e-6;
  fprintf (stdout, "coil inductance / H" SFMT, FMT (L_coil));
  
  /* calculate the coil dc resistance. */
  R_coil_dc = rho_wire * l_wire / A_wire;
  fprintf (stdout, "coil dc resistance / ohm" SFMT, FMT (R_coil_dc));
  
  /* get the frequency bounds of operation. */
  f_min = readnumber ("minimum signal frequency / hz");
  f_max = readnumber ("maximum signal frequency / hz");

  /* get the bandwidth correction factor. */
  bw_adj = readnumber ("filter bandwidth correction factor");
  
  /* calculate the signal bandwidth. */
  bw = bw_adj * (f_max - f_min);
  fprintf (stdout, "effective signal bandwidth / hz: %.2lf\n", bw);
  
  /* calculate the angular frequencies of operation. */
  w_min = 2.0 * M_PI * f_min;
  w_max = 2.0 * M_PI * f_max;

  /* calculate the skin depth values. */
  delta_min = sqrt ((2.0 * rho_wire) / (w_max * mu_wire));
  delta_max = sqrt ((2.0 * rho_wire) / (w_min * mu_wire));
  fprintf (stdout, "coil wire skin depth / m: %.2lf%s - %.2lf%s\n",
           FMT (delta_min), FMT (delta_max));

  /* calculate the coil ac resistance. */
  R_coil_ac_min = (rho_wire * l_wire) / 
                  (M_PI * delta_max * (d_wire - delta_max));
  R_coil_ac_max = (rho_wire * l_wire) / 
                  (M_PI * delta_min * (d_wire - delta_min));

  /* fix up the ac resistances. */
  R_coil_ac_min = (R_coil_ac_min > R_coil_dc ? R_coil_ac_min : R_coil_dc);
  R_coil_ac_max = (R_coil_ac_max > R_coil_dc ? R_coil_ac_max : R_coil_dc);

  /* print the ac resistances. */
  fprintf (stdout, "minimum coil wire ac resistance / ohm" SFMT,
           FMT (R_coil_ac_min));
  fprintf (stdout, "maximum coil wire ac resistance / ohm" SFMT,
           FMT (R_coil_ac_max));
  fprintf (stdout, "skin effect influences coil resistance: %s\n",
           R_coil_ac_min > R_coil_dc || R_coil_ac_max > R_coil_dc ?
           "yes" : "no");

  /* calculate the coil quality factors. */
  Q_min = w_min * L_coil / R_coil_ac_max;
  Q_max = w_max * L_coil / R_coil_ac_min;
  fprintf (stdout, "coil quality factor: %.2lf - %.2lf\n",
           FMT (Q_min), FMT (Q_max));

  /* calculate the thermal noise voltage. */
  v_noise_bw = sqrt (4.0 * K_B * T * R_coil_ac_max);
  v_noise = sqrt (4.0 * K_B * T * R_coil_ac_max * bw);
  fprintf (stdout, "coil thermal noise / v/sqrt(hz)" SFMT, FMT (v_noise_bw));
  fprintf (stdout, "coil rms thermal noise / v" SFMT, FMT (v_noise));

  /* calculate the water longitudinal relaxation time. */
  tau1 = T1_FIT (T);
  fprintf (stdout, "water T1 / s" SFMT, FMT (tau1));

  /* get whether the polarization is set by current or voltage. */
  poliv = readstring ("polarization type [i/v]");
  
  /* see how the polarization is controlled. */
  if (strncmp (poliv, "i", 1) == 0) {
    /* get the polarization current. */
    I_pol = readnumber ("polarization current / A");

    /* calculate the polarization voltage. */
    V_pol = I_pol * R_coil_dc;
    fprintf (stdout, "polarization voltage / v" SFMT, FMT (V_pol));
  }
  else {
    /* get the polarization voltage. */
    V_pol = readnumber ("polarization voltage / v");

    /* calculate the polarization current. */
    I_pol = V_pol / R_coil_dc;
    fprintf (stdout, "polarization current / A" SFMT, FMT (I_pol));
  }
  
  /* free the polarization type response string. */
  free (poliv);
  
  /* calculate the polarization power. */
  P_pol = V_pol * I_pol;
  fprintf (stdout, "polarization power / W" SFMT, FMT (P_pol));
  
  /* get the polarization time. */
  t_pol = readnumber ("polarization time / s");
  
  /* calculate the polarization energy. */
  E_pol = P_pol * t_pol;
  fprintf (stdout, "polarization energy / J" SFMT, FMT (E_pol));

  /* calculate the polarization fraction. */
  frac_pol = 1.0 - exp (-t_pol / tau1);
  fprintf (stdout, "polarization fraction: %.2lf%%\n", 100.0 * frac_pol);
  
  /* calculate the signal voltages. */
  v_sig_min =
    (CHI_V * w_min * MU_0 * n_turns * n_turns *
     I_pol * frac_pol * eta_coil * vol_coil) /
  (sqrt (2.0) * l_coil * l_coil);
  v_sig_max =
    (CHI_V * w_max * MU_0 * n_turns * n_turns *
     I_pol * frac_pol * eta_coil * vol_coil) /
    (sqrt (2.0) * l_coil * l_coil);
  fprintf (stdout, "minimum signal voltage / v" SFMT, FMT (v_sig_min));
  fprintf (stdout, "maximum signal voltage / v" SFMT, FMT (v_sig_max));
  
  /* calculate the signal to noise ratios. */
  SNR_min = v_sig_min / v_noise;
  SNR_max = v_sig_max / v_noise;
  fprintf (stdout, "signal to noise ratio: %.2lf - %.2lf\n",
           SNR_min, SNR_max);
  
  /* return success. */
  return 0;
}

