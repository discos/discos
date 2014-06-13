import unittest2
import random
import time
from DewarPositioner.positioner import Positioner, NotAllowedError


class PositionerInitTest(unittest2.TestCase):

    def test___init__(self):
        """Verify the right behavior of the initializer"""
        p = Positioner()
        self.assertEqual(p.isUpdating(), False)
        self.assertEqual(p.isConfigured(), False)
        self.assertEqual(p.isTerminated(), True)
        self.assertEqual(p.getOffset(), 0.0)
        self.assertEqual(p.getStartingPosition(), 0.0)
        self.assertRaises(NotAllowedError, p.getDeviceName)
        self.assertRaises(NotAllowedError, p._start, '', '') # TODO: call startUpdating()

if __name__ == '__main__':
    unittest2.main()
