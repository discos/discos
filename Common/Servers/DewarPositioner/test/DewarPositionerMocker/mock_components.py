class MockDevice(object):
    def __init__(self):
        self._setDefault()

    def _get_name(self):
        return 'mock device'

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

    def getMinLimit(self):
        return -130.0

    def getMaxLimit(self):
        return +130.0

    def getStep(self):
        return 60.0

    def setPosition(self, position):
        self.position = position

    def getActPosition(self):
        return self.position

    def park(self):
        self._setDefault()

    def _get_status(self):
        completion = Completion()
        return Property(0, completion)

    def _setDefault(self):
        self.position = 0.0
        self.is_ready = False
        self.is_tracking = False
        self.is_updating = False
        self.is_slewing = False


class Property(object):
    def __init__(self, value, completion):
        self.value = value
        self.completion = completion
    def get_sync(self):
        return (self.value, self.completion)


class Completion(object):
    def __init__(self, code=0):
        self.code = code

