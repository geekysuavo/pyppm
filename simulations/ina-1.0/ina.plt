set terminal png size 1920,1280 lw 2
set output 'ina.png'

set tmargin 0
set bmargin 0
set lmargin 12
set rmargin 6

set multiplot layout 3,1 title 'ina-1.0 response'

set xrange [100:1e6]
set log x
unset xtics
set ylabel 'Gain (dB)'

plot 'ina-meas.txt' u 1:(20*log10($3/$2/9.999e-6)) \
  w p pt 5 lt rgb '#ffbbbb' t 'G(w) (Actual)',\
     'ina-sim.txt' u 2:(20*log10($5/$3)) \
  w l lt rgb '#ff0000' t 'G(w) (Theory)'

set xtics
set yrange [-180:180]
set ytics 30
set xlabel 'Frequency (Hz)'
set ylabel 'Phase (deg)'

plot 'ina-meas.txt' u 1:(-abs($4)+180) \
  w p pt 5 lt rgb '#bbbbff' t 'phi(w) (Actual)',\
     'ina-sim.txt' u 2:(($4-$6)*180/pi) \
  w l lt rgb '#0000ff' t 'phi(w) (Theory)'

unset multiplot

