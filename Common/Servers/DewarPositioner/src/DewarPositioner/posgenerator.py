"""This module implements the position generators"""
import datetime
import time
from math import sin, cos, tan, atan, radians, degrees
from IRAPy import logger
from Acspy.Common.TimeHelper import getTimeStamp


class PosGenerator(object):

    def __init__(self, zdtimeout=5):
        self.zdtimeout = zdtimeout # Timeout in case of zero division error
      
    def goto(self, iStaticPos):
        yield iStaticPos

    def parallactic(self, source, siteInfo):
        """Return the parallactic angle"""
        try:
            latitude = radians(siteInfo['latitude'])
        except (KeyError, TypeError), ex:
            raise PosGeneratorError('cannot get the latitude: %s' %ex.message)
        except Exception, ex:
            raeson = 'unexpected exception getting the site latitude' 
            logger.logNotice(raeson)
            raise PosGeneratorError(raeson)

        last_zerodiv_time = datetime.datetime.now()
        while True:
            try:
                t = getTimeStamp().value + 1*10*6 # 100 ms in the future
                az, el = source.getRawCoordinates(t) # Values in radians
                yield PosGenerator.getParallacticAngle(latitude, az, el)
                last_zerodiv_time = datetime.datetime.now()
            except ZeroDivisionError:
                logger.logWarning('zero division error computing the parallactic angle')
                zerodiv_time = datetime.datetime.now() - last_zerodiv_time
                if zerodiv_time.seconds >= self.zdtimeout:
                    raeson = 'zero division for more than %ds' %self.zdtimeout
                    logger.logError(raeson)
                    raise PosGeneratorError(raeson)
                else:
                    time.sleep(0.5)
                    continue
            except Exception, ex:
                raeson = 'cannot get the %s (az, el)  values' %source._get_name()
                logger.logNotice('%s: %s' %(raeson, ex.message))
                raise PosGeneratorError(raeson)

    @staticmethod
    def getParallacticAngle(latitude, az, el):
        tan_p = - sin(az) / (tan(latitude)*cos(el) - sin(el)*cos(az))
        p = atan(tan_p)
        return degrees(p)

    
class PosGeneratorError(Exception):
    pass

