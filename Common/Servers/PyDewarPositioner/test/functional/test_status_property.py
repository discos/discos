import unittest
import time
from Acspy.Clients.SimpleClient import PySimpleClient
from ComponentErrors import NotAllowedEx


class StatusPropertyTest(unittest.TestCase):
    """..."""
    def setUp(self):
        client = PySimpleClient()
        self.positioner = client.getComponent('RECEIVERS/DewarPositioner')

    def tearDown(self):
        if self.positioner.isReady():
            self.positioner.park()

    def test_not_configured(self):
        """Vefify it clears the offset and puts the derotator in the new position."""
        status_obj = self.positioner._get_status()
        value, compl = status_obj.get_sync()
        self.assertEqual(value, 0)

    def test_configured(self):
        self.positioner.setup('KKG')
        status_obj = self.positioner._get_status()
        time.sleep(2)
        value, compl = status_obj.get_sync()
        self.assertEqual(value, 3)


if __name__ == '__main__':
    unittest.main()
