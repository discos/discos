#!/usr/bin/env python

# " $Id: startschedule.py,v 1.1 2009-01-24 14:19:27 a.orlati Exp $ "
# This is a python script that can be used to start a schedule via command line 
#who                                   when           what
#andrea orlati(a.orlati@ira.inaf.it)   24/01/2009     Creation

import sys
import string
import Acspy.Common.Err
import maciErrType
import maciErrTypeImpl
import ClientErrorsImpl
from Acspy.Clients.SimpleClient import PySimpleClient

def main():
    simpleClient = PySimpleClient()
    scheduler = None
    #******************see also default component.......
    schedulerInstance = "MANAGEMENT/Palmiro"
    #******************Now the beamswidth is set manually, in the future there should be somthing that sets it
    #****************** based on the configured frequency     
    try:
        scheduler=simpleClient.getComponent(schedulerInstance)
    except Exception , ex:
        newEx = ClientErrorsImpl.CouldntAccessComponentExImpl( exception=ex, create=1 )
        newEx.setComponentName(schedulerInstance)
        #ACS_LOG_ERROR
        newEx.log()
        simpleClient.disconnect()
        sys.exit(-1)
        
    if len(sys.argv)<3:
        simpleClient.getLogger().logError("Both schedule name and line number must be provided")
        simpleClient.disconnect()
        sys.exit(-1)
    #start the schedule
    try:
        scheduler.startSchedule(sys.argv[1],string.atoi(sys.argv[2]))
    except Exception, ex:
        newEx = ClientErrorsImpl.CouldntPerformActionExImpl( exception=ex, create=1 )
        newEx.setAction("Scheduler/startSchedule")
        newEx.log()
        simpleClient.releaseComponent(schedulerInstance)
        simpleClient.disconnect()
        sys.exit(-1)
    
    simpleClient.releaseComponent(schedulerInstance)
    simpleClient.disconnect()

if __name__=="__main__":
    main()
