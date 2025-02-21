import unittest
from Supervisor.SupervisorImpl import SupervisorImpl


class CommandTest(unittest.TestCase):
    """Test the Supervisor.command() method"""

    def setUp(self):
        self.s = SupervisorImpl()

    def test_unknown_command(self):
        """Verify the answer is the exception message and success is False"""
        cmd = 'pippo'
        success, answer = self.s.command(cmd)
        self.assertFalse(success)
        self.assertRegex(answer, f"Error - command '{cmd}' does not exist")
        cmd = 'pippo=='
        success, answer = self.s.command(cmd)
        self.assertFalse(success)
        self.assertRegex(answer, "Error - invalid command: maybe there are")

    def test_wrong_parameters(self):
        """Verify the answer is the exception message and success is False"""
        success, answer = self.s.command("supervisorSetup=a,b")
        self.assertFalse(success)
        self.assertRegex(answer, "Error - expecting 1")
        success, answer = self.s.command("supervisorSetup")
        self.assertFalse(success)
        self.assertRegex(answer, "Error - expecting 1")

    def test_command_fails(self):
        """The answer is the exception message and success is False"""
        success, answer = self.s.command("supervisorSetSunAvoidance=FOO")
        self.assertFalse(success)
        self.assertRegex(answer, "Error - mode 'FOO' not in")

    def test_command_returns_None(self):
        """Verify the answer is an empty string and success is True"""
        success, answer = self.s.command("supervisorSetWindAvoidance=ON")
        self.assertTrue(success)
        self.assertFalse(answer)


if __name__ == '__main__':
    unittest.main()
