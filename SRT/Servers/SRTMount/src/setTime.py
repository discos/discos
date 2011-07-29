#!/usr/bin/env python

# " $Id: setTime.py,v 1.1 2011-06-03 18:02:49 a.orlati Exp $ "
# This is a python test program that synchs the ACu through the setTime command 
#who                                   when           what
#Andrea Orlati(a.orlati@ira.inaf.it)   03/06/2011     Creation

import getopt, sys
import Acspy.Common.Err
import maciErrType
import maciErrTypeImpl
import ClientErrorsImpl
import ACSLog
from Acspy.Clients.SimpleClient import PySimpleClient
from Acspy.Common.TimeHelper import getTimeStamp
from Acspy.Common.EpochHelper import EpochHelper
import string
import math
import time


def main():
    
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
       
    #get current time
    ctime=getTimeStamp().value
    try:
        component.setTime(ctime)
    except Exception , ex:
        newEx = ClientErrorsImpl.CouldntPerformActionExImpl( exception=ex, create=1 )
        newEx.setAction("setTime")
        newEx.log(simpleClient.getLogger(),ACSLog.ACS_LOG_ERROR)
        sys.exit(1)
    
    print "time was successfully set"
        
    if not (compName==""):
        simpleClient.releaseComponent(compName)     
    simpleClient.disconnect()
    

if __name__=="__main__":
   main()
    
    
    
    
    