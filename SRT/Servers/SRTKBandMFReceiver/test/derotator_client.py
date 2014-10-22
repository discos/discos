#!/usr/bin/env python
# Author: Marco Buttu <mbuttu@oa-cagliari.inaf.it>
# Last modified: Thu Jun 11 10:32:37 CEST 2009

"""
A simple client used to get the properties from the derotator.
This module defines the following class:

- `Client`, a client for the Derotator component

Exception class:

- `CannotGetComponentExImpl`

Functions:

- `dec2deg(angle)`: take a decimal angle and convert it to degree.
- `int2bin(n, bits=13)`: return the binary of an integer n. The default result has 13 bits.
"""

__docformat__ = 'restructuredtext'

from Acspy.Clients.SimpleClient import PySimpleClient
from maciErrTypeImpl import CannotGetComponentExImpl
from decimal import Decimal
import time
import datetime

DEGREE_SYMBOL = unichr(176)

def dec2deg(angle):
    """Convert a angle from decimal degree to degree."""
    angle = str(angle)
    degrees = int(angle.split('.')[0])
    rest = Decimal('0.' + angle.split('.')[1])
    dec_min = str(rest * 60)
    minutes = int(dec_min.split('.')[0])
    rest = Decimal('0.' + dec_min.split('.')[1])
    dec_sec = str(rest * 60)
    seconds = int(dec_sec.split('.')[0])
    rest = int(dec_sec.split('.')[1])

    return "%d%s%d%s%d%s" %(degrees, DEGREE_SYMBOL, minutes, "'", seconds, '"')

def int2bin(n, bits=13):
    """Return the binary of integer n. The default result is on 13 bits.

    >>> int2bin(8, 4)
    '1000'
    >>> int2bin(8, 5)
    '01000'
    """
    return "".join([str((n >> y) & 1) for y in range(bits - 1, -1, -1)])

class Client:
    """
    ACS client that get, set and display the component properies. 
    The position is always an angle.

    Methods:

    - `__init__`: constructor
    - `__del__`: destructor
    - `get_position`: return a device position 
    - `set_position`: set the ICD position
    - `show_position`: show a device position
    - `get_difference`: return the position difference between ICD and sensor
    - `show_difference`: show the position difference between ICD and sensor
    - `show_verbose_status`: show the ICD verbose status
    """

    def __init__(self, component_name, root=None):
        """Initialize the client getting the component object.

        Parameters:

        - `component_name`: a string, the component name we want to get (for instance: Derotator).
        - `root`: a string, the main system name (for instance: MINORSERVO, ANTENNA).

        Exceptions:
        
        - `CannotGetComponentExImpl`
        """
        try:
            self.component_name = component_name
            self.root = root
            self.pyclient = PySimpleClient()
            self.full_name = root + '/' + component_name if root else component_name
            self.component = self.pyclient.getComponent(self.full_name) 
        except CannotGetComponentExImpl, ex:
            self.pyclient.getLogger().logDebug("Component %s unavailable. " %self.full_name + str(ex))
            print "Component %s unavailable:" %self.full_name, ex

    def __del__(self):
        self.pyclient.releaseComponent(self.full_name)
        self.pyclient.disconnect()
    
    def get_position(self, device, type=''):
        """Return the specified position of a device.

        Parameters:

        - `device`: a string, the device name we want to get the position. The derotator is
          a system of two devices: an ICD and a sensor position. The device name can be "icd"
          or "sensor".
        - `type`: a string, the type of position we want. Possible values are: ``Target``, ``zero``
          and ``URS``. Target position is the position we set, URS is the actual position in the
          User's Reference System and zero position is the initial position of device related to
          the user's system reference. The default is an empty type that means a device position 
          propery (the physical device position).

        Rerurn a dictionary of three items:

        - key 'double': the position as a double
        - key 'decimal_degree': the position in decimal degree
        - key 'degree': the position in degree
        """
        if not type:
            try:
                position_obj = getattr(self.component, '_get_' + device + '_position')()
                position, completion = position_obj.get_sync()
            except KeyError:
                print 'This component does not have any "' + device + '" position property'
                raise
        elif type == 'zero':
            # Return the ICD Zero Position in the user's reference system
            cdb_dict = self.pyclient.getCDBElement("alma/" + self.full_name, self.component_name)[0]
            position = float(cdb_dict['ICD_REFERENCE'])
        else:
            try:
                position = getattr(self.component, 'get%s%sPosition' %(device.title(), type))()
            except:
                raise
        
        parts = str(position).split('.')
        try:
            decimal_degree = "%s%s.%s" %(parts[0], DEGREE_SYMBOL, parts[1])
            degree = dec2deg(position)
            position_dic = {'double': position, 'decimal_degree': decimal_degree, 'degree': degree}
        except IndexError:
            degree = decimal_degree = "%s%s" %(parts[0], DEGREE_SYMBOL)
            position_dic = {'double': position, 'decimal_degree': decimal_degree, 'degree': degree}
        return position_dic

    def set_position(self):
        """Set the ICD position property."""
        try:
            position = float(raw_input("# Type a position (degree): "))
            position_obj = getattr(self.component, '_get_icd_position')()
            position_obj.set_sync(position)
        except ValueError:
            print "The input value must be a number"

    def show_position(self, device, type=''):
        """Show a specified position of a device.
        
        Parameters:

        - `device`: a string, the device name we want to get the position. The derotator is
          a system of two devices: an ICD and a sensor position. The device name can be "icd"
          or "sensor".
        - `type`: a string, the type of position we want. Possible values are: ``Target``, ``zero``
          and ``URS``. Target position is the position we set, URS is the actual position in the
          User's Reference System and zero position is the initial position of device related to
          the user's system reference. The default is an empty type that means a device position 
          propery (the physical device position).
        """
        try:
            position = self.get_position(device, type)['double']

            parts = str(position).split('.')
            try:
                decimal_degree = "%s%s.%s" %(parts[0], DEGREE_SYMBOL, parts[1])
                decimal = dec2deg(position)
                print "# %s %s position:\t\t %s | %s" %(device, type, decimal_degree, decimal)
            except IndexError:
                print "# %s %s position:\t\t %s%s" %(device, type, parts[0], DEGREE_SYMBOL)

        except Exception, e:
            print "Some problems getting the ICD %s Position" % type
            print e

    def get_difference(self):
        """
        Compute the position differece between the ICD and the sensor.
        """

        icd_position = self.get_position('icd')['double']
        sensor_position = self.get_position('sensor')['double']

        diff = icd_position - sensor_position
        parts = str(diff).split('.')
        try:
            decimal_degree = "%s%s.%s" %(parts[0], DEGREE_SYMBOL, parts[1])
        except IndexError:
            decimal_degree = "%s%s" %(parts[0], DEGREE_SYMBOL)
        try:
            degree = dec2deg(diff)
        except IndexError:
            degree = "%s%s" %(diff, DEGREE_SYMBOL)

        diff_dic = {'double': diff, 'decimal_degree': decimal_degree, 'degree': degree}
        return diff_dic

    def show_difference(self):
        """Print the position differece between the ICD and the sensor."""
        diff = self.get_difference()
        print "# Diff (ICD - Sensor):\t %s | %s" %(diff['decimal_degree'], diff['degree'])

    def show_verbose_status(self):
        """Print the verbose status pattern property."""

        status_obj = self.component._get_icd_verbose_status()
        status, completion = status_obj.get_sync()
        status=  int2bin(status)[-1::-1]
        print "# VERBOSE STATUS:"
        print "#\tPOLL_ERR:\t%s" %status[0]
        print "#\tWRONG_RFLAG:\t%s" %status[1]
        print "#\tERRCMD:\t\t%s" %status[2]
        print "#\tWRONG_RCODE:\t%s" %status[3] 
        print "#\tIMS_FLTSIG:\t%s" %status[4]
        print "#\tEMS_SIGN:\t%s" %status[5]
        print "#\tWARNING:\t%s" %status[6]
        print "#\tINIT_ERR:\t%s" %status[7]
        print "#\tQS_ACTIVE:\t%s" %status[8]
        print "#\tNOT_OP_ENABLE:\t%s" %status[9]
        print "#\tMOVING:\t\t%s" %status[10]
        print "#\tMOVING_ERR:\t%s" %status[11]
        print "#\tWRONG_POSITION:\t%s" %status[12]
        print "#"
        print "# Pay attention to ICD Status." if '1' in status else "# Fine!"

    def show_summary_status(self):
        """Print the summary status pattern property."""

        status_obj = self.component._get_icd_summary_status()
        status, completion = status_obj.get_sync()
        status=  int2bin(status, 6)[-1::-1]
        print "# SUMMARY STATUS:"
        print "#\tOFF:\t\t\t%s" %status[0]
        print "#\tFAILURE:\t\t%s" %status[1]
        print "#\tCOMMUNICATION ERROR:\t%s" %status[2]
        print "#\tNOT READY:\t\t%s" %status[3] 
        print "#\tSLEWING:\t\t%s" %status[4]
        print "#\tWARNING:\t\t%s" %status[5]
        print "#"
        print "# Pay attention to ICD Status." if '1' in status else "# Fine!"


if __name__ == "__main__":
    try:
        drt = Client("SRTKBandDerotator", root='SRTKBandMFReceiver')

        TITLE1 = "User's Reference System"
        TITLE2 = "Physical Properties"
        TITLE3 = "Status"

        while True:
            print '#' + '-' * 60
            print '\t0. EXIT'
            print
            print "\t%s" % TITLE1
            print "\t" + "="*len(TITLE1)
            print "\t\t1. Get ICD position"
            print "\t\t2. Set ICD position"
            print '\t\t3. Get ICD Target Position'
            print '\t\t4. Get sensor position'
            print '\t\t5. ICD Zero Position'
            print 
            print "\t%s" % TITLE2
            print "\t" + "="*len(TITLE2)
            print '\t\t6. Get ICD position'
            print '\t\t7. Get sensor position'
            print
            print "\t%s" % TITLE3
            print "\t" + "="*len(TITLE3)
            print '\t\t8.  Show ICD verbose status'
            print '\t\t9.  Show ICD summary status'
            print '\t\t10. Show position difference (ICD - Sensor)'
            print
            try:
                choice = int(raw_input("\n\tEnter a choice: "))
            except:
                print "Type an integer"
            print '#' + '-' * 60
            
            choices = {
                    1: lambda : drt.show_position('icd', 'URS'),
                    2: lambda : drt.set_position(),
                    3: lambda : drt.show_position('icd', 'Target'),
                    4: lambda : drt.show_position('sensor', 'URS'),
                    5: lambda : drt.show_position('icd', 'zero'),
                    6: lambda : drt.show_position('icd'), 
                    7: lambda : drt.show_position('sensor'), 
                    8: lambda : drt.show_verbose_status(),
                    9: lambda : drt.show_summary_status(),
                    10: lambda : drt.show_difference(),
            }

            if not choice:
                break
            else:
                if choice in range(1,11):
                    choices[choice]()
                else:
                    print "Type a correct choice"
        del drt
    except:
        del drt

