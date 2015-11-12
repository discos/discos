from __future__ import with_statement

import os
from xml.etree import ElementTree

import unittest2
from Acspy.Clients.SimpleClient import PySimpleClient
from ComponentErrors import ComponentErrorsEx
from Acspy.Util import ACSCorba


class TestLO(unittest2.TestCase):

    telescope = os.getenv('TARGETSYS')

    @classmethod
    def setUpClass(cls):
        cls.client = PySimpleClient()
        cls.lp = cls.client.getComponent('RECEIVERS/SRTLPBandReceiver')


    @classmethod
    def tearDownClass(cls):
        cls.client.releaseComponent('RECEIVERS/SRTLPBandReceiver')
        cls.client.disconnect()


    def setUp(self):
        self.lp.activate('LLP')


    def tearDown(self):
        self.lp.deactivate()


    def test_keep_the_current_value(self):
        """In case of setLO([-1, -1]), keep the current value"""
        min_values = get_cdb_values('LOMin')
        target = [(min_value + 1) for min_value in min_values]
        self.lp.setLO(target)
        self.lp.setLO([-1, -1])
        current_value = get_property(self.lp, 'LO') 
        self.assertEqual(current_value, target)


    def test_set_below_minimum_value(self):
        """LO below the minimum allowed value: operation not allowed"""
        min_values = get_cdb_values('LOMin')
        values_not_allowed = [(value - 0.5) for value in min_values]
        with self.assertRaises(ComponentErrorsEx):
            self.lp.setLO(values_not_allowed)


    def test_set_above_minimum_value(self):
        """LO above the minimum allowed value: operation not allowed"""
        max_values = get_cdb_values('LOMax')
        values_not_allowed = [(value + 0.5) for value in max_values]
        with self.assertRaises(ComponentErrorsEx):
            self.lp.setLO(values_not_allowed)


def get_cdb_values(attr_name, type_=float):
    dal = ACSCorba.cdb()
    dao = dal.get_DAO('alma/DataBlock/SRTLPBandReceiver/Modes/L3L4')
    body = ElementTree.fromstring(dao)
    str_values = body.attrib[attr_name].split()
    return [type_(item) for item in str_values]


def get_property(component_ref, property_name):
    property_obj = getattr(component_ref, '_get_%s' % property_name)
    value, comp = property_obj().get_sync()
    return value


if __name__ == '__main__':
    if 'Configuration' in os.getenv('ACS_CDB'):
        unittest2.main(verbosity=2, failfast=True) # Real test using the antenna CDB
    else:
        from PyLPBandTest import simunittest
        simunittest.run(TestLO)
