from __future__ import with_statement

import unittest2
from Acspy.Clients.SimpleClient import PySimpleClient

class TestSetLO(unittest2.TestCase):

    def setUp(self):
        client = PySimpleClient()
        self.lp = client.getComponent('RECEIVERS/SRTLPBandReceiver')

    def test_do_not_raise_exception(self):
        """Do not raise any exception"""
        self.lp.setLO([0, 0]) # Do nothing


if __name__ == '__main__':
    unittest2.main()
