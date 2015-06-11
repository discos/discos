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
        self.lat = 0.689 # 39.5 degrees

    def tearDown(self):
        if self.positioner.isReady():
            self.positioner.stopUpdating()
            time.sleep(1)
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

        # First step: we choose (az, el) in order to have a parallactic angle
        # out of range. That means the derotator has to rewind
        az, el = 1.2217, 0.6109 # 70 degrees, 35 degrees
        parallactic = PosGenerator.getParallacticAngle(self.lat, az, el) # -63
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
        time.sleep(0.5)
        self.assertAlmostEqual(expected, self.device.getActPosition(), delta=0.1)

        self.positioner.stopUpdating()

        # Second step: we do not call stopUpdating(), and we choose (az, el) 
        # in order to have a parallactic angle in range
        az, el = 5, 0.5 # In radians
        parallactic = PosGenerator.getParallacticAngle(self.lat, az, el) # 58.5
        self.antenna.setOffsets(az, el, ANT_HORIZONTAL)
        target = Pis + parallactic 
        self.assertGreater(target, min_limit)
        # We do not expect a rewind
        expected = target
        self.positioner.startUpdating(MNG_TRACK, ANT_NORTH, az, el, 0, 0)
        time.sleep(0.5)
        self.assertAlmostEqual(expected, self.device.getActPosition(), delta=0.1)

    def test_keep_updating_after_rewind(self):
        """Verify the dewar positioner stills updating after the rewind"""
        Pis = 120
        self.positioner.setPosition(Pis) 

        # First step: we choose (az, el) in order to have a parallactic angle
        # of a just one or two degrees, in order to stay in range. i.e. we
        # can set the following value to start the updating:
        #
        #     >>> az = math.radians(181)
        #     >>> el = math.radians(45)
        #     >>> PosGenerator.getParallacticAngle(self.lat, az, el)
        #     0.77546054825239319
        #
        # and at this point the following in order to go out of range:
        # 
        #    >>> az = math.radians(190)
        #    >>> PosGenerator.getParallacticAngle(self.lat, az, el)
        #    7.7330295207687838
        az = math.radians(181)
        el = math.radians(45)
        parallactic = PosGenerator.getParallacticAngle(self.lat, az, el) # 0.77

        max_limit = self.device.getMaxLimit() # 125.23 degrees for the K Band
        self.antenna.setOffsets(az, el, ANT_HORIZONTAL)
        target = Pis + parallactic 
        self.assertLess(target, max_limit)
        self.positioner.startUpdating(MNG_TRACK, ANT_NORTH, az, el, 0, 0)
        time.sleep(0.5)
        self.assertAlmostEqual(target, self.device.getActPosition(), delta=0.1)
        self.assertTrue(self.positioner.isUpdating())
        self.assertTrue(self.positioner.isTracking())

        # Second step: we set the az, in order to have a parallactic angle
        # that causes an out of range
        # in order to have a parallactic angle in range
        az = math.radians(190)
        self.antenna.setOffsets(az, el, ANT_HORIZONTAL)
        parallactic = PosGenerator.getParallacticAngle(self.lat, az, el) # 7.73
        target = Pis + parallactic
        self.assertGreater(target, max_limit)
        expected = target - 180
        # We expect a rewind
        time.sleep(0.5)
        self.assertAlmostEqual(expected, self.device.getActPosition(), delta=0.1)
        # Verify it keeps the system updating the position
        self.assertFalse(self.positioner.isRewinding())
        self.assertTrue(self.positioner.isUpdating())
        self.assertTrue(self.positioner.isTracking())


if __name__ == '__main__':
    unittest2.main()
