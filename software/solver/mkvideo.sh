#!/bin/bash

f="sol-hard-off.txt"
if [ ! "${1}" == "" ]; then
  f="${1}.txt"
fi

fbase=$(basename ${f} .txt)

n_start=1
if [ ! "${2}" == "" ]; then
  n_start=${2}
fi

n_skip=30
if [ ! "${3}" == "" ]; then
  n_skip=${3}
fi

n_stop=$(wc -l < ${f})
if [ ! "${4}" == "" ]; then
  n_stop=${4}
fi

nlst=$(seq ${n_start} ${n_skip} ${n_stop})
nn=$(echo ${nlst} | awk '{print NF}')

rm -f ${fbase}-*.png
for i in $(seq 1 ${nn}); do
  n=$(echo ${nlst} | awk -v i=${i} '{print $i}')
  ifmt=$(seq -f '%05g' ${i} ${i})
  pct=$(echo "scale=2; ${i} * 100.0 / ${nn}" | bc -l)

  l=$(sed -n "${n}p" ${f})

  t=$(echo ${l} | awk '{print $1}')

  bx=$(echo ${l} | awk '{print $2}')
  by=$(echo ${l} | awk '{print $3}')
  bz=$(echo ${l} | awk '{print $4}')

  mx=$(echo ${l} | awk '{print $5}')
  my=$(echo ${l} | awk '{print $6}')
  mz=$(echo ${l} | awk '{print $7}')

  cat > ${fbase}-${ifmt}.plt << EOF
set terminal pngcairo enhanced font 'Free Sans Bold,18' size 1920, 1280 lw 2
set output '${fbase}-${ifmt}.png'
unset key
set xrange [-1:1]
set yrange [-1:1]
set zrange [-1:1]
set xlabel 'M_x'
set ylabel 'M_y'
set zlabel 'M_z'
set size 1.0, 1.0
set origin 0.0, 0.0
set multiplot
set origin 0.0, 0.0
set size 1.0, 1.0
set title 'Adiabatic turn-off, t = ${t} s'
splot '${f}' using (0):(0):(0):(${bx}):(${by}):(${bz}) with vectors
set origin 0.1, 0.7
set size 0.8, 0.2
set xlabel 't'
set ylabel 'B_x'
unset title
unset yrange
set xrange [1:1.015]
plot \
 '${f}' using 1:5 with lines linetype rgb 'red', \
 '${f}' using (${t}):(${mx}) with points \
  pointtype 7 \
  pointsize 2 \
  linetype rgb 'red'
EOF

  echo "generating frame ${i}/${nn} (${pct}%) ..."
  gnuplot ${fbase}-${ifmt}.plt
  rm -f ${fbase}-${ifmt}.plt
done

rm -f ${fbase}.mp4
mencoder \
  mf://${fbase}-*.png -mf fps=30:type=png \
  -ovc lavc -lavcopts vcodec=mpeg4:mbd=2:trell \
  -oac copy \
  -o ${fbase}.mp4

