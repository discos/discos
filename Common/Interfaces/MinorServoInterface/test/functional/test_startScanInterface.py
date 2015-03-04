from __future__ import with_statement

import os
import math
import unittest2

import MinorServo
import Management
import Antenna

from PyMinorServoTest import simunittest
from Acspy.Clients.SimpleClient import PySimpleClient
from MinorServoErrors import MinorServoErrorsEx
from Acspy.Common.TimeHelper import getTimeStamp

__author__ = "Marco Buttu <mbuttu@oa-cagliari.inaf.it>"

class TestStartScanInterface(unittest2.TestCase):
    """startScan() must raise the expected exceptions"""

    def setUp(self):
        client = PySimpleClient()
        self.boss = client.getComponent('MINORSERVO/Boss')

        self.scan = MinorServo.MinorServoScan(
            range=50,
            total_time=500000000, # 50 seconds
            axis_code='SRP_TZ',
            is_empty_scan=False)

        self.antennaInfo = Antenna.TRunTimeParameters(
            targetName='dummy',
            azimuth=math.pi,
            elevation=math.pi/8,
            startEpoch=getTimeStamp().value + 100000000,
            onTheFly=False,
            slewingTime=100000000,
            section=Antenna.ANT_SOUTH,
            axis=Management.MNG_TRACK,
            timeToStop=0)

    def test_not_ready(self):
        """Raise a MinorServoErrorsEx in case the system is not ready"""
        with self.assertRaises(MinorServoErrorsEx):
            self.boss.startScan(0, self.scan, self.antennaInfo) 


if __name__ == '__main__':
    if 'Configuration' in os.getenv('ACS_CDB'):
        unittest2.main() # Real test using the antenna CDB
    else:
        simunittest.run(TestStartScanInterface)
