#!/usr/bin/env python

# $id: $
# This is a python script to interface ACS to Kstars. The script collects the observed 
# azimuth and elevation and it commands the collected cooordinates to kstars.
#who                                   when           what
#andrea orlati(a.orlati@ira.inaf.it)   22/05/2008     Creation



import sys
from Acspy.Clients.SimpleClient import PySimpleClient
from Acspy.Common.Callbacks import *
import ACS, ACS__POA
import AntennaBoss_idl
import Antenna__POA
import readline
import rlcompleter
import math
import mx.DateTime
import ClientErrorsImpl
import os
import time
from commands import *

readline.parse_and_bind("tab: complete")


def main():

    #open kstars 
    out = os.system("kstars&")

    sc = PySimpleClient()
    try:
        c = sc.getComponent("ANTENNA/Boss")
    except Exception , ex:
        newEx = ClientErrorsImpl.CouldntAccessComponentExImpl( exception=ex, create=1 )
        newEx.setComponentName("ANTENNA/Boss")
        newEx.log(ACS_LOG_ERROR)
        sys.exit(1)
        
    try:
        azimuth = c._get_observedAzimuth()
    except Exception, ex:
        newEx = ClientErrorsImpl.CouldntAccessPropertyExImpl( exception=ex, create=1 )
        newEx.setPropertyName("observedAzimuth")
        newEx.log(ACS_LOG_ERROR)
        sys.exit(1)
    try:
        elevation = c._get_observedElevation()
    except Exception, ex:
        newEx = ClientErrorsImpl.CouldntAccessPropertyExImpl( exception=ex, create=1 )
        newEx.setPropertyName("observedElevation")
        newEx.log(ACS_LOG_ERROR)
        sys.exit(1)

    kstarsInstance = getoutput("dcopfind -a 'kstars*'")

    vivo = 1

    while (vivo):
    	time.sleep(0.3)
    	az = azimuth.get_sync()[0]
    	az = az * 57.295779513082320876798154814105170332405472466564
    	el = elevation.get_sync()[0]
    	el = el * 57.295779513082320876798154814105170332405472466564
    	comando = "dcop %s KStarsInterface setAltAz %f %f" % (kstarsInstance, el, az)
    	exit = getoutput(comando)
    	kstarsInstance = getoutput("dcopfind -a 'kstars*'")
    	if kstarsInstance != '':
    		vivo = 1
    	else:
    		vivo = 0 

    print "Finished..."
    sc.releaseComponent("ANTENNA/Boss")
    sc.disconnect()


if __name__=="__main__":
   main()

