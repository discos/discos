"""Do not use this module to configure the file: use `onoff_switching.py` instead."""

__credits__ = """Author: Marco Buttu <mbuttu@oa-cagliari.inaf.it>
Licence: GPL 2.0 <http://www.gnu.org/licenses/gpl-2.0.html>"""

import logging
import sys
from models import Target


class Configuration(object):
 
    attributes = {
            'cycles': int,
            'acquisition_time': float,
            'positioning_time': float,
            'observer_name': str,
            'op': str,
            'offset': float,
            'reference': float,
            'simulate': bool,
            'stats': bool,
            'target': Target,
            'datestr_format': str,
            'observer_info': dict
    }
    
    observers_info = {
        'SRT': {
            'name': 'Sardinia Radio Telescope', 
            'latitude': "39:29:34", 
            'longitude': "09:14:42", 
            'elevation': 700
        },

        'Medicina': {
            'name': 'Medicina Radio Telescope',
            'latitude': "44:31:14", 
            'longitude': "11:38:43", 
            'elevation': 41
        }
    } 

    def __init__(self, **kwargs):
        for name, value in kwargs.items():
            setattr(self, name, value)

    def __setattr__(self, name, value):
        if not name in Configuration.attributes:
            raise AttributeError("Attribute name `%s` non allowed." %name)

        expected_type = Configuration.attributes[name]
        if not isinstance(value, expected_type):
            raise TypeError('The value %s is not of type %s' %(value, expected_type.__name__))
        else:
            super(Configuration, self).__setattr__(name, value)

        if name == 'observer_name':
            if value in Configuration.observers_info:
                self.observer_info = Configuration.observers_info[value]
            else:
                observers = ', '.join(Configuration.observers_info)
                raise KeyError('Observer name %s not in (%s)' %(value, observers))

    def __delattr__(self, name):
        if name in Configuration.attributes: 
            raise AttributeError('Read only attribute')
        else:
            super().__delattr__(name)

