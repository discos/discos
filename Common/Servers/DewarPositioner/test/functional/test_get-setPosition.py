from __future__ import with_statement
import unittest2
import time
from ComponentErrors import ComponentErrorsEx
from maciErrType import CannotGetComponentEx
from Acspy.Clients.SimpleClient import PySimpleClient
from Antenna import ANT_NORTH
from Management import  MNG_TRACK


class GetSetPositionTest(unittest2.TestCase):
    """Test the DewarPositioner.getPosition() method"""
    def setUp(self):
        client = PySimpleClient()
        self.positioner = client.getComponent('RECEIVERS/DewarPositioner')
        self.derotator = client.getComponent('RECEIVERS/SRTKBandDerotator')
        self.positioner.setup('KKG')
        time.sleep(1)

    def test_notReady(self):
        """The DewarPositioner must raise a NotAllowedEx when it's not ready"""
        self.positioner.park()
        time.sleep(1)
        self.assertRaises(ComponentErrorsEx, self.positioner.getPosition)

    def test_value(self):
        """Verify it returns the right value"""
        self.derotator.setPosition(1.5)
        time.sleep(2)
        self.assertAlmostEqual(self.positioner.getPosition(), 1.5, places=1)
        self.positioner.park()

    def test_max_limit(self):
        """Raise a ComponentErrorsEx in case of position above the max limit"""
        max_limit = self.positioner.getMaxLimit()
        with self.assertRaises(ComponentErrorsEx):
            self.positioner.setPosition(max_limit + 1)

    def test_min_limit(self):
        """Raise a ComponentErrorsEx in case of position below the max limit"""
        min_limit = self.positioner.getMinLimit()
        with self.assertRaises(ComponentErrorsEx):
            self.positioner.setPosition(min_limit - 1)


if __name__ == '__main__':
    unittest2.main()
