#!/usr/bin/env python

# This is a python script that can be used to send OR7 VBrain an emergency and
# alarms reset command
# who                                         when           what
# Giuseppe Carboni(giuseppe.carboni@inaf.it)  21/05/2024     Creation

import sys
import time
import requests
import urllib3
import cdbErrType
import ComponentErrorsImpl
import ClientErrorsImpl
from IRAPy import logger
from SimpleParserPy import add_user_message
from Acspy.Clients.SimpleClient import PySimpleClient
from Acspy.Util import ACSCorba
from MinorServo import ERROR_NO_ERROR


def get_cdb_args():
    try:
        path = 'alma/MINORSERVO/VBrain'
        dal = ACSCorba.cdb()
        dao = dal.get_DAO_Servant(path)
        field = 'Protocol'
        protocol = dao.get_field_data(field).strip()
        field = 'IPAddress'
        address = dao.get_field_data(field).strip()
        field = 'Port'
        port = dao.get_field_data(field).strip()
    except cdbErrType.CDBRecordDoesNotExistEx:
        reason = f'CDB record {path} does not exist'
        logger.logError(reason)
        exc = ComponentErrorsImpl.CouldntGetAttributeExImpl()
        exc.setData('Reason', reason)
        raise exc
    except cdbErrType.CDBFieldDoesNotExistEx:
        reason = f'CDB field {field} does not exist'
        logger.logError(reasong)
        exc = ComponentErrorsImpl.ValidationErrorExImpl()
        exc.setReason(reason)
        raise exc
    return protocol, address, port


def send_requests(protocol, address, port):
    url = \
    f'{protocol}://{address}:{port}/Exporting/json/ExecuteCommand?name'
    emergency = f'{url}=INAF_SRT_OR7_EMG_RESET_CMD'
    alarms = f'{url}=INAF_SRT_OR7_RESET_CMD'
    urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)
    try:
        # We just try to send the commands, if it fails, we do nothing, it
        # means there is a problem reaching the VBrain server so we cannot
        # reset the status anyway
        response = requests.get(emergency, verify=False, timeout=3)
        time.sleep(3)
        response = requests.get(alarms, verify=False, timeout=3)
    except:
        pass


if __name__ == "__main__":
    # Retrieve the CDB attributes
    protocol, address, port = get_cdb_args()

    # Retrieve the component
    compName = 'MINORSERVO/Boss'

    simpleClient = PySimpleClient()

    try:
        component = simpleClient.getComponent(compName)
        # Check if we have an error, if not we just exit immediately
        error_code = component.error_code.get_sync()[0]
        if error_code == ERROR_NO_ERROR:
            sys.exit(0)
    except Exception as ex:
        newEx = ClientErrorsImpl.CouldntAccessComponentExImpl(exception=ex, create=1)
        newEx.setComponentName(compName)
        add_user_message(newEx, 'MinorServoBoss not ready or not properly configured')
        simpleClient.disconnect()
        sys.exit(1)

    # Send the reset commands to VBrain
    send_requests(protocol, address, port)

    # Send the reset command to the component
    component.reset()
