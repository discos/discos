from math import fabs
import ReceiversErrorsImpl
import ComponentErrorsImpl
from ACSImpl.DevIO import DevIO

SYNTH_TOLLERANCE = 1.


class amplitudeDevIO(DevIO):
    def __init__(self, cl, value=0):
        DevIO.__init__(self, value)

    def read(self):
        return self.value

    def write(self, value):
        self.value = value


class frequencyDevIO(DevIO):

    def __init__(self, cl, value=0):
        DevIO.__init__(self, value)
        self.cl = cl

    def read(self):
        try:
            msg, value = self.cl.getFrequency()
            return value
        except ReceiversErrorsImpl.SynthetiserErrorExImpl as ex:
            exc = ComponentErrorsImpl.PropertyErrorExImpl(exception=ex)
            exc.setPropertyName("frequency")
            exc.setReason(ex.getDetails())
            raise exc

    def write(self, value):
        self.value = value

class isLockedDevIO(DevIO):

    def __init__(self, component, commandline, value=0):
        DevIO.__init__(self, value)
        self.cl = commandline
        self.impl = component

    def read(self):
        try:
            component_frequency = self.impl.freq
            msg, synth_frequency = self.cl.getFrequency()
            offset = fabs(component_frequency - synth_frequency)
            return 1 if offset <= SYNTH_TOLLERANCE else 0
        except ReceiversErrorsImpl.SynthetiserErrorExImpl as ex:
            exc = ComponentErrorsImpl.PropertyErrorExImpl(exception=ex)
            exc.setPropertyName("isLocked")
            exc.setReason(ex.getDetails())
            raise exc

    def write(self, value):
        self.value = value

class connectedDevIO(DevIO):

    def __init__(self, commandline, value=False):
        DevIO.__init__(self, value)
        self.cl = commandline

    def read(self):
        return 1 if self.cl.connected else 0
