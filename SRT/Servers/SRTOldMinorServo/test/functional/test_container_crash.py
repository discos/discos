"""Test issue https://github.com/discos/discos/issues/143"""

import os
import time
import unittest
from Acspy.Clients.SimpleClient import PySimpleClient

from acswrapper.system import acs
from acswrapper.containers import (
    Container, ContainerError, start_containers_and_wait,
    stop_containers_and_wait
)

__author__ = "Marco Buttu <mbuttu@oa-cagliari.inaf.it>"


class TestContainerCrash(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        if not acs.is_running():
            acs.start()
        cls.containers = [Container('MinorServoContainer', 'cpp')]
        try:
            start_containers_and_wait(cls.containers)
        except ContainerError, ex:
            cls.fail(ex.message)

    @classmethod
    def tearDownClass(cls):
        stop_containers_and_wait(cls.containers)

    def setUp(self):
        self.client = PySimpleClient()

    def tearDown(self):
        self.client.disconnect()

    def test_get_and_release_component(self):
        """The container must be alive after releasing the component"""
        self.client.getComponent('MINORSERVO/SRP')
        self.client.releaseComponent('MINORSERVO/SRP')
        time.sleep(3)
        for container in self.containers:
            self.assertTrue(container.is_running())

        
if __name__ == '__main__':
    if 'Configuration' in os.getenv('ACS_CDB'):
        unittest.main(verbosity=2, failfast=True) # Real test using the antenna CDB
    else:
        from testing import simulator
        simulator.run(TestContainerCrash, 'srt-mscu-sim')
