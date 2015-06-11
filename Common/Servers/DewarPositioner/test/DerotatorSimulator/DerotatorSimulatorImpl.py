from __future__ import with_statement

import operator
import time
import DerotatorErrorsImpl

from multiprocessing import Process, Value, Lock
from Receivers__POA import SRTKBandDerotator
from Acspy.Servants.CharacteristicComponent import CharacteristicComponent as cc
from Acspy.Servants.ContainerServices import ContainerServices as services
from Acspy.Servants.ComponentLifecycle import ComponentLifecycle as lcycle
from Acspy.Common.TimeHelper import getTimeStamp
from Acspy.Util.BaciHelper import addProperty
from DerotatorSimulator.devios import GenericDevIO


__copyright__ = "Marco Buttu <mbuttu@oa-cagliari.inaf.it>"


class DerotatorSimulatorImpl(SRTKBandDerotator, cc, services, lcycle):
 
    def __init__(self):
        services.__init__(self)
        self._setDefault()

    def initialize(self):
        addProperty(self, 'actPosition', devio_ref=GenericDevIO()) 
        addProperty(self, 'cmdPosition', devio_ref=GenericDevIO()) 
        addProperty(self, 'enginePosition', devio_ref=GenericDevIO()) 
        addProperty(self, 'positionDiff', devio_ref=GenericDevIO()) 
        addProperty(self, 'tracking', devio_ref=GenericDevIO()) 
        addProperty(self, 'status', devio_ref=GenericDevIO()) 
        addProperty(self, 'icd_verbose_status', devio_ref=GenericDevIO()) 

    def cleanUp(self):
        pass

    def setup(self):
        self.status.is_ready.value = True
        self.status.is_tracking.value = True

    def isReady(self):
        return self.status.is_ready.value

    def isSlewing(self):
        return self.status.is_slewing.value

    def isTracking(self):
        return self.status.is_tracking.value

    def isUpdating(self):
        return self.status.is_updating.value

    def powerOff(self):
        pass

    def getMinLimit(self):
        return -85.77

    def getMaxLimit(self):
        return +125.23

    def getStep(self):
        return 60.0

    def setPosition(self, position):
        self.cmd_position = position
        act_pos = self.getActPosition()
        p = Process(target=DerotatorSimulatorImpl._set_position_process, 
                    args=(self.status, act_pos, position, self.getSpeed()))
        p.start()
        if self.getMinLimit() < position < self.getMaxLimit():
            self.history.insert(position)
        else:
            exc = DerotatorErrorsImpl.OutOfRangeErrorExImpl()
            exc.setData('reason', 'position %2.f out of range' %position)
            raise exc

    def getActPosition(self):
        return self.history.get()

    def getPositionFromHistory(self, t):
         return self.history.get(t)

    def getCmdPosition(self):
        return self.cmd_position

    def setSpeed(self, speed):
        self.speed = speed

    def getSpeed(self):
        return self.speed

    def park(self):
        self._setDefault()

    def _setDefault(self):
        self.history = History()
        self.cmd_position = self.history.get()
        self.speed = 3 
        self.status = Status()

    @staticmethod
    def _set_position_process(status, act_pos, position, speed):
        status.is_slewing.value = True
        distance = float(abs(act_pos - position))
        time.sleep(distance/speed) # Speed degrees per second
        status.is_slewing.value = False


class History(object):
    lock = Lock()

    def __init__(self):
        self.history = []
        self.insert(0)

    def insert(self, position):
        data = (getTimeStamp().value, position)
        with History.lock:
            self.history.append(data)
            self.history.sort(key=operator.itemgetter(0))
            self.history = self.history[-2**15:] # Last 2**15 positions

    def clean(self, since=0):
        target_time = since if since else getTimeStamp().value
        with History.lock:
            idx = len(self.history)
            self.history.sort(key=operator.itemgetter(0))
            for idx, item in enumerate(self.history):
                timestamp = item[0]
                if timestamp > target_time:
                    break
            self.history = self.history[:idx] 

    def get(self, target_time=None):
        """Return the position @target_time"""
        if not target_time:
            target_time = getTimeStamp().value
        size = len(self.history)
        idx = -1
        with History.lock:
            while idx >= -size:
                data = self.history[idx]
                timestamp, position = data[0], data[1:]
                if timestamp <= target_time:
                    return data[-1]
                elif idx - 1 > -size:
                    prev_timestamp = self.history[idx - 1]
                    if prev_timestamp <= target_time:
                        return self.history[idx - 1][-1] # It's better to interpolate
                    else:
                        idx -= 1
                        continue
                else:
                    return self.history[-size][-1]
                    

class Status(object):
    def __init__(self):
        self.is_slewing = Value('i', 0)
        self.is_ready = Value('i', 0)
        self.is_tracking = Value('i', 0)
        self.is_updating = Value('i', 0)

