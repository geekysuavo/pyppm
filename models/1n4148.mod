* 1n4148 standard diode
*
* Node assignments
* connections: anode
*              | cathode
*              | |
.subckt 1n4148 1 2
*
R1 1 2 5.827e+9
D1 1 2 dx
*
.model dx d(
+ Is=4.352e-9
+ N=1.906
+ Bv=110
+ Ibv=0.0001
+ Rs=0.6458
+ Cjo=7.048e-13
+ Vj=0.869
+ M=0.03
+ Fc=0.5
+ Tt=3.48e-9)
*
.ends 1n4148
*
