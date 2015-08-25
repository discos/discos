import unittest2
from DewarPositioner.positioner import Status

class Dec2BinTest(unittest2.TestCase):

    def test_return_the_required_length(self):
        """The lenght of the string must be as specified"""
        for i in range(1000):
            self.assertEqual(len(Status.dec2bin(i, 6)), 6)

    def test_expected_value(self):
        self.assertEqual(Status.dec2bin(0, 7), '0'*7)

if __name__ == '__main__':
    unittest2.main()
