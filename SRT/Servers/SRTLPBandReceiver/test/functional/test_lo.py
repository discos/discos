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
        default = self.get_cdb_values('DefaultLO')
        self.lp.setLO([-1, -1])
        current_value = self.get_property('LO') 
        self.assertEqual(current_value, default)


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
        rf_min = self.get_cdb_values('LBandRFMin')
        values_not_allowed = [(value + 0.5) for value in rf_min]
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
        # I.e. LO=2324; rf=1300:1800; filter=1000 -> expected=0
        bandWidth = self.get_property('bandWidth') 
        if_min, if_max = self.get_ifs()
        # expected = (if_max - if_min) if (if_max - if_min) > 0 else 0.0, so:
        expected = [max(((mx - mn), 0.0)) for mn, mx in zip(if_min, if_max)]
        self.assertEqual(bandWidth, expected)


    def test_NO_default_bandWidth(self):
        # I.e. LO=2300; rf=1625:1715; filter=1000 -> expected=90
        self.lp.setMode('XXL5') # band 1625:1715
        self.lp.setLO([2300.0, 2300.0])
        bandWidth = self.get_property('bandWidth') 
        if_min, if_max = self.get_ifs()
        expected = [max(((mx - mn), 0.0)) for mn, mx in zip(if_min, if_max)]
        self.assertEqual(bandWidth, expected)


    def test_default_initialFrequency(self):
        # I.e. LO=0; rf_min=1300 -> if_min=1300
        initialFrequency = self.get_property('initialFrequency') 
        if_min, if_max = self.get_ifs()
        self.assertEqual(initialFrequency, if_min)


    def test_NO_default_initialFrequency(self):
        # I.e. LO=2300; rf_min=1625 -> if_min=-625
        self.lp.setMode('XXL5') # band 1625:1715
        self.lp.setLO([2300.0, 2300.0])
        initialFrequency = self.get_property('initialFrequency') 
        if_min, if_max = self.get_ifs()
        self.assertEqual(initialFrequency, if_min)


    def test_lowpassfilter_setMode_before_setLo(self):
        """The lowpass filter cuts the IF band: setMode before setLO"""
        self.lp.setMode('XXL4')  # RF band -> 1300:1800
        self.lp.setLO([600.0, 600.0])  # IF band -> 700:1200
        bandWidth = self.get_property('bandWidth') 
        self.assertEqual(bandWidth, [300.0, 300])


    def test_lowpassfilter_setMode_after_setLo(self):
        """The lowpass filter cuts the IF band: setMode after setLO"""
        self.lp.setLO([700.0, 700.0])  
        self.lp.setMode('XXL5') # band 1625:1715 -> IF band -> 925:1015
        bandWidth = self.get_property('bandWidth') 
        self.assertEqual(bandWidth, [75.0, 75])


    def test_setLO_moves_IF_over_lowpassfilter(self):
        """setLO can not move the IF over the low pass IF filter"""
        self.lp.setMode('XXL4')
        filter_max = self.get_cdb_values('LowpassFilterMax')
        rf_min = self.get_cdb_values('LBandRFMin')
        lo = [(rf - filter - 1.0) for (rf, filter) in zip(rf_min, filter_max)]
        with self.assertRaisesRegexp(ComponentErrorsEx, 'outside the low pass'):
            self.lp.setLO(lo) 


    def test_setMode_moves_band_over_lowpassfilter(self):
        """setMode can not move the IF band over the low pass IF filter"""
        # Example: Filter=1000; XXL4 (1300:1800); LO=400; IF=900:1000
        # setMode('XXL5') -> IF 1225:1315 over lowpass filter
        self.lp.setMode('XXL4') # Band 1300:1800
        filter_max = self.get_cdb_values('LowpassFilterMax')
        rf_min = self.get_cdb_values('LBandRFMin')
        filter_max = self.get_cdb_values('LowpassFilterMax')
        rf_min = self.get_cdb_values('LBandRFMin')
        lo = [(rf - filter + 100.0) for (rf, filter) in zip(rf_min, filter_max)]
        self.lp.setLO(lo) # IF (filter_max-100):filter_max -> bandwidth 100MHz
        with self.assertRaisesRegexp(ReceiversErrorsEx, 'outside the low pass'):
            self.lp.setMode('XXL5') # RF band 1625:1715


    def get_ifs(self):
        """Return the current IFs, taking in account the low pass filter"""
        rf_min = self.get_cdb_values('LBandRFMin')
        rf_max = self.get_cdb_values('LBandRFMax')
        lo = self.get_property('LO') 
        get_plain_ifs = lambda rf, lo: rf - lo
        # I.e. rf_min = [1300, 1300], rf_max = [1800, 1800], lo = [1000, 1000]
        if_min = map(get_plain_ifs, rf_min, lo)  # -> [300, 300]
        if_max = map(get_plain_ifs, rf_max, lo)  # -> [800, 800]
        filter = self.get_cdb_values('LowpassFilterMax')
        for i, value in enumerate(filter):
            if if_max[i] < value: 
                continue
            else:
                if_max[i] = value 
        return if_min, if_max # A tuple of tuples

        
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
