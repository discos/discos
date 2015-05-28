from __future__ import with_statement
import unittest2
import time
import math
from DewarPositioner.posgenerator import PosGenerator
from Antenna import ANT_HORIZONTAL, ANT_NORTH
from Management import MNG_TRACK

from Acspy.Clients.SimpleClient import PySimpleClient
from ComponentErrors import ComponentErrorsEx


class StartUpdatingTest(unittest2.TestCase):
    """Test the DewarPositioner.startUpdating() end-to-end method"""
    def setUp(self):
        self.client = PySimpleClient()
        self.device = self.client.getComponent('RECEIVERS/SRTKBandDerotator')
        self.antenna = self.client.getComponent('ANTENNA/BossSimulator')
        self.positioner = self.client.getComponent('RECEIVERS/DewarPositioner')
        self.positioner.setup('KKG')
        self.positioner.setConfiguration('custom')

    def tearDown(self):
        if self.positioner.isReady():
            self.positioner.park()
        time.sleep(0.5)
        self.client.releaseComponent('RECEIVERS/DewarPositioner')
        self.client.releaseComponent('ANTENNA/BossSimulator')
        self.device = self.client.releaseComponent('RECEIVERS/SRTKBandDerotator')

    def test_rewind_offset_properly_cleared(self):
        """Verify the startUpdating() clears previous rewinding offsets.

        For instance, in the following case:
        
        - startUpdating() out of range -> rewind        
        - new startUpdating() with (Pis + parallactic) in range

        In the first startUpdating() we want the position to be::

            Pis + Pip + Pid + rewinding_offset

        In the second startUpdating() the position must be::

            Pis + Pip + Pid
        """
        latitude = 0.689 # 39.5 degrees

        # First step: we choose (az, el) in order to have a parallactic angle
        # out of range. That means the derotator has to rewind
        az, el = 1.2217, 0.6109 # 70 degrees, 35 degrees
        parallactic = PosGenerator.getParallacticAngle(latitude, az, el) # -63
        min_limit = self.device.getMinLimit() # -85.77 degrees for the K Band
        Pis = -30
        # Final angle -93, lower than the min limit (-85.77)
        self.positioner.setPosition(Pis) 
        self.antenna.setOffsets(az, el, ANT_HORIZONTAL)
        target = Pis + parallactic 
        self.assertLess(target, min_limit)
        # We expect a rewind of 180 degrees: -93 + 180 -> 87
        expected = target + 180
        self.positioner.startUpdating(MNG_TRACK, ANT_NORTH, az, el, 0, 0)
        time.sleep(0.2)
        self.assertAlmostEqual(expected, self.device.getActPosition(), delta=0.1)

        self.positioner.stopUpdating()

        # Second step: we do not call stopUpdating(), and we choose (az, el) 
        # in order to have a parallactic angle in range
        az, el = 5, 0.5 # In radians
        parallactic = PosGenerator.getParallacticAngle(latitude, az, el) # 58.5
        self.antenna.setOffsets(az, el, ANT_HORIZONTAL)
        target = Pis + parallactic 
        self.assertGreater(target, min_limit)
        # We do not expect a rewind
        expected = target
        self.positioner.startUpdating(MNG_TRACK, ANT_NORTH, az, el, 0, 0)
        time.sleep(0.2)
        self.assertAlmostEqual(expected, self.device.getActPosition(), delta=0.1)
        
        self.positioner.stopUpdating()


if __name__ == '__main__':
    unittest2.main()
