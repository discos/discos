import unittest2
import time
from DewarPositioner.DewarPositionerImpl import DewarPositionerImpl


class DerotatorParkTest(unittest2.TestCase):
    """Test the derotatorPark command"""

    def test_park(self):
        dp = DewarPositionerImpl()
        success, answer = dp.command('derotatorIsReady')
        self.assertEqual((success, answer), (True, 'False'))
        dp.command('derotatorSetup=KKG')
        time.sleep(0.5)
        success, answer = dp.command('derotatorIsReady')
        self.assertEqual((success, answer), (True, 'True'))
        success, answer = dp.command('derotatorPark')
        self.assertEqual((success, answer), (True, ''))
        time.sleep(0.5)
        success, answer = dp.command('derotatorIsReady')
        self.assertEqual((success, answer), (True, 'False'))

if __name__ == '__main__':
    unittest2.main()
