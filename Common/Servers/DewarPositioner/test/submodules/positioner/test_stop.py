import unittest2
import threading
import random
import time
from maciErrType import CannotGetComponentEx
from Acspy.Clients.SimpleClient import PySimpleClient
from DewarPositioner.positioner import Positioner, PositionerError


class PositionerStopTest(unittest2.TestCase):

    def test_stopped(self):
        try:
            client = PySimpleClient()
            device = client.getComponent('RECEIVERS/SRTKBandDerotator')
        except CannotGetComponentEx:
            print '\nINFO -> component not available: we will use a mock device'
            from DewarPositionerMockers.mock_components import MockDevice
            device = MockDevice()

        cdb_info = {
                'UpdatingTime': 0.1,
                'RewindingTimeout': 1.5,
                'RewindingSleepTime': 1
        }
        p = Positioner(cdb_info)
        p.setup(siteInfo={}, source=None, device=device)
        def foo():
            time.sleep(0.5)
            yield 0
        time.sleep(1)
        self.assertEqual(p.isUpdating(), False)
        self.assertEqual(p.isTerminated(), True)
        t = threading.Thread(
                name='test_stopped', 
                target=p.updatePosition, 
                args=(foo, ()))
        t.start()
        time.sleep(0.5)
        self.assertEqual(p.isUpdating(), True)
        self.assertEqual(p.isTerminated(), False)
        p.stopUpdating()
        time.sleep(0.5)
        self.assertEqual(p.isUpdating(), False)
        self.assertEqual(p.isTerminated(), True)

if __name__ == '__main__':
    unittest2.main()
