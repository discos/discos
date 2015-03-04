import os
import sys
import time
import unittest2
import subprocess

def run(test_case):
    # Use the simulators and the testing CDB
    server_name = 'mscu' if os.getenv('TARGETSYS') == 'SRT' else ''
    if not server_name:
        sys.exit(0) 

    FNULL = open(os.devnull, 'w')
    try:
        subprocess.Popen(['%s-start' %server_name], stdout=FNULL, stderr=FNULL)
        time.sleep(0.5) # Give the server the time to start
        suite = unittest2.TestSuite()
        tests = unittest2.TestLoader().loadTestsFromTestCase(test_case)
        suite.addTests(tests)
        print 'Running the tests using the antenna simulators...'
        unittest2.TextTestRunner().run(suite)
    finally:
        subprocess.Popen(['%s-stop' %server_name], stdout=FNULL, stderr=FNULL)
        time.sleep(1) # Give the server the time to stop
