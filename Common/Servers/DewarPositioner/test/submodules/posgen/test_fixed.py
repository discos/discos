import unittest2
import mocker
from math import sin, cos, tan, atan, radians, degrees, pi
from DewarPositioner.posgenerator import PosGenerator, PosGeneratorError


class PosGeneratorFixedTest(unittest2.TestCase):

    def setUp(self):
        self.m = mocker.Mocker()
        self.source = self.m.mock()
        self.posgen = PosGenerator(timeout=2)

    def tearDown(self):
        self.m.restore()
        self.m.verify()

    def test_elevation(self):
        """Raise a PosGeneratorError in case of wrong site_info"""
        gen = self.posgen.fixed(self.source, site_info={'LATITUDE': 39})
        self.assertRaises(PosGeneratorError, gen.next)
        gen = self.posgen.fixed(self.source, site_info=11)
        self.assertRaises(PosGeneratorError, gen.next)

    def test_property_objs(self):
        """Raise a PosGeneratorError if cannot get source property objects"""
        self.source._get_azimuth()
        self.m.throw(RuntimeError)
        mocker.expect(self.source._get_name()).result('mocker')
        self.m.replay()
        gen = self.posgen.fixed(self.source, site_info={'latitude': 39})
        self.assertRaises(PosGeneratorError, gen.next)

    def test_property_values(self):
        """Raise a PosGeneratorError if cannot get source property values"""
        az_obj = self.m.mock()
        el_obj = self.m.mock()
        mocker.expect(az_obj.get_sync()).result((100, None))
        el_obj.get_sync()
        self.m.throw(RuntimeError)
        mocker.expect(self.source._get_azimuth()).result(az_obj)
        mocker.expect(self.source._get_elevation()).result(el_obj)
        mocker.expect(self.source._get_name()).result('mocker')
        self.m.replay()
        gen = self.posgen.fixed(self.source, site_info={'latitude': 39})
        self.assertRaises(PosGeneratorError, gen.next)

    def test_completion_error(self):
        """Raise a PosGeneratorError if the completion takes an error"""
        cmpl_az = self.m.mock()
        cmpl_el = self.m.mock()
        mocker.expect(cmpl_az.code).result(1)
        az_obj = self.m.mock()
        el_obj = self.m.mock()
        mocker.expect(az_obj.get_sync()).result((100, cmpl_az))
        mocker.expect(el_obj.get_sync()).result((45, cmpl_el))
        mocker.expect(self.source._get_azimuth()).result(az_obj)
        mocker.expect(self.source._get_elevation()).result(el_obj)
        mocker.expect(self.source._get_name()).result('mocker')
        self.m.replay()
        gen = self.posgen.fixed(self.source, site_info={'latitude': 39})
        self.assertRaises(PosGeneratorError, gen.next)

    def test_zero_division_timeout(self):
        """Raise a PosGeneratorError if case of countinuos zero division"""
        cmpl_az = self.m.mock()
        cmpl_el = self.m.mock()
        mocker.expect(cmpl_az.code).result(0)
        self.m.count(1, None)
        mocker.expect(cmpl_el.code).result(0)
        self.m.count(1, None)
        az_obj = self.m.mock()
        el_obj = self.m.mock()
        mocker.expect(az_obj.get_sync()).result((90, cmpl_az))
        self.m.count(1, None)
        mocker.expect(el_obj.get_sync()).result((0, cmpl_el))
        self.m.count(1, None)
        mocker.expect(self.source._get_azimuth()).result(az_obj)
        self.m.count(1, None) 
        mocker.expect(self.source._get_elevation()).result(el_obj)
        self.m.count(1, None)
        self.m.replay()
        gen = self.posgen.fixed(self.source, site_info={'latitude': 0})
        self.assertRaises(PosGeneratorError, gen.next)

    def test_right_behavior(self):
        """Generate 3 positions and do not stop in case of isolated zero div"""
        elevations = (45, 45, 0, 45) # The value 0 will cause a zero division
        azimuths = (45, 45, 90, 45)
        latitude = 0
        cmpl_az = self.m.mock()
        cmpl_el = self.m.mock()
        mocker.expect(cmpl_az.code).result(0)
        self.m.count(4)
        mocker.expect(cmpl_el.code).result(0)
        self.m.count(4)
        az_obj = self.m.mock()
        el_obj = self.m.mock()
        for (az, el) in zip(azimuths, elevations):
            mocker.expect(az_obj.get_sync()).result((az, cmpl_az))
            mocker.expect(el_obj.get_sync()).result((el, cmpl_el))
        mocker.expect(self.source._get_azimuth()).result(az_obj)
        mocker.expect(self.source._get_elevation()).result(el_obj)
        self.m.replay()
        gen = self.posgen.fixed(self.source, site_info={'latitude': latitude})
        latitude = radians(latitude)
        for azd, eld in zip(azimuths, elevations):
            el = radians(eld)
            az = radians(azd)
            try:
                tan_p = - sin(az) / (tan(latitude)*cos(el) - sin(el)*cos(az))
                p = atan(tan_p)
            except ZeroDivisionError:
                continue
            self.assertEqual(degrees(p), gen.next())


if __name__ == '__main__':
    unittest2.main()
