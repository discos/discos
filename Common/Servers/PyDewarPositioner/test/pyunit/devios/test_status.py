import unittest2
import mocker
from DewarPositioner.devios import StatusDevIO


class StatusDevIOTest(unittest2.TestCase):

    def setUp(self):
        self.m = mocker.Mocker()
        self.device = self.m.mock()

    def tearDown(self):
        self.m.restore()
        self.m.verify()

    def test_singlebit(self):
        """Test the result is as excpected"""
        self.device.getStatus()
        self.m.result('100000')
        self.m.replay()
        devio = StatusDevIO(self.device)
        self.assertEqual(devio.read(), 2**5)

    def test_severalbits(self):
        """Test the result is as excpected"""
        self.device.getStatus()
        self.m.result('111001')
        self.m.replay()
        devio = StatusDevIO(self.device)
        self.assertEqual(devio.read(), 2**5 + 2**4 + 2**3 + 2**0)


if __name__ == '__main__':
    unittest2.main()
