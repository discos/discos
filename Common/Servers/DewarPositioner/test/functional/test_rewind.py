from __future__ import with_statement
import unittest2
import time
import math
from DewarPositioner.posgenerator import PosGenerator
from Antenna import ANT_HORIZONTAL, ANT_NORTH, ANT_SOUTH
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


    def test_not_positive_number_of_steps(self):
        """Raise ComponentErrorsEx when the number of steps is not positive"""
        with self.assertRaisesRegexp(ComponentErrorsEx, 'steps must be positive'):
            self.positioner.setRewindingMode('MANUAL')
            az, el, target = self.prepare_negative_oor() 
            # after prepare_negative_oor(), startUpdating() will cause oor
            self.positioner.startUpdating(MNG_TRACK, ANT_SOUTH, az, el, 0, 0)
            time.sleep(0.5)
            self.assertTrue(self.positioner.isRewindingRequired())
            self.positioner.rewind(0)

    def test_number_of_steps_oor(self):
        """Raise ComponentErrors when the number of steps is out of range"""
        with self.assertRaisesRegexp(ComponentErrorsEx, 'cannot rewind the derotator'):
            self.positioner.setRewindingMode('MANUAL')
            az, el, target = self.prepare_negative_oor()
            # after prepare_negative_oor(), startUpdating() will cause oor
            self.positioner.startUpdating(MNG_TRACK, ANT_SOUTH, az, el, 0, 0)
            time.sleep(0.5)
            self.assertTrue(self.positioner.isRewindingRequired())
            self.positioner.rewind(4)

    def test_tracking_when_rewinding_required(self):
        """Verify the tracking flag is False when a rewinding is required"""
        self.positioner.setRewindingMode('MANUAL')
        az, el, target = self.prepare_negative_oor()
        # after prepare_negative_oor(), startUpdating() will cause oor
        self.positioner.startUpdating(MNG_TRACK, ANT_SOUTH, az, el, 0, 0)
        time.sleep(0.5)
        self.assertEqual(self.positioner.isRewindingRequired(), True)
        self.assertEqual(self.positioner.isRewinding(), False)
        self.assertEqual(self.positioner.isTracking(), False)

    def test_updating_and_position_after_manual_rewind(self):
        """Verify position and that it stills updating after a manual rewind"""
        self.positioner.setRewindingMode('MANUAL')
        az, el, target = self.prepare_negative_oor() 
        # after prepare_negative_oor(), startUpdating() will cause oor
        self.positioner.startUpdating(MNG_TRACK, ANT_SOUTH, az, el, 0, 0)
        time.sleep(0.5)
        steps = 2
        self.positioner.rewind(steps) 
        self.assertTrue(self.positioner.isUpdating())
        expected = target + steps * self.positioner.getRewindingStep()
        self.assertAlmostEqual(self.positioner.getPosition(), expected, delta=0.1)

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
        # If sector is SOUTH, we expect a rewind of 60 degrees: -93 + 60 -> -33
        expected = target + 60
        self.positioner.startUpdating(MNG_TRACK, ANT_SOUTH, az, el, 0, 0)
        time.sleep(0.5)
        self.assertAlmostEqual(expected, self.device.getActPosition(), delta=0.1)

        self.positioner.stopUpdating()

        # Second step: we do not call stopUpdating(), and we choose (az, el) 
        # in order to have a parallactic angle in range
        az = math.radians(120)
        el = math.radians(45)
        parallactic = PosGenerator.getParallacticAngle(self.lat, az, el) # -42.78
        self.antenna.setOffsets(az, el, ANT_HORIZONTAL)
        target = Pis + parallactic 
        self.assertGreater(target, min_limit)
        # We do not expect a rewind
        expected = target
        self.positioner.startUpdating(MNG_TRACK, ANT_SOUTH, az, el, 0, 0)
        time.sleep(0.5)
        self.assertAlmostEqual(expected, self.device.getActPosition(), delta=0.1)

    def test_keep_updating_after_auto_rewind(self):
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
        # and at this point we set the following in order to go out of range:
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
        self.positioner.startUpdating(MNG_TRACK, ANT_SOUTH, az, el, 0, 0)
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

    def test_rewinding_angle_sector_NORTH(self):
        """The rewinding angle depends of the antenna sector.
        
        For instance, this is the case: we set a Pis to 120 degrees and we are
        following the source in sector NORTH, so the derotator is going to the
        lower limit (CCW). At this point we set an antenna offset that causes
        the derotator movement slighly up to its higher limit. In that case to 
        guarantee the maximum movement space we have to rewind but going close
        to the higher limit, not the lower one. If this does not appen, so if 
        after the rewind we set an antenna position that causes a Pdp of -90 
        degrees, the derotator tries to go below the lower limit, and so
        performs a new rewind.
        """
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
        # and at this point we set the following in order to go out of range:
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
        az = math.radians(190)
        self.antenna.setOffsets(az, el, ANT_HORIZONTAL)
        # We expect a rewind
        time.sleep(0.5)
        # At this point we want the new position to be closer to the max_limit
        # than the min_limit
        actual_pos = self.positioner.getPosition()
        right_distance = abs(actual_pos - self.positioner.getMaxLimit())
        left_distance = abs(actual_pos - self.positioner.getMinLimit())
        self.assertGreater(left_distance, right_distance)

    def test_rewinding_angle_sector_SOUTH(self):
        """The rewinding angle depends of the antenna sector.
        
        Look at the test_rewinding_angle_sector_NORTH docstring.
        """
        Pis = -80
        self.positioner.setPosition(Pis) 

        # First step: we choose (az, el) in order to have a parallactic angle
        # of a just one or two degrees, in order to stay in range. i.e. we
        # can set the following value to start the updating:
        #
        #     >>> az = math.radians(179)
        #     >>> el = math.radians(45)
        #     >>> PosGenerator.getParallacticAngle(self.lat, az, el)
        #     -0.77546054825239319
        #
        # and at this point we set the following in order to go out of range:
        # 
        #    >>> az = math.radians(170)
        #    >>> PosGenerator.getParallacticAngle(self.lat, az, el)
        #    -7.7330295207687838
        az = math.radians(179)
        el = math.radians(45)
        parallactic = PosGenerator.getParallacticAngle(self.lat, az, el) # -0.77

        min_limit = self.device.getMinLimit() # -85.77 degrees for the K Band
        self.antenna.setOffsets(az, el, ANT_HORIZONTAL)
        target = Pis + parallactic 
        self.assertGreater(target, min_limit)
        self.positioner.startUpdating(MNG_TRACK, ANT_SOUTH, az, el, 0, 0)
        time.sleep(0.5)
        self.assertAlmostEqual(target, self.device.getActPosition(), delta=0.1)
        self.assertTrue(self.positioner.isUpdating())
        self.assertTrue(self.positioner.isTracking())

        # Second step: we set the az, in order to have a parallactic angle
        # that causes an out of range
        az = math.radians(170)
        self.antenna.setOffsets(az, el, ANT_HORIZONTAL)
        # We expect a rewind
        time.sleep(0.5)
        # At this point we want the new position to be closer to the min_limit
        # than the max_limit
        actual_pos = self.positioner.getPosition()
        right_distance = abs(actual_pos - self.positioner.getMaxLimit())
        left_distance = abs(actual_pos - self.positioner.getMinLimit())
        self.assertGreater(right_distance, left_distance)

    def prepare_negative_oor(self):
        """Set the derotator position an antenna az-el in order to
        cause an out of range when startUpdating() is called.
        """
        Pis = -80
        self.positioner.setPosition(Pis) 
        # We choose (az, el) in order to have a parallactic angle out of range
        #
        #    >>> az = math.radians(170)
        #    >>> el = math.radians(45)
        #    >>> PosGenerator.getParallacticAngle(self.lat, az, el)
        #    -7.7330295207687838
        az = math.radians(170)
        el = math.radians(45)
        parallactic = PosGenerator.getParallacticAngle(self.lat, az, el) # -0.77
        min_limit = self.device.getMinLimit() # -85.77 degrees for the K Band
        self.antenna.setOffsets(az, el, ANT_HORIZONTAL)
        target = Pis + parallactic 
        self.assertLess(target, min_limit)
        return az, el, target


if __name__ == '__main__':
    unittest2.main()
