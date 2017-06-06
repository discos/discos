"""Issue https://github.com/discos/discos/issues/143"""

import os
import time
import unittest
from subprocess import Popen, PIPE
from Acspy.Clients.SimpleClient import PySimpleClient


__author__ = "Marco Buttu <mbuttu@oa-cagliari.inaf.it>"

class TestContainerCrash(unittest.TestCase):

    telescope = os.getenv('STATION')
    
    def test_get_and_release_component(self):
        """The container must be alive after releasing the component"""
        client = PySimpleClient()
        srp = client.getComponent('MINORSERVO/SRP')
        pipes = Popen(['acsContainersStatus'], stdout=PIPE, stderr=PIPE)
        out, err = pipes.communicate()
        expected_regex = 'MinorServoContainer container is running'
        self.assertRegexpMatches(out, expected_regex)

        client.releaseComponent('MINORSERVO/SRP')
        time.sleep(1)
        pipes = Popen(['acsContainersStatus'], stdout=PIPE, stderr=PIPE)
        out, err = pipes.communicate()
        expected_regex = 'MinorServoContainer container is running'
        self.assertRegexpMatches(out, expected_regex)

        
if __name__ == '__main__':
    if 'Configuration' in os.getenv('ACS_CDB'):
        unittest.main(verbosity=2, failfast=True) # Real test using the antenna CDB
    else:
        from PyMinorServoTest import simunittest
        simunittest.run(TestContainerCrash)
