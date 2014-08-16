set terminal png size 1920,1280 lw 2
set output 'osc.png'

set title 'osc output'

set xrange [100:10e3]
set yrange [-100:20]
set log x
unset xtics
set ylabel 'Output (dB)'
set xlabel 'Frequency (Hz)'

plot 'osc-meas.txt' u 1:(20*log10($2)-98) \
  w lp pt 5 lt rgb '#ffbbbb' t 'G(w) (Actual)',\
     'osc-sim.txt' u 1:2 \
  w l lt rgb '#ff0000' t 'G(w) (Theory)'

