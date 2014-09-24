
def pp():
  return [
    [pyppm.RELAY, True],
    [pyppm.DEADTIME, 1],
    [pyppm.POLARIZE, True],
    [pyppm.DELAY, 5],
    [pyppm.POLARIZE, False],
    [pyppm.DEADTIME, 1],
    [pyppm.RELAY, False],
    [pyppm.DEADTIME, 10],
    [pyppm.ACQUIRE, 16384, 20],
    [pyppm.END]
  ]

