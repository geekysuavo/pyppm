set terminal pngcairo size 1920,1280 lw 2

set output 'sink-tran.png'
set key left top
set style data lines
set title 'Current Sink - Transient Response'
set xlabel 'Time (s)'
set ylabel 'Output (A)'

plot 'sink-tran-500.txt' u 1:($2/0.5) t '0.2 A',\
     'sink-tran-1000.txt' u 1:($2/0.5) t '0.4 A',\
     'sink-tran-1500.txt' u 1:($2/0.5) t '0.6 A',\
     'sink-tran-2000.txt' u 1:($2/0.5) t '0.8 A',\
     'sink-tran-2500.txt' u 1:($2/0.5) t '1.0 A',\
     'sink-tran-3000.txt' u 1:($2/0.5) t '1.2 A',\
     'sink-tran-3500.txt' u 1:($2/0.5) t '1.4 A',\
     'sink-tran-4000.txt' u 1:($2/0.5) t '1.6 A',\
     'sink-tran-4500.txt' u 1:($2/0.5) t '1.8 A',\
     'sink-tran-5000.txt' u 1:($2/0.5) t '2.0 A'

set output 'sink-hist.png'
set style data impulses
set title 'Current Sink - Response Histogram'
set xlabel 'Output (A)'
set ylabel 'Counts (a.u.)'

plot 'sink-hist-500.txt' u ($1/0.5):2 t '0.2 A',\
     'sink-hist-1000.txt' u ($1/0.5):2 t '0.4 A',\
     'sink-hist-1500.txt' u ($1/0.5):2 t '0.6 A',\
     'sink-hist-2000.txt' u ($1/0.5):2 t '0.8 A',\
     'sink-hist-2500.txt' u ($1/0.5):2 t '1.0 A',\
     'sink-hist-3000.txt' u ($1/0.5):2 t '1.2 A',\
     'sink-hist-3500.txt' u ($1/0.5):2 t '1.4 A',\
     'sink-hist-4000.txt' u ($1/0.5):2 t '1.6 A',\
     'sink-hist-4500.txt' u ($1/0.5):2 t '1.8 A',\
     'sink-hist-5000.txt' u ($1/0.5):2 t '2.0 A'

f(x)=a*x+b
fit f(x) 'sink-lin.txt' via a,b
set output 'sink-lin.png'
set style data points
set title 'Current Sink - Linearity'
set xlabel 'Input (V)'
set ylabel 'Output (A)'

plot 'sink-lin.txt' t 'Simulated', f(x) t 'Fit'

