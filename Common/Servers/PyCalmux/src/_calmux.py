#!/usr/bin/env python

# This is a python script that can be used to configure the noise calibration
# multiplexer
# who                                         when           what
# Giuseppe Carboni(giuseppe.carboni@inaf.it)  03/04/2019     Creation

from Acspy.Clients.SimpleClient import PySimpleClient
import ACSLog
import maciErrType
import maciErrTypeImpl
import ClientErrorsImpl
import ManagementErrorsImpl
import sys
import os
from IRAPy import logger, userLogger
from SimpleParserPy import add_user_message

def main():
    compName = 'BACKENDS/PyCalmux'

    simpleClient = PySimpleClient()

    try:
        component = simpleClient.getComponent(compName)
    except Exception as ex:
        newEx = ClientErrorsImpl.CouldntAccessComponentExImpl(exception=ex, create=1)
        newEx.setComponentName(compName)
        add_user_message(newEx, 'PyCalmux device not ready or not properly configured')
        userLogger.logException(newEx)
        simpleClient.disconnect()
        sys.exit(1)

    try:
        inputs = component.setup(sys.argv[1])
    except Exception as ex:
        newEx = ClientErrorsImpl.CouldntPerformActionExImpl(exception=ex, create=1)
        newEx.setReason('PyCalmux configuration')
        add_user_message(newEx, 'Unable to configure the PyCalmux device')
        userLogger.logException(newEx)
        simpleClient.disconnect()
        sys.exit(1)


if __name__ == "__main__":
    main()    
