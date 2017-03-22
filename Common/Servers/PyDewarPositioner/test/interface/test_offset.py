import unittest2
import time
from Acspy.Clients.SimpleClient import PySimpleClient
from ComponentErrors import NotAllowedEx


class OffsetTest(unittest2.TestCase):
    """Test the DewarPositioner setOffset() and getOffset() methods"""
    def setUp(self):
        client = PySimpleClient()
        self.positioner = client.getComponent('RECEIVERS/DewarPositioner')
        self.positioner.setup('KKG')
        time.sleep(2)

    def tearDown(self):
        self.positioner.park()

    def test_clearOffset(self):
        """Vefify it clears the offset and puts the derotator in the new position."""
        old_offset = self.positioner.getOffset()
        old_pos = self.positioner.getPosition()
        self.positioner.clearOffset()
        time.sleep(2)
        self.assertAlmostEqual(
                self.positioner.getPosition(), 
                old_pos - old_offset,
                places=0
        )

    def test_setOffset(self):
        """Vefify it sets the offset and puts the derotator in the new position."""
        old_offset = self.positioner.getOffset()
        old_pos = self.positioner.getPosition()
        new_offset = 0.5
        self.positioner.setOffset(new_offset)
        time.sleep(2)
        offset_diff = old_offset - new_offset
        self.assertAlmostEqual(
                self.positioner.getPosition(), 
                old_pos - offset_diff,
                places=1
        )

if __name__ == '__main__':
    unittest2.main()
