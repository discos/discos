from ACSImpl.DevIO import DevIO


class GenericDevIO(DevIO):

    def __init__(self, lo, property_name, value=0):
        DevIO.__init__(self, value)
        self.lo = lo
        self.property_name = property_name

    def read(self):
        return getattr(self.lo, property_name)

    def write(self, value):
        self.value = value
