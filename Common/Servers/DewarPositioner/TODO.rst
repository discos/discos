Bug enorme nel component del derotatore
=======================================

Importante: serie di comandi che fanno andare in crash il component 
del derotatore:

<0> derotatorSetup=KKG
<1> derotatorGetActualSetup
KKG
<2> derotatorIsReady
True
<3> derotatorGetPosition
0.0084d
<4> derotatorGetConfiguration
FIXED
<5> derotatorGetRewindingMode
AUTO
<6> derotatorSetConfiguration
Error - missing arguments, type help(derotatorSetConfiguration) for details
<7> derotatorSetConfiguration=BSC
<8> derotatorGetConfiguration
BSC
<9> derotatorSetConfiguration=FIXED
<10> derotatorGetConfiguration
FIXED
<11> derotatorSetPosition=10d


Bug in derotatorSetConfiguration
================================
Se si da un::
  
    > derotatorSetConfiguration=WRONGCODE

Viene cancellata la configurazione attuale...
