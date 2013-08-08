#!/usr/bin/env python
# Author: Marco Buttu <m.buttu@oa-cagliari.inaf.it>
# Copyright: This module has been placed in the public domain.
"""This module checks if the MinorServoBoss corrects the SRP position when the tracking is enabled."""

from Acspy.Clients.SimpleClient import PySimpleClient
import unittest
import time
import Antenna
from AntennaErrors import AntennaErrorsEx
from random import randrange


def isAlmostEqual(a, b, delta=0.2):
    return True if abs(a-b) < delta else False


class TestElevationTracking(unittest.TestCase):
    """Test the SRP positions when the antenna elevation changes."""

    def setUp(self):
        self.pyclient = PySimpleClient()
        self.minorservo_boss = self.pyclient.getComponent("MINORSERVO/Boss")
        self.srp = self.pyclient.getComponent("MINORSERVO/SRP")
        self.antenna_mount = self.pyclient.getComponent("ANTENNA/Mount")

    def tearDownClass():
        self.pyclient.releaseComponent(self.minorservo_boss)
        self.pyclient.releaseComponent(self.antenna_mount)

    def test_CCB(self):
        """Test the SRP elevation tracking for the CCB configuration"""
        self._test("CCB")

    def test_KKG(self):
        """Test the SRP elevation tracking for the KKG configuration"""
        self._test("KKG")

    def _test(self, setup_code):
        """Test if the SRP position changes with the elevation."""
        delay = 5 # seconds
        time.sleep(delay) # Wait a bit, until the boss is active
        self.minorservo_boss.setup(setup_code)
        time.sleep(1) # Wait a bit, until the boss begins the configuration process
        
        counter = 0
        print "\nExecuting the minor servo %s setup. Wait a bit ..." %setup_code
        while not self.minorservo_boss.isReady(): # Wait until the minor servo boss is ready
            time.sleep(delay) # Wait a bit, until the boss is active
            counter += delay
            if counter > 240:
                self.assertTrue(counter > 240)
                return

        print "\nThe MinorServoBoss is ready."
        self.minorservo_boss.turnTrackingOn() # Turn the tracking ON

        min_el = 6.0
        max_el = 89.0
        # Choice a range of 10 random elevation values
        elevations = sorted([randrange(min_el, max_el) for i in range(10)])

        # Get the actual azimut position
        az_obj = self.antenna_mount._get_azimuth()
        az, cmp = az_obj.get_sync()
        el_obj = self.antenna_mount._get_elevation()
        el, cmp = el_obj.get_sync()

        # The starting elevation is the closest between min_el and max_el
        target_el = max_el if el > (min_el + max_el)/2 else min_el
        if target_el == max_el:
            elevations.reverse()

        try:
            self.antenna_mount.changeMode(Antenna.ACU_PRESET, Antenna.ACU_PRESET)
        except AntennaErrorsEx, e:
             pass # The antenna is already in ACU_PRESET

        print "\nMoving the antenna to %s degrees of elevation..." %target_el
        self.antenna_mount.preset(az, target_el) # Set the antenna elevation to the min value
        while not isAlmostEqual(el, target_el, 0.2):
            time.sleep(delay)
            # get the actual elevation
            el, cmp = el_obj.get_sync()

        print "\nElevations to test: %s" %elevations
        print "\nStarting the test..."
        for i, elevation in enumerate(elevations):
            self.antenna_mount.preset(az, elevation) # Set the antenna elevation to the min value
            # Second test, moving the antenna to the maximum elevation
            while not isAlmostEqual(el, elevation):
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

            print "\n#" + '-'*50
            print "Actual elevation: %f" %el
            print "\tExpected position:\t%s" %([("%.2f" %pos) for pos in expected])
            print "\tReal position:\t\t%s" %([("%.2f" %pos) for pos in srp_pos])
            time.sleep(2)
            for ideal_pos, real_pos in zip(expected, srp_pos):
                self.assertAlmostEqual(ideal_pos, real_pos, places=0)
            message = "\nNext elevation -> %s degrees" %elevations[i+1] if i < len(elevations) - 1 else "Test done!"
            print "OK! " + message


if __name__ == "__main__":
    suite = unittest.TestLoader().loadTestsFromTestCase(TestElevationTracking)
    unittest.TextTestRunner(verbosity=2).run(suite)
    print "\n" + "="*70

