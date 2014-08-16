* ad8591 cmos, rail-to-rail, unity-gain stable operational amplifier
*
* Node assignments
* connections: non-inverting input
*              | inverting input
*              | |  positive supply
*              | |  |  negative supply
*              | |  |  |  output
*              | |  |  |  |  shutdown
*              | |  |  |  |  |
.subckt ad8591 1 2 99 50 45 80
*
* input stage, 1/2
M1   4  1 3 3 PIX L=0.8e-6 W=125.0e-6
M2   6  7 3 3 PIX L=0.8e-6 W=125.0e-6
RC1  4 50 4.0e+3
RC2  6 50 4.0e+3
C1   4  6 2.0e-12
I1  99  8 100.0e-6
*
* input stage, 2/2
M3  10  1 12 12 NIX L=0.8e-6 W=125.0e-6
M4  11  7 12 12 NIX L=0.8e-6 W=125.0e-6
RC3 10 99 4.0e+3
RC4 11 99 4.0e+3
C2  10 11 2.0e-12
I2  13 50 100.0e-6
*
EOS  7  2 POLY(3) (21,98) (73,98) (61,0) 1E-3 0 0 1
IOS  1  2 2.5e-12
V1  99  9 0.9
D1   3  9 DX
V2  14 50 0.9
D2  14 12 DX
S1   3  8 (82,98) SOPEN
S2  99  8 (98,82) SCLOSE
S3  12 13 (82,98) SOPEN
S4  13 50 (98,82) SCLOSE
*
* cmrr = 64 db, zero at 20khz
ECM1 20 98 POLY(2) (1,98) (2,98) 0 .5 .5
RCM1 20 21 79.6e+3
CCM1 20 21 100.0e-12
RCM2 21 98 50
*
* psrr = 80 db, zero at 200hz
RPS1 70  0 1.0e+6
RPS2 71  0 1.0e+6
CPS1 99 70 1.0e-5
CPS2 50 71 1.0e-5
EPSY 98 72 POLY(2) (70,0) (0,71) 0 1 1
RPS3 72 73 1.59e+6
CPS3 72 73 500.0e-12
RPS4 73 98 80
*
* internal voltage reference
EREF 98  0 POLY(2) (99,0) (50,0) 0 .5 .5
GSY  99 50 POLY(1) (99,50) 20.0e-6 10.0e-7
*
* shutdown section
E1 81 98 (80,50) 1
R1 81 82 1.0e+3
C3 82 98 1.0e-9
*
* voltage noise reference of 30 nv/rt(hz)
VN1 60 0 0
RN1 60 0 16.45e-3
HN  61 0 VN1 30
RN2 61 0 1
*
* gain stage
G2  98 30 POLY(2) (4,6) (10,11) 0 2.19E-5 2.19E-5
R2  30 98 13.0e+6
CF  45 30 5.0e-12
S5  30 98 (98,82) SCLOSE
D3  30 31 DX
D4  32 30 DX
V3  99 31 0.6
V4  32 50 0.6
*
* output stage
M5  45 46 99 99 POX L=0.8e-6 W=16.0e-3
M6  45 47 50 50 NOX L=0.8e-6 W=16.0e-3
EG1 99 48 POLY(1) (98,30) 1.06 1
EG2 49 50 POLY(1) (30,98) 1.05 1
RG1 48 46 10.0e+3
RG2 49 47 10.0e+3
S6  46 99 (98,82) SCLOSE
S7  47 50 (98,82) SCLOSE
*
* models used in the subcircuit:
.model PIX pmos(level=2 Kp=20.0e-6 Vto=-0.7 lambda=0.01 Kf=1.0e-31)
.model NIX nmos(level=2 Kp=20.0e-6 Vto=0.7 lambda=0.01 Kf=1.0e-31)
.model POX pmos(level=2 Kp=8.00e-6 Vto=-1 lambda=0.067)
.model NOX nmos(level=2 Kp=13.4e-6 Vto=1 lambda=0.067)
.model SOPEN vswitch(Von=2.4 Voff=0.8 Ron=10 Roff=1.0e+9)
.model SCLOSE vswitch(Von=-0.8 Voff=-2.4 Ron=10 Roff=1.0e+9)
.model DX d(Is=1.0e-14)
*
.ends ad8591
*
