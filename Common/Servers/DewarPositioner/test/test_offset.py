import unittest2
from Acspy.Clients.SimpleClient import PySimpleClient
from ComponentErrors import NotAllowedEx


class OffsetTest(unittest2.TestCase):
    """Test the DewarPositioner setOffset() and getOffset() methods"""
    def setUp(self):
        client = PySimpleClient()
        self.positioner = client.getComponent('RECEIVERS/DewarPositioner')
        self.positioner.setup('KKG')

    def test_xxxOffset(self):
        """Verify after setting an offset, the value is really applyed"""
        offset = 5
        self.positioner.setOffset(offset)
        self.assertAlmostEqual(self.positioner.getOffset(), offset, places=3)

if __name__ == '__main__':
    unittest2.main()
