* ad8541 cmos, rail-to-rail, unity-gain stable operational amplifier
*
* Node Assignments
* connections: non-inverting input
*              | inverting input
*              | |  positive supply
*              | |  |  negative supply
*              | |  |  |  output
*              | |  |  |  |
.subckt ad8541 1 2 99 50 45
*
* input stage
M1   4  1 8 8 PIX L=0.6E-6 W=16E-6
M2   6  7 8 8 PIX L=0.6E-6 W=16E-6
M3  11  1 10 10 NIX L=0.6E-6 W=16E-6
M4  12  7 10 10 NIX L=0.6E-6 W=16E-6
RC1  4 50 20E3
RC2  6 50 20E3
RC3 99 11 20E3
RC4 99 12 20E3
C1   4  6 1.5E-12
C2  11 12 1.5E-12
I1  99  8 1E-5
I2  10 50 1E-5
V1  99  9 0.2
V2  13 50 0.2
D1   8  9 DX
D2  13 10 DX
EOS  7  2 POLY(3) (22,98) (73,98) (81,0) 1E-3 1 1 1
IOS  1  2 2.5E-12
*
* cmrr = 64 db, zero at 20khz
ECM1 21 98 POLY(2) (1,98) (2,98) 0 .5 .5
RCM1 21 22 79.6E3
CCM1 21 22 100E-12
RCM2 22 98 50
*
* psrr = 90 db, zero at 200hz
RPS1 70  0 1E6
RPS2 71  0 1E6
CPS1 99 70 1E-5
CPS2 50 71 1E-5
EPSY 98 72 POLY(2) (70,0) (0,71) 0 1 1
RPS3 72 73 1.59E6
CPS3 72 73 500E-12
RPS4 73 98 25
*
* voltage noise reference of 35 nv/rt(hz)
VN1 80 0 0
RN1 80 0 16.45E-3
HN  81 0 VN1 35
RN2 81 0 1
*
* internal voltage reference
VFIX 90 98 DC 1
S1   90 91 (50,99) VSY_SWITCH
VSN1 91 92 DC 0
RSY  92 98 1E3
EREF 98  0 POLY(2) (99,0) (50,0) 0 .5 .5
GSY  99 50 POLY(1) (99,50) 0 3.7E-6
*
* adaptive gain stage
G1  98 30 POLY(2) (4,6) (11,12) 0 2.5E-5 2.5E-5
VR1 30 31 DC 0
H1  31 98 POLY(2) VR1 VSN1 0 5.45E6 0 0 49.05E9
CF  45 30 10E-12
D3  30 99 DX
D4  50 30 DX
*
* output stage
M5  45 46 99 99 POX L=0.6E-6 W=375E-6
M6  45 47 50 50 NOX L=0.6E-6 W=500E-6
EG1 99 46 POLY(1) (98,30) 1.05 1
EG2 47 50 POLY(1) (30,98) 1.04 1
*
* models used in the subcircuit
.model POX pmos(level=2 Kp=20.0e-6 Vto=-1.0 lambda=0.067)
.model NOX nmos(level=2 Kp=20.0e-6 Vto=1.0 lambda=0.067)
.model PIX pmos(level=2 Kp=20.0e-6 Vto=-0.7 lambda=0.01 Kf=1.0e-31)
.model NIX nmos(level=2 Kp=20.0e-6 Vto=0.7 lambda=0.01 Kf=1.0e-31)
.model DX d(Is=1.0e-14)
.model VSY_SWITCH vswitch(Roff=100.0e+3 Ron=1.0 Voff=-4.2 Von=-3.5)
*
.ends ad8541
*
