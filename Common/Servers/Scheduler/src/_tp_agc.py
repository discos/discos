#!/usr/bin/env python

# This is a python program that can be used to adjust the levels of the backend inputs to the optimal value 
#who                                   when           what
#andrea orlati(a.orlati@ira.inaf.it)   14/05/2013     Creation

from Acspy.Clients.SimpleClient import PySimpleClient
import ACSLog
import maciErrType
import maciErrTypeImpl
import ClientErrorsImpl
import sys

def main():

    inputs = 0;
    attenuation= []
    compName = sys.argv[1]
        
    simpleClient = PySimpleClient()
    
    try:
        edge = int (sys.argv[2])
    except Exception , ex:
        newEx = ClientErrorsImpl.InvalidArgumentExImpl( exception=ex, create=1 )
        newEx.log(simpleClient.getLogger(),ACSLog.ACS_LOG_ERROR)
        sys.exit(1)
    
    try:
        component=simpleClient.getComponent(compName)
    except Exception , ex:
        newEx = ClientErrorsImpl.CouldntAccessComponentExImpl( exception=ex, create=1 )
        newEx.setComponentName(compName)
        newEx.log(simpleClient.getLogger(),ACSLog.ACS_LOG_ERROR)
        sys.exit(1)
    
    try:
        inputs=component._get_inputsNumber().get_sync()[0]
    except Exception, ex:
        newEx = ClientErrorsImpl.CouldntAccessPropertyExImpl( exception=ex, create=1 )
        newEx.setPropertyName("inputsNumber")
        newEx.log(simpleClient.getLogger(),ACSLog.ACS_LOG_ERROR)
        sys.exit(1)

    try:
        attenuation=component._get_attenuation().get_sync()[0]
    except Exception, ex:
        newEx = ClientErrorsImpl.CouldntAccessPropertyExImpl( exception=ex, create=1 )
        newEx.setPropertyName("attenuation")
        newEx.log(simpleClient.getLogger(),ACSLog.ACS_LOG_ERROR)
        sys.exit(1)
        
    try:
        tpi=component.getTpi()
    except Exception, ex:
        newEx = ClientErrorsImpl.CouldntPerformActionExImpl( exception=ex, create=1 )
        newEx.setAction("getTpi")
        newEx.setReason("Could not get TPI");
        newEx.log(simpleClient.getLogger(),ACSLog.ACS_LOG_ERROR)
        sys.exit(1)
        
    edgeL=edge/1.27;
    edgeH=edge*1.27;
    for i in range(0,inputs):
        while tpi[i]<edgeL:
            attenuation[i]=attenuation[1]-1
            tpi[i]=tpi[i]*1.27
        while tpi[i]>edgeH:
            attenuation[i]=attenuation[i]+1
            tpi[i]=tpi[i]/1.27
        try:
            component.setAttenuation(i,attenuation[i])
        except Exception, ex:
            newEx = ClientErrorsImpl.CouldntPerformActionExImpl( exception=ex, create=1 )
            newEx.setAction("setAttenuation")
            newEx.setReason("Could change attenuation level");
            newEx.log(simpleClient.getLogger(),ACSLog.ACS_LOG_ERROR)
            sys.exit(1)
            
    
if __name__=="__main__":
   main()    