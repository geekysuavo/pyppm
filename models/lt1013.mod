* lt1013 precision operational amplifier
*
* Node assignments:
* connections: non-inverting input
*              | inverting input
*              | | positive voltage
*              | | | negative voltage
*              | | | | output
*              | | | | |
.subckt lt1013 1 2 3 4 5
*
C1 11 12 6.49e-12
C2 6 7 20.0e-12
Dc 5 53 dx
De 54 5 dx
Dlp 90 91 dx
Dln 92 90 dx
Dp 4 3 dx
Egnd 99 0 POLY(2) (3,0) (4,0) 0 0.5 0.5
Fb 7 99 POLY(5) Vb Vc Ve Vlp Vln 0 3.865e+9 -4.0e+9 4.0e+9 4.0e+9 -4.0e+9
Ga 6 0 11 12 87.97e-6
Gcm 0 6 10 99 98.71e-12
Iee 3 10 dc 8.03e-6
Hlim 90 0 Vlim 1k
Q1 11 2 13 qx
Q2 12 1 14 qx
R2 6 9 100.0e+3
Rc1 4 11 11.37e+3
Rc2 4 12 11.37e+3
Re1 13 10 4.883e+3
Re2 14 10 4.883e+3
Ree 10 99 24.91e+6
Ro1 8 5 50
Ro2 7 99 25
Rp 3 4 87.72e+3
Vb 9 0 dc 0
Vc 3 53 dc 1.6
Ve 54 4 dc 1.6
Vlim 7 8 dc 0
Vlp 91 0 dc 25
Vln 0 92 dc 25
*
* models used in the subcircuit:
.model dx d(Is=800.0e-18)
.model qx pnp(Is=800.0e-18 Bf=266.7)
*
.ends lt1013
*
