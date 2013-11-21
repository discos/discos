"""This module defines an on/off switching handler.

The Handler.run() method execute N on/off cycles, each one is briefly describe as follow::

    1. compute and set the antenna "on source" position (ra, dec)
    2. wait untill the antenna is on the target position
    3. compute and set the LO value; there are two ways to update it (selected by a 
       Handler.run() argument):
           A. countinuosly during acquisition, every K seconds
           B. just once before acquisition, with the the value computed for a time:
              
                  `acatual_time + delta`
              
              The delta is:

                  acquisition_time * reference
              
              where acquisition_time is the time (backend.stoptime - backend.startingtime),
              and `reference` is in the range (0, 1). That means the time for which we compute 
              the LO value is between start_acquisition_time and stop_acquisition_time.
    4. start backend acquisition, wait for acquisition_time second and then stop acquisition
    5. as the previous points, but refer to the "off source" position

The target antenna radec position is computed as:

    actual_time + time_to_track + delta

where `delta` is the same used in the LO time value calculation, and `time_to_track` is the
time (in seconds) we suppose the antenna needs to reach the target (on or off) position. Because
(maybe...) we do not know this time in advance, the handler during the job executions saves
delays in *.stats files (when statistics are enables), so if we want we can perform statitics or
predict this time dynamically at runtime.
The handler also logs info, warnings and exceptions.
Here is an example that shows how to create a hadler:

    >>> from info import Observer, Target, observer_info
    >>> from handler import Handler
    >>> observer = Observer(**observer_info['SRT'])
    >>> observer.name
    'Sardinia Radio Telescope'
    >>> op = "C/2012 S1 (ISON),h,11/28.7757/2013,62.3948,295.6536,345.5636,\
    ... 1.000002,0.012446,2000,10.0,3.2" # Orbital parameters
    >>> target = Target(op, observer)
    >>> target.name
    'C/2012 S1 (ISON)'
    >>> target.nickname
    'ISON'
    >>> h = Handler(target, acquisition_time=2.0, time_to_track=0, reference=0.5)
    >>> h.acquisition_time
    2.0
    >>> h.reference
    0.5
    >>> h.delta
    1.0

The Handler.run() method executes the jobs. It takes two arguments: 

    1 cycles: the number of on/off cycles
    2 daemon_cycle_time: 

        * 0 means we want to compute the LO value just before acquisition,
        * a positive float means we want to compute and set the LO value countinuously 
          during acquisition, and in general in a concurrent way with respect to the main handler. 
          This job is performed by a concurrent process, that compute and set the LO value 
          every daemon_cycle_time.
        * negative values are not allowed

An example of Handler.run() in action, updating the LO continuously::

    >>> h.run(cycles=1, daemon_cycle_time=0.5) # doctest: +SKIP
    goToTarget(on) -> (13:07:22.83, -8:40:48.3)
    waitForTracking()
    acquire(): starting acquisition...
    @ --> Updating the LO value
    @ --> Updating the LO value
    @ --> Updating the LO value
    acquire(): acquisition done!
    goToTarget(off) -> (13:07:22.85, -8:40:48.4)
    waitForTracking()
    acquire(): starting acquisition...
    @ --> Updating the LO value
    @ --> Updating the LO value
    @ --> Updating the LO value
    @ --> Updating the LO value
    acquire(): acquisition done!
    @ --> LO updating stopped properly.
"""
from __future__ import division

__credits__ = """Author: Marco Buttu <mbuttu@oa-cagliari.inaf.it>
The author wants to say thanks to: S.Poppi, M.Bartolini, A.Orlati, C.Migoni, and A.Melis
Licence: GPL 2.0 <http://www.gnu.org/licenses/gpl-2.0.html>"""

import datetime
import logging
import time
from multiprocessing import Process, Value
from os.path import join, exists
from os import mkdir


class DaemonJob(object):
    def __init__(self, job_name, job, updating_time=1.0, delay=0, args=()):
        self.name = job_name
        self.job = job    
        self.starting_time = datetime.datetime.utcnow() + datetime.timedelta(seconds=delay)
        self.updating_time = updating_time
        self.args = args
        self.create()
    def create(self):
        self._terminate = Value('I', 0)
        self._update = Value('I', 0)
        args = (self.starting_time, self._update, self._terminate, self.updating_time) + self.args
        self.p = Process(target=self.job, args=args)
        self.daemon = True
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


class Handler(object):
    def __init__(self, target, acquisition_time, time_to_track=0, reference=0.5, offset=0, stats=True):
        """Initialize the Handler.

        Parameters
        ==========
        - target: a Target instance
        - acquisition_time: time (in seconds) between a backend.start() and backend.stop()
        - time_to_track: time (in seconds) we suppose the antenna needs to reach the target 
          (on or off) position
        - reference: it is a float between 0.0 and 1.0: 
               * a value of 0.0 means we want to compute the future target position referred 
                 at the time the acquisition starts
               * a value of 1.0 means we want to compute the future target position referred 
                 at the time the acquisition ends
               * a value of 0.5 means we want to compute the future target position referred 
                 at the central time between starting and ending acquisition time.
          A value out of range is set to 0.5.
        - offset: the offset we want to add to the off source position
        - stats: when True, the handler saves information that could be useful in order to 
          perform statistics or predict the time_to_track.

        Attributes
        ==========
        - timestamp: a dict of three timestamps; it has the following keys:
              * 'go_on': time of goOnSource()
              * 'go_off': time of goOffSource()
              * 'on_source': the time the on source acquisition starts
              * 'off_source': the time the off source acquisition starts
          We want to know the timestamps in order to compute some statistics about the delay 
          between the time the position is commanded and the time the acquisitions start. 
        - all the attributes related to the parameters
        """
        self.target = target
        self.acquisition_time = acquisition_time
        self.time_to_track = time_to_track
        if not 0 <= reference <= 1:
            self.reference = 0.5
        else:
            self.reference = reference
        self.delta = self.acquisition_time * self.reference
        self.timestamp = {'go_on': None, 'go_off': None, 'on_source': None, 'off_source': None}
        self._enablestats(stats)
        self._createstatsfile()
      
    def goToTarget(self, target):
        """Compute and set the next target position. The target must be 'on' or 'off'."""
        if target not in ('on', 'off'):
            raise ValueError('target %s not allowed' %target)
        self._savedelay(target) # We could use it in order to perform statistics
        now = datetime.datetime.utcnow()
        time_to_track = datetime.timedelta(seconds=self.time_to_track)
        delta = datetime.timedelta(seconds=self.delta)
        time_from_now = time_to_track + delta
        at_time = now + time_from_now
        ra, dec = self.target.getRaDec(at_time) 
        if target == 'off':
            pass # TODO: add the position offset
        from_now = '%s.%s' %(time_from_now.seconds, time_from_now.microseconds)
        logging.info('%s: next position computed for %s second in advance from now' %(target, from_now))
        logging.info('@ %s: go to %s at %s' %(datetime.datetime.utcnow(), [ra, dec], at_time))
        # TODO: set the antenna target position
        print 'goToTarget(%s) -> (%s, %s)' %(target, ra, dec)
        self.timestamp['go_%s' %target] = datetime.datetime.utcnow()

    def waitForTracking(self):
        """Wait until the antenna reaches the target position."""
        print 'waitForTracking()' # With timeout, in order to promp a warning in case the time is too long

    def updateLO(self):
        """Compute the LO value and set it."""
        now = datetime.datetime.utcnow()
        at_time = now + datetime.timedelta(seconds=self.delta)
        logging.info('At %s: LO value computed for %s' %(now, at_time))
        # TODO: get the local oscillator value
        print 'updateLO() -> ', '@ %s: LO value computed for %s' %(now, at_time)
        # TODO: set the local oscillator

    def acquire(self, which):
        """Start and stop the backend acquisition."""
        if which not in ('on_source', 'off_source'):
            raise KeyError('key %s not allowed' %which)
        self.timestamp[which] = datetime.datetime.utcnow() # Set the timestamp before acquiring
        print 'acquire(): starting acquisition...'
        logging.info('Starting acquisition at %s' %datetime.datetime.utcnow())
        time.sleep(self.acquisition_time) # Wait for acquisition_time seconds
        logging.info('Acquisition done at %s' %datetime.datetime.utcnow())
        print 'acquire(): acquisition done!'

    @staticmethod
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
                    logging.info('Updating the LO value at %s', datetime.datetime.utcnow())
                if terminate.value:
                    break
                time.sleep(updating_time)
        except KeyboardInterrupt:
            print '@ --> Stopping the LO updating...'
        except Exception, e:
            print '@ --> Unexpected error: %s' %e
            logging.exception('Got exception on updateLOLoop()')
        finally:
            logging.info('LO updating stopped at %s' %datetime.datetime.utcnow())
            print '@ --> LO updating stopped properly.'

    def getObservationTitle(self):
        format_ = '[%d/%b/%Y:%H:%M:%S]' # [day/month/year:hour:minute:second]
        starting_time = datetime.datetime.utcnow()
        title = "ON/OFF observation from the %s to %s, at %s" \
                %(self.target.from_, self.target.nickname, starting_time.strftime(format_))
        line = len(title) * '='
        return line + '\n' + title + '\n' + line + '\n'

    def _enablestats(self, flag):
        self.stats = flag

    def _createstatsfile(self):
        if not self.stats:
            return
        statsdir = 'stats'
        if not exists(statsdir):
            try:
                mkdir(statsdir)
            except Exception:
                print 'Cannot create the %s dir.' %statsdir
                logging.warning('Cannot create the %s dir', statsdir)
                self._enablestats(False)
                return
        try:
            file_name = '%s.stats' %(self.target.nickname.lower())
            statsfile = join(statsdir, file_name)
            self.statsfile = open(statsfile, 'a')
            title = self.getObservationTitle()
            self.statsfile.write(title)
        except Exception, e:
            print 'Warning: cannot open or write %s file' %statsfile
            logging.exception('Cannot open %s file', statsfile)
            self._enablestats(False)

    def _savedelay(self, key):
        """Save the delay for statistics."""
        if not self.stats:
            return
        starting_time = self.timestamp['go_%s' %key]
        acquisition_time = self.timestamp['%s_source' %key] 
        if acquisition_time and starting_time:
            if acquisition_time <= starting_time:
                logging.warning('At %s: (acquisition_time - starting time) <= 0' %datetime.datetime.utcnow())
            delay = (acquisition_time - starting_time)
            seconds = delay.seconds
            useconds = delay.microseconds
            self.statsfile.write('DELAY_%s: %s.%s seconds\n' %(key.upper(), seconds, useconds))

    def run(self, cycles, daemon_cycle_time=0):
        """Run the jobs in loop for cycles times."""
        if daemon_cycle_time < 0:
            raise ValueErro("The daemon_cycle_time must be positive.")
        if daemon_cycle_time:
            lo_daemon = DaemonJob('update local oscillator', self.updateLOLoop, daemon_cycle_time)
            lo_daemon()

        try:
            for cycle in range(cycles):
                for target in ('on', 'off'):
                    self.goToTarget(target)
                    self.waitForTracking()
                    if not daemon_cycle_time:
                        self.updateLO()
                    else:
                        lo_daemon.update()
                    self.acquire('%s_source' %target)
                    if daemon_cycle_time:
                        lo_daemon.doNotUpdate()
        except KeyboardInterrupt:
            print 'Program stopped at at %s' %datetime.datetime.utcnow()
            logging.info('Program stopped at at %s' %datetime.datetime.utcnow())
        except Exception, e: 
            print 'Unexpected error: %s' %e
            logging.exception('Got exception on main handler')
        finally:
            if daemon_cycle_time: 
                lo_daemon.terminate()

if __name__ == '__main__':
    import doctest
    doctest.testmod()

