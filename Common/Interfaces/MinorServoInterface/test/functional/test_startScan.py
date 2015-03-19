from __future__ import with_statement
import random 
import math
import time
import os
from datetime import datetime

import unittest2 # https://pypi.python.org/pypi/unittest2
import Management
import MinorServo
import Antenna

from MinorServoErrors import MinorServoErrorsEx
from Acspy.Common.TimeHelper import getTimeStamp
from Acspy.Clients.SimpleClient import PySimpleClient
from Acspy.Util import ACSCorba

from PyMinorServoTest import simunittest


__author__ = "Marco Buttu <mbuttu@oa-cagliari.inaf.it>"

class StartScanBaseTest(unittest2.TestCase):

    telescope = os.getenv('TARGETSYS')
    
    @classmethod
    def setUpClass(cls):
        cls.client = PySimpleClient()
        cls.boss = cls.client.getComponent('MINORSERVO/Boss')
        
    @classmethod
    def tearDownClass(cls):
        cls.client.releaseComponent('MINORSERVO/Boss')

    def setUp(self):
        self.antennaInfo = Antenna.TRunTimeParameters(
            targetName='dummy',
            azimuth=math.pi,
            elevation=math.pi/2 * 1/random.randrange(2, 10),
            startEpoch=getTimeStamp().value + 100000000,
            onTheFly=False,
            slewingTime=100000000,
            section=Antenna.ANT_SOUTH,
            axis=Management.MNG_TRACK,
            timeToStop=0)

        self.scan = MinorServo.MinorServoScan(
            range=20,
            total_time=100000000, # 10 seconds
            axis_code='SRP_TZ' if self.telescope == 'SRT' else 'Z',
            is_empty_scan=False)
        

class StartScanTest(StartScanBaseTest):

    def setUp(self):    
        super(StartScanTest, self).setUp()
        setupCode = 'KKG' if self.telescope == 'SRT' else 'KKC'

        # Wait (maximum one minute) in case the boss is parking
        if self.boss.isParking():
            t0 = datetime.now()
            while self.boss.isParking() and (datetime.now() - t0).seconds < 60:
                time.sleep(2)
            if self.boss.isParking():
                self.fail('The system can not exit form a parking state')

        if self.boss.getActualSetup() != setupCode:
            self.boss.setup(setupCode)
            # Wait (maximum 5 minutes) in case the boss is starting
            t0 = datetime.now()
            while not self.boss.isReady() and (datetime.now() - t0).seconds < 60*5:
                time.sleep(2)
            if not self.boss.isReady():
                self.fail('The system is not ready for executing the tests')

        self.boss.setElevationTracking('OFF')
        self.boss.setASConfiguration('OFF')
        axes, units = self.boss.getAxesInfo()
        self.idx = axes.index('SRP_TZ')

        getPosition = getattr(self, 'get%sPosition' %self.telescope)
        centerScanPosition = getPosition(
                self.boss.getActualSetup(), 
                'SRP', 
                math.degrees(self.antennaInfo.elevation))
        self.centerScan = centerScanPosition[self.idx]

    def tearDown(self):
        if self.boss.isScanActive():
            self.boss.closeScan()

    def test_empty_scan(self):
        """Do nothing in case of empty scan"""
        self.scan.is_empty_scan = True
        startTime = 0
        self.boss.startScan(startTime, self.scan, self.antennaInfo)
        self.assertFalse(self.boss.isScanActive())

    def test_start_ASAP(self):
        """The starting time is unknown and the scan must start ASAP"""
        startTime = 0
        t = self.boss.startScan(startTime, self.scan, self.antennaInfo)
        self.assertGreater(t, getTimeStamp().value)
        self.isAssertScan(t)

    def test_start_at_given_time(self):
        """The scan has to start at given time"""
        startTime = getTimeStamp().value + 60*10**7 # Start in one minute 
        t = self.boss.startScan(startTime, self.scan, self.antennaInfo)
        self.assertEqual(t, startTime)
        self.isAssertScan(t)

    def test_scan_too_fast(self):
        """Servo not enough fast for accomplishing the scan in total_time"""
        startTime = getTimeStamp().value + 60*10**7 # Start in one minute
        self.scan.total_time = 5000000 # 0.5 seconds
        with self.assertRaises(MinorServoErrorsEx):
            self.boss.startScan(startTime, self.scan, self.antennaInfo)

    def _test_out_of_range(self):
        """The scan goes out of the servo position limits"""
        startTime = 0
        self.scan.range = 1000 # 1 meter
        with self.assertRaises(MinorServoErrorsEx):
            self.boss.startScan(startTime, self.scan, self.antennaInfo)

    def _test_start_time_too_close_to_now(self):
        """The starting time is too close to the current time"""
        startTime = getTimeStamp().value + 1 # Start in 1 second from now
        with self.assertRaises(MinorServoErrorsEx):
            self.boss.startScan(startTime, self.scan, self.antennaInfo)

    def isAssertScan(self, startTime):
        self.assertTrue(self.boss.isScanning())
        # Assertions to verify right after startTime
        self.waitUntil(startTime)
        self.assertTrue(self.boss.isScanActive())
        self.assertTrue(self.boss.isScanning())
        self.assertAlmostEqual(
                self.boss.getCentralScanPosition(), 
                self.centerScan, 
                delta=0.1)

        # Wait untill the scan finishes (one second after the scan)
        targetTime = startTime + self.scan.total_time + 1*10**7 
        self.waitUntil(targetTime)
        startPos = self.boss.getAxesPosition(startTime)[self.idx]
        endPos = self.boss.getAxesPosition(targetTime)[self.idx]
        self.assertTrue(self.boss.isScanActive())
        self.assertFalse(self.boss.isScanning())
        self.assertAlmostEqual(startPos + self.scan.range, endPos, delta=0.1)

    def waitUntil(self, targetTime):
        while getTimeStamp().value < targetTime:
            time.sleep(0.1)

    def getSRTPosition(self, conf_code, servo_name, elevation=45):
        """Return the servo position related to the elevation.

        Parameters:
        - conf_code: value returned by getActualSetup() (CCB, CCB_ASACTIVE,...)
        - servo_name: SRP, GFR, M3R, PFP
        - elevation: the antenna elevation, in degrees
        """
        dal = ACSCorba.cdb()
        dao = dal.get_DAO_Servant('alma/MINORSERVO/Boss')
        body = dao.get_field_data(conf_code) 
        configurations = body.strip().split('@')
        servos_conf = {}
        for conf in configurations:
           if conf:
               name, value = conf.split(':')
               servos_conf[name.strip()] = value.strip()
        
        # Example of servo_conf: 
        #   >>> servos_conf['PFP'] 
        #   'RY(mm)=(-25.75); TX(mm)=(458); TZ(mm)=(-46.2);'
        srp_conf = servos_conf[servo_name]
        srp_items = [item.strip() for item in srp_conf.split(';')]
        srp_axes = []
        for item in srp_items:
           if '=' in item:
               name, value = item.split('=')
               srp_axes.append(value.strip())

        # Example of srp_axes:
        #   >>> srp_axes
        #   ['(-25.75)', '(458)', '(-46.2)']
        position = []
        for axis in srp_axes:
            axis = axis.lstrip('(')
            axis = axis.rstrip(')')
            # At this point, axis is something like '-0.23, 0.01, 3.2'
            coeffs = [float(item) for item in axis.split(',')]
            value = 0
            for idx, coeff in enumerate(coeffs):
                value += coeff * elevation**idx
            # Value: -25.75*elevation**0 + 485*elevation**1 -46.2*elevation**2
            position.append(value)
        return position


class StartScanInterfaceTest(StartScanBaseTest):

    def test_system_not_ready(self):
        """Raise a MinorServoErrorsEx in case the system is not ready"""
        with self.assertRaises(MinorServoErrorsEx):
            t = self.boss.checkScan(0, self.scan, self.antennaInfo) 


if __name__ == '__main__':
    if 'Configuration' in os.getenv('ACS_CDB'):
        unittest2.main() # Real test using the antenna CDB
    else:
        simunittest.run(StartScanTest)
        simunittest.run(StartScanInterfaceTest)
