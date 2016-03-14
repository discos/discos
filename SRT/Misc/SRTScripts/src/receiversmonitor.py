#! /usr/bin/env python

import logging
import time
from math import degrees

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
    ('ANTENNA/Boss', ('rawAzimuth', 'rawElevation')),
    ('RECEIVERS/SRTLPBandReceiver', common_receiver_properties),
    ('RECEIVERS/SRTKBandMFReceiver', common_receiver_properties),
    ('RECEIVERS/SRT7GHzReceiver', common_receiver_properties),
)


timing = 120  # seconds

while True:
    if getManager():
        try:
            components = []
            client = PySimpleClient()
            for component_name, property_names in mapping:
                try:
                    component = client.getComponent(component_name)
                    components.append((component, property_names))
                except:
                    pass

            for component, property_names in components:
                for pname in property_names:
                    get_property_obj = getattr(component, '_get_%s' % pname)
                    property_obj = get_property_obj()
                    raw_value, completion = property_obj.get_sync()
                    value = degrees(raw_value) if pname.startswith('raw') else raw_value
                    cname = component._get_name()
                    line = '%s.%s' % (cname, pname)
                    logging.info('  %s%e' % (line.ljust(65), value))
                try:
                    component._release()
                except:
                    pass

        except KeyboardInterrupt:
            logging.info('program closed by the user')
            raise
        except Exception, ex:
            pass
        finally:
            try:
                if getManager():
                    client.disconnect()
            except:
                pass

    time.sleep(timing)
