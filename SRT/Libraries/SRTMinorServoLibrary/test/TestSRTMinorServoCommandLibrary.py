#!/usr/bin/env python
import unittest
import time
import SRTMinorServoCommandLibrary

class TestPySRTMinorServoCommandLibrary(unittest.TestCase):

    def test_status(self):
        command = SRTMinorServoCommandLibrary.status()
        expected_command = 'STATUS\r\n'
        self.assertEqual(command, expected_command)

    def test_status_servo(self):
        command = SRTMinorServoCommandLibrary.status('PFP')
        expected_command = 'STATUS=PFP\r\n'
        self.assertEqual(command, expected_command)

    def test_setup(self):
        command = SRTMinorServoCommandLibrary.setup('KKG')
        expected_command = 'SETUP=KKG\r\n'
        self.assertEqual(command, expected_command)

    def test_stow(self):
        command = SRTMinorServoCommandLibrary.stow('SRP')
        expected_command = 'STOW=SRP,1\r\n'
        self.assertEqual(command, expected_command)

    def test_stow_position(self):
        command = SRTMinorServoCommandLibrary.stow('SRP', 2)
        expected_command = 'STOW=SRP,2\r\n'
        self.assertEqual(command, expected_command)

    def test_stop(self):
        command = SRTMinorServoCommandLibrary.stop('SRP')
        expected_command = 'STOP=SRP\r\n'
        self.assertEqual(command, expected_command)

    def test_preset(self):
        command = SRTMinorServoCommandLibrary.preset('PFP', [0.1, 1.1, 2.1])
        expected_command = 'PRESET=PFP,0.1,1.1,2.1\r\n'
        self.assertEqual(command, expected_command)

    def test_programTrack(self):
        now = time.time()
        command = SRTMinorServoCommandLibrary.programTrack('PFP', 0, 0, [0.1, 1.1, 2.1], now)
        now = int(now * 1000)
        expected_command = 'PROGRAMTRACK=PFP,0,0,%d,0.1,1.1,2.1\r\n' % now
        self.assertEqual(command, expected_command)
        for i in range(1, 10):
            command = SRTMinorServoCommandLibrary.programTrack('PFP', 0, i, [0.1, 1.1, 2.1])
            expected_command = 'PROGRAMTRACK=PFP,0,%d,*,0.1,1.1,2.1\r\n' % i
            self.assertEqual(command, expected_command)

    def test_offset(self):
        command = SRTMinorServoCommandLibrary.offset('PFP', [0.1, 1.1, 2.1])
        expected_command = 'OFFSET=PFP,0.1,1.1,2.1\r\n'
        self.assertEqual(command, expected_command)


if __name__ == '__main__':
    unittest.main()
