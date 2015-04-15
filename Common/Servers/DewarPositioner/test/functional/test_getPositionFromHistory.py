from __future__ import with_statement

import time
import unittest2
from ComponentErrors import ComponentErrorsEx, ComponentErrorsEx
from Acspy.Clients.SimpleClient import PySimpleClient
from Acspy.Common.TimeHelper import getTimeStamp

from ComponentErrors import ComponentErrorsEx
from DewarPositioner.cdbconf import CDBConf


class GetPositionFromHistoryTest(unittest2.TestCase):

    @classmethod
    def setUpClass(self):
        client = PySimpleClient()
        self.positioner = client.getComponent('RECEIVERS/DewarPositioner')
        self.positioner.setup('KKG')
        self.cmdPos = 0.8
        time.sleep(0.2)

    @classmethod
    def tearDownClass(self):
        self.positioner.park()
        time.sleep(0.2)

    def test_notReady(self):
        self.positioner.park()
        with self.assertRaises(ComponentErrorsEx):
            self.positioner.getPositionFromHistory(0)

    def test_AtTime(self):
        """Verify the position at a given time"""
        p0 = self.positioner.getPosition()
        t0 = getTimeStamp().value
        # Change the position
        self.positioner.setPosition(self.cmdPos)
        time.sleep(1)
        # Verify the position we get at the time t0
        p = self.positioner.getPositionFromHistory(t0)
        self.assertAlmostEqual(p, p0, places=1)

    def test_timeLesser(self):
        """Verify the position at a time lesser then the older one"""
        p0 = self.positioner.getPosition()
        t0 = getTimeStamp().value
        # Change the position
        self.positioner.setPosition(self.cmdPos)
        time.sleep(1)
        # Verify the position we get at 60 seconds in the past
        pOld = self.positioner.getPositionFromHistory(t0 - 60*10**7)
        self.assertAlmostEqual(p0, pOld, places=1)

    def test_timeGreater(self):
        """Verify the position at a time greater then the latest one"""
        # Change the position (it was 0 after the park())
        self.positioner.setPosition(self.cmdPos)
        time.sleep(1)
        # Verify the position we get at 60 seconds in the future
        t = getTimeStamp().value + 60*10**7
        p = self.positioner.getPositionFromHistory(t)
        self.assertAlmostEqual(p, self.cmdPos, places=1)

    def test_timeZero(self):
        """Verify the position at time 0"""
        # Change the position (it was 0 after the park())
        self.positioner.setPosition(self.cmdPos)
        time.sleep(1)
        # Verify the position we get at 60 seconds in the future
        p = self.positioner.getPositionFromHistory(0)
        self.assertAlmostEqual(p, self.cmdPos, places=1)


if __name__ == '__main__':
    unittest2.main()
