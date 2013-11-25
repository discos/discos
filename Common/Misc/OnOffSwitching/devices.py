from __future__ import division
__all__ = ['antenna', 'Recorder', 'mount', 'receiver', 'scheduler', 'ato', 'ANT_J2000', 'ACU_NEUTRAL']

__credits__ = """Author: Marco Buttu <mbuttu@oa-cagliari.inaf.it>
Licence: GPL 2.0 <http://www.gnu.org/licenses/gpl-2.0.html>"""

import datetime
import logging
import time
import sys
from os.path import join, exists, curdir, abspath
from os import mkdir


class Recorder(object):
    def __init__(self, lower_freq, upper_freq, backend_name='BACKENDS/XBackends', writer_name='MANAGEMENT/FitsZilla',\
            projectName='ison', observerName='Gavino'):
        client = PySimpleClient()
        self.writer = client.getComponent(writer_name)
        self.backend = client.getComponent(backend_name)
        self.backend.setSectionsNumber(2)
        self.backend.setSection(0, lower_freq - 125, upper_freq - lower_freq, 1, 2, 31.25, -1)
        self.backend.setSection(1, 60, upper_freq - lower_freq, 1, 2, 31.25, -1)
        # Data stuffs
        self.projectNickname = '_'.join(projectName.split())
        datadir = '%s_data/' %self.projectNickname
        if not exists(datadir):
            mkdir(datadir)
        day_datadir = datadir + datetime.datetime.utcnow().strftime('%Y-%b-%d/')
        if not exists(day_datadir):
            mkdir(day_datadir)
        for i in range(1000):
            obs_datadir = day_datadir + '%03d/'%(i+1)
            if not exists(obs_datadir):
                mkdir(obs_datadir)
                break
        self.abs_obs_path = join(abspath(curdir), obs_datadir)
        self.obs_path = join(curdir, obs_datadir)
        self.scanSetup = Management.TScanSetup(
            scanTag=0, 
            scanId=1, 
            projectName=projectName,
            observerName=observerName,
            path=self.abs_obs_path, 
            extraPath='',
            baseName='',
            scanLayout='', 
            schedule='noSchedule', 
            device=0)
        self.subScanSetup = Management.TSubScanSetup(
            startUt = getTimeStamp().value,
            subScanId=0,
            axis=Management.MNG_NO_AXIS,
            targetID='',
            extraPath='',
            baseName='')
        self._setStartTime(datetime.datetime.utcnow())
        self._setStopTime(datetime.datetime.utcnow())
        self.startDelay = 2.0 # Seconds of delay before the acquistion starts

    def start(self, cycle, cycle_type):
        """Start the acquisition.
        Arguments:
            - cycle: the cycle id (0, 1, 2, ...)
            - cycle_type: ('on_source', 'off_source', 'calibration')"""
        self.writer.reset();
        time.sleep(self.startDelay/2)
        self.backend.connect(self.writer);
        self._updateScanSetup(cycle, cycle_type)
        self.writer.startScan(self.scanSetup)
        time.sleep(self.startDelay/2)
        self.backend.sendHeader()
        self._updateSubScanSetup()
        self.writer.startSubScan(self.subScanSetup);
        self.backend.sendData(self.subScanSetup.startUt)
        self._setStartTime(datetime.datetime.utcnow() + datetime.timedelta(1))
        while getTimeStamp().value < self.subScanSetup.startUt + 110000000:
            time.sleep(1)

    def stop(self):
        self.backend.sendStop();
        while self.writer.isRecording():
            time.sleep(1)
        time.sleep(2)
        self._setStopTime(datetime.datetime.utcnow())
        self.writer.stopScan() 
        time.sleep(1)
        self.backend.terminate()
        self.backend.disconnect()
        self.writer.closeReceiver()

    def _updateScanSetup(self, cycle, cycle_type):
        cycle_datadir = self.obs_path + 'cycle%02d/' %cycle
        if not exists(cycle_datadir):
            mkdir(cycle_datadir)
        cycle_type_datadir = cycle_datadir + cycle_type + '-%s/' %(datetime.datetime.utcnow().strftime("h%Hm%Ms%S"))
        if not exists(cycle_type_datadir):
            mkdir(cycle_type_datadir)
        else:
            logging.error('Dir %s already exists')
            print 'WARNING: Dir %s already exists'
        self.scanSetup.scanTag += 1
        utc = datetime.datetime.utcnow()
        self.scanSetup.path = abspath(cycle_type_datadir)

    def _updateSubScanSetup(self):
        self.subScanSetup.startUt = getTimeStamp().value + 10000000 # Add one second
        self.subScanSetup.subScanId += 1 

    def _setStartTime(self, value):
        self.startTime = value

    def _setStopTime(self, value):
        self.stopTime = value

try:
    from Acspy.Clients.SimpleClient import PySimpleClient
    from Acspy.Common.TimeHelper import getTimeStamp
    from maciErrTypeImpl import CannotGetComponentExImpl
    from Antenna import ANT_J2000, ACU_NEUTRAL
    from AntennaErrors import AntennaErrorsEx
    import Management
    client = PySimpleClient()
    antenna = client.getComponent('ANTENNA/Boss')
    receiver = client.getComponent('RECEIVERS/Boss')
    scheduler = client.getComponent("MANAGEMENT/Gavino")
    mount = client.getComponent('ANTENNA/Mount')
    # If the backend rounds the time to a next upper value, set a negative `ato` 
    ato = -5 # Offset to add to the backend acquisition time
except Exception, e:
    logging.exception('Cannot get the ACS modules and components')
    print('ERROR: Cannot get the ACS modules and components')
    raise


