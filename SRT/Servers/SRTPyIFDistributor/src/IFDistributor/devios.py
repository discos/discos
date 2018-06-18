from ACSImpl.DevIO import DevIO
from IRAPy import logger


class GenericDevIO(DevIO):

    def __init__(self, comp, what):
        self.comp = comp
        self.what = what
        DevIO.__init__(self, self.read())

    def read(self):
        return getattr(self.comp, self.what)

    def write(self, value):
        attrib = getattr(self.comp, self.what)
        attrib = value
