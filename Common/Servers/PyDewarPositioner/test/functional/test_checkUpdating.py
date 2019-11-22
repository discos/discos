from __future__ import with_statement
import unittest
import time
from Antenna import ANT_HORIZONTAL, ANT_NORTH, ANT_SOUTH
from Management import MNG_TRACK, MNG_BEAMPARK

from Acspy.Clients.SimpleClient import PySimpleClient
from Acspy.Common.TimeHelper import getTimeStamp
from ComponentErrors import ComponentErrorsEx


class TestCheckUpdating(unittest.TestCase):
    """Test the DewarPositioner.startUpdating() end-to-end method"""
    def setUp(self):
        self.client = PySimpleClient()
        self.positioner = self.client.getComponent('RECEIVERS/DewarPositioner')
        self.positioner.setup('KKG')
        self.positioner.setConfiguration('custom')
        self.lat = 0.689 # 39.5 degrees

    def tearDown(self):
        if self.positioner.isReady():
            self.positioner.stopUpdating()
            time.sleep(1)
            self.positioner.park()
        time.sleep(0.5)
        self.client.releaseComponent('RECEIVERS/DewarPositioner')


    def test_system_not_ready(self):
        """Raise ComponentErrorsEx when the system is not ready"""
        with self.assertRaisesRegexp(ComponentErrorsEx, 'positioner not configured'):
            self.positioner.park()
            time.sleep(0.5)
            starting_time = getTimeStamp().value + 100000000
            az, el, ra, dec = 0, 0, 0, 0
            self.positioner.checkUpdating(
                    starting_time,
                    MNG_TRACK,
                    ANT_SOUTH,
                    az, el, ra, dec
            )


    def test_system_not_ready(self):
        """Return (True, starting_time) when axis is MNG_BEAMPARK"""
        starting_time = getTimeStamp().value + 100000000
        az, el, ra, dec = 0, 0, 0, 0
        value, feasible_time = self.positioner.checkUpdating(
                starting_time,
                MNG_BEAMPARK,
                ANT_SOUTH,
                az, el, ra, dec
        )
        self.assertEqual(value, True)
        self.assertEqual(feasible_time, starting_time)



if __name__ == '__main__':
    unittest.main()
