from __future__ import division

__credits__ = """Author: Marco Buttu <mbuttu@oa-cagliari.inaf.it>
Licence: GPL 2.0 <http://www.gnu.org/licenses/gpl-2.0.html>"""

import datetime
import logging
import ephem
import math
import time
import sys
from multiprocessing import Process, Value
from os.path import join, exists
from os import mkdir
from models import Observer, Target

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
                            antenna.sidereal(conf.target.name, ra, dec, ANT_J2000, ACU_NEUTRAL)
                        except Exception, e:
                            logging.exception('Cannot set the antenna position')
                            print 'ERROR: Cannot set the antenna position'
                            terminate.value = True
                            sys.exit(1)

                    where = 'on source' if on_source.value else 'off source'
                    now = datetime.datetime.utcnow()
                    logging.info('%s: ra_dec(%.4f, %.4f) @ %s', where, ra, dec, now)
                    print '@ Tracking %s: ra_dec(%.4f, %.4f)' %(where, ra, dec)
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

        self.ato = -0.5 # Acquisition time offset (the value assegned in the no-simul mode is imported below)
        if not conf.simulate:
            try:
                from devices import recorder, ato, receiver, scheduler, mount
                self.recorder = recorder
                self.ato = ato
                self.receiver = receiver
                self.scheduler = scheduler
                self.cmd_az_obj = mount._get_commandedAzimuth()
                self.act_az_obj = mount._get_azimuth()
                self.cmd_el_obj = mount._get_commandedElevation()
                self.act_el_obj = mount._get_elevation()
            except Exception, e:
                logging.exception('Cannot get the devices')
                print 'ERROR: Cannot get the devices'
                raise

        if conf.positioning_time < 0:
            raise ValueErro("The positioning time must be positive.")

        if not 0 <= conf.reference <= 1:
            conf.reference = 0.5

        obs = Observer(**conf.observer_info)
        conf.target = Target(op=conf.op, observer=obs)
        self.conf = conf

        utcnow = datetime.datetime.utcnow()
        self._startAcquisitionTime = {'on_source': utcnow, 'off_source': utcnow, 'calibration': utcnow}
        self._stopAcquisitionTime = {'on_source': utcnow, 'off_source': utcnow, 'calibration': utcnow}
        self._enablestats(self.conf.stats)
        self._createstatsfile()
        self._createHorizons()
     
    def waitForTracking(self):
        """Wait until the antenna reaches the target position."""
        slice_time = 1.0
        duration = 0
        timeout = 60 # seconds
        if not self.conf.simulate:
            track_distance = 0.001 # (move to conf)
            track = False
            time.sleep(self.conf.positioning_time + 2) # Wait untill the antenna get the cmd pos
            while not track:
                try:
                    cmd_az, comp = self.cmd_az_obj.get_sync()
                    act_az, comp = self.act_az_obj.get_sync()
                    cmd_el, comp = self.cmd_el_obj.get_sync()
                    act_el, comp = self.act_el_obj.get_sync()
                    distance = math.sqrt(abs(cmd_az - act_az)**2 + abs(cmd_el - act_el)**2)
                    track = True if distance < track_distance else False
                    time.sleep(slice_time)
                    duration += slice_time
                    if duration > timeout:
                        break
                except Exception, e:
                    logging.exception('Cannot get the tracking value')
                    print 'Cannot get the tracking value'
                if duration > timeout:
                    break
            if not track:
                raise Exception('Cannot track the source')
        else:
            print("waitForTracking()")
            time.sleep(2.5)


    def updateLO(self):
        """Compute and set the LO value."""
        # Add 2.5 seconds of tracking time. We will calculate this time later, dynamically
        acq_time = self.conf.acquisition_time 
        stop_acq_time = self._stopAcquisitionTime['off_source']
        start_acq_time = self._startAcquisitionTime['on_source']
        realDelta = (stop_acq_time - start_acq_time).seconds / 2
        delta = 2 * acq_time * self.conf.reference + 2.5 
        delta = realDelta if acq_time < realDelta <  2*acq_time else delta
        startTime = datetime.datetime.utcnow() if self.conf.simulate else self.recorder.startTime
        at_time = startTime + datetime.timedelta(seconds=delta)
        found = False 
        size = len(self._horizons)
        for idx, item in enumerate(self._horizons):
            date, radialSpeed = item
            if date == at_time:
                found = True
                break
            if date < at_time:
                if idx == size - 1:
                    found = True
                    break # The last element
                format_ = '%Y-%b-%d %H:%M'
                t0, t1 = date, self._horizons[idx+1][0]
                rs0, rs1 = float(radialSpeed), float(self._horizons[idx+1][1])
                if t0 < at_time < t1:
                    radialSpeed = rs0 + (rs1 - rs0)*((at_time - t0).seconds/(t1 - t0).seconds)
                    found = True
                    break 
                else:
                    continue
        if found:
            vobs = self.conf.lab_freq * (1 - radialSpeed/(ephem.c/1000))
            lo = vobs - ((self.conf.upper_freq + self.conf.lower_freq)/2)
            del self._horizons[:idx]
        else:
            raise Exception('Cannot get a proper date in the horizons file')

        if not self.conf.simulate:
            try:
                self.receiver.setLO([lo])
            except Exception, e:
                logging.exception('Cannot get the receiver')
                print 'ERROR: Cannot get the receiver'
                raise
        now = datetime.datetime.utcnow()
        logging.info('At %s: LO value -> %s', now, lo)
        print '+> LO(now + %s seconds) -> %.1f MHz' %(delta, lo)

    def acquire(self, which):
        """Start and stop the backend acquisition."""
        if which not in self._startAcquisitionTime:
            raise KeyError('key %s not allowed' %which)

        try:
            if self.conf.simulate:
                duration = self.conf.acquisition_time
            else:
                if self.conf.acquisition_time < 15: # Offset needed by Xarcos on Nuraghe 0.2
                    duration = 20
                else:
                    duration = self.conf.acquisition_time
            stop_time = datetime.datetime.utcnow() + datetime.timedelta(seconds=duration + self.ato)
            self._startAcquisitionTime[which] = datetime.datetime.utcnow()
            if not self.conf.simulate:
                if which == 'calibration':
                    try:
                        self.receiver.calOn()
                    except Exception, e:
                        logging.exception('Cannot turn the mark ON')
                        print '+> ERROR: Cannot turn the mark ON'
                        sys.exit(1)
                    logging.info('Calibration mark ON')
                    print '+> Calibration mark ON'
                    time.sleep(1)
                print '+> Starting acquisition %s' %which
                logging.info('Starting acquisition at %s', datetime.datetime.utcnow())
                self.recorder.start()
            else:
                if which == 'calibration':
                    logging.info('Calibration mark ON')
                    print '+> Calibration mark ON (simulation mode)'
                    logging.info('Starting acquisition at %s', datetime.datetime.utcnow())
                    print '+> Starting acquisition %s' %which
        except Exception, e:
            logging.exception('Some problems recording')
            print('Some problems recording')
            raise
        finally:
            while datetime.datetime.utcnow() < stop_time:
                time.sleep(1)
            if not self.conf.simulate:
                self.recorder.stop()
                logging.info('Acquisition done at %s', datetime.datetime.utcnow())
                print '+> Acquisition done!'
                if which == 'calibration':
                    try:
                        self.receiver.calOff()
                        logging.info('Calibration mark OFF')
                        print '+> Calibration mark OFF'
                    except Exception, e:
                        logging.exception('Cannot turn the mark OFF')
                        print '+> ERROR: Cannot turn the mark OFF'
                        sys.exit(1)
            else:
                logging.info('Acquisition done at %s', datetime.datetime.utcnow())
                print '+> Acquisition done!'
                if which == 'calibration':
                    logging.info('Calibration mark OFF (simulation mode)')
                    print '+> Calibration mark OFF'
            self._stopAcquisitionTime[which] = datetime.datetime.utcnow()

    def getObservationTitle(self):
        from_ = self.conf.target.from_
        nickname = self.conf.target.nickname 
        datestr = datetime.datetime.utcnow().strftime(self.conf.datestr_format)
        title = "ON/OFF observation from the %s to %s, at %s" %(from_, nickname, datestr)
        line = len(title) * '='
        return line + '\n' + title + '\n' + line + '\n'

    def _createHorizons(self):
        format_ = '%Y-%b-%d %H:%M'
        self._horizons = []
        found = False
        try:
            for idx, line in enumerate(open(join('horizons', self.conf.horizons_file_name))):
                if line.startswith('$$SOE'):
                    found = True
                elif line.startswith('$$EOE'):
                    break
                elif found:
                    items = line.split()
                    if len(items) not in (21, 22):
                        raise Exception('Unexpected number of items in %s' %file_name)
                    datestr = items[0] + ' ' + items[1]
                    delotstr = items[17] if len(items) == 21 else items[18]
                    radialSpeed = float(delotstr)
                    date = datetime.datetime.strptime(datestr, format_)
                    if date >= datetime.datetime.utcnow() - datetime.timedelta(minutes=20):
                        self._horizons.append((date, radialSpeed))
        except Exception, e:
            logging.error('Cannot get the radial speeds from the horizons file')
            print('Cannot get the radial speeds from the horizons file')
            sys.exit(1)

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
        calibrations = int(self.conf.cycles * self.conf.calibrations / 100)
        total_cycles = self.conf.cycles if not calibrations else self.conf.cycles + 1
        if not calibrations:
            print 'WARNING: Observation without calibration'
            logging.warning('Observation withoud calibration')
            cal_idx = 0
            user_input = raw_input('Are you sure you want to continue? (y/n): ')
            if user_input.strip() != 'y':
                sys.exit(0)
        else:
            cal_idx = int(total_cycles / calibrations)

        positioner = Positioner(self.conf)
        positioner() # Start the daemon

        try:
            for i in range(total_cycles):
                if positioner.terminated():
                    raise Exception('Antenna positioning process terminated. See the log file.')
                positioner.goOnSource()
                self.waitForTracking()
                self.updateLO() # LO value in the on/off middle position
                self.acquire('on_source')
                positioner.goOffSource()
                self.waitForTracking()
                if cal_idx and not i % cal_idx:
                    self.acquire('calibration')
                else:
                    self.acquire('off_source')
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

