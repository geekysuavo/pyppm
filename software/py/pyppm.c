
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
  /* declare required variables. */
  PyObject *obj;

  /* ensure that the parameters are up to date. */
  if (!PyPPM_refresh (self))
    return NULL;

  /* initialize a new list object. */
  obj = PyList_New (0);

  /* FIXME: implement PyPPM_getprog() */

  /* return the python object. */
  return obj;
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

  /* FIXME: implement PyPPM_setprog() */

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

/* PyPPM_methods: the python method structure for ppm devices.
 */
static PyMethodDef PyPPM_methods[] = {
  { "reset",
    (PyCFunction) PyPPM_reset,
    METH_VARARGS,
    "performs a hardware reset of the device"
  },
  { "execute",
    (PyCFunction) PyPPM_execute,
    METH_VARARGS,
    "executes the pulse program stored on the device"
  },
  { NULL }
};

/* PyPPM_getset: the python get/set structure for ppm device objects.
 */
static PyGetSetDef PyPPM_getset[] = {
  { "filename",
    (getter) PyPPM_getfilename,
    (setter) PyPPM_setfilename,
    "the device filename",
    NULL
  },
  { "version",
    (getter) PyPPM_getversion,
    (setter) PyPPM_setversion,
    "the device firmware version",
    NULL
  },
  { "pulprog",
    (getter) PyPPM_getprog,
    (setter) PyPPM_setprog,
    "the device pulse program",
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
  "ppm device object",              /* tp_doc */
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
    "compute the fast Fourier transform of a free induction decay"
  },
  { "hrft",
    (PyCFunction) pyppm_math_hrft,
    METH_VARARGS | METH_KEYWORDS,
    "compute the high-resolution Fourier transform of a free induction decay"
  },
  { "stft",
    (PyCFunction) pyppm_math_stft,
    METH_VARARGS,
    "compute the short-time Fourier transform of a free induction decay"
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
  "proton precession magnetometer interface module for python.",
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
  pyppm = Py_InitModule3 ("pyppm", PyPPM_ModuleMethods,
    "proton precession magnetometer interface module for python.");
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
  PyModule_AddIntConstant (pyppm, "DEADTIME", PPM_PULPROG_DEADTIME);

  /* pulprog element: delay. */
  PyModule_AddIntConstant (pyppm, "DELAY", PPM_PULPROG_DELAY);

  /* pulprog element: polarization. */
  PyModule_AddIntConstant (pyppm, "POLARIZE", PPM_PULPROG_POLARIZE);

  /* pulprog element: relay. */
  PyModule_AddIntConstant (pyppm, "RELAY", PPM_PULPROG_RELAY);

  /* pulprog element: acquisition. */
  PyModule_AddIntConstant (pyppm, "ACQUIRE", PPM_PULPROG_ACQUIRE);

  /* pulprog element: tx rising edge. */
  PyModule_AddIntConstant (pyppm, "TX_RISE", PPM_PULPROG_TXRISE);

  /* pulprog element: tx falling edge. */
  PyModule_AddIntConstant (pyppm, "TX_FALL", PPM_PULPROG_TXFALL);

  /* pulprog element: tx pulse. */
  PyModule_AddIntConstant (pyppm, "TX_PULSE", PPM_PULPROG_TXPULSE);

  /* pulprog element: tuning. */
  PyModule_AddIntConstant (pyppm, "TUNE", PPM_PULPROG_TUNE);

  /* pulprog element: x-shim. */
  PyModule_AddIntConstant (pyppm, "SHIM_X", PPM_PULPROG_SHIM_X);

  /* pulprog element: y-shim. */
  PyModule_AddIntConstant (pyppm, "SHIM_Y", PPM_PULPROG_SHIM_Y);

  /* pulprog element: z-shim. */
  PyModule_AddIntConstant (pyppm, "SHIM_Z", PPM_PULPROG_SHIM_Z);

  /* pulprog element: end. */
  PyModule_AddIntConstant (pyppm, "END", PPM_PULPROG_END);

  /* return the initialized module. */
#if PY_MAJOR_VERSION >= 3
  return pyppm;
#endif
}
