* lt1012 picoamp input current, microvolt offset, low-noise opamp
*
* Node assignments:
* connections: non-inverting input
*              | inverting input
*              | | positive voltage
*              | | | negative voltage
*              | | | | output
*              | | | | |
.subckt lt1012 3 2 7 4 6
*
* input section:
Rc1 7 80 8842
Rc2 7 90 8842
Q1 80 2 10 Qm1
Q2 90 3 11 Qm2
Ddm1 2 3 Dm2
Ddm2 3 2 Dm2
C1 80 90 5.46e-12
Re1 10 12 224.6
Re2 11 12 224.6
Iee 12 4 6.0e-6
Re 12 0 33330000
Ce 12 0 1.579e-12
*
* intermediate gain stage:
Gcm 0 8 12 0 2.841e-11
Ga 8 0 80 90 1.131e-04
R2 8 0 100000
C2 1 8 3.0e-11
Gb 1 0 8 0 196
*
* output stage:
Ro1 1 6 100
Ro2 1 0 900
Rc 17 0 1.063e-04
Gc 0 17 6 0 9408
D1 1 17 Dm1
D2 17 1 Dm1
D3 6 13 Dm2
D4 14 6 Dm2
Vc 7 13 1.785
Ve 14 4 1.785
Ip 7 4 0.000374
Dsub 4 7 Dm2
*
* models used in the subcircuit:
.model Qm1 npn(Is=8.0e-16 Bf=75000)
.model Qm2 npn(Is=8.003e-16 Bf=150000)
.model Dm1 d(Is=1.179e-19)
.model Dm2 d(Is=8.0e-16)
*
.ends lt1012
*
