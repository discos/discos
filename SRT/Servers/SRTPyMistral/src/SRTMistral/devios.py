from ACSImpl.DevIO import DevIO
from Acspy.Common.TimeHelper import getTimeStamp
import Management


class NumberDevIO(DevIO):

    properties = {
        'bandWidth': 2000,
        'frequency': 50,
        'sampleRate': 0.000025,
        'attenuation': 9,
        'polarization': 0,
        'bins': 1,
        'feed': 0,
        'systemTemperature': 0,
        'inputSection': 0,
        'inputsNumber': 1,
        'sectionsNumber': 1,
        'time': getTimeStamp().value,
        'integration': 40,
    }

    def __init__(self, name):
        self.name = name
        DevIO.__init__(self, 0)

    def read(self):
        return NumberDevIO.properties[self.name]


class StringDevIO(DevIO):

    def __init__(self, name):
        self.name = name
        DevIO.__init__(self, '')

    def read(self):
        if self.name == 'backendName':
            return 'BACKENDS/SRTMistral'
        return ''
