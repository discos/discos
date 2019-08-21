from ACSImpl.DevIO import DevIO

class GenericDevIO(DevIO):
    def __init__(self, value=0):
        DevIO.__init__(self, value)

    def read(self):
        return self.value

    def write(self, value):
        self.value = value


class PositionDevIO(DevIO):
    def __init__(self, device):
        self.device = device
        DevIO.__init__(self, device.getPosition())

    def read(self):
        return self.device.getPosition()

class SequenceDevIO(DevIO):
    def __init__(self, device):
        self.device = device
        DevIO.__init__(self, self.read())

    def read(self):
        return self.device.getSequence()
