class MockDevice(object):
    def __init__(self):
        self._setDefault()

    def _get_name(self):
        return 'mock device'

    def setup(self):
        self.is_ready = True

    def isReady(self):
        return self.is_ready

    def isSlewing(self):
        return False

    def isTracking(self):
        return True

    def getMinLimit(self):
        return -120.0

    def getMaxLimit(self):
        return +120.0

    def setPosition(self, position):
        self.position = position

    def getActPosition(self):
        return self.position

    def park(self):
        self._setDefault()

    def _setDefault(self):
        self.position = 0.0
        self.is_ready = False
