import unittest
import socket
import time

import simulators.backend.mistral
from simulators import backend
from simulators.server import Simulator
from simulators.utils import ACS_TO_UNIX_TIME

from Acspy.Clients.SimpleClient import PySimpleClient
from Acspy.Util import ACSCorba

component_name = 'BACKENDS/SRTMistral'


class TestCommands(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.task_time = 2
        simulators.backend.mistral.System.setup_time = cls.task_time
        simulators.backend.mistral.System.sweep_time = cls.task_time
        cls.server = Simulator(backend, system_type='mistral')
        cls.server.start(daemon=True)
        dao = ACSCorba.cdb().get_DAO_Servant(f'alma/{component_name}')
        cls.status_frequency = float(dao.get_field_data('StatusFrequency'))

    @classmethod
    def tearDownClass(cls):
        cls.server.stop()

    def setUp(self):
        self.client = PySimpleClient()
        self.component = self.client.getComponent(component_name)
        time.sleep(self.status_frequency*5)

    def tearDown(self):
        self.client.releaseComponent(component_name)
        time.sleep(self.status_frequency*5)
        self.client.disconnect()
        server_reset()

    def test_status_not_initialized(self):
        self.check_status('STATUS: OK -- system not initialized')

    def test_status_setup_in_progress(self):
        self.run_command('mistralSetup')
        self.check_status('STATUS: OK -- setup in progress')

    def test_status_setup_done(self):
        self.run_command('mistralSetup', wait_to_finish=True)
        self.check_status('STATUS: OK -- ready to run a task')

    def test_status_not_acquiring(self):
        self.check_status('not acquiring')

    def test_setup_during_setup(self):
        self.run_command('mistralSetup')
        answer = self.run_command('mistralSetup')
        self.assertRegex(answer, 'SETUP: FAIL -- setup in progress')

    def test_vna_sweep_not_initialized(self):
        answer = self.run_command('mistralVnaSweep')
        self.assertRegex(answer, 'VNA-SWEEP: FAIL -- system not initialized')

    def test_vna_sweep_during_setup(self):
        self.run_command('mistralSetup')
        answer = self.run_command('mistralVnaSweep')
        self.assertRegex(answer, 'VNA-SWEEP: FAIL -- setup in progress')

    def test_vna_sweep_during_acquisition(self):
        self.run_command('mistralSetup', wait_to_finish=True)
        self.run_command('mistralStartAcquisition')
        self.check_status('STATUS: OK -- acquisition in progress')
        answer = self.run_command('mistralVnaSweep')
        self.assertRegex(answer, 'VNA-SWEEP: FAIL -- acquisition in progress')

    def test_status_vna_sweep_in_progress(self):
        self.run_command('mistralSetup', wait_to_finish=True)
        self.run_command('mistralVnaSweep')
        self.check_status('STATUS: OK -- vna-sweep in progress')

    def test_vna_sweep_done(self):
        self.run_command('mistralSetup', wait_to_finish=True)
        self.run_command('mistralVnaSweep', wait_to_finish=True)
        self.check_status('STATUS: OK -- ready to run a task')

    def test_target_sweep_not_initialized(self):
        answer = self.run_command('mistralTargetSweep')
        self.assertRegex(answer, 'TARGET-SWEEP: FAIL -- system not initialized')

    def test_target_sweep_during_setup(self):
        self.run_command('mistralSetup')
        answer = self.run_command('mistralTargetSweep')
        self.assertRegex(answer, 'TARGET-SWEEP: FAIL -- setup in progress')

    def test_target_sweep_during_acquisition(self):
        self.run_command('mistralSetup', wait_to_finish=True)
        self.run_command('mistralStartAcquisition')
        self.check_status('STATUS: OK -- acquisition in progress')
        answer = self.run_command('mistralTargetSweep')
        self.assertRegex(answer, 'TARGET-SWEEP: FAIL -- acquisition in progress')

    def test_status_target_sweep_in_progress(self):
        self.run_command('mistralSetup', wait_to_finish=True)
        self.run_command('mistralTargetSweep')
        self.check_status('STATUS: OK -- target-sweep in progress')

    def test_target_sweep_done(self):
        self.run_command('mistralSetup', wait_to_finish=True)
        self.run_command('mistralTargetSweep', wait_to_finish=True)
        self.check_status('STATUS: OK -- ready to run a task')

    def test_start_not_initialized(self):
        answer = self.run_command('mistralStartAcquisition')
        self.assertRegex(answer, 'START: FAIL -- system not initialized')

    def test_start_during_setup(self):
        self.run_command('mistralSetup')
        answer = self.run_command('mistralStartAcquisition')
        self.assertRegex(answer, 'START: FAIL -- setup in progress')

    def test_start_during_start(self):
        self.run_command('mistralSetup', wait_to_finish=True)
        self.run_command('mistralStartAcquisition')
        self.check_status('STATUS: OK -- acquisition in progress')
        answer = self.run_command('mistralStartAcquisition')
        self.assertRegex(answer, 'START: FAIL -- acquisition in progress')

    def test_start_done(self):
        self.run_command('mistralSetup', wait_to_finish=True)
        self.run_command('mistralStartAcquisition')
        self.check_status('STATUS: OK -- acquisition in progress')

    def test_stop_not_acquiring(self):
        answer = self.run_command('mistralStopAcquisition')
        self.assertRegex(answer, 'STOP: FAIL -- not acquiring')
        self.run_command('mistralSetup', wait_to_finish=True)
        answer = self.run_command('mistralStopAcquisition')
        self.assertRegex(answer, 'STOP: FAIL -- not acquiring')

    def test_stop_when_acquiring(self):
        self.test_start_done()
        answer = self.run_command('mistralStopAcquisition')
        self.assertRegex(answer, 'STOP: OK')
        self.check_status('STATUS: OK -- ready to run a task')

    def test_set_filename(self):
        name = 'dummyname.fits'
        answer = self.run_command(f'mistralSetFilename={name}')
        self.assertRegex(answer, 'SET-FILENAME: OK')
        answer = self.run_command('mistralGetFilename')
        self.assertEqual(answer, f'GET-FILENAME: OK -- {name}')
 
    def check_status(self, message):
        # Wait for the status to be updated by the thread
        time.sleep(self.status_frequency*3)
        success, answer = self.component.command('mistralStatus')
        self.assertEqual(success, True)
        self.assertRegex(answer, message)

    def run_command(self, cmd, wait_to_finish=False):
        success, answer = self.component.command(cmd)
        time.sleep(self.task_time*1.5) if wait_to_finish else time.sleep(0.1)
        return answer

def server_reset():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.settimeout(0.5)
        try:
            sock.connect(('127.0.0.1', 12802))
            sock.recv(50)
            sock.sendall(b'?reset\r\n')
            sock.recv(50)
        except:
            pass


if __name__ == '__main__':
    unittest.main()
