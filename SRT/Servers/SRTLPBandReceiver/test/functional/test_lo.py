from __future__ import with_statement

import os
from xml.etree import ElementTree

import unittest2
from Acspy.Clients.SimpleClient import PySimpleClient
from ComponentErrors import ComponentErrorsEx
from ReceiversErrors import ReceiversErrorsEx
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
        min_values = self.get_cdb_values('LOMin')
        target = [(min_value + 1) for min_value in min_values]
        self.lp.setLO(target)
        self.lp.setLO([-1, -1])
        current_value = self.get_property('LO') 
        self.assertEqual(current_value, target)


    def test_set_below_minimum_value(self):
        """LO below the minimum allowed value: not allowed"""
        min_values = self.get_cdb_values('LOMin')
        values_not_allowed = [(value - 0.5) for value in min_values]
        with self.assertRaises(ComponentErrorsEx):
            self.lp.setLO(values_not_allowed)


    def test_set_above_minimum_value(self):
        """LO above the minimum allowed value: not allowed"""
        max_values = self.get_cdb_values('LOMax')
        values_not_allowed = [(value + 0.5) for value in max_values]
        with self.assertRaises(ComponentErrorsEx):
            self.lp.setLO(values_not_allowed)


    def test_lo_inside_default_sky_band(self):
        """LO inside the default observed sky band: not allowed"""
        rfmin = self.get_cdb_values('LBandRFMin')
        values_not_allowed = [(value + 0.5) for value in rfmin]
        with self.assertRaisesRegexp(ComponentErrorsEx, 'within the band'):
            self.lp.setLO(values_not_allowed)


    def test_lo_inside_no_default_sky_band(self):
        """LO inside the (no-default) observed sky band"""
        self.lp.setMode('XXL5') # Bandwidth 1625:1715
        self.lp.setLO([1500.0, 1500.0]) # OK, it is outside the RF band
        values_not_allowed = [1650.00, 1650.00]
        with self.assertRaisesRegexp(ComponentErrorsEx, 'within the band'):
            self.lp.setLO(values_not_allowed)


    def test_setMode_sets_lo_within_sky_band(self):
        """setMode() after setLO, having LO within the band: not allowed"""
        self.lp.setMode('XXL5') # Bandwidth 1625:1715
        self.lp.setLO([1500.0, 1500.0]) # OK, it is outside the RF band
        with self.assertRaisesRegexp(ReceiversErrorsEx, 'within the band'):
            self.lp.setMode('XXL4') # Bandwidth 1300:1800


    def test_default_LO(self):
        """The LO value after setup must be the default one"""
        default = self.get_cdb_values('DefaultLO')
        current_value = self.get_property('LO') 
        self.assertEqual(default, current_value)


    def test_default_bandWidth(self):
        # I.e. LO=0; rf=1300:1800; filter=1000 -> expected=0
        bandWidth = self.get_property('bandWidth') 
        ifmin, ifmax = self.get_ifs()
        # expected = (ifmax - ifmin) if (ifmax - ifmin) > 0 else 0.0, so:
        expected = [max(((mx - mn), 0.0)) for mn, mx in zip(ifmin, ifmax)]
        self.assertEqual(bandWidth, expected)


    def test_NO_default_bandWidth(self):
        # I.e. LO=2300; rf=1625:1715; filter=1000 -> expected=90
        self.lp.setMode('XXL5') # band 1625:1715
        self.lp.setLO([2300.0, 2300.0])
        bandWidth = self.get_property('bandWidth') 
        ifmin, ifmax = self.get_ifs()
        expected = [max(((mx - mn), 0.0)) for mn, mx in zip(ifmin, ifmax)]
        self.assertEqual(bandWidth, expected)


    def test_default_initialFrequency(self):
        # I.e. LO=0; rfmin=1300 -> ifmin=1300
        initialFrequency = self.get_property('initialFrequency') 
        ifmin, ifmax = self.get_ifs()
        self.assertEqual(initialFrequency, ifmin)


    def test_NO_default_initialFrequency(self):
        # I.e. LO=2300; rfmin=1625 -> ifmin=-625
        self.lp.setMode('XXL5') # band 1625:1715
        self.lp.setLO([2300.0, 2300.0])
        initialFrequency = self.get_property('initialFrequency') 
        ifmin, ifmax = self.get_ifs()
        self.assertEqual(initialFrequency, ifmin)


    def get_ifs(self):
        """Return the current IFs, taking in account the low pass filter"""
        rfmin = self.get_cdb_values('LBandRFMin')
        rfmax = self.get_cdb_values('LBandRFMax')
        lo = self.get_property('LO') 
        get_plain_ifs = lambda rf, lo: rf - lo
        # I.e. rfmin = [1300, 1300], rfmax = [1800, 1800], lo = [1000, 1000]
        ifmin = map(get_plain_ifs, rfmin, lo)  # -> [300, 300]
        ifmax = map(get_plain_ifs, rfmax, lo)  # -> [800, 800]
        filter = self.get_cdb_values('LowpassFilterMax')
        for i, value in enumerate(filter):
            if ifmax[i] < value: 
                continue
            else:
                ifmax[i] = value 
        return ifmin, ifmax # A tuple of tuples

        
    def get_cdb_values(self, attr_name, type_=float):
        mode = self.get_property('mode')
        dal = ACSCorba.cdb()
        dao = dal.get_DAO('alma/DataBlock/SRTLPBandReceiver/Modes/%s' % mode)
        body = ElementTree.fromstring(dao)
        str_values = body.attrib[attr_name].split()
        return [type_(item) for item in str_values]


    def get_property(self, property_name):
        property_obj = getattr(self.lp, '_get_%s' % property_name)
        value, comp = property_obj().get_sync()
        return value


if __name__ == '__main__':
    if 'Configuration' in os.getenv('ACS_CDB'):
        unittest2.main(verbosity=2, failfast=True) # Real test using the antenna CDB
    else:
        from PyLPBandTest import simunittest
        simunittest.run(TestLO)
