DefaultMode="L3L4" (1300:1800 MHz)
LOmin -> 0
LOmax -> 3000
DefaultLO -> 0
Low pass IF Filter -> 0:1000


Setup
=====
> receiversSetup=LLP


Test 1: LO below the minimum allowed value
==========================================
> setLO=-1 # Target command

Expected: raises ComponentErrors::ComponentErrorsEx


Test 2: LO above the maximum allowed value
==========================================
> setLO=3001 # Target command

Expected: raises ComponentErrors::ComponentErrorsEx


Test 3: LO inside the default observed sky band
===============================================
> setLO=1500 # Target command

Expected: raises ComponentErrors::ComponentErrorsEx
Expected Message: "LO frequency value not allowed. It is within 
the band and might generate strong aliasing."


Test 4: LO inside the (no-default) observed sky band
====================================================
> receiversMode=XXL5 (1625:1715)
> setLO=1500
> setLO=1650 # Target command

Expected: raises ComponentErrors::ComponentErrorsEx
Expected Message: "LO frequency value not allowed. It is within 
the band and might generate strong aliasing."


Test 5: receiversMode after setLO: LO within the band
=====================================================
> receiversMode=XXL5 (1625:1715)
> setLO=1500 
> receiversMode=XXL4 (1300:1800) # Target command

Expected: raises ComponentErrors::ComponentErrorsEx
Expected Message: "Mode not allowed. The LO value is within 
the band and might generate strong aliasing."


Test 6: default LO value after
==============================
Expected: LO == DefaultLO


Test 7: bandwidth and start freq (default)
==========================================
Expected: bandwidth==500
Expected: start frequency==1300 (1300-0)


Test 8: bandwidth and start freq (NO-default)
=============================================
> receiversMode=L1L5 (1625:1715)
> setLO=2300

Expected: bandwidth==90
Expected: start frequency==-675 (1625-2300)


Test 9: low pass IF filter (default)
====================================
> setL0=600 (IF band: 700-1200)

Expected: Bandwidth==300 (700:1000)
Expected: start frequency==700


Test 10: low pass IF filter (NO-default)
=======================================
> receiversMode=L1L5 (1625-1715)
> setL0=700 (IF band: 925-1015)

Expected: Bandwidth==75 (925:1000)
Expected: start frequency==925


Test 11: low pass IF filter (NO-default): NOT IMPLMENTED
========================================================
> receiversMode=L1L5 (1625-1715)
> setL0=500 (IF band: 1125-1215) # Target command

Expected: raises ComponentErrors::ComponentErrorsEx
Expected Message: "LO frequency value not allowed. The IF bandwidth
is outside the low pass filter (0-1000MHz)".


.. note:: Maybe we should raise a more specific exception type. For
   instance, a ReceiverErrors::ValidationError. In that case, we have
   to change the implementation of every receiver, because this change
   affects also the K and C band, when checking for the max and min
   values in setLO().
