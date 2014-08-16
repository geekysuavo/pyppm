set terminal png size 1920,1280 lw 2
set output 'dig-sin1k.png'

set xlabel 'Frequency (kHz)'
set ylabel 'Input (dBFS)'

plot 'dig-sin1k.txt' u ($1/1e3):(20*log10($2)-83.6) \
  w l lt rgb '#ff0000' t 'Input (Actual)'

set output 'dig-floor.png'
set multiplot layout 2,1 title 'dig noise floor'

set xlabel 'Frequency (kHz)'
set ylabel 'Input (dBFS)'

plot 'dig-floor-freq.txt' u ($1/1e3):(20*log10($2)-83.6) \
  w l lt rgb '#ff0000' t 'Noise floor (Actual)'

set xrange [0.0000:0.8192]
set yrange [0.0005:0.0020]
set xlabel 'Time (s)'
set ylabel 'Input (V)'

plot 'dig-floor-time.txt' u 1:2 \
  w l lt rgb '#0000ff' t 'Noise floor (Actual)'

