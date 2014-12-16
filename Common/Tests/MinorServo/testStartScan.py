import math
import time
import os
from datetime import datetime

import unittest2 # https://pypi.python.org/pypi/unittest2
import Management
import MinorServo
import Antenna
from Acspy.Common.TimeHelper import getTimeStamp
from Acspy.Clients.SimpleClient import PySimpleClient


__author__ = "Marco Buttu <mbuttu@oa-cagliari.inaf.it>"

class StartScanTest(unittest2.TestCase):
    """Test the MinorServoBoss.startScan() behavior"""

    def setUp(self):    
        code = 'KKG' if os.getenv('TARGETSYS') == 'SRT' else 'KKC'
        if hasattr(self, 'boss') and self.boss.isReady():
            pass
        else:
            client = PySimpleClient()
            self.boss = client.getComponent('MINORSERVO/Boss')
            self.boss.setup(code)
            t0 = datetime.now()
            while not self.boss.isReady() and (datetime.now() - t0).seconds < 60*5):
                time.sleep(2)
            self.assertTrue(self.boss.isReady()) # Timeout expired?

        self.assertEqual(self.boss.getActualSetup(), code)

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

        self.boss.setElevationTracking('OFF')
        self.boss.setASConfiguration('OFF')

    def testStartASAP(self):
        """Check the behavior when the scan must start as soon as possible"""
        timeBeforeScan = getTimeStamp().value
        startTime = 0
        self.boss.startScan(startTime, self.scan, self.antennaInfo) 
        p_a = self.boss.getAxesPosition(startTime.value)['SRP_TZ']
        total_time = (self.scan.total_time + 10000000) / 10**7
        print 'starting the scan; time required: %s seconds' %total_time
        time.sleep(total_time) # Wait until the end end of the scan
        p_b = self.boss.getAxesPosition(startTime.value + self.scan.total_time)['SRP_TZ']
        self.assertGreater(startTime, timeBeforeScan)
        self.assertTrue(self.boss.isScanActive())
        self.assertFalse(self.boss.isScanning())
        self.assertAlmostEqual(abs(p_a - p_b), self.scan.range, places=2)

    def testStartAtGivenTime(self):
        """Check the behavior when the scan must start at a given time"""
        timeBeforeScan = getTimeStamp().value
        startTime = getTimeStamp().value + 60*10**7 # Start in a minute from now
        self.boss.startScan(startTime, self.scan, self.antennaInfo) 
        p_a = self.boss.getAxesPosition(startTime.value)['SRP_TZ']
        total_time = (self.scan.total_time + 10000000) / 10**7
        print 'starting the scan; time required: %s seconds' %total_time
        time.sleep(total_time) # Wait until the end end of the scan
        p_b = self.boss.getAxesPosition(startTime.value + self.scan.total_time)['SRP_TZ']
        self.assertGreater(startTime, timeBeforeScan)
        self.assertTrue(self.boss.isScanActive())
        self.assertFalse(self.boss.isScanning())
        self.assertAlmostEqual(abs(p_a - p_b), self.scan.range, places=2)


if __name__ == '__main__':
    unittest2.main()

