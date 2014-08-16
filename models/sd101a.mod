* sd101a diode spice model.
*
* Node assignments:
* connections: anode
*              | cathode
*              | |
.subckt sd101a 1 2
*
.model SD d(
+ N=1.70463
+ Is=1.55043e-007
+ Rs=31.5955
+ Eg=0.69
+ Xti=2 
+ Cjo=2.0e-12
+ Vj=0.314625
+ M=0.188302
+ Fc=0.5
+ Tt=1.4427e-09
+ Bv=60
+ Ibv=0.001
+ Kf=0
+ Af=1)
*
.model PND d(
+ N=2.01276
+ Is=2.08639e-009
+ Rs=0.577781
+ Eg=1.11
+ Xti=3)
*
D1 1 2 SD
D2 1 2 PND
*
.ends sd101a
*
