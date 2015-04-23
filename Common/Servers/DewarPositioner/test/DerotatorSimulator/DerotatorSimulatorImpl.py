from Receivers__POA import SRTKBandDerotator as POA
from Acspy.Servants.CharacteristicComponent import CharacteristicComponent as cc
from Acspy.Servants.ContainerServices import ContainerServices as services
from Acspy.Servants.ComponentLifecycle import ComponentLifecycle as lcycle
from Acspy.Util.BaciHelper import addProperty
from DerotatorSimulator.devios import GenericDevIO


__copyright__ = "Marco Buttu <mbuttu@oa-cagliari.inaf.it>"


class DerotatorSimulatorImpl(POA, cc, services, lcycle):
 
    def __init__(self):
        cc.__init__(self)
        services.__init__(self)
        self._setDefault()

    def initialize(self):
        addProperty(self, 'enginePosition', devio_ref=GenericDevIO())
        # addProperty(self, 'actPosition', devio_ref=GenericDevIO())
        # addProperty(self, 'cmdPosition', devio_ref=GenericDevIO())
        # addProperty(self, 'positionDiff', devio_ref=GenericDevIO())
        # addProperty(self, 'tracking', devio_ref=GenericDevIO())
        # addProperty(self, 'icd_verbose_status', devio_ref=GenericDevIO())
        # addProperty(self, 'status', devio_ref=GenericDevIO())

    def cleanUp(self):
        pass

    def _get_name(self):
        return 'DerotatorSimulator'

    def setup(self):
        self.is_ready = True
        self.is_tracking = True

    def isReady(self):
        return self.is_ready

    def isSlewing(self):
        return self.is_slewing

    def isTracking(self):
        return self.is_tracking

    def isUpdating(self):
        return self.is_updating

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
        if self.getMinLimit() < position < self.getMaxLimit():
            self.position = position

    def getActPosition(self):
        return self.position

    def getPositionFromHistory(self, t):
         return self.position # TODO

    def getCmdPosition(self):
        return self.cmd_position

    def setSpeed(self, speed):
        self.speed = speed

    def getSpeed(self):
        return self.speed

    def park(self):
        self._setDefault()

    def _setDefault(self):
        self.position = self.cmd_position = 0.0
        self.speed = 3 
        self.is_ready = False
        self.is_tracking = False
        self.is_updating = False
        self.is_slewing = False

