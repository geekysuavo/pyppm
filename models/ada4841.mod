* ada4841 high-speed precision operational amplifier
*
* Node assignments
* connections:  non-inverting input
*               |  inverting input
*               |  |  positive supply
*               |  |  |  negative supply
*               |  |  |  |  output
*               |  |  |  |  |
.subckt ada4841 1  2  99 50 4
*
* differential stage:
Q1 13 12 14 npn
Q2 17 2 16 npn
Rc1 98 13 3.200
Rc2 98 17 3.200
Re1 14 15 3.148
Re2 15 16 3.148
Ibias 15 51 1.00e+00
Dcmlim1 18 15 Dquiet
Vcmlim1 18 51 -0.074
*
* voltage noise generation:
HVnoise 12 7 Vvnoise 1
VVnoise 501 0 0
DVnoise 501 0 Dvnoise
RVnoise 501 0 0.003755102
*
* current noise generation, non-inverting input:
FInoise1 12 0 VInoise1 1
VInoise1 502 0 0
DInoise1 502 0 DInoise1
RInoise1 502 0 8448.979592
*
* current noise generation, inverting input:
FInoise2 2 0 VInoise2 1
VInoise2 503 0 0
DInoise2 503 0 DInoise2
RInoise2 503 0 8448.979592
*
* common-mode injection:
Rcm1 1 601 100meg
Rcm2 2 601 100meg
Gcmr 0 602 601 75 1.00e-06
Rcmr1 602 603 1meg
Rcmr2 603 604 1.778
Lcmr 604 0 9.434e-05
Ecmr 10 9 603 0 1.000e+00
*
* positive power supply rejection:
Epsr1 700 0 98 0 1
Rpsr1 700 701 100
Rpsr2 701 702 3.162e-04
Lpsr1 702 0 1.007e-07
Epsr2 11 10 701 0 1
*
* negative power supply rejection:
Epsr3 703 0 51 0 1
Rpsr3 703 704 100
Rpsr4 704 705 1.0e-04
Lpsr2 705 0 3.183e-09
Epsr4 12 11 704 0 1
*
* input offset and bias:
Vos 1 7 4.000e-05
Ios 1 2 5.000e-08
*
* input impedance:
Cinv 2 0 2.00e-12
Cninv 1 0 2.00e-12
*
* first gain and slew limiting:
Gslew 0 101 17 13 1.0e+00
Rslew 101 0 250
Dslew1 101 102 Dzener
Dslew2 0 102 Dzener
*
* second gain and dominant pole with output voltage limiting:
Gp1 51 201 101 0 1.885e-06
Rp1 201 51 2.122e+09
Cp1 201 51 1.50e-12
Vlim1 97 206 -0.2
Dlim1 201 206 dquiet
Vlim2 207 52 -0.2
Dlim2 207 201 dquiet
Esupref1 97 98 51 0 1
Esupref2 52 51 51 0 1
*
* second pole:
Gp2 0 202 201 51 1.00e-03
Rp2 202 0 1k
Cp2 202 0 2.27364e-12
*
* third pole:
Gp3 0 203 202 0 1.00e-03
Rp3 203 0 1k
Cp3 203 0 1.59155e-16
*
* fourth pole:
Gp4 0 204 203 0 1.00e-03
Rp4 204 0 1k
Cp4 204 0 1.59155e-16
*
* fifth pole:
Gp5 0 205 204 0 1.00e-03
Rp5 205 0 1k
Cp5 205 0 1.592e-16
*
* first zero:
Gz1 0 301 205 0 1.00e-03
Rz1 301 302 1k
Lz1 302 0 1.592e-10
*
* second zero:
Gz2 0 303 301 0 1.00e-03
Rz2 303 304 1k
Lz2 304 0 1.592e-10
*
* third zero:
Gz3 0 305 303 0 1.00e-03
Rz3 305 306 1k
Lz3 306 0 1.59e-10
*
* buffer:
Gbuf 0 401 305 0 1.00e-02
Rbuf 401 0 100
*
* output with current limiting:
Eout 404 0 401 0 1.000e+00
Rout 404 405 1
Lout 405 406 2.00e-08
Cout 406 0 1.00e-12
Voutmon 406 4 0
Dout1 401 407 Dquiet
Vout1 407 406 -7.600e-01
Dout2 408 401 Dquiet
Vout2 406 408 -7.600e-01
*
* voltage reference generator:
Eref1 98 0 99 0 1
Eref2 51 0 50 0 1
Rref1 98 901 100meg
Rref2 901 51 100meg
Eref3 75 0 901 0 1
*
* supply current correction:
Iq 99 50 0.0006
Fsup1 802 0 Voutmon 1
DZsup1 0 802 Dzener2
Dsup1 99 802 Dquiet
Fsup2 0 804 Voutmon -1
DZsup2 804 0 Dzener2
Dsup2 804 50 Dquiet
*
* models used throughout the subcircuit:
.model npn npn(Bf=166665.666666667
.model dquiet d()
.model dvnoise d(Kf=220500)
.model dinoise1 d(Kf=0.196)
.model dinoise2 d(Kf=0.196)
.model dzener d(Bv=9.74508003908849)
.model dzener2 d(Bv=50)
*
.ends ada4841
*
