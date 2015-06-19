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



__author__ = "Marco Buttu <mbuttu@oa-cagliari.inaf.it>"

class ScanBaseTest(unittest2.TestCase):

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
            rightAscension=0,
            declination=0,
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

    def tearDown(self):
        if self.boss.isScanActive():
            t = self.boss.closeScan()
            self.waitUntil(t)

    def waitUntil(self, targetTime):
        while getTimeStamp().value < targetTime:
            time.sleep(0.1)
        

class ScanTest(ScanBaseTest):
    """Test checkScan(), startScan() and closeScan()"""

    def setUp(self):    
        super(ScanTest, self).setUp()
        setupCode = 'KKG' if self.telescope == 'SRT' else 'CCC'

        # Wait (maximum one minute) in case the boss is parking
        if self.boss.isParking():
            t0 = datetime.now()
            while self.boss.isParking() and (datetime.now() - t0).seconds < 60:
                time.sleep(2)
            if self.boss.isParking():
                self.fail('The system can not exit form a parking state')

        if self.boss.getActualSetup() != setupCode or not self.boss.isReady():
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
        self.idx = axes.index(self.scan.axis_code)

        getPosition = getattr(self, 'get%sPosition' %self.telescope)
        centerScanPosition = getPosition(
                self.boss.getActualSetup(), 
                'SRP', 
                math.degrees(self.antennaInfo.elevation))
        self.centerScan = centerScanPosition[self.idx]

    def test_startScan_empty_scan(self):
        """Do nothing in case of empty scan"""
        self.scan.is_empty_scan = True
        startTime = 0
        self.boss.startScan(startTime, self.scan, self.antennaInfo)
        self.assertFalse(self.boss.isScanActive())

    def test_startScan_ASAP(self):
        """Starting time unknown: the scan must start ASAP"""
        startTime = 0
        t = self.boss.startScan(startTime, self.scan, self.antennaInfo)
        self.assertGreater(t, getTimeStamp().value)
        self.isAssertScan(t)

    def test_startScan_ASAP_at_checkScan_time(self):
        """Starting time given by checkScan()"""
        startTime = 0
        res, msInfo = self.boss.checkScan(startTime, self.scan, self.antennaInfo)
        startTime = msInfo.startEpoch 
        t = self.boss.startScan(startTime, self.scan, self.antennaInfo)
        self.assertGreater(t, getTimeStamp().value)
        self.isAssertScan(t)

    def test_startScan_at_given_time(self):
        """Start at given time"""
        startTime = getTimeStamp().value + 60*10**7 # Start in one minute 
        t = self.boss.startScan(startTime, self.scan, self.antennaInfo)
        self.assertEqual(t, startTime)
        self.isAssertScan(t)

    def test_startScan_too_fast(self):
        """Servo not enough fast for accomplishing the scan in total_time"""
        startTime = getTimeStamp().value + 60*10**7 # Start in one minute
        self.scan.total_time = 5000000 # 0.5 seconds
        with self.assertRaises(MinorServoErrorsEx):
            self.boss.startScan(startTime, self.scan, self.antennaInfo)

    def test_startScan_out_of_range(self):
        """Scan out of the servo position limits"""
        startTime = 0
        self.scan.range = 5000 # 5 meters
        with self.assertRaises(MinorServoErrorsEx):
            self.boss.startScan(startTime, self.scan, self.antennaInfo)

    def test_startScan_time_too_close_to_now(self):
        """Starting time too close to the current time"""
        startTime = getTimeStamp().value + 1*10**7 # Start in 1 second from now
        with self.assertRaises(MinorServoErrorsEx):
            self.boss.startScan(startTime, self.scan, self.antennaInfo)

    def test_closeScan_time_to_stop(self):
        """Return the time_to_stop"""
        startTime = 0
        t = self.boss.startScan(startTime, self.scan, self.antennaInfo)
        self.waitUntil(startTime)
        time_to_stop = self.boss.closeScan()
        # The time_to_stop should be greater than now
        self.assertGreater(time_to_stop, getTimeStamp().value) 

    def test_checkScan_empty_scan_start_ASAP(self):
        """Starting time unknown: the scan must start ASAP"""
        startTime = 0
        self.scan.is_empty_scan = True

        res, msInfo = self.boss.checkScan(startTime, self.scan, self.antennaInfo)
        self.assertTrue(res)
        self.assertAlmostEqual(msInfo.centerScan, self.centerScan, delta=0.01)
        self.assertGreater(msInfo.startEpoch, getTimeStamp().value)
        self.assertEqual(msInfo.scanAxis, self.scan.axis_code)
        self.assertEqual(msInfo.timeToStop, 
                         msInfo.startEpoch + self.scan.total_time)

    def test_checkScan_not_empty_scan_start_ASAP(self):
        """Scan not empty: starting time unknown, the scan must start ASAP"""
        startTime = 0

        res, msInfo = self.boss.checkScan(startTime, self.scan, self.antennaInfo)
        self.assertTrue(res)
        self.assertAlmostEqual(msInfo.centerScan, self.centerScan, delta=0.01)
        self.assertTrue(msInfo.onTheFly)
        self.assertGreater(msInfo.startEpoch, getTimeStamp().value)
        self.assertEqual(msInfo.scanAxis, self.scan.axis_code)
        self.assertEqual(
                msInfo.timeToStop, 
                msInfo.startEpoch + self.scan.total_time)

    def test_checkScan_empty_scan_start_at_given_time(self):
        """Starting time known and achievable"""
        startTime = getTimeStamp().value + 60*10**7 # Start in a minute 
        self.scan.is_empty_scan = True

        res, msInfo = self.boss.checkScan(startTime, self.scan, self.antennaInfo)
        self.assertTrue(res)
        self.assertAlmostEqual(msInfo.centerScan, self.centerScan, delta=0.01)
        self.assertFalse(msInfo.onTheFly)
        self.assertEqual(msInfo.startEpoch, startTime)
        self.assertEqual(msInfo.scanAxis, self.scan.axis_code)
        self.assertEqual(
                msInfo.timeToStop, 
                msInfo.startEpoch + self.scan.total_time)

    def test_checkScan_not_empty_scan_start_at_given_time(self):
        """Scan not empty: starting time known and achievable"""
        startTime = getTimeStamp().value + 60*10**7 # Start in a minute 

        res, msInfo = self.boss.checkScan(startTime, self.scan, self.antennaInfo)
        self.assertTrue(res)
        self.assertAlmostEqual(msInfo.centerScan, self.centerScan, delta=0.01)
        self.assertTrue(msInfo.onTheFly)
        self.assertEqual(msInfo.startEpoch, startTime)
        self.assertEqual(msInfo.scanAxis, self.scan.axis_code)
        self.assertEqual(
                msInfo.timeToStop, 
                msInfo.startEpoch + self.scan.total_time)

    def test_checkScan_too_fast(self):
        """Servo not enough fast for accomplishing the scan in total_time"""
        startTime = getTimeStamp().value + 60*10**7 # Start in a minute from now
        self.scan.total_time = 5000000 # 0.5 seconds
        res, msInfo = self.boss.checkScan(startTime, self.scan, self.antennaInfo)
        self.assertFalse(res)

    def test_checkScan_out_of_range(self):
        """The scan goes out of the servo position limits"""
        startTime = 0
        self.scan.range = 1000 # 1 meter
        res, msInfo = self.boss.checkScan(startTime, self.scan, self.antennaInfo)
        self.assertFalse(res) 

    def test_checkScan_start_time_too_close_to_now(self):
        """Starting time too close to the current time"""
        startTime = getTimeStamp().value + 1*10**6 # Start in 0.1 second from now
        res, msInfo = self.boss.checkScan(startTime, self.scan, self.antennaInfo)
        self.assertFalse(res)

    def isAssertScan(self, startTime):
        self.assertFalse(self.boss.isScanning())
        self.assertTrue(self.boss.isScanActive())
        # Assertions to verify right after startTime
        self.waitUntil(startTime)
        self.assertTrue(self.boss.isScanning())
        self.assertTrue(self.boss.isScanActive())
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

    def getMEDPosition(self, conf_code, servo_name="", elevation=45):
        """Return the servo position related to the elevation for MED
        radiotelescope.

        Parameters:
        - conf_code: value returned by getActualSetup() (CCC, KKC,...)
        - servo_name: "" not use at Med
        - elevation: the antenna elevation, in degrees
        """
        from xml.dom.minidom import parseString
        dal = ACSCorba.cdb()
        dao = dal.get_DAO('alma/DataBlock/MinorServoParameters')
        root = parseString(dao).documentElement
        position = []
        coefficients = []
        for minorservo in root.getElementsByTagName("MinorServo"):
            for code in minorservo.getElementsByTagName("code"):
                if code.firstChild.data == conf_code:
                    if minorservo.getElementsByTagName("primary")[0].firstChild.data == "1":
                        yp_string_poly = minorservo.getElementsByTagName("YPaxis")[0].firstChild.data
                        coefficients.append(map(float,yp_string_poly.split(",")[3:]))
                        zp_string_poly = minorservo.getElementsByTagName("ZPaxis")[0].firstChild.data
                        coefficients.append(map(float,zp_string_poly.split(",")[3:]))
                    else:
                        x_string_poly = minorservo.getElementsByTagName("Xaxis")[0].firstChild.data
                        coefficients.append(map(float,x_string_poly.split(",")[3:]))
                        y_string_poly = minorservo.getElementsByTagName("Yaxis")[0].firstChild.data
                        coefficients.append(map(float,y_string_poly.split(",")[3:]))
                        z_string_poly = minorservo.getElementsByTagName("Zaxis")[0].firstChild.data
                        coefficients.append(map(float,z_string_poly.split(",")[3:]))
                        tx_string_poly = minorservo.getElementsByTagName("THETAXaxis")[0].firstChild.data
                        coefficients.append(map(float,tx_string_poly.split(",")[3:]))
                        ty_string_poly = minorservo.getElementsByTagName("THETAYaxis")[0].firstChild.data
                        coefficients.append(map(float,ty_string_poly.split(",")[3:]))
        for coefficient in coefficients:
            axis_position = 0
            for exp, coeff in enumerate(coefficient):
                axis_position += (elevation)**(exp) * coeff
            position.append(axis_position)
        return position


class ScanInterfaceTest(ScanBaseTest):
    """Test the interface of startScan() and closeScan()"""

    def test_checkScan_system_not_ready(self):
        """Raise a MinorServoErrorsEx in case the system is not ready"""
        with self.assertRaises(MinorServoErrorsEx):
            t = self.boss.checkScan(0, self.scan, self.antennaInfo) 

    def test_closeScan_scan_not_active(self):
        """Do nothing in case no scan is active"""
        self.boss.closeScan() 


if __name__ == '__main__':
    if 'Configuration' in os.getenv('ACS_CDB'):
        unittest2.main(verbosity=2, failfast=True) # Real test using the antenna CDB
    else:
        from PyMinorServoTest import simunittest
        simunittest.run(ScanTest)
        simunittest.run(ScanInterfaceTest)
