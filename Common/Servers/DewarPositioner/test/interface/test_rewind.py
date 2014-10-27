from __future__ import with_statement
import unittest2
import time
from Acspy.Clients.SimpleClient import PySimpleClient
from ComponentErrors import OperationErrorEx


class RewindTest(unittest2.TestCase):
    """Test the DewarPositioner.rewind() method"""
    def setUp(self):
        client = PySimpleClient()
        self.positioner = client.getComponent('RECEIVERS/DewarPositioner')
        self.positioner.setup('KKG')

    def tearDown(self):
        if self.positioner.isReady():
            self.positioner.park()

    def test_number_of_steps_oor(self):
        """Raise OperationError when the number of steps is out of range"""
        with self.assertRaisesRegexp(OperationErrorEx, 'actual pos: {0.0}'):
            self.positioner.rewind(4)

    def test_not_positive_number_of_steps(self):
        """Raise OperationError when the number of steps is not positive"""
        with self.assertRaisesRegexp(OperationErrorEx, 'steps must be positive'):
            self.positioner.rewind(0)



if __name__ == '__main__':
    unittest2.main()
