from __future__ import with_statement

import unittest2
from Acspy.Clients.SimpleClient import PySimpleClient
from ComponentErrors import ComponentErrorsEx

class TestSetLO(unittest2.TestCase):

    def setUp(self):
        client = PySimpleClient()
        self.lp = client.getComponent('RECEIVERS/SRTLPBandReceiver')

    def test_operation_not_allowed(self):
        """Raise an exception because the LBand has no local oscillator"""
        with self.assertRaises(ComponentErrorsEx):
            self.lp.setLO([0, 0])


if __name__ == '__main__':
    unittest2.main()
