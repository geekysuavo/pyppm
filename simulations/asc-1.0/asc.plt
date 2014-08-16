set terminal pngcairo size 1920,1280 lw 2
set output 'asc.png'

set tmargin 0
set bmargin 0
set lmargin 12
set rmargin 6

set multiplot layout 3,1 title 'asc-1.0 response'

set xrange [200:10e3]
set log x
unset xtics
set ylabel 'Gain (dB)'

plot 'asc-meas.txt' u 1:(20*log10($3/$2)) \
  w p pt 5 lt rgb '#ffbbbb' t 'G(w) (Actual)',\
     'asc-sim.txt' u 2:(20*log10($5/$3)) \
  w l lt rgb '#ff0000' t 'G(w) (Theory)'

set xtics
set yrange [-180:180]
set ytics 30
set xlabel 'Frequency (Hz)'
set ylabel 'Phase (deg)'

plot 'asc-meas.txt' u 1:($4) \
  w p pt 5 lt rgb '#bbbbff' t 'phi(w) (Actual)',\
     'asc-sim.txt' u 2:(($4-$6)*180/pi) \
  w l lt rgb '#0000ff' t 'phi(w) (Theory)'

unset multiplot

