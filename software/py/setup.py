#!/usr/bin/env python

import sys, glob

from distutils.core import setup, Extension

pyppm_ext = Extension('pyppm',
                      sources = ['pyppm.c'],
                      include_dirs = ['../core'],
                      extra_objects = glob.glob('*.o')
)

setup(name         = 'pyppm',
      version      = '0.0.1',
      description  = 'Python Proton Precession Magnetometer Interface',
      author       = 'Bradley Worley',
      author_email = 'geekysuavo@gmail.com',
      url          = 'http://www.geekysuavo.org/',
      ext_modules  = [pyppm_ext]
)

