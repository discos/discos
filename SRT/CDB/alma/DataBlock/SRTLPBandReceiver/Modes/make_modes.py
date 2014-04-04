"""
This script makes for each mode its directory and .xml file (Python 3.3 or higher)

Each mode is composed by 4 characters, that indicate respectively the:

    - L band polarization (three character choices: *, L, C)
    - L band filter ID (1, 2, 3, 4, 5):
        * 1 -> all band filter, 1300-1800 no filter
        * 2 -> 1320-1780 MHz
        * 3 -> 1350-1450 MHz (VLBI)
        * 4 -> 1300-1800 MHz (band-pass)
        * 5 -> 1625-1715 MHz (VLBI)
    - P band polarization (*, L, C)
    - P band filter ID (1, 2, 3):
        * 1 -> all band filter, 305-410 no filter
        * 2 -> 310-350 MHz
        * 3 -> 305-410 MHz (band-pass filter)

For instance, the code C1L2 means:

    - Circular polarization of the L band
    - Filter n.1 of the L band 
    - Linear polarization of the P band
    - Filter n.2 of the P band

The * character means the corresponding value should not change. 
For instance, the code *1** means:

    - Do not change the polarization of the L band
    - Filter n.1 of the L band
    - Do not change the polarization of the P band
    - Do not change the filter of the P band
"""

import os

lband_filters = {
        1: (1300.0, 1800.0), # all band filter, 1300-1800 no filter 
        2: (1320.0, 1780.0), # 1320-1780 MHz
        3: (1350.0, 1450.0), # 1350-1450 MHz (VLBI)
        4: (1300.0, 1800.0), # 1300-1800 MHz (band-pass)
        5: (1625.0, 1715.0)  # 1625-1715 MHz (VLBI)
}

pband_filters = {
        1: (305.0, 410.0), # all band filter, 305-410 no filter
        2: (310.0, 350.0), # 310-350 MHz
        3: (305.0, 410.0)  # 305-410 MHz (band-pass filter)
}

formatter = {}
for lband_pol in 'LC':
    for pband_pol in 'LC':
        for lid, lband in lband_filters.items():
            for pid, pband in pband_filters.items():
                code = "%s%s%s%s" %(lband_pol, lid, pband_pol, pid)
                formatter['code'] = code
                formatter['lband_pol'] = 'L R' if lband_pol == 'C' else 'H V'
                formatter['pband_pol'] = 'L R' if pband_pol == 'C' else 'H V'
                formatter['lband_rf_min'] = lband[0]
                formatter['lband_rf_max'] = lband[1]
                formatter['pband_rf_min'] = pband[0]
                formatter['pband_rf_max'] = pband[1]
                formatter['lband_filter_id'] = lid
                formatter['pband_filter_id'] = pid
                file_name = os.path.join(code, code + '.xml')
                os.mkdir(code)
                outfile = open(os.path.join(code, code + '.xml'), 'w')
                text = """<?xml version='1.0' encoding='ISO-8859-1'?>
                
                <SRTLPBandReceiverModeSetup xmlns="urn:schemas-cosylab-com:SRTLPBandReceiverModeSetup:1.0"
                	xmlns:baci="urn:schemas-cosylab-com:BACI:1.0"
                	xmlns:cdb="urn:schemas-cosylab-com:CDB:1.0"
                	xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                	
                	Mode="{code}"
                    LBandRFMin="{lband_rf_min:.1f} {lband_rf_min:.1f}"
                    LBandRFMax="{lband_rf_max:.1f} {lband_rf_max:.1f}"
                    PBandRFMin="{pband_rf_min:.1f} {pband_rf_min:.1f}"
                    PBandRFMax="{pband_rf_max:.1f} {pband_rf_max:.1f}"
                    LBandFilterID="{lband_filter_id}"
                    PBandFilterID="{pband_filter_id}"
                    IFs="2"
                    LBandIFMin="100.0 100.0"
                    PBandIFMin="100.0 100.0"
                    Feeds="2" 
                    LBandPolarization="{lband_pol}"
                    PBandPolarization="{pband_pol}"
                />
                """.format_map(formatter)
                lines = [line.lstrip() + '\n' for line in text.split('\n')]
                outfile.writelines(lines)
 
