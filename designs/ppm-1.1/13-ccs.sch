v 20100214 2
C 40000 40000 0 0 0 title-B.sym
T 50000 40700 14 18 1 0 0 0 1
PPM | Constant Current Sink
T 50000 40400 14 10 1 0 0 0 1
13-ccs.sch
T 50100 40100 14 14 1 0 0 0 1
13
T 51600 40100 14 14 1 0 0 0 1
15
T 54000 40100 14 10 1 0 0 0 1
Bradley Worley
T 54000 40400 14 10 1 0 0 0 1
1.0
C 46700 45400 1 0 0 ad8591_sot26-1.sym
{
T 47400 46200 5 10 0 0 0 0 1
device=OPAMP
T 47600 46400 5 10 1 1 0 0 1
refdes=U12
T 47400 46800 5 10 0 0 0 0 1
symversion=0.1
T 47600 46200 5 10 1 1 0 0 1
value=AD8591
T 46700 45400 5 10 0 0 0 0 1
footprint=SOT26
}
C 47100 44900 1 0 0 gnd-earth-1.sym
C 46300 47400 1 90 0 capacitor-1.sym
{
T 45600 47600 5 10 0 0 90 0 1
device=CAPACITOR
T 46400 47900 5 10 1 1 0 0 1
refdes=C59
T 45400 47600 5 10 0 0 90 0 1
symversion=0.1
T 46400 47700 5 10 1 1 0 0 1
value=100n
T 46300 47400 5 10 0 0 0 0 1
footprint=SMD_SIMPLE 80 50
}
C 44400 48300 1 270 0 capacitor-2.sym
{
T 45100 48100 5 10 0 0 270 0 1
device=POLARIZED_CAPACITOR
T 44900 47900 5 10 1 1 0 0 1
refdes=C58
T 45300 48100 5 10 0 0 270 0 1
symversion=0.1
T 44900 47700 5 10 1 1 0 0 1
value=10u
T 44400 48300 5 10 0 0 0 0 1
footprint=SMD_POLAR 120 60
}
N 47050 48500 44600 48500 4
N 46100 48500 46100 48300 4
N 44600 48300 44600 48700 4
N 44600 47400 44600 47000 4
N 44600 47200 46100 47200 4
N 46100 47200 46100 47400 4
N 47050 46300 47050 48500 4
C 44500 46700 1 0 0 gnd-earth-1.sym
C 44400 48700 1 0 0 generic-power.sym
{
T 44600 48950 5 10 0 1 0 3 1
net=Vbus:1
T 44600 49000 3 9 1 1 0 4 1
value=Vbus
}
N 47050 46500 47350 46500 4
N 47350 46500 47350 46100 4
N 47200 45400 47200 45200 4
C 45100 45900 1 0 0 input-2.sym
{
T 45100 46100 5 10 0 0 0 0 1
net=CCS_EN:1
T 45700 46600 5 10 0 0 0 0 1
device=none
T 45600 46000 5 10 1 1 0 7 1
value=CCS_EN
}
N 46500 46000 46700 46000 4
C 51900 47300 1 270 0 battery-3.sym
{
T 52600 47000 5 10 0 0 270 0 1
device=BATTERY
T 52400 46800 5 10 1 1 0 0 1
refdes=B01
T 52850 47000 5 10 0 0 270 0 1
symversion=0.1
T 52400 46600 5 10 1 1 0 0 1
value=6V
T 51900 47300 5 10 0 0 0 0 1
footprint=Power_Jack_PJ037
}
C 52000 45700 1 0 0 gnd-earth-1.sym
N 52100 46200 52100 46000 4
C 48700 47400 1 0 0 input-2.sym
{
T 48700 47600 5 10 0 0 0 0 1
net=CCS_POS:1
T 49300 48100 5 10 0 0 0 0 1
device=none
T 49200 47500 5 10 1 1 0 7 1
value=CCS+
}
C 48700 46900 1 0 0 input-2.sym
{
T 48700 47100 5 10 0 0 0 0 1
net=CCS_NEG:1
T 49300 47600 5 10 0 0 0 0 1
device=none
T 49200 47000 5 10 1 1 0 7 1
value=CCS-
}
N 50100 47500 52100 47500 4
N 52100 47500 52100 47300 4
N 50100 47000 50300 47000 4
N 50300 47000 50300 46400 4
C 49800 45600 1 0 0 nmos-3.sym
{
T 50400 46100 5 10 0 0 0 0 1
device=NMOS_TRANSISTOR
T 50500 46200 5 10 1 1 0 0 1
refdes=Q01
T 50500 46000 5 10 1 1 0 0 1
value=IRLR024N
T 49800 45600 5 10 0 0 0 0 1
footprint=DPAK
}
C 48800 44700 1 90 0 capacitor-1.sym
{
T 48100 44900 5 10 0 0 90 0 1
device=CAPACITOR
T 48900 45200 5 10 1 1 0 0 1
refdes=C57
T 47900 44900 5 10 0 0 90 0 1
symversion=0.1
T 48900 45000 5 10 1 1 0 0 1
value=1n
T 48800 44700 5 10 0 0 0 0 1
footprint=SMD_SIMPLE 80 50
}
N 47700 45800 49800 45800 4
N 48600 45800 48600 45600 4
N 46700 45600 46500 45600 4
N 46500 45600 46500 44500 4
N 46500 44500 50300 44500 4
N 48600 44500 48600 44700 4
C 50400 43400 1 90 0 resistor-1.sym
{
T 50000 43700 5 10 0 0 90 0 1
device=RESISTOR
T 50500 43900 5 10 1 1 0 0 1
refdes=R31
T 50500 43700 5 10 1 1 0 0 1
value=2.5
T 50400 43400 5 10 0 0 0 0 1
footprint=Bourns-DPAK
}
N 50300 45600 50300 44300 4
C 50200 42900 1 0 0 gnd-earth-1.sym
N 50300 43400 50300 43200 4