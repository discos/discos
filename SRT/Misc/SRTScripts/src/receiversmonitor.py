#! /usr/bin/env python

import datetime
import logging
import time

from Acspy.Clients.SimpleClient import PySimpleClient
from Acspy.Util.ACSCorba import getManager

logging.basicConfig(
    filename='/home/gavino/logs/receivers.log',
    format='%(asctime)s\t%(message)s')

common_receiver_properties = (
    'cryoTemperatureCoolHead', 
    'cryoTemperatureCoolHeadWindow', 
    'cryoTemperatureLNA', 
    'cryoTemperatureLNAWindow', 
    'environmentTemperature',
    'vacuum')

mapping = (
    ('ANTENNA/Boss', ('rawAzimut', 'rawElevation')),
    ('RECEIVERS/SRTLPBandReceiver', common_receiver_properties),
    ('RECEIVERS/SRTKBandMFReceiver', common_receiver_properties),
    ('RECEIVERS/SRT7GHzReceiver', common_receiver_properties),
)


timing = 180 #seconds

while True:
    if getManager():
        try:
            for component_name, property_names in mapping:
                components = []
                client = PySimpleClient()
                try:
                    component = client.getComponent(component_name)
                    components.append((component, property_names))
                except:
                    pass

                for component, property_names in components:
                    for pname in property_names:
                        get_property_obj = getattr(component, '_get_%s()' % pname)
                        property_obj = get_property_obj()
                        value, completion = property_obj.get_sync()
                        cname = component._get_name().split('/')[-1]
                        t = datetime.datetime.now()
                        line = '%s -> %s.%s' % (t, cname, pname)
                        logging.info('  %s%.2f' % (final_name.ljust(52), value))

        except KeyboardInterrupt:
            logging.info('program closed by the user')
            raise
        except:
            pass
        finally:
            try:
                if getManager():
                    client.disconnect()
            except:
                logging.error('can not disconnect the client')

    time.sleep(timing)
