#!/usr/bin/env python

# This is a python script that can be used to configure the filters of the SRT
# temporary IF distributor. 
#who                                   when           what
#andrea orlati(andrea.orlati@inaf.it)  22/06/2018     Creation

from Acspy.Clients.SimpleClient import PySimpleClient
import ACSLog
import maciErrType
import maciErrTypeImpl
import ClientErrorsImpl
import ManagementErrorsImpl
import sys
import os
from IRAPy import logger,userLogger
from SimpleParserPy import add_user_message

def main():


    compName = "RECEIVERS/SRTIFDistributor"
        
    simpleClient = PySimpleClient()
    #check we are at the SRT
    station=os.environ['STATION']
    
    if station!="SRT":
		newEx = ManagementErrorsImpl.UnsupportedOperationExImpl()
		add_user_message(newEx,"The command is not available or it is not supported")
		userLogger.logExcpetion(newEx)
		simpleClient.disconnect()
		sys.exit(1)
    
    try:
        component=simpleClient.getComponent(compName)
    except Exception , ex:
        newEx = ClientErrorsImpl.CouldntAccessComponentExImpl( exception=ex, create=1 )
        newEx.setComponentName(compName)
        add_user_message(newEx,"IFD device not ready or not properly configured")
        userLogger.logException(newEx)
        simpleClient.disconnect()
        sys.exit(1)
    
    try:
        inputs=component.setup(sys.argv[1])
    except Exception, ex:
        newEx = ClientErrorsImpl.CouldntPerformActionExImpl( exception=ex, create=1 )
        newEx.setReason("IF distributor configuration")
        add_user_message(newEx,"Unable to configure the IFD device")
        userLogger.logException(newEx)
        simpleClient.disconnect()
        sys.exit(1)
        
    
if __name__=="__main__":
   main()    