#! /usr/bin/env python
#
# Define a function which converts ACS time stamps found in the
# spectral files output by CORRGui into human readable format.
#
# How to use it?
# tjuerges@delphinus:~ 10: python -i convertACSTimeStamp.py 
# >>> convertACSTimeStamp(134168868294240000)
#
# 134168868294240000 = 2007-12-14T01:00:29.4240000


def convertACSTimeStamp(ts = 0, printIt = True):
 import Acspy.Common.TimeHelper
 import time

 epoch = Acspy.Common.TimeHelper.TimeUtil()
 epochPy = epoch.epoch2py(ts)
 pyEpoch = epoch.py2epoch(epochPy)
 fracSecs = ts - pyEpoch.value
 t = time.gmtime(epochPy)

 if printIt == False:
  return (t, fracSecs)

 print "\n%d = %04d-%02d-%02dT%02d:%02d:%02d.%07d\n" % (ts, t.tm_year, t.tm_mon,
 t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, fracSecs)

 return
