* ltc6241 low input current, microvolt offset, low-noise dual opamp
*
* Node assignments:
* connections:  non-inverting input
*               | inverting input
*               | | positive voltage
*               | | | negative voltage
*               | | | | output
*               | | | | |
.subckt ltc6241 3 2 7 4 6
*
IBIASDEF2 0 2 7E-13
IBIASDEF1 0 3 1.2E-12
RG1 24 0 6E3
CG1 24 0 1E-6
GCMRR 0 51 50 4 6.31E-9
RCMRR 51 0 1E3
GPSRR 0 53 7 4 3.2E-9
RPSRR1 53 0 1E3
GG3 0 26 24 0 6.2832
RG3 26 0 0.159155
CG3 26 0 1.45E-6
RVNOI 41 0 1E3
CIN3 2 4 1.5pF
CIN2 4 3 1.5pF
DVNF 42 0 DVNOI
IVNF 0 42 1
DIN1 3 7 DIN
DIN2 2 7 DIN
GZP1 0 28 26 0 0.1
RZP1 28 29 10
RZP4 29 0 45
LZP2 29 0 3.11E-7
GG5 0 27 30 0 6.2832
RG5 27 0 0.159155
CG5 27 0 8E-8
DIN3 4 3 DIN
DIN4 4 2 DIN
IDUM1 41 0 0
DVNFX 43 0 DVNOIX
IVNFX 0 43 1
RINX 21 0 9.2
GG1 0 24 VALUE={15*TANH(V(21))}
MOUT1 77 27 6 6 NOUT
MOUT2 44 27 6 6 POUT
CIN4 7 3 1.5pF
CIN5 2 7 1.5pF
DVL1 25 24 DVLIM
DVL2 25 0 DVLIM
EICM 50 0 VALUE={(V(3)+V(2))/2}
CCMRR 50 52 3E-13
RCMRR2 52 51 3E3
RPSRR2 53 54 3E3
CPSRR1 54 7 5E-12
RPSRR3 53 55 3E3
CPSRR2 55 4 2E-11
GZP2 0 30 28 0 0.1
RZP2 30 31 10
RZP3 31 0 45
LZP1 31 0 3.11E-7
GITYP 7 4 VALUE={1.15E-3+1.4E-4*V(7,4)}
CIN1 3 2 0.5p
GLIM1 7 77 VALUE={MIN(0.03,0.0333*V(7,77))}
GLIM2 44 4 VALUE={MIN(0.03,0.0333*V(44,4))}
GB 0 27 VALUE={3.14*(V(7)+V(4))}
GINX 0 21 VALUE={V(3,2)+V(41)+V(42,43)+V(51)+V(53)-3E-5}
GINOI 2 3 41 0 8E-8
*
* models used in the subcircuit:
.model NOUT nmos(Kf=0 Kp=5 Vto=-10m)
.model POUT pmos(Kf=0 Kp=5 Vto=10m)
.model DIN d(Kf=0 Rs=0)
.model DVLIM d(Bv=5 Is=1.0e-10 Kf=0 Rs=0)
.model DVNOIX d(Kf=0 Rs=0)
.model DVNOI d(Rs=0 Kf=3.15e-12)
*
.ends ltc6241
*
