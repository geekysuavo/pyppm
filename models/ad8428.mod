* ad8428 low-noise instrumentation amplifier
*
* Node assignments
* connections: inverting input
*              |   non-inverting input
*              |   |   negative supply
*              |   |   |   reference
*              |   |   |   |   output
*              |   |   |   |   |    positive supply
*              |   |   |   |   |    |
*              |   |   |   |   |    |
*              |   |   |   |   |    |
.subckt ad8428 in- in+ -Vs ref Vout +Vs
*
Rgain Rg+ Rg- 3.001
R1 sub_out sub_neg 5.0e+3
R2 sub_neg Inverting_Out 5.0e+3
R3 sub_pos noninverting_out 5002.1
R4 ref sub_pos 5.0e+3
R5 Rg- N003 3.0e+3
R6 Rg+ N010 3000
D3 N003 P001 D
D4 P002 N003 D
V3 P002 VNEGx 0.94
V4 VPOSx P001 0.71
D5 N010 P003 D
D6 P004 N010 D
V5 P004 VNEGx 0.94
V6 VPOSx P003 0.71
D7 N005 P005 D
D8 P006 N005 D
V7 P006 VNEGx 2.5
V8 VPOSx P005 3.2
D9 N016 P007 D
D10 P008 N016 D
V9 P008 VNEGx 1.5
V10 VPOSx P007 3.2
D11 N009 P009 D
D12 P010 N009 D
V11 P010 N017 2.2
V12 N008 P009 1.55
D13 ref P011 D
D14 P012 ref D
V13 P012 VNEGx 0.3
V14 VPOSx P011 0.3
D15 sub_pos P013 D
D16 P014 sub_pos D
V15 P014 VNEGx 0.9
V16 VPOSx P013 0.9
E4 Inverting_Out 0 N003 0 1
E5 noninverting_out 0 N010 0 1
V1 VBIAS +Vs 20
I1 VBIAS Pos_Fdbk 660.0e-6
I2 VBIAS Inv_Fdbk 660.0e-6
C1 N003 Inv_Fdbk 11.0e-12
C2 N010 Pos_Fdbk 11.0e-12
E8 N002 0 N005 0 1
E9 N013 0 N016 0 1
VOSI_Neg N004 in- 50.0e-6
VOSI_Pos in+ N014 49.0e-6
VOSO Vout N009 575.0e-6
C3 Rg- 0 12.0e-12
C4 Rg+ 0 11.45e-12
I23 in- 0 -30e-9
I24 in+ 0 -25e-9
G1 0 in+ N018 N019 .0025e-9
R13 in+ N018 10e9
R14 N018 in- 10e9
R15 +Vs N019 10e9
R16 N019 -Vs 10e9
G2 0 in- N018 N019 .0025e-9
E10 VPOSx 0 +Vs 0 1
I3 +Vs -Vs 6.7e-3
G3 +Vs -Vs +Vs -Vs 2.0e-6
E11 VNEGx 0 -Vs 0 1
*H1 VPOSx N008 POLY(1) VOSO 0 0 8000
BH1 VPOSx N008 V=8000*i(VOSO)
*H2 N017 VNEGx POLY(1) VOSO 0 0 8000
BH2 N017 VNEGx V=8000*i(VOSO)
H3 N006 N004 V24 0.352
V24 N001 0 0
R19 N001 0 0.0166
V25 N007 0 0
R20 N007 0 0.0166
H5 N015 N014 V26 0.352
V26 N011 0 0
R21 N011 0 0.0166
G4 0 N005 N006 N005 1
G5 0 N016 N015 N016 1
G6 0 N003 VBIAS Inv_Fdbk 1
G7 0 N010 VBIAS Pos_Fdbk 1
G8 0 sub_out sub_pos sub_neg 1
R10 N005 0 10.0e+9
R7 N003 0 10.0e+9
R11 N016 0 10.0e+9
R8 N010 0 10.0e+9
R9 sub_out 0 10.0e+9
Q1 Pos_Fdbk N013 Rg+ 0 NPN
Q2 Inv_Fdbk N002 Rg- 0 NPN
H4 X sub_out V25 39.45
G9 0 N012 Y N009 1
C8 Y 0 1.0e-9
R22 Y 0 1.0e+9
G10 0 N009 N012 0 8.32e-3
R12 N012 0 1.0e+9
R17 N009 0 120.13
C5 N012 0 1.06e-8
C6 N009 0 180.0e-12
*BG11 0 Y I=min(max(1.0*v(X,Y),0.025),-0.025)
BG11 0 Y I=1.0*v(X,Y)
R18 VBIAS Inv_Fdbk 1.0e+9
R23 Pos_Fdbk VBIAS 1.0e+9
D1 sub_out P015 D
V2 VPOSx P015 1.55
D2 P016 sub_out D
V17 P016 VNEGx 2.2
I4 +Vs 0 -1.3e-3
* subcircuit models
*
.model D D
.model NPN NPN
.model PNP PNP
*
.ends ad8428
*
