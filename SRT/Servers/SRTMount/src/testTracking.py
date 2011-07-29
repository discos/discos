#!/usr/bin/env python

# " $Id: testTracking.py,v 1.2 2011-06-03 18:02:49 a.orlati Exp $ "
# This is a python test program that loads the Mount with a table of time tagged program track points.
# in order to test the mount capabilities to manage such an operating mode.
# the generated tracking curve is a sine function for both axis.
#who                                   when           what
#Andrea Orlati(a.orlati@ira.inaf.it)   01/06/2011     Creation

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
    print "testTracking [-h|--help] [-g|--gap=] [-n|--points=] [-a|--amplitude=] [-t|--period=]"
    print ""
    print "[-h|--help]        displays this help"
    print "[-g|--gap=]        allows to give the gap, in microseconds, between each point of the table [default 100000]"
    print "[-n|--points=]     allows to give how many points will be added to the table [default 500]"
    print "[-a|--amplitude]   allows to give the amplitude of the sine curve [default 2 degrees]"
    print "[-t|--period=]     allows to provide the period of the sine curve [default 30 seconds]"
    
def main():
    
    MAXSPEED=0.5
    
    try:
        opts, args = getopt.getopt(sys.argv[1:],"hg:n:a:t:",["help","gap=","points=","amplitude","period"])
    except getopt.GetoptError, err:
        print str(err)
        usage()
        sys.exit(1)
        
    gap=100000
    points=500
    amplitude=2.0
    period=30.0
            
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
        elif o in ("-t", "--period"):
            period = string.atof(a)
            
    if (amplitude>(MAXSPEED*period/(2*math.pi))):
        print "resulting speed will be too high"
        sys.exit(1)

    
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
    
    print "current azimuth and elevation: %lf %lf, wait for 10 seconds before starting" % (azimuth,elevation)
    
    #get current time
    ctime=getTimeStamp().value
    #take 10 seconds before stating
    ctime=ctime+100000000;
    #convert the gap from microseconds to seconds
    gapS=gap/1000000.0;
    step=0
    
    for index in range(points):
        j=amplitude*math.sin(2*math.pi*step/period)
        ctime=ctime+gap*10;
        step=step+gapS
        tH=EpochHelper(ctime)
        y=elevation+j
        x=azimuth+j
        print "%02d:%02d:%02d.%06d\t\t%lf\t%lf" % (tH.hour(),tH.minute(),tH.second(),tH.microSecond(),x,y)
        try:
            if (index==0):
                component.programTrack(x,y,ctime,1)
            else:
                component.programTrack(x,y,ctime,0)
        except Exception , ex:
            newEx = ClientErrorsImpl.CouldntPerformActionExImpl( exception=ex, create=1 )
            newEx.setAction("programTrack")
            newEx.log(simpleClient.getLogger(),ACSLog.ACS_LOG_ERROR)
            sys.exit(1)
        time.sleep(gapS/2)
        
    if not (compName==""):
        simpleClient.releaseComponent(compName)     
    simpleClient.disconnect()
    

if __name__=="__main__":
   main()
    
    
    
    
    