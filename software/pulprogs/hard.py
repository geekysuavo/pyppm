
import pyppm

def single(t = 10, n = 16384, rate = 20):
  return [
    [pyppm.RELAY, True],
    [pyppm.DEADTIME, 1],
    [pyppm.POLARIZE, True],
    [pyppm.DELAY, t],
    [pyppm.POLARIZE, False],
    [pyppm.DEADTIME, 1],
    [pyppm.RELAY, False],
    [pyppm.DEADTIME, 10],
    [pyppm.ACQUIRE, n, rate],
    [pyppm.END]
  ]

def double(t = 10, n = 16384, rate = 20):
  P = single(t, n, rate)
  P.insert(0, [pyppm.ACQUIRE, n, rate])
  return P

