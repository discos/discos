import os
import time
import signal
import datetime

from subprocess import Popen, PIPE
from acswrapper import logfile


class Container(object):

    def __init__(self, name, lang):
        self.name = name
        self.lang = lang
        self.start_process = None

    def start(self):
        """Start the container and do not wait for the process to finish.

        This is a non blocking call.  The method calls the acsStartContainer
        command and returns immediately without waiting for the process to
        finish its job.  To know if the container is already started, call
        the Container.is_running() method."""
        with open(logfile, 'a') as outfile:
            if not self.is_running():
                self.start_process = Popen(
                    'acsStartContainer -%s %s' % (self.lang, self.name),
                    stdout=outfile, stderr=outfile, shell=True,
                    preexec_fn=os.setsid)
            else:
                outfile.write('Container %s already running' % self.name)

    def stop(self):
        """Stop the container and do not wait for the process to finish.

        This is a non blocking call.  The method calls the acsStopContainer
        command and returns immediately without waiting for the process to
        finish its job.  To know if the container is already stopped, call
        the Container.is_running() method."""
        with open(logfile, 'a') as outfile:
            Popen(
                'acsStopContainer %s' % self.name,
                stdout=outfile, stderr=outfile, shell=True)
        if self.start_process is not None:
            os.killpg(os.getpgid(self.start_process.pid), signal.SIGTERM) 
            self.start_process = None

    def wait_until_running(self, timeout=10):
        """Wait until the container is running."""
        t0 = datetime.datetime.now()
        while (datetime.datetime.now() - t0).seconds < timeout:
            if self.is_running():
                break
            else:
                time.sleep(0.5)

    def wait_until_not_running(self, timeout=10):
        """Wait until the container is not more running."""
        t0 = datetime.datetime.now()
        while (datetime.datetime.now() - t0).seconds < timeout:
            if not self.is_running():
                break
            else:
                time.sleep(0.5)

    def is_running(self):
        """Return True if the container is running."""
        pipes = Popen(['acsContainersStatus'], stdout=PIPE, stderr=PIPE)
        out, _ = pipes.communicate()
        return ('%s container is running' % self.name) in out


class ContainerError(RuntimeError):
    pass


def start_containers_and_wait(containers, timeout=10):
    """Start the containers and wait until they are actually started.

    This is a blocking call.  The method starts the containers and
    waits until they are actually started.  It raises RuntimeError
    in case the timeout expires."""
    for container in containers:
        container.start()
    for container in containers:
        container.wait_until_running(timeout)
        if not container.is_running():
            raise RuntimeError('cannot run %s' % container.name)


def stop_containers_and_wait(containers, timeout=10):
    """Stop the containers and wait until they are actually stopped.

    This is a blocking call.  The method stops the containers and
    waits until they are actually stopped.  It raises RuntimeError
    in case the timeout expires."""
    for container in containers:
        container.stop()
    for container in containers:
        container.wait_until_not_running(timeout)
        if container.is_running():
            raise RuntimeError('cannot stop %s' % container.name)
