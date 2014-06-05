import unittest2
import random
import time
from DewarPositioner.positioner import Positioner, PositionerError


class PositionerTest(unittest2.TestCase):

    def setUp(self):
        self.p = Positioner()

    def tearDown(self):
        self.p.terminate()

    def test_updatingException(self):
        """When an exception is raised in Positioner.updatePosition(), then
        Positioner.updating() must return False"""
        def raise_exc():
            time.sleep(1)
            raise Exception('A dummy exception')
        self.p.start(raise_exc)
        time.sleep(0.2) # Wait for starting...
        self.assertEqual(self.p.updating(), True)
        time.sleep(1.5) # Wait a bit so we are sure the exception is raised
        self.assertEqual(self.p.updating(), False)

    def test_stop(self):
        """After a call to stop() Positioner.updating() must return False"""
        def foo():
            time.sleep(1)
        self.p.start(foo)
        time.sleep(0.2) # Wait for starting...
        self.assertEqual(self.p.updating(), True)
        self.p.terminate()
        time.sleep(0.2) # Wait for stopping...
        self.assertEqual(self.p.updating(), False)

if __name__ == '__main__':
    unittest2.main()
