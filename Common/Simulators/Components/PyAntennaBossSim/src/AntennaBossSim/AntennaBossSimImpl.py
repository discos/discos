from __future__ import with_statement

import operator
import math
import time
from multiprocessing import Process, Value, Lock
from Antenna__POA import AntennaBoss
from Antenna import TCoordinateFrame, ANT_EQUATORIAL, ANT_HORIZONTAL
from Management__POA import CommandInterpreter as CI
from Management__POA import Subsystem
from Acspy.Servants.CharacteristicComponent import CharacteristicComponent as CC
from Acspy.Servants.ContainerServices import ContainerServices as Services
from Acspy.Servants.ComponentLifecycle import ComponentLifecycle as Lcycle
from Acspy.Common.TimeHelper import getTimeStamp
from Acspy.Util.BaciHelper import addProperty
from AntennaBossSim.devios import GenericDevIO


__copyright__ = "Marco Buttu <mbuttu@oa-cagliari.inaf.it>"


class AntennaBossSimImpl(AntennaBoss, CI, Subsystem, CC, Services, Lcycle):
 
    def __init__(self):
        CC.__init__(self)
        Services.__init__(self)
        self._setDefault()

    def initialize(self):
        addProperty(self, 'target', devio_ref=GenericDevIO())
        addProperty(self, 'targetRightAscension', devio_ref=GenericDevIO())
        addProperty(self, 'targetDeclination', devio_ref=GenericDevIO())
        addProperty(self, 'targetVrad', devio_ref=GenericDevIO())
        addProperty(self, 'targetFlux', devio_ref=GenericDevIO())
        addProperty(self, 'vradReferenceFrame', devio_ref=GenericDevIO())
        addProperty(self, 'vradDefinition', devio_ref=GenericDevIO())
        addProperty(self, 'azimuthOffset', devio_ref=GenericDevIO())
        addProperty(self, 'elevationOffset', devio_ref=GenericDevIO())
        addProperty(self, 'rightAscensionOffset', devio_ref=GenericDevIO())
        addProperty(self, 'declinationOffset', devio_ref=GenericDevIO())
        addProperty(self, 'longitudeOffset', devio_ref=GenericDevIO())
        addProperty(self, 'latitudeOffset', devio_ref=GenericDevIO())
        addProperty(self, 'rawAzimuth', devio_ref=GenericDevIO())
        addProperty(self, 'rawElevation', devio_ref=GenericDevIO())
        addProperty(self, 'observedAzimuth', devio_ref=GenericDevIO())
        addProperty(self, 'observedElevation', devio_ref=GenericDevIO())
        addProperty(self, 'observedRightAscension', devio_ref=GenericDevIO())
        addProperty(self, 'observedDeclination', devio_ref=GenericDevIO())
        addProperty(self, 'observedGalLongitude', devio_ref=GenericDevIO())
        addProperty(self, 'observedGalLatitude', devio_ref=GenericDevIO())
        addProperty(self, 'enabled', devio_ref=GenericDevIO())
        addProperty(self, 'correctionEnabled', devio_ref=GenericDevIO())
        addProperty(self, 'generatorType', devio_ref=GenericDevIO())
        addProperty(self, 'FWHM', devio_ref=GenericDevIO())
        addProperty(self, 'waveLength', devio_ref=GenericDevIO())
        addProperty(self, 'pointingAzimuthCorrection', devio_ref=GenericDevIO())
        addProperty(self, 'pointingElevationCorrection', devio_ref=GenericDevIO())
        addProperty(self, 'refractionCorrection', devio_ref=GenericDevIO())

    def cleanUp(self):
        pass
        
    def getApparentCoordinates(self, t):
        """Return (az, el, ra, dec) + (jepoch, lon, lat)"""
        coordinates = self.history.get(t) + (0, 0, 0)
        return tuple(coordinates)

    def getRawCoordinates(self, t):
        """Return (az, el)"""
        az, el = self.history.get(t)[:2]
        return az, el

    def setOffsets(self, lon, lat, frame):
        """Set the (az, el) or (ra, dec) offset, depending of the frame"""
        az, el, ra, dec = self.coordinates
        if frame == ANT_HORIZONTAL:
            az, el = lon, lat
        elif frame == ANT_EQUATORIAL:
            ra, dec = lon, lat
        else:
            raise ValueError('Frame %s not supported' %frame)
        self.coordinates = az, el, ra, dec
        self.history.insert(self.coordinates)

    def _setDefault(self):
        self.coordinates = [math.pi/4] * 4 # az, el, ra, dec -> 45 degrees
        self.history = History(self.coordinates)


class History(object):
    lock = Lock()

    def __init__(self, position):
        self.history = []
        self.insert(position) 

    def insert(self, position):
        data = (getTimeStamp().value, position)
        with History.lock:
            self.history.append(data)
            self.history.sort(key=operator.itemgetter(0))
            self.history = self.history[-2**15:] # Last 2**15 positions
        # print 'position inserted in the history: ', position

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

