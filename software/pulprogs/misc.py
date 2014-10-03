
import pyppm

def acquire(n = 16384, rate = 20):
  return [
    [pyppm.ACQUIRE, n, rate],
    [pyppm.END]
  ]

def retune(f = 0, L = 14.62e-3):
  return [
    [pyppm.TUNE, f, L],
    [pyppm.END]
  ]

def reshim(v, axis = pyppm.SHIM_Z):
  if (axis is not pyppm.SHIM_X and
      axis is not pyppm.SHIM_Y and
      axis is not pyppm.SHIM_Z):
    return []

  return [
    [axis, v],
    [pyppm.END]
  ]

