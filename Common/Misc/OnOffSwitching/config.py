"""Do not use this module to configure the file: use `onoff_switching.py` instead."""

__credits__ = """Author: Marco Buttu <mbuttu@oa-cagliari.inaf.it>
Licence: GPL 2.0 <http://www.gnu.org/licenses/gpl-2.0.html>"""

import logging
import sys
from models import Target


class Configuration(object):
 
    observers_info = {
        'SRT': {
            'name': 'Sardinia Radio Telescope', 
            'latitude': "39:29:34.93742", 
            'longitude': "09:14:42.5764", 
            'elevation': 700
        },

        'Medicina': {
            'name': 'Medicina Radio Telescope',
            'latitude': "44:31:14", 
            'longitude': "11:38:43", 
            'elevation': 41
        }
    } 

    def _checkPositive(value, attr_name):
        if not value > 0:
            raise AttributeError('%s must be positive' %attr_name)

    def _checkRange(value, attr_name, min_value, max_value):
        if not min_value <= value <= max_value:
            raise AttributeError('%s must be in the range %s' %(attr_name, (min_value, max_value)))

    def _acquisition_timeCheck(value):
        if not value > 10 or value % 10:
            raise AttributeError('The acquisition_time value must be a multiple of 10')

    def _observer_nameCheck(value):
        if value not in Configuration.observers_info:
            observers = ', '.join(Configuration.observers_info)
            raise AttributeError('Observer name %s not in (%s)' %(value, observers))

    def _checkFreq(value, attr_name):
        if value < 125:
            raise AttributeError('Lower and upper frequencies must be greather than 125 MHz')

        if attr_name == 'lower_freq':
            Configuration._lower_freq = value
            if hasattr(Configuration, '_upper_freq') and value >= Configuration._upper_freq:
                raise AttributeError('lower_freq >= upper_freq')
        elif attr_name == 'upper_freq':
            Configuration._upper_freq = value
            if hasattr(Configuration, '_lower_freq') and Configuration._lower_freq >= value:
                raise AttributeError('lower_freq >= upper_freq')
        else:
            raise AttributeError('%s is not a valid attribute name' %attr_name)
        
    attributes = {
            # Attribute name: (type, checkrule)
            'cycles': (int, _checkPositive, ('cycles',)),
            'acquisition_time': (int, _acquisition_timeCheck, ()),
            'positioning_time': (float, _checkPositive, ('positioning_time',)),
            'observer_name': (str, _observer_nameCheck, ()),
            'op': (str, None, ()),
            'offset': (float, None, ()),
            'reference': (float, _checkRange, ('reference', 0, 1)),
            'simulate': (bool, None, ()),
            'stats': (bool, None, ()),
            'target': (Target, None, ()),
            'datestr_format': (str, None, ()),
            'observer_info': (dict, None, ()),
            'horizons_file_name': (str, None, ()),
            'lab_freq': (float, _checkPositive, ('lab_freq',)),
            'lower_freq': (float, _checkFreq, ('lower_freq',)),
            'upper_freq': (float, _checkFreq, ('upper_freq',)),
            'calibrations': (int, _checkRange, ('calibrations', 0, 50)),
            'datadir': (str, None, ())
    }
   
    def __init__(self, **kwargs):
        for name, value in kwargs.items():
            setattr(self, name, value)

    def __setattr__(self, name, value):
        if not name in Configuration.attributes:
            raise AttributeError("Attribute name `%s` non allowed." %name)

        expected_type, checkrule, args = Configuration.attributes[name]
        if not isinstance(value, expected_type):
            raise TypeError('The `%s` value must be of type %s' %(name, expected_type.__name__))
        if callable(checkrule):
            checkrule(value, *args)

        super(Configuration, self).__setattr__(name, value)
        if name == 'observer_name':
                self.observer_info = Configuration.observers_info[value]

    def __delattr__(self, name):
        if name in Configuration.attributes: 
            raise AttributeError('Read only attribute')
        else:
            super().__delattr__(name)

