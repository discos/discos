This program calculates the usd positions for every elevation
from 0 to 90 degrees, starting from a FEM or real lookup table.

to compile, type:
g++ SRTActiveSurfaceActuatorsPositions.cpp -o ~/Introot/bin/SRTActiveSurfaceActuatorsPositions -lgsl -lgslcblas

to use, cd in $ACS_CDB/CDB/alma/AS directory and type:
a) touch actuatorsCorrections_FEM.txt
b) touch actuatorsCorrections.txt
c) SRTActiveSurfaceActuatorsPositions
