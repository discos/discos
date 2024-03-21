# SRTMinorServoCommandLibrary Python wrapper

In order to use the SRTMinorServoCommandLibrary python wrapper, it is sufficient to import the SRTCommandLibrary package like this:

`import SRTMinorServoCommandLibrary`

The all the functions of the library can be used in the following fashion:

`SRTMinorServoCommandLibrary.status()`
`SRTMinorServoCommandLibrary.status('PFP')`
`SRTMinorServoCommandLibrary.setup('KKG')`
`SRTMinorServoCommandLibrary.stow('PFP')`
`SRTMinorServoCommandLibrary.stow('PFP', 2)`
`SRTMinorServoCommandLibrary.stop('SRP')`
`SRTMinorServoCommandLibrary.preset('SRP', [0, 1, 2, 3, 4, 5])`
`SRTMinorServoCommandLibrary.programTrack('PFP', 0, 0, [0, 1, 2, 3, 4, 5], <start_time>)`
`SRTMinorServoCommandLibrary.programTrack('PFP', 0, 1, [0, 1, 2, 3, 4, 5])`
`SRTMinorServoCommandLibrary.offset('SRP', [6, 7, 8, 9, 10, 11])`
`SRTMinorServoCommandLibrary.parseAnswer('OUTPUT:GOOD,1665757400.123456,PFP_ENABLED=5|PFP_STATUS=44|PFP_BLOCK=7|PFP_WARNING=47|PFP_ROTARY_AXIS_ENABLE=52|PFP_COORD_1=94')`
