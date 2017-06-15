import os
import sys
import time
import signal
import unittest
import subprocess


def run(test_case):
    # Use the simulators and the testing CDB
    server_name = 'srt-mscu' if test_case.telescope == 'SRT' else ''
    if not server_name:
        sys.exit(0) 

    try:
        FNULL = open(os.devnull, 'w')
        process = subprocess.Popen(
            '%s-sim start' % server_name, stdout=FNULL, stderr=FNULL,
            shell=True, preexec_fn=os.setsid)
        time.sleep(1) # Give the server the time to start
        suite = unittest.TestSuite()
        tests = unittest.TestLoader().loadTestsFromTestCase(test_case)
        suite.addTests(tests)
        print 'Running the tests using the antenna simulators...'
        unittest.TextTestRunner(verbosity=2).run(suite)
    finally:
        os.killpg(os.getpgid(process.pid), signal.SIGTERM) 
        time.sleep(2) # Give the server the time to stop
        FNULL.close()
