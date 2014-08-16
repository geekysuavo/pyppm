* ad797 operational amplifier macromodel subcircuit
*
* Node assignments
* connections: non-inverting input
*              |  inverting input
*              |  |  positive supply
*              |  |  |  negative supply
*              |  |  |  |  output
*              |  |  |  |  |
*              |  |  |  |  |
.subckt ad797  1  2  99 50 38
*
* input stage, pole at 500mhz
Ios 1 2 dc 50e-9
Cind 1 2 20e-12
Cinc1 1 98 5e-12
Brcm1 1 98 I=5.0e-9*V(1,31)+5.0e-9*V(2,31)
Gn1 0 1 44 0 1e-3
Cinc2 2 98 5e-12
Brcm2 2 98 I=5.0e-9*V(1,31)+5.0e-9*V(2,31)
Gn2 0 2 47 0 1e-3
Bos 9 3 V=25e-6+1.0*V(22,31)
En 3 1 41 0 0.1
D1 2 9 dx
D2 9 2 dx
Q1 5 2 4 qx
Q2 6 9 4 qx
R3 97 5 0.5172
R4 97 6 0.5172
C2 5 6 3.0772e-10
I1 4 51 100e-3
Epos 97 0 99 0 1
Eneg 51 0 50 0 1
*
* input voltage noise generator
Vn1 40 0 dc 2
Dn1 40 41 den
Dn2 41 42 den
Vn2 0 42 dc 2
*
* positive input current noise generator
Vn3 43 0 dc 2
Dn3 43 44 din
Dn4 44 45 din
Vn4 0 45 dc 2
*
* negative input current noise generator
Vn5 46 0 dc 2
Dn5 46 47 din
Dn6 47 48 din
Vn6 0 48 dc 2
*
* gain stage, dominant pole at 7.33hz
Eref 98 0 31 0 1
G1 98 10 5 6 10
R7 10 98 10
B1 99 11 V=-2.294+1.0*V(99,31)
D3 10 11 dx
B2 12 50 V=-2.294+1.0*V(31,50)
D4 12 10 dx
G2 98 13 10 31 1e-3
R8 13 98 10
G3 99 14 98 13 34.558e-3
G4 99 16 98 98 34.558e-3
G5 14 15 15 14 20e-3
G6 16 17 17 14 20e-3
R9 15 18 400
R10 17 18 400
E3 18 98 16 98 1
R11 16 98 4.3406e8
C5 16 98 50e-12
V1 99 19 dc 2.2542
D5 16 19 dx
V2 20 50 dc 2.2542
D6 20 16 dx
Rdc 14 98 1e15
*
* common-mode gain network, zero at 1.35khz
Bcm 21 98 V=158.11e-3*V(1,31)+158.11e-3*V(2,31)
Rcm1 21 22 1
Ccm 21 22 1.1789e-4
Rcm2 22 98 1e-6
*
* pole-zero pair at 3.9mhz/10mhz
Gpz 98 23 16 98 1
Rpz1 23 98 1
Rpz2 23 24 0.63934
Cpz 24 98 24.893e-9
*
* negative zero at -300mhz
Enz 25 98 23 31 1e6
Rnz1 25 26 1
Cnz 25 26 -5.3052e-10
Rnz2 26 98 1e-6
*
* pole at 300mhz
Gp2 98 27 26 31 1
Rp2 27 98 1
Cp2 27 98 5.3052e-10
*
* pole at 500mhz
Gp3 98 28 27 31 1
Rp3 28 98 1
Cp3 28 98 3.1831e-10
*
* pole at 500mhz
Gp4 98 29 28 31 1
Rp4 29 98 1
Cp4 29 98 3.1831e-10
*
* output stage
Vw 29 30 dc 0
Rdc1 99 31 23.25e3
Cdc 31 0 1e-6
Rdc2 31 50 23.25e3
Go1 98 32 37 30 25e-3
Do1 32 33 dx
Vo1 33 98 dc 0
Do2 34 32 dx
Vo2 98 34 dc 0
Bdc 99 50 I=7.56e-3+1.0*I(Vo1)+1.0*I(Vo2)
Vsc1 35 37 0.945
Dsc1 30 35 dx
Vsc2 37 36 0.745
Dsc2 36 30 dx
Fsc1 37 0 Vsc1 1
Fsc2 0 37 Vsc2 1
Go3 37 99 99 30 25e-3
Go4 50 37 30 50 25e-3
Ro1 99 37 40
Ro2 37 50 40
Lo 37 38 10e-9
*
* models used throughout subcircuit
.model qx npn(Bf=2e5)
.model dx d(Is=1e-15)
.model den d(Is=1e-12 Rs=6.3708e3 Af=1 Kf=1.59e-15)
.model din d(Is=1e-12 Rs=474 Af=1 Kf=7.816e-15)
*
.ends ad797
*
