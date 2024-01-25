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
        expected_command = 'PRESET=PFP,0.100000,1.100000,2.100000\r\n'
        self.assertEqual(command, expected_command)

    def test_programTrack(self):
        now = time.time()
        command = SRTMinorServoCommandLibrary.programTrack('PFP', 0, 0, [0.1, 1.1, 2.1], now)
        expected_command = f'PROGRAMTRACK=PFP,0,0,{now},0.100000,1.100000,2.100000\r\n'
        self.assertEqual(command, expected_command)
        for i in range(1, 10):
            command = SRTMinorServoCommandLibrary.programTrack('PFP', 0, i, [0.1, 1.1, 2.1])
            expected_command = f'PROGRAMTRACK=PFP,0,{i},*,0.100000,1.100000,2.100000\r\n'
            self.assertEqual(command, expected_command)

    def test_offset(self):
        command = SRTMinorServoCommandLibrary.offset('PFP', [0.1, 1.1, 2.1])
        expected_command = 'OFFSET=PFP,0.100000,1.100000,2.100000\r\n'
        self.assertEqual(command, expected_command)

    def test_parseAnswer(self):
        answer = "OUTPUT:GOOD,1665743366.123456,CURRENT_CONFIG=21|SIMULATION_ENABLED=34|PLC_TIME=78|PLC_VERSION=69|CONTROL=14|POWER=38|EMERGENCY=69|ENABLED=51|OPERATIVE_MODE=94"
        args = {
            'OUTPUT': 'GOOD',
            'TIMESTAMP': 1665743366.123456,
            'CURRENT_CONFIG': 21,
            'SIMULATION_ENABLED': 34,
            'PLC_TIME': 78,
            'PLC_VERSION': 69,
            'CONTROL': 14,
            'POWER': 38,
            'EMERGENCY': 69,
            'ENABLED': 51,
            'OPERATIVE_MODE': 94
        }
        self.assertEqual(
            SRTMinorServoCommandLibrary.parseAnswer(answer),
            args
        )

        answer = "OUTPUT:GOOD,1665743366.654321"
        args = {
            "OUTPUT": "GOOD",
            "TIMESTAMP": 1665743366.654321
        }
        self.assertEqual(
            SRTMinorServoCommandLibrary.parseAnswer(answer),
            args
        )

        answer = "OUTPUT:GOOD,CURRENT_CONFIG=21|SIMULATION_ENABLED=34|PLC_TIME=78|PLC_VERSION=69|CONTROL=14|POWER=38|EMERGENCY=69|ENABLED=51|OPERATIVE_MODE=94"
        self.assertEqual(SRTMinorServoCommandLibrary.parseAnswer(answer), {})

        answer = "OUTPUT:123456"
        self.assertEqual(SRTMinorServoCommandLibrary.parseAnswer(answer), {})

        answer = "OUTPUT:GOOD,12345,67890"
        self.assertEqual(SRTMinorServoCommandLibrary.parseAnswer(answer), {})

        answer = "OUTPUT:GOOD,12345.ABCD"
        self.assertEqual(SRTMinorServoCommandLibrary.parseAnswer(answer), {})


if __name__ == '__main__':
    unittest.main()
