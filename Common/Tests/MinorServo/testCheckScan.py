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


class CheckScanTest(unittest2.TestCase):
    """Test the MinorServoBoss.checkScan() behavior"""

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
            self.assertEqual(self.boss.isReady(), True) # Timeout expired?

        self.assertEqual(self.boss.getActualSetup(), code)

        self.scan = MinorServo.MinorServoScan(
            range=50,
            total_time=500000000,
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
        center = self.boss.getAxesPosition(getTimeStamp().value)['SRP_TZ']
        startTime = 0
        msInfo = MinorServo.TRunTimeParameters
        res = self.boss.checkScan(startTime, self.scan, self.antennaInfo, msInfo)

        self.assertEqual(res, True)
        # Cannot know the startEpoch time...
        self.assertGreater(msInfo.startEpoch, getTimeStamp.now().value)
        self.assertEqual(msInfo.onTheFly, True)
        self.assertEqual(msInfo.centerScan, center)
        self.assertEqual(msInfo.scanAxis, 'SRP_TZ')
        # Cannot know the timeToStop time...
        self.assertGreater(msInfo.timeToStop, getTimeStamp.now().value+50000000)

    def testStartAtGivenTime(self):
        """Check the behavior when the scan must start at a given time"""
        center = self.boss.getAxesPosition(getTimeStamp().value)['SRP_TZ']
        startTime = getTimeStamp().value + 60 # Start in a minute from now
        msInfo = MinorServo.TRunTimeParameters
        res = self.boss.checkScan(startTime, self.scan, self.antennaInfo, msInfo)

        self.assertEqual(res, True)
        self.assertEqual(msInfo.startEpoch, startTime)
        self.assertEqual(msInfo.onTheFly, True)
        self.assertEqual(msInfo.centerScan, center)
        self.assertEqual(msInfo.scanAxis, 'SRP_TZ')
        # Cannot know the timeToStop time...
        self.assertGreater(msInfo.timeToStop, getTimeStamp.now().value+50000000)

    def testTooFast(self):
        """The result must be False when the total time is not enough"""
        center = self.boss.getAxesPosition(getTimeStamp().value)['SRP_TZ']
        startTime = getTimeStamp().value + 60 # Start in a minute from now
        msInfo = MinorServo.TRunTimeParameters
        self.scan.total_time = 500000 # 50 ms
        res = self.boss.checkScan(startTime, self.scan, self.antennaInfo, msInfo)
        self.assertEqual(res, False)

    def testOutOfRange(self):
        """The result must be False in case of scan out of range"""
        center = self.boss.getAxesPosition(getTimeStamp().value)['SRP_TZ']
        startTime = getTimeStamp().value + 60 # Start in a minute from now
        msInfo = MinorServo.TRunTimeParameters
        self.scan.range = 5000 # 5 meters...
        res = self.boss.checkScan(startTime, self.scan, self.antennaInfo, msInfo)
        self.assertEqual(res, False) # does it currently throw an exception?

    def testTooCloseToNow(self):
        """The result must be False when the starting time is too close to now"""
        center = self.boss.getAxesPosition(getTimeStamp().value)['SRP_TZ']
        startTime = getTimeStamp().value + 2 # Start in 2 seconds from now
        msInfo = MinorServo.TRunTimeParameters
        res = self.boss.checkScan(startTime, self.scan, self.antennaInfo, msInfo)
        self.assertEqual(res, False) # does it currently throw an exception?

    def testStartASAPEmptyScan(self):
        """An empty scan when that must start as soon as possible"""
        self.scan = MinorServo.MinorServoScan(
            range=0,
            total_time=0,
            axis_code='',
            is_empty_scan=True)
        center = self.boss.getAxesPosition(getTimeStamp().value)['SRP_TZ']
        startTime = 0
        msInfo = MinorServo.TRunTimeParameters
        res = self.boss.checkScan(startTime, self.scan, self.antennaInfo, msInfo)

        self.assertEqual(res, True)
        # Cannot know the startEpoch time...
        self.assertGreater(msInfo.startEpoch, getTimeStamp.now().value)
        self.assertEqual(msInfo.onTheFly, False)
        self.assertEqual(msInfo.centerScan, center)
        self.assertEqual(msInfo.scanAxis, '')
        self.assertEqual(msInfo.timeToStop, 0)

    def testStartAtGivenTimeEmptyScan(self):
        """An empty scan that must start at a given time"""
        self.scan = MinorServo.MinorServoScan(
            range=0,
            total_time=0,
            axis_code='',
            is_empty_scan=True)
        center = self.boss.getAxesPosition(getTimeStamp().value)['SRP_TZ']
        startTime = getTimeStamp().value + 60 # Start in a minute from now
        msInfo = MinorServo.TRunTimeParameters
        res = self.boss.checkScan(startTime, self.scan, self.antennaInfo, msInfo)

        self.assertEqual(res, True)
        self.assertEqual(msInfo.startEpoch, startTime)
        self.assertEqual(msInfo.onTheFly, False)
        self.assertEqual(msInfo.centerScan, center)
        self.assertEqual(msInfo.scanAxis, '')
        self.assertGreater(msInfo.timeToStop, 0)


if __name__ == '__main__':
    unittest2.main()

