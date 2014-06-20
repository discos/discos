import unittest2
import random
import time
from maciErrType import CannotGetComponentEx
from Acspy.Clients.SimpleClient import PySimpleClient
from DewarPositioner.positioner import Positioner, PositionerError

from mock_components import MockDevice


class PositionerStopTest(unittest2.TestCase):

    def test_stopped(self):
        try:
            client = PySimpleClient()
            device = client.getComponent('RECEIVERS/SRTKBandDerotator')
        except CannotGetComponentEx:
            print '\nINFO -> component not available: we will use a mock device'
            from mock_components import MockDevice
            device = MockDevice()

        p = Positioner()
        p.setup(site_info={}, source=None, device=device)
        def foo():
            time.sleep(1)
            yield 0
        time.sleep(1)
        self.assertEqual(p.isUpdating(), False)
        self.assertEqual(p.isTerminated(), True)
        p._start(foo) # TODO: change calling startUpdating()
        time.sleep(1)
        self.assertEqual(p.isUpdating(), True)
        self.assertEqual(p.isTerminated(), False)
        p.stopUpdating()
        self.assertEqual(p.isUpdating(), False)
        self.assertEqual(p.isTerminated(), True)

if __name__ == '__main__':
    unittest2.main()
