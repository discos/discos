import os
import time
import datetime
from subprocess import Popen, PIPE

__author__ = "Marco Buttu <mbuttu@oa-cagliari.inaf.it>"


ACSDATA = os.getenv('ACSDATA')
logfile = os.path.join(ACSDATA, 'logs', 'testing.log')


class Container(object):

    def __init__(self, name, lang):
        self.name = name
        self.lang = lang

    def start(self):
        with open(logfile, 'a') as outfile:
            Popen(
                ['acsStartContainer', '-%s' % self.lang, self.name],
                stdout=outfile)

    def stop(self):
        with open(logfile, 'a') as outfile:
            p = Popen(['acsStopContainer', self.name], stdout=outfile)
            p.wait()  # Block until acsStopContainer exits

    def wait_until_running(self, timeout=10):
        t0 = datetime.datetime.now()
        while (datetime.datetime.now() - t0).seconds < timeout:
            if self.is_running():
                break
            else:
                time.sleep(0.5)

    def is_running(self):
        pipes = Popen(['acsContainersStatus'], stdout=PIPE, stderr=PIPE)
        out, err = pipes.communicate()
        return ('%s container is running' % self.name) in out
