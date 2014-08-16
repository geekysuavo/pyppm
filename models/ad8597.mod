* ad8597 low noise, low distortion operational amplifier
*
* Node assignments
* connections:  non-inverting input
*               |  inverting input
*               |  |  positive supply
*               |  |  |  negative supply
*               |  |  |  |  output
*               |  |  |  |  |
.subckt ad8597  3  2  7  4  6
*
*
*
* Input Impedances
G3 3 2 3 2 82U
R1 3 0 1.6G
R2 2 0 1.6G
C1 3 0 2.25P
C2 2 0 2.25P
C3 3 2 7P
*
* Input Clamping
D1 3 12 D1
D2 12 2 D1
D3 2 13 D1
D4 13 3 D1
.MODEL D1 D IS=1E-16
*
* Preconditioning Gain Stage and Sum Node for Gain Control
* and Noise Insertion
E1 20 19 3 2 100
R3 19 0 10M
* Second-Order Frequency Shaping
R50 20 21 10
C50 21 0 110P
R51 21 22 50
C51 22 0 17P
R52 22 23 250
C52 23 0 17P
R53 23 40 1.25K
C53 40 0 11P
* Limiting Amplifier with Slew-Rate Control
G4 41 0 40 0 100U
G5 41 0 41 0 5U
D5 42 41 D2
D6 41 43 D2
V1 0 42 460
V2 43 0 460
.MODEL D2 D IS=1E-16 EG=0.6
*
* Primary Gain Block, Dominant Pole/Zero and Swing Limiting Network
G6 44 0 41 0 10U
G7 44 0 44 0 33.3N
C4 44 48 310P
R4 48 0 60
D7 44 46 D2
D8 47 44 D2
E2 46 0 POLY(1) 7 0 -1.76 1
E3 47 0 POLY(1) 4 0 1.95 1
*
* Output Stage and Gain-Reduction Feedback
M1 7 44 45 4 MN AD=0 AS=0 L=1U W=4700U
M2 4 44 45 7 MP AD=0 AS=0 L=1U W=4700U
R6 45 6 1M
G8 19 0 45 6 1000
R5 44 50 180
C5 50 6 400P
*
.MODEL MN NMOS LEVEL=1 KP=2E-5 PHI=1.0 VTO=-120M GAMMA=0 LAMBDA=0
.MODEL MP PMOS LEVEL=1 KP=2E-5 PHI=1.0 VTO=120M GAMMA=0 LAMBDA=0
*
* Quiescent Supply Current
R7 7 4 52K
I1 7 4 3.53M
*
* Noise Modeling
D60 60 0 DN1 1000
I60 0 60 1M
D61 61 0 DN4
I61 0 61 1U
D62 62 0 DN3
I62 0 62 1U
D63 63 0 DN2
I63 0 63 1U
G60 3 0 61 60 .00018
G61 2 0 61 60 .00018
G62 3 2 62 60 .000092
G63 19 0 63 60 700
.MODEL DN1 D IS=1E-16
.MODEL DN2 D IS=1E-16 AF=1 KF=1.05E-17
.MODEL DN3 D IS=1E-16 AF=1 KF=2.8E-17
.MODEL DN4 D IS=1E-16 AF=1 KF=4.5E-17
*
.ends ad8597
*
