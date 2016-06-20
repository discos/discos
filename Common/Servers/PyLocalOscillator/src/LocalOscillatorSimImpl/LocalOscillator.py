import Receivers__POA
from Acspy.Servants.CharacteristicComponent import CharacteristicComponent as CC
from Acspy.Servants.ContainerServices import ContainerServices as CS
from Acspy.Servants.ComponentLifecycle import ComponentLifecycle as CL
from Acspy.Util.BaciHelper import addProperty
from LocalOscillatorSimImpl.devios import GenericDevIO as GDevIO


class LocalOscillator(Receivers__POA.LocalOscillator, CC, CS, CL):

    def __init__(self):
        CC.__init__(self)
        CS.__init__(self)
        self.power = 0
        self.frequency = 0
        self.amplitude = 0
        self.isLocked = 0


    def initialize(self):
        addProperty(self, 'frequency', devio_ref=GDevIO(self, 'frequency'))
        addProperty(self, 'amplitude', devio_ref=GDevIO(self, 'amplitude'))
        addProperty(self, 'isLocked', devio_ref=GDevIO(self, 'isLocked'))


    def set(self, power, frequency):
        self.power = power
        self.frequency = frequency


    def get(self):
        return self.power, self.frequency


    def rfon(self):
        pass


    def rfoff(self):
        pass
