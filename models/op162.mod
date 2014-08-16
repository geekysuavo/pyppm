* op162 precision operational amplifier
*
* Node assignments:
* connections: non-inverting input
*              | inverting input
*              | | positive supply
*              | | |  negative supply
*              | | |  |  output
*              | | |  |  |
.subckt op162  1 2 99 50 45
*
* input stage:
Q21a 5a  7 3a PIX 5; 3a; 15a
Q22a 6a  2 4a PIX 5; 4a; 15b
Q21b 5a  7 4c PIX 1; 4c; 15b
Q22b 6a  2 3c PIX 1; 3c; 15a
Ios   1  2 10e-9
I1a  99a 50 25e-6
I1b  99b 50 25e-6
Q5a  15a 99a 99 PCM 1;  
Q6a  99a 99a 99 PCM 1
Q5b  15b 99b 99 PCM 1
Q6b  99b 99b 99 PCM 1
EOS  7  1 POLY(4) (14,20) (73,98) (83,98) (81,98) 250e-6 1 1 1 1;  
RC1 5a 50 6.0e+3
RC2 6a 50 6.0e+3
RE1a  3a 15a 5.362e+01;
RE2a  4c 15b 5.362e+01;
RE1b  3c 15a 5.362e+01;
RE2b  4a 15b 5.362e+01;
C1   15a 15b 10.0e-12
*
Ccm1 1 50 1.85e-12; 3.2p total
Ccm2 2 50 1.85e-12; 3.2p total
Cdm  1  2 4.3e-12; 9.0p total
QD23 1 1 2 PD
QD24 2 2 1 PD
*
* first gain stage:
G1   10 98 (5a,6a) 2.0e-03
R1   10 98 5.0e+02
*
* pole, zero:
G2 21 98 (98,10) 1.176e-03
R4 21 98 8.5e+02
R5 21 22 8.5e+02
C4 22 98 112.42e-12;  62.42e-12
*
* zero, pole:
E1  23 98 (21,98) 2
R6  23 24 53e+03
R7  24 98 53e+03
C5  23 24 0.61e-12;  
*
* second gain stage:
G3  25 98 (23 ,98) 1.834e-04;  
R8 25 98 1.0e+06;  
D3  25a 98 DX
VD3 25a 25 -2
D4  98 25b DX
VD4 25 25b -2
*
* common-mode stage:
ECM 13 98 POLY(2) (1,98) (2,98) 0 1.252E-01 1.252E-01
R2  13 14 3.979e+01
R3  14 98 1.989e-02
C3  13 14 1.0e-06
*
* power supply rejection ratio network:
EPSY 98 72 POLY(1) (99,50) -4.036e-00 4.036e-01
RPS3 72 73 2.274e+04
RPS4 73 98 7.958e+00
CPS3 72 73 1.000e-09
*
*
* voltage noise reference:
VN1 80 98 0
RN1 80 98 2.4e-01
HN  81 98 VN1 9.90e+00
RN2 81 98 1
*
* flicker noise corner:
DFN 82 98 DNOISE 1000
IFN 98 82 DC 1.0e-03
DFN2 182 98 DY
IFN2 98 182 DC 1.0e-06
GFN 83 98 POLY(1) (182,82) 1.00e-13 1.00e-01
RFN 83 98 1
*
* current noise model:
D60 60 98 DN1 1000
I60 98 60 1.0e-03
D61 61 98 DN4
I61 98 61 1.0e-06
G60 1 50 61 60 3.3e-05
G61 2 50 61 60 3.45e-05
*
GSY  99 50 POLY(1) (99,50) 291.0e-6 +2.3e-06
R9   99 20 500.0e+03
R10  20 50 500.0e+03
EREF 98  0 (20,0) 1
*
* output stage:
Q3   45 41 99 POUT 40
Q4   45 43 50 NOUT 20
EB1  99 40 POLY(1) (98,25) 6.7e-01  1;
EB2  42 50 POLY(1) (25,98) 6.8e-01  1;
RB1  40 41 5.0e+03
RB2  42 43 5.0e+03
Eflip 125a,98 (45,98) 1
RZ  125a 125b 5.0e+01
CF   125b 25 8.4e-12
D5   46 99 DX
D6   47 43 DX
V3   46 41 0.7
V4   47 50 0.7
*
* models used in the subcircuit:
.model PIX pnp(
+ Bf=100
+ Vaf=28
+ Tf=6.0e-11
+ Cje=0.5e-12
+ Cjc=0.45e-12)
*
.model PD npn(
+ Bf=100
+ Is=1.0e-18
+ Vaf=100
+ Rc=1500
+ Tf=5.0e-11)
*
.model PCM pnp(
+ Bf=30
+ Vaf=25
+ Rc=100
+ Tf=6.0e-11)
*
.model POUT pnp(
+ Bf=90
+ Is=2.782e-17
+ Vaf=25
+ Kf=3.0e-17
+ Re=2
+ Rc=240
+ Tf=6.0e-11)
*
.model NOUT npn(
+ Bf=120
+ Is=1.786e-17
+ Vaf=100
+ Kf=3.0e-17
+ Re=2
+ Rc=270
+ Tf=4.0e-11)
*
.model DN1 d(Is=1.0e-16)
.model DN4 d(Is=1.0e-16 Af=1 Kf=6.35e-17)
.model DX d()
.model DY d(Is=1.0e-16 Rs=1.0e-1)
.model DNOISE d(Is=1.0e-16 Rs=1.0e-01 Kf=1.42e-12)
*
.ends op162
*
