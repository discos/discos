import os
import sys
import time
import unittest
import subprocess

def run(test_case):
    # Use the simulators and the testing CDB
    server_name = 'receiverboard' if test_case.telescope == 'SRT' else ''
    if not server_name:
        sys.exit(0) 

    FNULL = open(os.devnull, 'w')
    try:
        subprocess.Popen(['%s-sim' %server_name, 'start'], stdout=FNULL, stderr=FNULL)
        time.sleep(1) # Give the server the time to start
        suite = unittest.TestSuite()
        tests = unittest.TestLoader().loadTestsFromTestCase(test_case)
        suite.addTests(tests)
        print 'Running the tests using the antenna simulators...'
        unittest.TextTestRunner(verbosity=2).run(suite)
    finally:
        subprocess.Popen(['%s-sim' % server_name, 'stop'], stdout=FNULL, stderr=FNULL)
        time.sleep(2) # Give the server the time to stop
