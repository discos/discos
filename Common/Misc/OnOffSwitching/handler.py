from __future__ import division
import datetime
import logging
import math
import time
import sys
from multiprocessing import Process, Value
from os.path import join, exists
from os import mkdir
from models import Observer, Target

__credits__ = """Author: Marco Buttu <mbuttu@oa-cagliari.inaf.it>
Licence: GPL 2.0 <http://www.gnu.org/licenses/gpl-2.0.html>"""


class Positioner(object):
    def __new__(cls, conf):
        """Singleton."""
        if not hasattr(cls, '_instance'):
            cls._instance = super(Positioner, cls).__new__(cls, conf)
        return cls._instance

    def __init__(self, conf):
        """A daemon process class.

        Parameters
        ==========
        ...
        """
        self.conf = conf
        self.create()
    def create(self):
        self._update = Value('I', 1)
        self._on_source = Value('I', 1)
        self._terminate = Value('I', 0)
        args = (self.conf, self._update, self._on_source, self._terminate)
        self.p = Process(target=self.updatePosition, args=args)
        self.daemon = True
    def __call__(self):
        self.p.start()
    def terminate(self, delay=2):
        self._terminate.value = True
        time.sleep(2)
        self.p.terminate()
        self.p.join()
    def update(self, flag=True):
        self._update.value = flag
    def goOnSource(self):
        self._on_source.value = True
    def goOffSource(self):
        self._on_source.value = False
    def terminated(self):
        return self._terminate.value
    @staticmethod
    def updatePosition(conf, update, on_source, terminate):
        """Compute and set the target position.

        Arguments
        =========
        - update: if 1 the process updates the position, elsewhere it does not.
        - on_source: if 1 then the target is the source, if 0 it is the off source position
        - terminate: if 1 the process terminates properly

        The position changes from on source to off source when `on_source` flag changes from
        1 to 0, and viceversa from the off source position to on source one.
        """

        ra_offset = math.radians(conf.offset) # Offset in radians
        if not conf.simulate:
            try:
                from devices import antenna, ANT_J2000, ACU_NEUTRAL
            except Exception, e:
                logging.exception('Cannot get the antenna component')
                print 'ERROR: Cannot get the antenna component'
                terminate.value = True
                sys.exit(1)

        try:
            while True:
                if update.value:
                    ra, dec = conf.target.getRaDec() if on_source.value else \
                            map(lambda x, y: x + y, conf.target.getRaDec(), (ra_offset, 0))

                    # If it is not a simulation, set the antenna position
                    if not conf.simulate:
                        try:
                            antenna.sidereal(target.name, ra, dec, ANT_J2000, ACU_NEUTRAL)
                        except Exception, e:
                            logging.exception('Cannot set the antenna position')
                            print 'ERROR: Cannot set the antenna position'
                            terminate.value = True
                            sys.exit(1)

                    where = 'on source' if on_source else 'off source'
                    now = datetime.datetime.utcnow()
                    logging.info('%s -> (%.4f, %.4f) @ %s', where, ra, dec, now)
                    print '%s -> (%.4f, %.4f) @ %s' %(where, ra, dec, now)
                if terminate.value:
                    break
                time.sleep(conf.positioning_time)
        except KeyboardInterrupt:
            print 'Stopping the positioning...'
        except Exception, e:
            print 'UNEXPECTED ERROR in updatePosition(): \n\t%s' %e
            logging.exception('Got exception on updatePoition()')
        else:
            datestr = datetime.datetime.utcnow().strftime(conf.datestr_format)
            logging.info('Observation terminated at %s', datestr)
            print('\nObservation terminated at %s' %datestr)


class Handler(object):
    def __init__(self, conf):

        self.ato = -0.5 # Acquisition time offset
        if not conf.simulate:
            try:
                from devices import recorder 
                self.recorder = recorder
                from devices import ato
                self.ato = ato
            except Exception, e:
                logging.exception('Cannot get the recorder')
                print 'ERROR: Cannot get the recorder'
                raise

        if conf.positioning_time < 0:
            raise ValueErro("The positioning time must be positive.")

        if not 0 <= conf.reference <= 1:
            conf.reference = 0.5

        obs = Observer(**conf.observer_info)
        conf.target = Target(op=conf.op, observer=obs)

        self.delta = conf.acquisition_time * conf.reference
        self.timestamp = {'go_on': None, 'go_off': None, 'on_source': None, 'off_source': None}
        self._enablestats(conf.stats)
        self._createstatsfile()
        self.conf = conf
     
    def waitForTracking(self):
        """Wait until the antenna reaches the target position."""
        print 'waitForTracking()'

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

        try:
            duration = self.conf.acquisition_time if self.conf.acquisition_time > 15 else 20
            stop_time = datetime.datetime.utcnow() + datetime.timedelta(seconds=duration + self.ato)
            print 'acquire(): starting acquisition...'
            logging.info('Starting acquisition at %s', datetime.datetime.utcnow())
            if not self.conf.simulate:
                self.recorder.start()
                while datetime.datetime.utcnow() < stop_time:
                    time.sleep(1)
                self.recorder.stop()
            logging.info('Acquisition done at %s', datetime.datetime.utcnow())
            print 'acquire(): acquisition done!'
        except Exception, e:
            logging.exception('Cannot get the recorder')
            print 'ERROR: Cannot get the recorder'
            raise

    def getObservationTitle(self):
        from_ = self.conf.target.from_
        nickname = self.conf.target.nickname 
        datestr = datetime.datetime.utcnow().strftime(self.conf.datestr_format)
        title = "ON/OFF observation from the %s to %s, at %s" %(from_, nickname, datestr)
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
            file_name = '%s.stats' %Target.getNickName(self.conf.op)
            statsfile = join(statsdir, file_name)
            self.statsfile = open(statsfile, 'a')
            title = self.getObservationTitle()
            self.statsfile.write(title)
        except Exception, e:
            print 'Warning: cannot open or write the statitstics file'
            logging.exception('Cannot open or write the statistics file')
            self._enablestats(False)

    def run(self):
        """Run the jobs in loop for cycles times."""
        positioner = Positioner(self.conf)
        positioner() # Start the daemon

        try:
            for cycle in range(self.conf.cycles):
                if positioner.terminated():
                    raise Exception('Antenna positioning process terminated. See the log file.')
                for target in ('on', 'off'):
                    self.waitForTracking()
                    self.updateLO()
                    self.acquire('%s_source' %target)
        except KeyboardInterrupt:
            print 'Program stopped at at %s' %datetime.datetime.utcnow()
            logging.info('Program stopped at at %s' %datetime.datetime.utcnow())
        except Exception, e: 
            print 'UNEXPECTED ERROR: %s' %e
            logging.exception('Got exception on main handler')
        finally:
            positioner.terminate()

if __name__ == '__main__':
    import doctest
    doctest.testmod()

