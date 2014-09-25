
/* pyppm: A python C extension module for interfacing to the PPM device.
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

/* include the python headers. */
#include <Python.h>
#include "structmember.h"

/* include the ppm core header. */
#include <ppm.h>

/* declare the globally available ppm type structure. */
static PyTypeObject PyPPM_Type;

/* PyPPM_Check: macro to check if a PyObject is of type PyPPM_Type.
 */
#define PyPPM_Check(v) (Py_TYPE (v) == &PyPPM_Type)

/* PyPPM_DELAY: macro to wait for the device to breathe a bit.
 */
#define PyPPM_DELAY()  (usleep (200000))

/* PyPPM: the python object structure for ppm types.
 */
typedef struct {
  PyObject_HEAD

  /* ppm device filename. */
  char *fname;

  /* ppm hardware version. */
  int ver, rev;

  /* ppm device pulse program. */
  unsigned int szpp;
  ppm_prog pp;
  int fresh;
}
PyPPM;

/* pyppm_unpack_prog: method to unpack a list of lists into a pulse program
 * data structure.
 */
static int
pyppm_unpack_prog (PyObject *L, ppm_prog *pp) {
  /* declare required variables. */
  PyObject *l, *s;
  unsigned int ipp;
  int iL, nL, nl;

  /* declare variables to get the command string. */
  unsigned int cmd;
  PyObject *bytes;
  char *str;

  /* ensure the object is a list. */
  if (!PyList_Check (L)) {
    /* throw an exception. */
    PyErr_SetString (PyExc_TypeError, "list expected");
    return 0;
  }

  /* get the length of the outer list. */
  nL = PyList_Size (L);

  /* initialize the contents of the pulse program array and initialize the
   * array index that will be used during traversal.
   */
  ppm_prog_zero (pp);
  ipp = 0;

  /* loop over the elements of the outer list. */
  for (iL = 0; iL < nL; iL++) {
    /* extract the current item of the outer list. */
    l = PyList_GetItem (L, iL);

    /* ensure the object is an allocated list. */
    if (!l || !PyList_Check (l)) {
      /* throw an exception. */
      PyErr_Format (PyExc_TypeError, "list expected at index %d", iL);
      return 0;
    }

    /* get the length of the inner list. */
    nl = PyList_Size (l);

    /* ensure the list is non-empty. */
    if (nl < 1) {
      /* throw an exception. */
      PyErr_SetString (PyExc_ValueError, "list must not be empty");
      return 0;
    }

    /* get the first list item. */
    s = PyList_GetItem (l, 0);

    /* ensure the first list item is a string. */
    if (!PyUnicode_Check (s)) {
      /* throw an exception. */
      PyErr_Format (PyExc_TypeError, "string expected at index %d,0", iL);
      return 0;
    }

    /* get the ascii value of the object. */
    bytes = PyUnicode_AsASCIIString (s);
    if (!bytes) {
      /* throw an exception. */
      PyErr_SetString (PyExc_TypeError, "failed to convert unicode to bytes");
      return 0;
    }

    /* get the native string. */
    str = PyBytes_AsString (bytes);
    if (!str) {
      /* throw an exception. */
      PyErr_SetString (PyExc_TypeError, "failed to convert bytes to string");
      return 0;
    }

    /* identify the command byte from the string representation. */
    cmd = ppm_prog_id_from_string (str);

    /* act based on the command byte. */
    switch (cmd) {
      /* short, imprecise delay. */
      case PPM_PULPROG_DEADTIME:
        /* ensure the list is sized correctly. */
        if (PyList_Size (l) != 2) {
          /* throw an exception. */
          PyErr_SetString (PyExc_ValueError, "invalid argument count");
          return 0;
        }

        /* read out the arguments. */
        double dt_ms = PyFloat_AsDouble (PyList_GetItem (l, 1));

        /* add the command bytes into the pulse program array. */
        ppm_prog_add_deadtime (pp, &ipp, dt_ms);

        /* break out. */
        break;

      /* precise delay. */
      case PPM_PULPROG_DELAY:
        /* ensure the list is sized correctly. */
        if (PyList_Size (l) != 2) {
          /* throw an exception. */
          PyErr_SetString (PyExc_ValueError, "invalid argument count");
          return 0;
        }

        /* read out the arguments. */
        double delay_s = PyFloat_AsDouble (PyList_GetItem (l, 1));

        /* add the command bytes into the pulse program array. */
        ppm_prog_add_delay (pp, &ipp, delay_s);

        /* break out. */
        break;

      /* polarization control. */
      case PPM_PULPROG_POLARIZE:
        /* ensure the list is sized correctly. */
        if (PyList_Size (l) != 2) {
          /* throw an exception. */
          PyErr_SetString (PyExc_ValueError, "invalid argument count");
          return 0;
        }

        /* read out the arguments. */
        long ccs_en = PyLong_AsLong (PyList_GetItem (l, 1));

        /* add the command bytes into the pulse program array. */
        ppm_prog_add_polarize (pp, &ipp, ccs_en);

        /* break out. */
        break;

      /* relay control. */
      case PPM_PULPROG_RELAY:
        /* ensure the list is sized correctly. */
        if (PyList_Size (l) != 2) {
          /* throw an exception. */
          PyErr_SetString (PyExc_ValueError, "invalid argument count");
          return 0;
        }

        /* read out the arguments. */
        long rel_en = PyLong_AsLong (PyList_GetItem (l, 1));

        /* add the command bytes into the pulse program array. */
        ppm_prog_add_relay (pp, &ipp, rel_en);

        /* break out. */
        break;

      /* acquisition. */
      case PPM_PULPROG_ACQUIRE:
        /* ensure the list is sized correctly. */
        if (PyList_Size (l) != 3) {
          /* throw an exception. */
          PyErr_SetString (PyExc_ValueError, "invalid argument count");
          return 0;
        }

        /* read out the arguments. */
        long acq_pts = PyLong_AsLong (PyList_GetItem (l, 1));
        double acq_rate = PyFloat_AsDouble (PyList_GetItem (l, 2));

        /* add the command bytes into the pulse program array. */
        ppm_prog_add_acquire (pp, &ipp, acq_pts, acq_rate);

        /* break out. */
        break;

      /* adiabatic polarization rising,falling edge. */
      case PPM_PULPROG_TXRISE:
      case PPM_PULPROG_TXFALL:
        /* ensure the list is sized correctly. */
        if (PyList_Size (l) != 3) {
          /* throw an exception. */
          PyErr_SetString (PyExc_ValueError, "invalid argument count");
          return 0;
        }

        /* read out the arguments. */
        double txe_ms = PyFloat_AsDouble (PyList_GetItem (l, 1));
        double txe_amp = PyFloat_AsDouble (PyList_GetItem (l, 2));

        /* add the command bytes into the pulse program array. */
        ppm_prog_add_txedge (pp, &ipp, cmd, txe_ms, txe_amp);

        /* break out. */
        break;

      /* sinusoidal pulse. */
      case PPM_PULPROG_TXPULSE:
        /* ensure the list is sized correctly. */
        if (PyList_Size (l) != 4) {
          /* throw an exception. */
          PyErr_SetString (PyExc_ValueError, "invalid argument count");
          return 0;
        }

        /* read out the arguments. */
        double pul_t = PyFloat_AsDouble (PyList_GetItem (l, 1));
        double pul_f = PyFloat_AsDouble (PyList_GetItem (l, 2));
        double pul_a = PyFloat_AsDouble (PyList_GetItem (l, 3));

        /* add the command bytes into the pulse program array. */
        ppm_prog_add_txpulse (pp, &ipp, pul_t, pul_f, pul_a);


        /* break out. */
        break;

      /* capacitive tuning. */
      case PPM_PULPROG_TUNE:
        /* ensure the list is sized correctly. */
        if (PyList_Size (l) != 2) {
          /* throw an exception. */
          PyErr_SetString (PyExc_ValueError, "invalid argument count");
          return 0;
        }

        /* read out the arguments. */
        double tun_f = PyFloat_AsDouble (PyList_GetItem (l, 1));

        /* add the command bytes into the pulse program array. */
        ppm_prog_add_tune (pp, &ipp, tun_f);

        /* break out. */
        break;

      /* x,y,z-axis shim. */
      case PPM_PULPROG_SHIM_X:
      case PPM_PULPROG_SHIM_Y:
      case PPM_PULPROG_SHIM_Z:
        /* ensure the list is sized correctly. */
        if (PyList_Size (l) != 2) {
          /* throw an exception. */
          PyErr_SetString (PyExc_ValueError, "invalid argument count");
          return 0;
        }

        /* read out the arguments. */
        double shm = PyFloat_AsDouble (PyList_GetItem (l, 1));

        /* add the command bytes into the pulse program array. */
        ppm_prog_add_shim (pp, &ipp, cmd, shm);


        /* break out. */
        break;

      /* end execution. */
      case PPM_PULPROG_END:
        break;

      /* unrecognized command. */
      default:
        /* throw an exception. */
        PyErr_SetString (PyExc_ValueError, "unrecognized command byte");
        return 0;
    }
  }

  /* return success. */
  return 1;
}

/* pyppm_pack_prog_iter: iterator for pyppm_pack_prog(). */
void pyppm_pack_prog_iter (uint8_t cmd, uint8_t *args, void *pdata) {
  /* declare required variables. */
  double t32, f32, a16, d16, a8;
  long n32;

  /* declare python list objects. */
  PyObject *L, *l;

  /* dereference the master list object. */
  L = (PyObject *) pdata;
  l = NULL;

  /* determine the current command type. */
  switch (cmd) {
    /* short, imprecise delay. */
    case PPM_PULPROG_DEADTIME:
      /* create a new inner list. */
      l = PyList_New (0);

      /* pack the command name into the list. */
      PyList_Append (l, PyUnicode_FromString (PPM_PULPROG_STR_DEADTIME));

      /* pack the arguments into the list. */
      d16 = (double) WORD (args[0], args[1]);
      d16 *= PPM_PULPROG_F_DT_DEADTIME;
      PyList_Append (l, PyFloat_FromDouble (d16));

      /* break out. */
      break;

    /* precise delay. */
    case PPM_PULPROG_DELAY:
      /* create a new inner list. */
      l = PyList_New (0);

      /* pack the command name into the list. */
      PyList_Append (l, PyUnicode_FromString (PPM_PULPROG_STR_DELAY));

      /* pack the arguments into the list. */
      d16 = (double) WORD (args[0], args[1]);
      d16 *= PPM_PULPROG_F_DT_DELAY;
      PyList_Append (l, PyFloat_FromDouble (d16));

      /* break out. */
      break;

    /* polarization control. */
    case PPM_PULPROG_POLARIZE:
      /* create a new inner list. */
      l = PyList_New (0);

      /* pack the command name into the list. */
      PyList_Append (l, PyUnicode_FromString (PPM_PULPROG_STR_POLARIZE));

      /* pack the arguments into the list. */
      PyList_Append (l, PyBool_FromLong ((long) args[0]));

      /* break out. */
      break;

    /* relay control. */
    case PPM_PULPROG_RELAY:
      /* create a new inner list. */
      l = PyList_New (0);

      /* pack the command name into the list. */
      PyList_Append (l, PyUnicode_FromString (PPM_PULPROG_STR_RELAY));

      /* pack the arguments into the list. */
      PyList_Append (l, PyBool_FromLong ((long) args[0]));

      /* break out. */
      break;

    /* acquisition. */
    case PPM_PULPROG_ACQUIRE:
      /* create a new inner list. */
      l = PyList_New (0);

      /* pack the command name into the list. */
      PyList_Append (l, PyUnicode_FromString (PPM_PULPROG_STR_ACQUIRE));

      /* pack the first argument into the list. */
      n32 = (long) WORD32 (args[0], args[1], args[2], args[3]);
      PyList_Append (l, PyLong_FromLong (n32));

      /* pack the second argument into the list. */
      d16 = (double) WORD (args[4], args[5]);
      d16 = PPM_PULPROG_F_DT_ACQUIRE / d16;
      PyList_Append (l, PyFloat_FromDouble (d16));

      /* break out. */
      break;

    /* adiabatic polarization rising edge. */
    case PPM_PULPROG_TXRISE:
      /* create a new inner list. */
      l = PyList_New (0);

      /* pack the command name into the list. */
      PyList_Append (l, PyUnicode_FromString (PPM_PULPROG_STR_TXRISE));

      /* pack the first argument into the list. */
      d16 = (double) WORD (args[0], args[1]);
      d16 *= PPM_PULPROG_F_DT_TXEDGE;
      PyList_Append (l, PyFloat_FromDouble (d16));

      /* pack the second argument into the list. */
      a16 = (double) WORD (args[2], args[3]);
      PyList_Append (l, PyFloat_FromDouble (a16 / pow (2.0, 15.0)));

      /* break out. */
      break;

    /* adiabatic polarization falling edge. */
    case PPM_PULPROG_TXFALL:
      /* create a new inner list. */
      l = PyList_New (0);

      /* pack the command name into the list. */
      PyList_Append (l, PyUnicode_FromString (PPM_PULPROG_STR_TXFALL));

      /* pack the first argument into the list. */
      d16 = (double) WORD (args[0], args[1]);
      d16 *= PPM_PULPROG_F_DT_TXEDGE;
      PyList_Append (l, PyFloat_FromDouble (d16));

      /* pack the second argument into the list. */
      a16 = (double) WORD (args[2], args[3]);
      PyList_Append (l, PyFloat_FromDouble (a16 / pow (2.0, 15.0)));

      /* break out. */
      break;

    /* sinusoidal pulse. */
    case PPM_PULPROG_TXPULSE:
      /* create a new inner list. */
      l = PyList_New (0);

      /* pack the command name into the list. */
      PyList_Append (l, PyUnicode_FromString (PPM_PULPROG_STR_TXPULSE));

      /* pack the first argument into the list. */
      t32 = (double) WORD32 (args[0], args[1], args[2], args[3]);
      t32 *= PPM_PULPROG_F_DT_TXPULSE;
      PyList_Append (l, PyFloat_FromDouble (t32));

      /* pack the second argument into the list. */
      f32 = (double) WORD32 (args[4], args[5], args[6], args[7]);
      f32 *= PPM_PULPROG_F_TW_TXPULSE;
      PyList_Append (l, PyFloat_FromDouble (f32));

      /* pack the third argument into the list. */
      a8 = (double) args[8];
      a8 /= PPM_PULPROG_F_AV_TXPULSE;
      PyList_Append (l, PyFloat_FromDouble (a8));

      /* break out. */
      break;
 
    /* capacitive tuning. */
    case PPM_PULPROG_TUNE:
      /* create a new inner list. */
      l = PyList_New (0);

      /* pack the command name into the list. */
      PyList_Append (l, PyUnicode_FromString (PPM_PULPROG_STR_TUNE));

      /* pack the arguments into the list. */
      a16 = (double) WORD (args[0], args[1]);
      PyList_Append (l, PyFloat_FromDouble (a16 / (pow (2.0, 16.0) - 1.0)));

      /* break out. */
      break;
 
    /* shimming. */
    case PPM_PULPROG_SHIM_X:
    case PPM_PULPROG_SHIM_Y:
    case PPM_PULPROG_SHIM_Z:
      /* create a new inner list. */
      l = PyList_New (0);

      /* pack the command name into the list. */
      if (cmd == PPM_PULPROG_SHIM_X)
        PyList_Append (l, PyUnicode_FromString (PPM_PULPROG_STR_SHIM_X));
      else if (cmd == PPM_PULPROG_SHIM_Y)
        PyList_Append (l, PyUnicode_FromString (PPM_PULPROG_STR_SHIM_Y));
      else
        PyList_Append (l, PyUnicode_FromString (PPM_PULPROG_STR_SHIM_Z));

      /* pack the arguments into the list. */
      a16 = (double) WORD (args[0], args[1]);
      PyList_Append (l, PyFloat_FromDouble (a16 / (pow (2.0, 16.0) - 1.0)));

      /* break out. */
      break;
 
    /* end execution. */
    case PPM_PULPROG_END:
      /* create a new inner list. */
      l = PyList_New (0);

      /* pack the command name into the list. */
      PyList_Append (l, PyUnicode_FromString (PPM_PULPROG_STR_END));

      /* break out. */
      break;

    /* unrecognized command. */
    default:
      /* silently ignore it... */
      break;
  }

  /* check if a new list item was created. */
  if (l) {
    /* append the item onto the master list. */
    PyList_Append (L, l);
  }
}

/* pyppm_pack_prog: method to pack a pulse program data structure into a
 * list of lists.
 */
static PyObject*
pyppm_pack_prog (ppm_prog *pp) {
  /* declare required variables. */
  PyObject *obj;

  /* create a new list. */
  obj = PyList_New (0);
  if (!obj)
    return NULL;

  /* iterate through the pulse program, building the innards of the list
   * as we go along.
   */
  ppm_prog_iterate (pp, pyppm_pack_prog_iter, obj);

  /* return the packed list of lists. */
  return obj;
}

/* pyppm_unpack_data: method to unpack a tuple of tuples into an acquisition
 * data structure.
 */
static int
pyppm_unpack_data (PyObject *args, ppm_data *dat) {
  /* declare required variables. */
  PyObject *xv, *x, *v;
  unsigned int i, n;
  double xi, vi;

  /* try to parse a straight tuple first. */
  if (!PyArg_ParseTuple (args, "O", &xv)) {
    /* clear the error so we can try again. */
    PyErr_Clear ();

    /* try to parse a pair of tuples. */
    if (!PyArg_ParseTuple (args, "OO", &x, &v))
      return 0;
  }
  else {
    /* ensure the object is a tuple. */
    if (!PyTuple_Check (xv)) {
      /* throw an exception. */
      PyErr_SetString (PyExc_TypeError, "tuple expected");
      return 0;
    }

    /* ensure the tuple contains two elements. */
    if (PyTuple_Size (xv) != 2) {
      /* throw an exception. */
      PyErr_SetString (PyExc_TypeError, "two-element tuple expected");
      return 0;
    }

    /* borrow references to the first two tuple elements. */
    x = PyTuple_GetItem (xv, 0);
    v = PyTuple_GetItem (xv, 1);
  }

  /* ensure the objects are tuples. */
  if (!PyTuple_Check (x) || !PyTuple_Check (v)) {
    /* throw an exception. */
    PyErr_SetString (PyExc_TypeError, "pair of tuples expected");
    return 0;
  }

  /* ensure the tuple lengths match. */
  if (PyTuple_Size (x) != PyTuple_Size (v)) {
    /* throw an exception. */
    PyErr_SetString (PyExc_TypeError, "tuple lengths do not match");
    return 0;
  }

  /* store the data length. */
  n = (unsigned int) PyTuple_Size (x);

  /* allocate memory to store the contents of the tuples. */
  if (!ppm_data_alloc (dat, n)) {
    /* throw an exception. */
    PyErr_SetString (PyExc_MemoryError, "failed to allocate local memory");
    return 0;
  }

  /* loop through the tuple elements. */
  for (i = 0; i < n; i++) {
    /* grab the tuple elements. */
    xi = PyFloat_AsDouble (PyTuple_GetItem (x, i));
    vi = PyFloat_AsDouble (PyTuple_GetItem (v, i));

    /* store the values. */
    dat->x[i] = xi;
    dat->v[i] = vi;
  }

  /* return success. */
  return 1;
}

/* pyppm_pack_data: method to pack an acquisition data structure into a
 * tuple of tuples.
 */
static PyObject*
pyppm_pack_data (ppm_data *dat) {
  /* declare required variables. */
  PyObject *obj, *x, *v;
  unsigned int i;
  double xi, vi;

  /* check if the data to pack is empty, and return nothing if so. */
  if (dat->n == 0)
    Py_RETURN_NONE;

  /* create a tuple object to fill with time values. */
  x = PyTuple_New (dat->n);
  if (!x) return NULL;

  /* create a tuple object to fill with amplitude values. */
  v = PyTuple_New (dat->n);
  if (!v) return NULL;

  /* loop through the data points. */
  for (i = 0; i < dat->n; i++) {
    /* refer to the currently indexed time and amplitude values. */
    xi = dat->x[i];
    vi = dat->v[i];

    /* update the currently indexed entries in the tuples. */
    PyTuple_SET_ITEM (x, i, PyFloat_FromDouble (xi));
    PyTuple_SET_ITEM (v, i, PyFloat_FromDouble (vi));
  }

  /* build a tuple of the time and amplitude tuples. */
  obj = PyTuple_Pack (2, x, v);

  /* return the packed tuple of tuples. */
  return obj;
}

/* pyppm_pack_fall: method to pack a waterfall data structure into a
 * tuple of tuples (of tuples).
 */
static PyObject*
pyppm_pack_fall (ppm_fall *dat) {
  /* declare required variables. */
  PyObject *obj, *tobj, *fobj, *vobj, *rowobj;
  unsigned int i, j;

  /* create the time tuple object. */
  tobj = PyTuple_New (dat->ntime);
  if (!tobj) return NULL;

  /* create the frequency tuple object. */
  fobj = PyTuple_New (dat->nfreq);
  if (!fobj) return NULL;

  /* create the column tuple object. */
  vobj = PyTuple_New (dat->ntime);
  if (!vobj) return NULL;

  /* loop through the time values. */
  for (i = 0; i < dat->ntime; i++)
    PyTuple_SET_ITEM (tobj, i, PyFloat_FromDouble (dat->t[i]));

  /* loop through the frequency values. */
  for (j = 0; j < dat->nfreq; j++)
    PyTuple_SET_ITEM (fobj, j, PyFloat_FromDouble (dat->f[j]));

  /* loop through the time values. */
  for (i = 0; i < dat->ntime; i++) {
    /* create a row tuple object. */
    rowobj = PyTuple_New (dat->nfreq);
    if (!rowobj) return NULL;

    /* loop through the values of the current row. */
    for (j = 0; j < dat->nfreq; j++)
      PyTuple_SET_ITEM (rowobj, j, PyFloat_FromDouble (dat->v[i][j]));

    /* add the row tuple into the main tuple. */
    PyTuple_SET_ITEM (vobj, i, rowobj);
  }

  /* build a tuple of the time, frequency and amplitude tuples. */
  obj = PyTuple_Pack (3, tobj, fobj, vobj);

  /* return the packed tuple of tuples. */
  return obj;
}

/* pyppm_math_fft: module method for fast Fourier transforms.
 */
static PyObject*
pyppm_math_fft (PyObject *self, PyObject *args) {
  /* declare required variables. */
  ppm_data tacq, facq;
  PyObject *obj;

  /* initialize the output object. */
  obj = NULL;

  /* unpack the python arguments into an acquisition data structure. */
  if (!pyppm_unpack_data (args, &tacq))
    return NULL;

  /* compute the fft. */
  if (!ppm_fft (&tacq, &facq)) {
    /* throw an exception. */
    PyErr_SetString (PyExc_ArithmeticError, "failed to compute the fft");
    return NULL;
  }

  /* build a two-element tuple containing the results. */
  obj = pyppm_pack_data (&facq);

  /* free the acquisition data structures. */
  ppm_data_empty (&tacq);
  ppm_data_empty (&facq);

  /* return the results tuple. */
  return obj;
}

/* pyppm_math_hrft: module method for high-resolution Fourier transforms.
 */
static PyObject*
pyppm_math_hrft (PyObject *self, PyObject *args, PyObject *keywords) {
  /* declare required variables. */
  PyObject *obj, *arg, *targ, *aarg;
  ppm_data tacq, facq;
  double fmin, fmax;

  /* declare the keyword lists. */
  static char *kwlist1[] = { "data", "fmin", "fmax", NULL };
  static char *kwlist2[] = { "t", "a", "fmin", "fmax", NULL };

  /* initialize the output object. */
  obj = NULL;

  /* parse the method arguments and keywords. */
  if (PyArg_ParseTupleAndKeywords (args, keywords, "Odd", kwlist1,
                                   &arg, &fmin, &fmax)) {
    /* unpack the python arguments into an acquisition data structure. */
    if (!pyppm_unpack_data (arg, &tacq))
      return NULL;
  }
  else {
    /* clear the error so we can try again. */
    PyErr_Clear ();

    /* try to parse with two object arguments. */
    if (PyArg_ParseTupleAndKeywords (args, keywords, "OOdd", kwlist2,
                                     &targ, &aarg, &fmin, &fmax)) {
      /* build a tuple to hold the two objects. */
      arg = PyTuple_Pack (2, targ, aarg);
      if (!arg) return NULL;

      /* unpack the python arguments into an acquisition data structure. */
      if (!pyppm_unpack_data (arg, &tacq))
        return NULL;

      /* free the tuple we built a second ago. wasteful, i know. */
      Py_DECREF (arg);
    }
    else
      return NULL;
  }

  /* compute the hrft. */
  if (!ppm_hrft (&tacq, &facq, fmin, fmax)) {
    /* throw an exception. */
    PyErr_SetString (PyExc_ArithmeticError, "failed to compute the hrft");
    return NULL;
  }

  /* build a two-element tuple containing the results. */
  obj = pyppm_pack_data (&facq);

  /* free the acquisition data structures. */
  ppm_data_empty (&tacq);
  ppm_data_empty (&facq);

  /* return the results tuple. */
  return obj;
}

/* pyppm_math_stft: module method for short-time Fourier transforms.
 */
static PyObject*
pyppm_math_stft (PyObject *self, PyObject *args) {
  /* declare required variables. */
  ppm_data tacq;
  ppm_fall wfall;
  PyObject *obj;

  /* initialize the output object. */
  obj = NULL;

  /* unpack the python arguments into an acquisition data structure. */
  if (!pyppm_unpack_data (args, &tacq))
    return NULL;

  /* compute the stft. */
  if (!ppm_wfall (&tacq, &wfall)) {
    /* throw an exception. */
    PyErr_SetString (PyExc_ArithmeticError, "failed to compute the stft");
    return NULL;
  }

  /* build a multi-element tuple containing the results. */
  obj = pyppm_pack_fall (&wfall);

  /* free the acquisition data structures. */
  ppm_data_empty (&tacq);
  ppm_fall_empty (&wfall);

  /* return the results tuple. */
  return obj;
}

/* PyPPM_refresh: pulprog refresh method for ppm objects.
 */
static int
PyPPM_refresh (PyPPM *self) {
  /* check if the pulprog needs to be re-synced from the device. */
  if (self->fresh)
    return 1;

  /* open the device to ascertain the pulse program size. */
  int fd = ppm_device_open (self->fname);

  /* check if the device was opened successfully. */
  if (fd == -1) {
    /* throw an exception. */
    PyErr_SetString (PyExc_IOError, "failed to open device");
    return 0;
  }

  /* get the device version information. */
  if (!ppm_ver_fd (fd, &self->ver, &self->rev)) {
    /* throw an exception. */
    PyErr_SetString (PyExc_IOError, "failed to retrieve device version");
    return 0;
  }

  /* get the device pulse program array size. */
  self->szpp = ppm_szpp_fd (fd);

  /* check if the size was retrieved successfully. */
  if (!self->szpp) {
    /* throw an exception. */
    PyErr_SetString (PyExc_IOError, "failed to retrieve pulprog size");
    return 0;
  }

  /* attempt to read the pulse program data. */
  if (!ppm_rpp_fd (fd, &self->pp)) {
    /* throw an exception. */
    PyErr_SetString (PyExc_IOError, "failed to retrieve device pulprog");
    return 0;
  }

  /* close the device. */
  ppm_device_close (fd);

  /* wait a while for the device to catch up. */
  PyPPM_DELAY ();

  /* indicate that the parameters are now synced. */
  self->fresh = 1;

  /* return success. */
  return 1;
}

/* PyPPM_new: allocation method for ppm objects.
 */
static PyObject*
PyPPM_new (PyTypeObject *type, PyObject *args, PyObject *keywords) {
  /* declare required variables. */
  PyPPM *self;

  /* allocate the new object. */
  self = (PyPPM *) type->tp_alloc (type, 0);

  /* check if allocation was successful. */
  if (self != NULL) {
    /* initialize the device filename. */
    self->fname = NULL;

    /* initialize the freshness of the pulprog info. */
    self->fresh = 0;
  }

  /* return the newly allocated object pointer. */
  return (PyObject *) self;
}

/* PyPPM_init: initialization method for ppm objects.
 */
static int
PyPPM_init (PyPPM *self, PyObject *args, PyObject *keywords) {
  /* declare the keyword list. */
  static char *kwlist[] = { "filename", NULL };

  /* attempt to parse the method arguments. */
  if (!PyArg_ParseTupleAndKeywords (args, keywords, "|z", kwlist,
                                    &self->fname))
    return -1;

  /* attempt a device refresh. */
  if (!PyPPM_refresh (self))
    return -1;

  /* wait a while for the device to catch up. */
  PyPPM_DELAY ();

  /* return success. */
  return 0;
}

/* PyPPM_dealloc: deallocation method for ppm objects.
 */
static void
PyPPM_dealloc (PyPPM *self) {
  /* free the memory allocated for the core structures. */
  /* for now, nothing to free. */

  /* release memory allocated for the object. */
  Py_TYPE (self)->tp_free ((PyObject *) self);
}

/* PyPPM_repr: representation method for ppm objects.
 */
static PyObject*
PyPPM_repr (PyPPM *self) {
  /* declare required variables. */
  PyObject *obj;
  long addy;

  /* get the memory address. */
  addy = (long) self;

  /* build the representation string. */
  obj = PyUnicode_FromFormat ("<pyppm.PPM v%dr%d '%s' at 0x%x>",
                              self->ver, self->rev,
                              self->fname ?
                                self->fname :
                                PPM_DEVICE_FILENAME,
                              addy);

  /* return the python object. */
  return obj;
}

/* PyPPM_getfilename: getter method for ppm object 'filename' attributes.
 */
static PyObject*
PyPPM_getfilename (PyPPM *self, void *closure) {
  /* declare required variables. */
  PyObject *obj;

  /* convert the ppm filename into a python object. */
  if (self->fname)
    obj = PyUnicode_FromString (self->fname);
  else
    obj = PyUnicode_FromString (PPM_DEVICE_FILENAME);

  /* return the python object. */
  return obj;
}

/* PyPPM_setfilename: setter method for ppm object 'filename' attributes.
 */
static int
PyPPM_setfilename (PyPPM *self, PyObject *value, void *closure) {
  /* declare required variables. */
  char *filename;
  PyObject *bytes;

  /* ensure the value is defined. */
  if (!value) {
    /* throw an exception. */
    PyErr_SetString (PyExc_TypeError, "cannot unset the filename attribute");
    return -1;
  }

  /* ensure the value is of the correct type. */
  if (!PyUnicode_Check (value)) {
    /* raise an error. */
    PyErr_SetString (PyExc_TypeError,
                     "the filename attribute must be a string");

    /* return the exception code. */
    return -1;
  }

  /* get the ascii value of the attribute. */
  bytes = PyUnicode_AsASCIIString (value);
  if (!bytes)
    return -1;

  /* get the native string. */
  filename = PyBytes_AsString (bytes);
  if (!filename)
    return -1;

  /* decrement the reference count of the converted bytes object. */
  Py_DECREF (bytes);

  /* check for a device at the indicated filename. */
  if (!ppm_chk (filename)) {
    /* throw an exception. */
    PyErr_SetString (PyExc_IOError, "failed to open device");
    return -1;
  }

  /* all went well to this point. set the new filename. */
  self->fname = filename;

  /* refresh the device information. */
  self->fresh = 0;
  if (!PyPPM_refresh (self))
    return -1;

  /* return success. */
  return 0;
}

/* PyPPM_getversion: getter method for ppm object 'version' attributes.
 */
static PyObject*
PyPPM_getversion (PyPPM *self, void *closure) {
  /* create and return the major and minor version numbers in a tuple. */
  return Py_BuildValue ("(ii)", self->ver, self->rev);
}

/* PyPPM_setversion: setter method for ppm object 'version' attributes.
 */
static int
PyPPM_setversion (PyPPM *self, PyObject *value, void *closure) {
  /* throw an exception. */
  PyErr_SetString (PyExc_TypeError, "cannot set version attribute");
  return -1;
}

/* PyPPM_getprog: getter method for ppm object 'pulprog' attributes.
 */
static PyObject*
PyPPM_getprog (PyPPM *self, void *closure) {
  /* ensure that the parameters are up to date. */
  if (!PyPPM_refresh (self))
    return NULL;

  /* pack the pulse program into a new list object. */
  return pyppm_pack_prog (&self->pp);
}

/* PyPPM_setprog: setter method for ppm object 'pulprog' attributes.
 */
static int
PyPPM_setprog (PyPPM *self, PyObject *value, void *closure) {
  /* ensure the value is defined. */
  if (!value) {
    /* throw an exception. */
    PyErr_SetString (PyExc_TypeError, "cannot unset the pulprog attribute");
    return -1;
  }

  /* ensure the value is of the correct type. */
  if (!PyList_Check (value)) {
    /* raise an error. */
    PyErr_SetString (PyExc_TypeError,
                     "the pulprog attribute must be a list");

    /* return the exception code. */
    return -1;
  }

  /* unpack the pulse program, formatted as a list of lists. */
  if (!pyppm_unpack_prog (value, &self->pp))
    return -1;

  /* indicate that the pulse programs on the device and host are
   * possibly out of sync in case the transfer fails.
   */
  self->fresh = 0;

  /* attempt to push the new pulse program to the device. */
  if (!ppm_wpp (self->fname, &self->pp)) {
    /* throw an exception. */
    PyErr_SetString (PyExc_IOError, "failed to write new device pulprog");
    return -1;
  }

  /* wait a while for the device to catch up. */
  PyPPM_DELAY ();

  /* indicate that the pulse programs on the device and host are
   * back in sync.
   */
  self->fresh = 1;

  /* return success. */
  return 0;
}

/* PyPPM_reset: method for resetting a ppm object.
 */
static PyObject*
PyPPM_reset (PyPPM *self, PyObject *args) {
  /* indicate that the device information will no longer up to date. */
  self->fresh = 0;

  /* attempt to reset the device. */
  if (!ppm_rst (self->fname)) {
    /* throw an exception. */
    PyErr_SetString (PyExc_IOError, "device reset failed");
    return NULL;
  }

  /* wait a while for the device to catch up. */
  PyPPM_DELAY ();

  /* return nothing. */
  Py_RETURN_NONE;
}

/* PyPPM_execute: method for running pulse programs on a ppm object.
 */
static PyObject*
PyPPM_execute (PyPPM *self, PyObject *args) {
  /* declare required variables. */
  PyObject *obj;
  ppm_data acq;

  /* initialize the acquisition sample count. */
  acq.n = 0;

  /* ensure that the parameters are up to date. */
  if (!PyPPM_refresh (self))
    return NULL;

  /* attempt to execute a pulse program the device. */
  if (!ppm_zg (self->fname, &self->pp, &acq)) {
    /* throw an exception. */
    PyErr_SetString (PyExc_IOError, "failed to execute pulprog");
    return NULL;
  }

  /* wait a while for the device to catch up. */
  PyPPM_DELAY ();

  /* pack up a tuple of tuples with the acquired data. */
  obj = pyppm_pack_data (&acq);

  /* free the data acquired from the device. */
  ppm_data_empty (&acq);

  /* return the python object. */
  return obj;
}

/* PyPPM_AddUnicodeStringConstant: adds a string constant to a module
 * object, ensuring that the constant is unicode.
 */
static void
PyPPM_AddUnicodeStringConstant (PyObject *module,
                                const char *key,
                                const char *val) {
  /* declare required variables */
  PyObject *U;

  /* create a new unicode object from the value string. */
  U = PyUnicode_FromString (val);
  if (!U)
    return;

  /* add the unicode object to the module. */
  PyModule_AddObject (module, key, U);
}

/* * * * BEGIN DOCUMENTATION STRINGS * * * */

PyDoc_STRVAR (
  pyppm_module_doc,
"PyPPM device management.\n"
"\n"
"This module provides a class (PPM) that provides a simple means\n"
"of communicating with PyPPM-type physical devices.\n"
"\n"
"The PyPPM hardware is a single-board solution to the task of\n"
"performing Nuclear Magnetic Resonance (NMR) experiments using\n"
"the magnetic field of the earth, or so-called Earth's Field\n"
"NMR. To use a connected PyPPM device via this module, one must\n"
"create a 'PPM' instance, download a pulse program to the device\n"
"via the instance, and execute the pulse program.\n"
"\n"
"More information on precisely how to use this module may be\n"
"found in the Markdown documentation at the PyPPM GitHub\n"
"repository."
);

PyDoc_STRVAR (
  pyppm_class_doc,
"PPM([filename]) -> PPM object\n"
"\n"
"Establish communication with a connected PyPPM device. The\n"
"default filename depends on the operating system, and will\n"
"correspond to the expected device filename of a USB CDC-ACM\n"
"device with a serial string equal to 'PyPPM'.\n"
);

PyDoc_STRVAR (
  pyppm_class_reset_doc,
"reset() -> None\n"
"\n"
"Perform a hardware reset of the PyPPM device.\n"
"\n"
"This will force a reset of the microcontroller of the PyPPM,\n"
"which in turn causes the USB host controller to re-enumerate\n"
"the bus, so this action may cause the PyPPM device to\n"
"re-appear with a different device filename.\n"
"\n"
"You should never need to hardware reset the PyPPM in this\n"
"manner.\n"
);

PyDoc_STRVAR (
  pyppm_class_execute_doc,
"execute() -> None\n"
"execute() -> (t, a)\n"
"\n"
"Execute the pulse program currently loaded onto the PyPPM\n"
"device.\n"
"\n"
"When the currently loaded pulse program contains no\n"
"'acquire' instructions, this method will return None.\n"
"\n"
"When the pulse program does contain one or more 'acquire'\n"
"instructions, then the tuples t and a will be returned\n"
"with all acquired samples. Acquired samples will be\n"
"stored serially in the tuples, meaning that the results\n"
"of multiple acquisitions will be concatenated. However\n"
"The time values (in t) of each acquisition will always\n"
"begin at zero."
);

PyDoc_STRVAR (
  pyppm_getset_filename,
"The device filename. (read/write)\n"
"\n"
"The PyPPM device file is only opened during communication\n"
"with the hardware. This occurs during class instantiation,\n"
"pulse program transmission and execution, and resets.\n"
"\n"
"When the device file is not opened, the filename may be\n"
"changed to change which connected PyPPM device is being\n"
"communicated with."
);

PyDoc_STRVAR (
  pyppm_getset_version,
"The device firmware version. (read-only)\n"
"\n"
"All supported devices will report major and minor version\n"
"numbers in a 2-tuple via this read-only property."
);

PyDoc_STRVAR (
  pyppm_getset_prog,
"The device pulse program. (read/write)\n"
"\n"
"Pulse programs are represented as lists of lists, where the\n"
"inner list elements are the individual instructions of the\n"
"pulse program. Each type of instruction requires a specific\n"
"set of arguments in it's list.\n"
"\n"
"More information on pulse program syntax is available in the\n"
"PyPPM GitHub repository."
);

PyDoc_STRVAR (
  pyppm_math_fft_doc,
"fft(t, a) -> (f, A)\n"
"\n"
"Compute the radix-2 fast Fourier-transform (FFT) of a pair\n"
"of time (t) and amplitude (a) tuples, and return the result\n"
"as a frequency tuple (f) and an amplitude tuple (A). The\n"
"FFT is the workhorse of the three transformations in the\n"
"PyPPM module, providing a fast means of identifying major\n"
"spectral components in a time-domain signal.\n"
"\n"
"No window function is applied prior to transformation."
);

PyDoc_STRVAR (
  pyppm_math_hrft_doc,
"hrft(t, a, f1, f2) -> (f, A)\n"
"\n"
"Compute the high-resolution Fourier-transform (HRFT) of a pair\n"
"of time (t) and amplitude (a) tuples, and return the result\n"
"as a frequency tuple (f) and an amplitude tuple (A). The\n"
"HRFT differs from the FFT in the sense that it computes\n"
"a highly interpolated subspectrum between the bounding\n"
"frequencies (f1, f2).\n"
"\n"
"The HRFT is most useful as a transform to precisely find\n"
"the center frequency of a spectral component.\n"
"\n"
"A Blackman window function is applied prior to transformation."
);

PyDoc_STRVAR (
  pyppm_math_stft_doc,
"stft(t, a) -> (ts, fs, As)\n"
"\n"
"Compute the short-time Fourier-transform (STFT) of a pair of\n"
"time (t) and amplitude (a) tuples, and return the result as a\n"
"short time tuple (ts), a frequency tuple (fs) and an amplitude\n"
"matrix (As) expressed as a tuple of tuples.\n"
"\n"
"The STFT is most useful to identify time-dependent changes in\n"
"amplitude of spectral components. One expects steady-state\n"
"interference to have a constant amplitude, for example, while\n"
"the amplitude of NMR signals will decay away exponentially.\n"
"\n"
"A Blackman window function is applied to each short-time\n"
"segment of the data prior to Fourier transformation."
);

/* * * * END DOCUMENTATION STRINGS * * * */

/* PyPPM_methods: the python method structure for ppm devices.
 */
static PyMethodDef PyPPM_methods[] = {
  { "reset",
    (PyCFunction) PyPPM_reset,
    METH_VARARGS,
    pyppm_class_reset_doc
  },
  { "execute",
    (PyCFunction) PyPPM_execute,
    METH_VARARGS,
    pyppm_class_execute_doc
  },
  { NULL }
};

/* PyPPM_getset: the python get/set structure for ppm device objects.
 */
static PyGetSetDef PyPPM_getset[] = {
  { "filename",
    (getter) PyPPM_getfilename,
    (setter) PyPPM_setfilename,
    pyppm_getset_filename,
    NULL
  },
  { "version",
    (getter) PyPPM_getversion,
    (setter) PyPPM_setversion,
    pyppm_getset_version,
    NULL
  },
  { "pulprog",
    (getter) PyPPM_getprog,
    (setter) PyPPM_setprog,
    pyppm_getset_prog,
    NULL
  },
  { NULL }
};

/* PyPPM_Type: the python type structure for ppm devices.
 */
static PyTypeObject PyPPM_Type = {
  PyVarObject_HEAD_INIT(NULL, 0)
  "pyppm.PPM",
  sizeof(PyPPM   ),                 /* tp_basicsize */
  0,                                /* tp_itemsize */
  (destructor) PyPPM_dealloc,       /* tp_dealloc */
  0,                                /* tp_print */
  0,                                /* tp_getattr */
  0,                                /* tp_setattr */
  0,                                /* tp_reserved */
  (reprfunc) PyPPM_repr,            /* tp_repr */
  0,                                /* tp_as_number */
  0,                                /* tp_as_sequence */
  0,                                /* tp_as_mapping */
  0,                                /* tp_hash  */
  0,                                /* tp_call */
  (reprfunc) PyPPM_repr,            /* tp_str */
  0,                                /* tp_getattro */
  0,                                /* tp_setattro */
  0,                                /* tp_as_buffer */
  Py_TPFLAGS_DEFAULT |
  Py_TPFLAGS_BASETYPE,              /* tp_flags */
  pyppm_class_doc,                  /* tp_doc */
  0,                                /* tp_traverse */
  0,                                /* tp_clear */
  0,                                /* tp_richcompare */
  0,                                /* tp_weaklistoffset */
  0,                                /* tp_iter */
  0,                                /* tp_iternext */
  PyPPM_methods,                    /* tp_methods */
  0,                                /* tp_members */
  PyPPM_getset,                     /* tp_getset */
  0,                                /* tp_base */
  0,                                /* tp_dict */
  0,                                /* tp_descr_get */
  0,                                /* tp_descr_set */
  0,                                /* tp_dictoffset */
  (initproc) PyPPM_init,            /* tp_init */
  0,                                /* tp_alloc */
  PyPPM_new,                        /* tp_new */
};

/* PyPPM_ModuleMethods: methods to be exposed at the module level of 'pyppm'.
 */
static PyMethodDef PyPPM_ModuleMethods[] = {
  { "fft",
    (PyCFunction) pyppm_math_fft,
    METH_VARARGS,
    pyppm_math_fft_doc
  },
  { "hrft",
    (PyCFunction) pyppm_math_hrft,
    METH_VARARGS | METH_KEYWORDS,
    pyppm_math_hrft_doc
  },
  { "stft",
    (PyCFunction) pyppm_math_stft,
    METH_VARARGS,
    pyppm_math_stft_doc
  },
  { NULL, NULL, 0, NULL }
};

/* PyPPM_type_init: type initialization method for ppm device objects.
 */
static int
PyPPM_type_init (PyObject *mod) {
  /* initialize the ppm device type object. */
  if (PyType_Ready (&PyPPM_Type) < 0)
    return 0;

  /* add the ppm device type object to the module. */
  Py_INCREF (&PyPPM_Type);
  PyModule_AddObject (mod, "PPM", (PyObject *) &PyPPM_Type);

  /* return success. */
  return 1;
}

#if PY_MAJOR_VERSION >= 3
/* pyppmmodule: module definition structure for ppm device objects.
 */
static PyModuleDef pyppmmodule = {
  PyModuleDef_HEAD_INIT,
  "pyppm",
  pyppm_module_doc,
  -1,
  PyPPM_ModuleMethods,
  NULL, NULL, NULL, NULL
};
#endif

/* ensure the PyMODINIT_FUNC declaration exists in some form. */
#ifndef PyMODINIT_FUNC
#define PyMODINIT_FUNC void
#endif

#if PY_MAJOR_VERSION >= 3
/* PyInit_pyppm: module initialization method for pyppm, python3.
 */
PyMODINIT_FUNC
PyInit_pyppm (void) {
#else
/* initpyppm: module initialization method for pyppm, python2.
 */
void
initpyppm (void) {
#endif
  /* declare the module python object. */
  PyObject *pyppm;

  /* create the module python object. */
#if PY_MAJOR_VERSION >= 3
  pyppm = PyModule_Create (&pyppmmodule);
  if (!pyppm)
    return NULL;
#else
  pyppm = Py_InitModule3 ("pyppm", PyPPM_ModuleMethods, pyppm_module_doc);
  if (!pyppm)
    return;
#endif

  /* initialize the ppm device type and add it to the module. */
  if (!PyPPM_type_init (pyppm))
#if PY_MAJOR_VERSION >= 3
    return NULL;
#else
    return;
#endif

  /* pulprog element: dead time. */
  PyPPM_AddUnicodeStringConstant (pyppm,
    "DEADTIME", PPM_PULPROG_STR_DEADTIME);

  /* pulprog element: delay. */
  PyPPM_AddUnicodeStringConstant (pyppm,
    "DELAY", PPM_PULPROG_STR_DELAY);

  /* pulprog element: polarization. */
  PyPPM_AddUnicodeStringConstant (pyppm,
    "POLARIZE", PPM_PULPROG_STR_POLARIZE);

  /* pulprog element: relay. */
  PyPPM_AddUnicodeStringConstant (pyppm,
    "RELAY", PPM_PULPROG_STR_RELAY);

  /* pulprog element: acquisition. */
  PyPPM_AddUnicodeStringConstant (pyppm,
    "ACQUIRE", PPM_PULPROG_STR_ACQUIRE);

  /* pulprog element: tx rising edge. */
  PyPPM_AddUnicodeStringConstant (pyppm,
    "TX_RISE", PPM_PULPROG_STR_TXRISE);

  /* pulprog element: tx falling edge. */
  PyPPM_AddUnicodeStringConstant (pyppm,
    "TX_FALL", PPM_PULPROG_STR_TXFALL);

  /* pulprog element: tx pulse. */
  PyPPM_AddUnicodeStringConstant (pyppm,
    "TX_PULSE", PPM_PULPROG_STR_TXPULSE);

  /* pulprog element: tuning. */
  PyPPM_AddUnicodeStringConstant (pyppm,
    "TUNE", PPM_PULPROG_STR_TUNE);

  /* pulprog element: x-shim. */
  PyPPM_AddUnicodeStringConstant (pyppm,
    "SHIM_X", PPM_PULPROG_STR_SHIM_X);

  /* pulprog element: y-shim. */
  PyPPM_AddUnicodeStringConstant (pyppm,
    "SHIM_Y", PPM_PULPROG_STR_SHIM_Y);

  /* pulprog element: z-shim. */
  PyPPM_AddUnicodeStringConstant (pyppm,
    "SHIM_Z", PPM_PULPROG_STR_SHIM_Z);

  /* pulprog element: end. */
  PyPPM_AddUnicodeStringConstant (pyppm,
    "END", PPM_PULPROG_STR_END);

  /* return the initialized module. */
#if PY_MAJOR_VERSION >= 3
  return pyppm;
#endif
}
