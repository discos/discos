import os
import sys
import time
import signal
import unittest
import subprocess


def run(test_case, server_name='antenna'):
    # In case of `server_name=='antenna'`, the simulator will not
    # be run because we are exectuting the tests on the real hardware
    if server_name == 'antenna':
        sys.exit(0) 

    try:
        FNULL = open(os.devnull, 'w')
        process = subprocess.Popen(
            '%s start' % server_name, stdout=FNULL, stderr=FNULL,
            shell=True, preexec_fn=os.setsid)
        time.sleep(1) # Give the server the time to start
        suite = unittest.TestSuite()
        tests = unittest.TestLoader().loadTestsFromTestCase(test_case)
        suite.addTests(tests)
        print 'Running the tests using the antenna simulators...'
        unittest.TextTestRunner(verbosity=2).run(suite)
    finally:
        subprocess.Popen('%s stop' % server_name, shell=True)
        time.sleep(2) # Give the server the time to stop
        os.killpg(os.getpgid(process.pid), signal.SIGTERM) 
        FNULL.close()
