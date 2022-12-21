import unittest
from SRTMistral.SRTMistralImpl import SRTMistralImpl


class CommandTest(unittest.TestCase):
    """Test the command() method"""

    def setUp(self):
        self.mistral = SRTMistralImpl()

    def test_unknown_command(self):
        cmd = 'pippo'
        success, answer = self.mistral.command(cmd)
        self.assertEqual(success, False)
        self.assertRegex(answer, f'ERROR: command `{cmd}` does not exist')

    def test_invalid_command(self):
        cmd = 'pippo=='
        success, answer = self.mistral.command(cmd)
        self.assertEqual(success, False)
        self.assertRegex(answer, 'ERROR: invalid command, maybe there are')

    def test_missing_arguments(self):
        cmd = 'test'
        success, answer = self.mistral.command(cmd)
        self.assertEqual(success, False)
        self.assertRegex(answer, 'ERROR: missing arguments, type help')

    def test_wrong_arguments(self):
        cmd = 'test=1'
        success, answer = self.mistral.command(cmd)
        self.assertEqual(success, False)
        self.assertRegex(answer, 'ERROR: wrong arguments, type help')

    def test_no_method_for_command(self):
        cmd = 'test_no_method'
        success, answer = self.mistral.command(cmd)
        self.assertEqual(success, False)
        self.assertRegex(answer, 'as no attribute')


if __name__ == '__main__':
    unittest.main()
