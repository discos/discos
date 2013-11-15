import logging
import time
import sys
import ison_jobs
from multiprocessing import Process, Value
from datetime import datetime, timedelta

class Job(object):
    def __init__(self, job_name, job, updating_time=1.0, delay=0, args=()):
        self.name = job_name
        self.job = job    
        self.starting_time = datetime.now() + timedelta(delay)
        self.updating_time = updating_time
        self.args = args
        self.create()

    def __call__(self):
        raise NotImplemented()

    @staticmethod
    def wait(target_time):
        while datetime.now() < target_time:
            time.sleep(0.05)

class DaemonJob(Job):
    def create(self):
        self._terminate = Value('I', 0)
        self._update = Value('I', 0)
        args = (self.starting_time, self._update, self._terminate, self.updating_time) + self.args
        self.p = Process(target=self.job, args=args)
        self.daemon = True
        self.doNotUpdate()
    def __call__(self):
        self.p.start()
    def terminate(self, delay=2):
        self._terminate.value = 1
        time.sleep(2)
        self.p.terminate()
        self.p.join()
    def doNotUpdate(self):
        self._update.value = 0
    def update(self):
        self._update.value = 1

class ForegroundJob(Job):
    def create(self):
        pass
    def __call__(self):
        Job.wait(self.starting_time)
        self.job(*self.args)

def goOnSource():
    """
    1. Compute the next position
    2. Set the next position
    """
    print 'goOnSource()'

def goOffSource():
    """
    1. Compute a proper of source position
    2. Set the position
    """
    print 'goOffSource()'

def waitForTracking():
    """Wait until the antenna is tracking and prompt something."""
    print 'waitForTracking()'

def acquire(where, acquiring_time):
    """
    backend.startAquisition()
    # wait until acquisition time expires and prompt something
    backend.stopAquisition()
    """
    print 'acquire(): starting acquisition'
    logging.info('Starting acquisition (%s) at %s' %(where, str(datetime.now())))
    time.sleep(acquiring_time)
    logging.info('Acquisition (%s) done at %s' %(where, str(datetime.now())))
    print 'acquire(): acquisition done!'

def updateLOLoop(starting_time, update, terminate, updating_time):
    """
    1. Compute the LO value 
    2. Set the LO value
    3. Wait a bit 
    """
    try:
        while True:
            if update.value:
                print '@ --> Updating the LO value'
                logging.info('Updating the LO value at %s' %str(datetime.now()))
            if terminate.value:
                break
            time.sleep(updating_time)
    except KeyboardInterrupt:
        print '@ --> Goodby from updateLOLoop()'
    except Exception, e:
        print '@ --> Unexpected error:', str(e)
        logging.exception('Got exception on updateLOLoop()')
    finally:
        print '@ --> Closed properly'

def updateLO(updating_time):
    """
    1. Compute the LO value 
    2. Set the LO value
    """
    print 'updateLO()'
    logging.info('Updating the LO value at %s' %str(datetime.now()))

