
import pyppm

# FIXME - not verified to work!
def single(t = 10, n = 16384, rate = 20):
  return [
    [pyppm.TX_RISE, 400e-3, 1],
    [pyppm.DELAY, t],
    [pyppm.TX_FALL, 400e-3, 0],
    [pyppm.DEADTIME, 1],
    [pyppm.TX_PULSE, 10e-3, 2000, 1],
    [pyppm.DEADTIME, 1],
    [pyppm.ACQUIRE, n, rate],
    [pyppm.END]
  ]

def double(t = 10, n = 16384, rate = 20):
  P = single(t, n, rate)
  P.insert(0, [pyppm.ACQUIRE, n, rate])
  return P

