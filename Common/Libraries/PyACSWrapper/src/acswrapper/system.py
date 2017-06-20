__all__ = ['acs']

import os
import time
import signal
import datetime
from subprocess import Popen, call

from Acspy.Util.ACSCorba import getManager
from acswrapper import logfile


class ACS(object):

    def __new__(cls, *args, **kwargs):
        if not hasattr(cls, '_instance'):
            cls._instance = object.__new__(cls, *args, **kwargs)
            cls._start_process = None
        return cls._instance

    def start(self):
        """Start ACS and wait until it is running."""
        with open(logfile, 'a') as outfile:
            if not self.is_running():
                self._start_process = Popen(
                    'acsStart', stdout=outfile, stderr=outfile,
                    shell=True, preexec_fn=os.setsid)
                self.wait_until_running()
            else:
                outfile.write('ACS is already running')
    
    def stop(self):
        """Stop ACS and wait until killACS terminates."""
        with open(logfile, 'a') as outfile:
            if self._start_process is not None:
                os.killpg(os.getpgid(self._start_process.pid), signal.SIGTERM) 
                self._start_process = None
            Popen('acsStop', stdout=outfile, stderr=outfile, shell=True)
            self.wait_until_not_running()
            if self.is_running():
                outfile.write('acsStop is not able to stop ACS')
            call('killACS', stdout=outfile, stderr=outfile, shell=True)
            if self.is_running():
                outfile.write('killACS is not able to stop ACS')
    
    def wait_until_running(self, timeout=180):
        """Wait until ACS is running."""
        t0 = datetime.datetime.now()
        while (datetime.datetime.now() - t0).seconds < timeout:
            if self.is_running():
                break
            else:
                time.sleep(0.5)
    
    def wait_until_not_running(self, timeout=240):
        """Wait until ACS is not more running."""
        t0 = datetime.datetime.now()
        while (datetime.datetime.now() - t0).seconds < timeout:
            if not self.is_running():
                break
            else:
                time.sleep(0.5)
    
    def is_running(self):
        """Return True if ACS is running."""
        try:
            mng = getManager()
            return bool(mng and mng.ping())
        except Exception:
            return False


acs = ACS()
