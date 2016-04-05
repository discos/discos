#! /usr/bin/env python

import logging
import time
import sys
import os
from math import degrees

from Acspy.Clients.SimpleClient import PySimpleClient
from Acspy.Util.ACSCorba import getManager

# Exit in case the process is already running
process = os.path.basename(__file__)
running_processes = os.popen("ps aux").read()
counter = 0
for line in running_processes.split('\n'):
    if 'python' in line and process in line:
        if str(os.getpid()) in line.split():
            continue
        else:
            counter += 1
    if counter > 1:
        print '%s already running, everything is OK' % process
        sys.exit(0)

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
