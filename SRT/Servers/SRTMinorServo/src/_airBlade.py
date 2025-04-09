#!/usr/bin/env python

# This is a python script that can be used to set the air blade status
# who                                         when           what
# Giuseppe Carboni(giuseppe.carboni@inaf.it)  18/05/2024     Creation

from Acspy.Clients.SimpleClient import PySimpleClient
import ACSLog
import maciErrType
import maciErrTypeImpl
import ClientErrorsImpl
import MinorServoErrors
import ManagementErrorsImpl
import sys
from SimpleParserPy import add_user_message

def main():
    compName = 'MINORSERVO/Boss'

    simpleClient = PySimpleClient()

    try:
        component = simpleClient.getComponent(compName)
    except Exception as ex:
        newEx = ClientErrorsImpl.CouldntAccessComponentExImpl(exception=ex, create=1)
        newEx.setComponentName(compName)
        add_user_message(newEx, 'MinorServoBoss not ready or not properly configured')
        simpleClient.disconnect()
        sys.exit(1)

    try:
        inputs = component.setGregorianAirBladeStatus(sys.argv[1])
    except MinorServoErrors.MinorServoErrorsEx as ex:
        newEx = ClientErrorsImpl.CouldntPerformActionExImpl(exception=ex, create=1)
        newEx.setReason('MinorServoBoss gregorian air blade status')
        add_user_message(newEx, 'Unable to set the gregorian air blade status')
        simpleClient.disconnect()
        sys.exit(1)


if __name__ == "__main__":
    main()    
