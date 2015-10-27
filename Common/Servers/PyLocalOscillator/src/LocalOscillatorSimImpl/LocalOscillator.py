import Receivers__POA
from Acspy.Servants.CharacteristicComponent import CharacteristicComponent as CC
from Acspy.Servants.ContainerServices import ContainerServices as CS
from Acspy.Servants.ComponentLifecycle import ComponentLifecycle as CL
from Acspy.Util.BaciHelper import addProperty
from PyLocalOscillator.LocalOscillatorSimImpl import devios


class LocalOscillator(Receivers__POA.LocalOscillator, CC, CS, CL):

    def __init__(self):
        CC.__init__(self)
        CS.__init__(self)


    def initialize(self):
        addProperty(self, 'frequency', devio_ref=devios.frequencyDevIO())
        addProperty(self, 'amplitude', devio_ref=devios.amplitudeDevIO())
        addProperty(self, 'isLocked', devio_ref=devios.isLockedDevIO())


    def set(self, rf_power, rf_freq):
        pass


    def get(self, rf_power, rf_freq):
        pass


    def rfon(self):
        pass


    def rfoff(self):
        pass
