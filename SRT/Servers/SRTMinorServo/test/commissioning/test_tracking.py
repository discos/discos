#!/usr/bin/env python
# Author: Marco Buttu <m.buttu@oa-cagliari.inaf.it>
# Copyright: This module has been placed in the public domain.
"""Use this module to test the SRP elevation tracking."""

from Acspy.Clients.SimpleClient import PySimpleClient
import unittest
import time


def isAlmostEqual(a, b, delta=0.1):
    return true if abs(a-b) < delta else false


class TestElevationTracking(unittest.TestCase):
    """Test the minor servo properties."""

    def setUp(self):
        self.sockobj.connect(server) 
        self.pyclient = PySimpleClient()
        self.minorservo_boss = self.pyclient.getComponent("MINORSERVO/MinorServoBoss")
        self.srp = self.pyclient.getComponent("MINORSERVO/SRP")
        self.antenna_mount = self.pyclient.getComponent("ANTENNA/Mount")

    def tearDownClass():
        self.pyclient.releaseComponent(self.minorservo_boss)
        self.pyclient.releaseComponent(self.antenna_mount)

    def test_tracking(self):
        """Test the SRP corrections due to elevation deformations."""
        delay = 5 # seconds
        time.sleep(delay) # Wait a bit, until the boss is active
        self.minorservo_boss.setup('CCB')
        time.sleep(1) # Wait a bit, until the boss begins the configuration process
        
        counter = 0
        while not self.minorservo_boss.isReady(): # Wait until the minor servo boss is ready
            time.sleep(delay) # Wait a bit, until the boss is active
            counter += delay
            if counter > 240:
                self.assertTrue(counter > 240)
                return
        
        min_el = 6.0
        max_el = 88.0
        # Get the actual azimut position
        az_obj = self.antenna_mount._get_azimuth()
        az, cmp = az_obj.get_sync()
        el_obj = self.antenna_mount._get_elevation()
        el, cmp = el_obj.get_sync()

        # First test, moving the antenna to the minimum elevation
        self.antenna_mount.preset(az, min_el) # Set the antenna elevation to the min value
        while not isAlmostEqual(el, min_el, 1.0):
            time.sleep(delay)
            # get the actual elevation
            el, cmp = el_obj.get_sync()
            # Compute the expected values
            expected = [
                0,
                29.8166666666665 + 0.263472663139432 * el - 0.018206701940039 * el**2 + 0.000072373113855 * el**3,
                -6.37463319147492 + 0.169826241751738 * el - 0.000419997047419 * el**2 - 0.000003985237184 * el**3,
                -0.03222222222222361 + 0.00014822163433269445 * el + 0.000027586713698 * el**2 - 0.000000077732053 * el**3,
                0,
                0
            ]
            pos_obj = self.srp._get_actPos()
            srp_pos, cmp = pos_obj.get_sync()

            for ideal_pos, real_pos in zip(expected, srp_pos):
                self.assertAlmostEqual(ideal_pos, real_pos, places=0)

        # Second test, moving the antenna to the maximum elevation
        self.antenna_mount.preset(az, max_el) # Set the antenna elevation to the max value
        while not isAlmostEqual(el, max_el, 1.0):
            time.sleep(delay)
            # get the actual elevation
            el, cmp = el_obj.get_sync()
            # Compute the expected values
            expected = [
                0,
                29.8166666666665 + 0.263472663139432 * el - 0.018206701940039 * el**2 + 0.000072373113855 * el**3,
                -6.37463319147492 + 0.169826241751738 * el - 0.000419997047419 * el**2 - 0.000003985237184 * el**3,
                -0.03222222222222361 + 0.00014822163433269445 * el + 0.000027586713698 * el**2 - 0.000000077732053 * el**3,
                0,
                0
            ]
            pos_obj = self.srp._get_actPos()
            srp_pos, cmp = pos_obj.get_sync()

            for ideal_pos, real_pos in zip(expected, srp_pos):
                self.assertAlmostEqual(ideal_pos, real_pos, places=0)


if __name__ == "__main__":
    suite = unittest.TestLoader().loadTestsFromTestCase(TestElevationTracking)
    unittest.TextTestRunner(verbosity=2).run(suite)
    print "\n" + "="*70

