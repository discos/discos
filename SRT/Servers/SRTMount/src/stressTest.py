#!/usr/bin/env python

# This is a python test program that send continuosly different positions to the mount Mount in order to stress the drivers
#who                                   when           what
#Andrea Orlati(a.orlati@ira.inaf.it)   15/11/2011     Creation

import getopt, sys
import Acspy.Common.Err
import maciErrType
import maciErrTypeImpl
import ClientErrorsImpl
import ACSLog
from Acspy.Clients.SimpleClient import PySimpleClient
from Acspy.Common.TimeHelper import getTimeStamp
from Acspy.Common.EpochHelper import EpochHelper
from Acspy.Common.DurationHelper import DurationHelper
import string
import math
import time


def usage():
    print "stressTest [-h|--help] [-g|--gap=] [-n|--points=]"
    print ""
    print "[-h|--help]        displays this help"
    print "[-g|--gap=]        allows to give the gap, in microseconds, between each new position [default 5000000]"
    print "[-n|--points=]     allows to give how many points will be commanded [default 240]"
    print "[-d|--length=]     allows to give the displacement (degrees) to be commanded for each new position [default 5]"
    
def main():
    
    try:
        opts, args = getopt.getopt(sys.argv[1:],"hg:n:d:",["help","gap=","points=","length="])
    except getopt.GetoptError, err:
        print str(err)
        usage()
        sys.exit(1)
        
    gap=5000000
    points=240
    step=5.0
    
    for o, a in opts:
        if o in ("-h", "--help"):
            usage()
            sys.exit()
        elif o in ("-g", "--gap"):
            gap = string.atoi(a)
        elif o in ("-n", "--points"):
            points = string.atoi(a)
        elif o in ("-a", "--amplitude"):
            amplitude = string.atof(a)
        elif o in ("-d", "--length"):
            step = string.atof(a)
            
    
    #get the link to the SRTmountcomponent
    simpleClient = PySimpleClient()
    compName=""
    compType = "IDL:alma/Antenna/SRTMount:1.0"
    try:
        component=simpleClient.getDefaultComponent(compType)
        compName=component._get_name()
    except Exception , ex:
        newEx = ClientErrorsImpl.CouldntAccessComponentExImpl( exception=ex, create=1 )
        newEx.setComponentName(compType)
        newEx.log(simpleClient.getLogger(),ACSLog.ACS_LOG_ERROR)
        sys.exit(1)  
    
    try:
        azimuth=component._get_azimuth().get_sync()[0]
    except Exception , ex:
        newEx = ClientErrorsImpl.CouldntAccessPropertyExImpl( exception=ex, create=1 )
        newEx.setPropertyName("azimuth")
        newEx.log(simpleClient.getLogger(),ACSLog.ACS_LOG_ERROR)
        sys.exit(1)  

    try:
        elevation=component._get_elevation().get_sync()[0]
    except Exception , ex:
        newEx = ClientErrorsImpl.CouldntAccessPropertyExImpl( exception=ex, create=1 )
        newEx.setPropertyName("elevation")
        newEx.log(simpleClient.getLogger(),ACSLog.ACS_LOG_ERROR)
        sys.exit(1)  
            
    print "current azimuth and elevation: %lf %lf" % (azimuth,elevation)
    
    #convert the gap from microseconds to seconds
    gapS=gap/1000000.0;
    x = [ azimuth+step, azimuth, azimuth-step, azimuth ]
    y = [ elevation, elevation+step, elevation, elevation-step]
    
    for index in range(points):
        num = index % 4
        print "New position:\t%lf\t%lf" % (x[num],y[num])
        try:
            component.preset(x[num],y[num])
        except Exception , ex:
            newEx = ClientErrorsImpl.CouldntPerformActionExImpl( exception=ex, create=1 )
            newEx.setAction("preset")
            newEx.log(simpleClient.getLogger(),ACSLog.ACS_LOG_ERROR)
            sys.exit(1)
        time.sleep(gapS)
        
    if not (compName==""):
        simpleClient.releaseComponent(compName)
    
    print "cycle completed"
             
    simpleClient.disconnect()
    

if __name__=="__main__":
   main()
    
    
    
    
    