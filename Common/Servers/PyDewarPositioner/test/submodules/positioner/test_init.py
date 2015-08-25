import unittest2
import random
import time
from DewarPositioner.positioner import Positioner, NotAllowedError
from DewarPositioner.cdbconf import CDBConf


class PositionerInitTest(unittest2.TestCase):

    def test___init__(self):
        """Verify the right behavior of the initializer"""
        cdbconf = CDBConf()
        p = Positioner(cdbconf)
        self.assertEqual(p.isUpdating(), False)
        self.assertEqual(p.isConfigured(), False)
        self.assertEqual(p.isTerminated(), True)
        self.assertEqual(p.getOffset(), 0.0)
        self.assertRaises(NotAllowedError, p.getDeviceName)
        self.assertRaises(NotAllowedError, p.startUpdating, 'axis', 'sector', 1, 1)

if __name__ == '__main__':
    unittest2.main()
