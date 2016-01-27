from ACSImpl.DevIO import DevIO
from math import fabs
from CommandLine import CommandLineError
from IRAPy import logger

SYNTH_TOLLERANCE = 1.

class GenericDevIO(DevIO):
    def __init__(self, value=0):
        DevIO.__init__(self, value)

    def read(self):
        return self.value

    def write(self, value):
        self.value = value


class amplitudeDevIO(DevIO):
    def __init__(self, cl,value=0):
        DevIO.__init__(self, value)

    def read(self):
        return self.value

    def write(self, value):
        self.value = value
                                    
class frequencyDevIO(DevIO):
    '''
       frequency read from the synth.
       
    ''' 
    def __init__(self, cl,value=0):
        DevIO.__init__(self,value)
        self.cl=cl
    def read(self):
        try:
           msg,value=self.cl.getFrequency()
           return value
        except CommandLineError, ex:
                success = False
                msg = ex.message if ex.message else 'unexpected exception'
                answer = 'Error - %s' %(ex.getReason() if hasattr(ex, 'getReason') else msg)
                logger.logError(answer)
                return 0
        
        except Exception, ex:
                success = False
                msg = ex.message if ex.message else 'unexpected exception'
                answer = 'Error - %s' %(ex.getReason() if hasattr(ex, 'getReason') else msg)
                logger.logError(answer)
                return 0

    def write(self, value):
        self.value = value
class isLockedDevIO(DevIO):
    '''
      This checks if the Synthetizer is actually set  to the set frequecy 
       
    ''' 
    
    def __init__(self, component,commandline,value=0):
        DevIO.__init__(self, value)
        self.cl=commandline
        self.impl=component
    def read(self):
        try:
           component_frequency=self.impl.getInternalFrequency()  #read freq from component
           msg,synth_frequency=self.cl.getFrequency() #read freq from synth
           offset=fabs(component_frequency-synth_frequency)
           if offset <= SYNTH_TOLLERANCE:
               return 1
           else:
               return 0
        except CommandLineError, ex:
                success = False
                msg = ex.message if ex.message else 'unexpected exception'
                answer = 'Error - %s' %(ex.getReason() if hasattr(ex, 'getReason') else msg)
                logger.logError(answer)
                return 0
        
        except Exception, ex:
                success = False
                msg = ex.message if ex.message else 'unexpected exception'
                answer = 'Error - %s' %(ex.getReason() if hasattr(ex, 'getReason') else msg)
                logger.logError(answer)
                return 0
             
    def write(self, value):
        self.value = value
