#! /usr/bin/env python

import logging
import time

from Acspy.Clients.SimpleClient import PySimpleClient
from Acspy.Util.ACSCorba import getManager

logging.basicConfig(
    filename='/home/gavino/logs/receivers.log', 
    format='%(asctime)s\t%(message)s')

receivers = ['SRTLPBandReceiver', 'SRTKBandMFReceiver', 'SRT7GHzReceiver']


while True:
    if getManager():
        try:
            components = []
            client = PySimpleClient()
            for receiver in receivers:
                component = client.getComponent('RECEIVERS/' + receiver)
                components.append(component)

        except Exception:
            pass
        except KeyboardInterrupt:
            logging.info('program closed by the user')
            raise
        try:
            for component in components:
                temp_obj = component._get_cryoTemperatureLNA()
                value, comp = temp_obj.get_sync()
                name = component._get_name().split('/')[-1]
                logging.info('  %s%.2f' % (name.ljust(22), value))
        except:
            pass

    time.sleep(60)
