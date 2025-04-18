#!/usr/bin/env python

# " $Id: setupkkc.py,v 1.4 2010-05-24 09:32:01 a.orlati Exp $ "
# This is a python script that can be used to configure the Medicina radiotelescope in order to use
# the 22Ghz Multifeed receiver 
# @todo The pointing model should be got as default component
#who                                   when           what
#andrea orlati(a.orlati@ira.inaf.it)   24/01/2009     Creation

import sys
import Acspy.Common.Err
import maciErrType
import maciErrTypeImpl
import ClientErrorsImpl
from Acspy.Clients.SimpleClient import PySimpleClient

def main():
    simpleClient = PySimpleClient()
    antennaBoss = None
    receiversBoss = None
    totalPowerBackend = None
    antennaBossType = "IDL:alma/Antenna/AntennaBoss:1.0"
    receiversBossType = "IDL:alma/Receivers/ReceiversBoss:1.0"
    totalPowerBackendName = "BACKENDS/TotalPower"
    #Now the beamswidth is set manually, in the future there should be somthing that sets it
    # based on the configured frequency
    HPBW = 0.000523599
    REC_CODE = "KKC"
    try:
        antennaBoss=simpleClient.getDefaultComponent(antennaBossType)
    except Exception as ex:
        newEx = ClientErrorsImpl.CouldntAccessComponentExImpl( exception=ex, create=1 )
        newEx.setComponentName(antennaBossType)
        #ACS_LOG_ERROR
        newEx.log() 
        simpleClient.disconnect()
        sys.exit(-1)
    #setup the telescope: prepare it for movement
    try:
        antennaBoss.setup(REC_CODE);
    except Exception as ex:
        newEx = ClientErrorsImpl.CouldntPerformActionExImpl( exception=ex, create=1 )
        newEx.setAction("AntennaBoss/setup()")
        #ACS_LOG_ERROR
        newEx.log()
        simpleClient.releaseComponent(antennaBoss._get_name())
        simpleClient.disconnect()
        sys.exit(-1)

    #setup the receiver
    try:
        receiversBoss=simpleClient.getDefaultComponent(receiversBossType)
    except Exception as ex:
        newEx = ClientErrorsImpl.CouldntAccessComponentExImpl( exception=ex, create=1 )
        newEx.setComponentName(receiversBossType)
        #ACS_LOG_ERROR
        newEx.log() 
        simpleClient.disconnect()
        sys.exit(-1)
    try:
        receiversBoss.setup(REC_CODE);
    except Exception as ex:
        newEx = ClientErrorsImpl.CouldntPerformActionExImpl( exception=ex, create=1 )
        newEx.setAction("ReceiversBoss/setup()")
        #ACS_LOG_ERROR
        newEx.log()
        simpleClient.releaseComponent(antennaBoss._get_name())
        simpleClient.disconnect()
        sys.exit(-1)
     
    try:
        totalPowerBackend= simpleClient.getComponent(totalPowerBackendName)
    except Exception as ex:
        newEx = ClientErrorsImpl.CouldntAccessComponentExImpl( exception=ex, create=1 )
        newEx.setComponentName(receiversBossType)
        #ACS_LOG_ERROR
        newEx.log() 
        simpleClient.disconnect()
        sys.exit(-1)
    try:
        totalPowerBackend.initialize(REC_CODE);
    except Exception as ex:
        newEx = ClientErrorsImpl.CouldntPerformActionExImpl( exception=ex, create=1 )
        newEx.setAction("TotalPower/initialize()")
        #ACS_LOG_ERROR
        newEx.log()
        simpleClient.releaseComponent(antennaBoss._get_name())
        simpleClient.disconnect()
        sys.exit(-1)
     
    try:
        antennaBoss.setHPBW(HPBW)
    except Exception as ex:
        newEx = ClientErrorsImpl.CouldntPerformActionExImpl( exception=ex, create=1 )
        newEx.setAction("AntennaBoss/setHPBW")
        #ACS_LOG_ERROR
        newEx.log()
        simpleClient.releaseComponent(antennaBoss._get_name())
        simpleClient.disconnect()
        sys.exit(-1)
    simpleClient.releaseComponent(antennaBoss._get_name())
        
    simpleClient.getLogger().logNotice("setupkkc done")
    simpleClient.disconnect()

if __name__=="__main__":
    main()
