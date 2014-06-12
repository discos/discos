import unittest2
import random
import time
from maciErrType import CannotGetComponentEx
from DewarPositioner.positioner import Positioner, PositionerError


class PositionerStopTest(unittest2.TestCase):

    def test_stopped(self):
        p = Positioner()
        device_name = 'RECEIVERS/SRTKBandDerotator'
        p.setup(device_name)
        def foo():
            time.sleep(3)
            yield 0
        time.sleep(2)
        self.assertEqual(p.isUpdating(), False)
        self.assertEqual(p.isTerminated(), True)
        p.start(foo)
        time.sleep(1)
        self.assertEqual(p.isUpdating(), True)
        self.assertEqual(p.isTerminated(), False)
        p.stop()
        self.assertEqual(p.isUpdating(), False)
        self.assertEqual(p.isTerminated(), True)

if __name__ == '__main__':
    unittest2.main()
