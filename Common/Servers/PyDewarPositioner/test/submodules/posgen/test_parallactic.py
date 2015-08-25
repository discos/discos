import unittest2
import mocker
from math import sin, cos, tan, atan, radians, degrees, pi
from DewarPositioner.posgenerator import PosGenerator, PosGeneratorError


class PosGeneratorParallacticTest(unittest2.TestCase):

    def setUp(self):
        self.m = mocker.Mocker()
        self.source = self.m.mock()
        self.posgen = PosGenerator(zdtimeout=2)

    def tearDown(self):
        self.m.restore()
        self.m.verify()

    def test_wrong_site_info(self):
        """Raise a PosGeneratorError in case of wrong site_info"""
        gen = self.posgen.parallactic(self.source, siteInfo={'LATITUDE': 39})
        self.assertRaises(PosGeneratorError, gen.next)
        gen = self.posgen.parallactic(self.source, siteInfo=11)
        self.assertRaises(PosGeneratorError, gen.next)

    def test_az_el_values(self):
        """Raise a PosGeneratorError if cannot get the (az, el) values"""
        source = self.m.mock()
        mocker.expect(source.getRawCoordinates(mocker.ANY)).result((None, None))
        mocker.expect(source._get_name()).result('mocker')
        self.m.replay()
        gen = self.posgen.parallactic(source, siteInfo={'latitude': 39})
        self.assertRaises(PosGeneratorError, gen.next)

    def test_zero_division_timeout(self):
        """Raise a PosGeneratorError if case of countinuos zero division"""
        source = self.m.mock()
        mocker.expect(
                source.getRawCoordinates(mocker.ANY)
                ).result((90, 0))
        self.m.count(1, None)
        self.m.count(1, None)
        self.m.count(1, None) 
        self.m.count(1, None)
        self.m.replay()
        gen = self.posgen.parallactic(source, siteInfo={'latitude': 0})
        self.assertRaises(PosGeneratorError, gen.next)

    def test_right_behavior(self):
        """Generate 3 positions and do not stop in case of isolated zero div"""
        azimuths = (45, 45, 90, 45)
        elevations = (45, 45, 0, 45) # The value 0 will cause a zero division
        latitude = 0
        source = self.m.mock()
        for (az, el) in zip(azimuths, elevations):
            mocker.expect(
                    source.getRawCoordinates(mocker.ANY)
                    ).result((radians(az), radians(el)))
            self.m.count(1)
        self.m.replay()
        gen = self.posgen.parallactic(source, siteInfo={'latitude': latitude})
        latitude = radians(latitude)
        for azd, eld in zip(azimuths, elevations):
            az = radians(azd)
            el = radians(eld)
            try:
                tan_p = - sin(az) / (tan(latitude)*cos(el) - sin(el)*cos(az))
                p = atan(tan_p)
            except ZeroDivisionError:
                continue
            self.assertEqual(degrees(p), gen.next())


if __name__ == '__main__':
    unittest2.main()
