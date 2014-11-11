#!/usr/bin/env python

# " $Id: medsubr.py,v 1.2 2009-01-29 21:21:21 a.orlati Exp $ "
# This is a python script written as a workaround to command the Medicina subreflector
# it should  disappear in the final release of the system. 
# It connect to the FieldSystem, passing to it the current elevation. In that way the Field System
# can move the subreflector for us 
#who                                   when           what
#andrea orlati(a.orlati@ira.inaf.it)   26/01/2009     Creation

import socket
import ClientErrorsImpl
import time
import sys
from Acspy.Clients.SimpleClient import PySimpleClient


def main():
    simpleClient = PySimpleClient()
    antennaBoss = None
    antennaBossInstance = "ANTENNA/Boss"
    Address = '192.167.189.62' 
    Port= 5002
    r2d=57.295779513082320876798154814105170332405472466564
    
    elProp=None
    azProp=None
    
    #get the antennaBoss component
    try:
        antennaBoss=simpleClient.getComponent(antennaBossInstance)
    except Exception , ex:
        simpleClient.getLogger().logError("Error in retriving antennaBoss component");
        simpleClient.disconnect()
        sys.exit(-1)
    
    #socket initialization and connection
    try: 
        mySocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        mySocket.connect((Address,Port))
    except socket.error, (value,message):
        simpleClient.getLogger().logError("socket error: %s" % message)
        simpleClient.releaseComponent(antennaBossInstance)
        simpleClient.disconnect()
        sys.exit(-1)
        
    try:
        elProp=antennaBoss._get_rawElevation()
        azProp=antennaBoss._get_rawAzimuth()
    except Exception, ex:
        simpleClient.getLogger().logError("Can't read antennaBoss properties")
        simpleClient.releaseComponent(antennaBossInstance)
        mySlocket.close()
        simpleClient.disconnect()
        sys.exit(-1)

    simpleClient.getLogger().logInfo("Subreflector tracking is enabled")
    
    az=0.0
    el=0.0
    
    try:
        while 1:
            az=azProp.get_sync()[0]
            el=elProp.get_sync()[0]
            
            az=az*r2d
            el=el*r2d
            
            sendBuffer="current=0,0,%lf,%lf\n" % (az,el)
            try:
                mySocket.send(sendBuffer)
            except socket.error, (value,message):
                simpleClient.getLogger().logError("Send error: %s" % message)
                simpleClient.releaseComponent(antennaBossInstance)
                mySocket.close()
                simpleClient.disconnect()
                sys.exit(-1)
            simpleClient.getLogger().logDebug("Buffer sent")
            time.sleep(5.0)
            
    except KeyboardInterrupt:
        simpleClient.getLogger().logInfo("Subreflector tracking is disabled")

    simpleClient.releaseComponent(antennaBossInstance)
    mySocket.close()
    simpleClient.disconnect()
    
    return 0
    
    
    
if __name__=="__main__":
    main() 