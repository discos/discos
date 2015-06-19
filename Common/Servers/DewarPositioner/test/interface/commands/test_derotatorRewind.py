import unittest2
import mocker
import time
from DewarPositioner.DewarPositionerImpl import DewarPositionerImpl


class DerotatorRewindTest(unittest2.TestCase):
    """Test the derotatorRewind command"""


    def test_rewind(self):
        dp = DewarPositionerImpl()
        dp.command('derotatorSetup=KKG')
        success, answer = dp.command('derotatorIsRewinding')
        self.assertEqual(answer, 'False')
        time.sleep(2)
        success, answer = dp.command('derotatorRewind=6')
        self.assertRegexpMatches(answer, 'cannot rewind the derotator')


if __name__ == '__main__':
    unittest2.main()
