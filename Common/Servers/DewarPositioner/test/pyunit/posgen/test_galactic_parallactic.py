import unittest2
import mocker
from math import sin, cos, tan, atan, radians, degrees, pi, atan2
from DewarPositioner.posgenerator import PosGenerator, PosGeneratorError


class PosGeneratorGalacticParallacticTest(unittest2.TestCase):

    def setUp(self):
        self.m = mocker.Mocker()
        self.source = self.m.mock()
        self.posgen = PosGenerator(zdtimeout=2)

    def tearDown(self):
        self.m.restore()
        self.m.verify()

    def test_ra_dec_values(self):
        """Raise a PosGeneratorError if cannot get the (ra, dec) values"""
        source = self.m.mock()
        mocker.expect(source.getApparentCoordinates(mocker.ANY)).result((None, None))
        mocker.expect(source._get_name()).result('mocker')
        self.m.replay()
        gen = self.posgen.galactic_parallactic(source, siteInfo={'latitude': 39})
        self.assertRaises(PosGeneratorError, gen.next)

    def test_right_behavior(self):
        """Generate 3 positions and do not stop in case of isolated zero div"""
        azimuths = (45, 45, 90, 45)
        elevations = (45, 45, 0, 45) # The value 0 will cause a zero division
        ra, dec = (2.0, 0.5)
        ra0 = 3.3660332687500043 # Radians
        dec0 = 0.47347728280415174 # Radians
        latitude = 0
        source = self.m.mock()
        for (az, el) in zip(azimuths, elevations):
            mocker.expect(
                    source.getApparentCoordinates(mocker.ANY)
                    ).result((radians(az), radians(el), ra, dec) + (None,)*3)
            self.m.count(1)
        self.m.replay()
        gen = self.posgen.galactic_parallactic(source, siteInfo={'latitude': latitude})
        latitude = radians(latitude)
        for azd, eld in zip(azimuths, elevations):
            az = radians(azd)
            el = radians(eld)
            p = atan2(-sin(az), (tan(latitude)*cos(el) - sin(el)*cos(az)))
            gp = atan2(sin(ra-ra0), cos(dec)*tan(dec0) - sin(dec)*cos(ra-ra0))
            self.assertAlmostEqual(degrees(p + gp), gen.next(), delta=0.01)


if __name__ == '__main__':
    unittest2.main()
