#!/usr/bin/env python

# " $Id: testTracking.py,v 1.2 2011-06-03 18:02:49 a.orlati Exp $ "
# This is a python test program that loads the Mount with a table of time tagged program track points.
# in order to test the mount capabilities to manage such an operating mode.
# the generated tracking curve is a sine function for both axis.
#who                                   when           what
#Andrea Orlati(a.orlati@ira.inaf.it)   28/07/2015     Creation

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
    print "testTracking [-h|--help] [-g|--gap=] [-n|--points=] [-a|--amplitude=] [-t|--period=] [-o|off=]"
    print ""
    print "[-h|--help]        displays this help"
    print "[-g|--gap=]        allows to give the gap, in second, between each cycle [default 300]"
    print "[-c|--cycles=]     allows to give how many cycles will be performed [default 50]"
  
    
def main():
    
    MAXSPEED=0.5
    
    try:
        opts, args = getopt.getopt(sys.argv[1:],"hg:c:",["help","gap=","cycles="])
    except getopt.GetoptError, err:
        print str(err)
        usage()
        sys.exit(1)
        
    gap=300
    cycles=50
            
    for o, a in opts:
        if o in ("-h", "--help"):
            usage()
            sys.exit()
        elif o in ("-g", "--gap"):
            gap = string.atoi(a)
        elif o in ("-c", "--cycles"):
            cycles = string.atoi(a)
    
    #get the link to the mountcomponent
    simpleClient = PySimpleClient()
    compName=""
    compType = "IDL:alma/Antenna/MedicinaMount:1.0"
    try:
        component=simpleClient.getDefaultComponent(compType)
        compName=component._get_name()
    except Exception , ex:
        newEx = ClientErrorsImpl.CouldntAccessComponentExImpl( exception=ex, create=1 )
        newEx.setComponentName(compType)
        newEx.log(simpleClient.getLogger(),ACSLog.ACS_LOG_ERROR)
        sys.exit(1)  
    
    azimuth=175.0
    span=10.0
    elevation=55.0
    points=200
    step=span/points    
    gapS=1000000
   
    for index in range(cycles):
    	#get current time
    	ctime=getTimeStamp().value
    	ctH=EpochHelper(ctime)
    	print "current time: %02d:%02d:%02d.%06d\t" % (ctH.hour(),ctH.minute(),ctH.second(),ctH.microSecond())
    	for p in range(points):
    		x=azimuth+step*p
    		y=elevation
    		ctime=ctime+gapS*p
        	tH=EpochHelper(ctime)
        	print "%02d:%02d:%02d.%06d\t\t%lf\t%lf" % (tH.hour(),tH.minute(),tH.second(),tH.microSecond(),x,y)
        	try:
            	if (p==0):
                	component.programTrack(x,y,ctime,1)
            	else:
                	component.programTrack(x,y,ctime,0)
        	except Exception , ex:
            	newEx = ClientErrorsImpl.CouldntPerformActionExImpl( exception=ex, create=1 )
            	newEx.setAction("programTrack")
            	newEx.log(simpleClient.getLogger(),ACSLog.ACS_LOG_ERROR)
            	sys.exit(1)
      		time.sleep(gap)
        
    if not (compName==""):
        simpleClient.releaseComponent(compName)     
    simpleClient.disconnect()
    

if __name__=="__main__":
   main()
    
    
    
    
    