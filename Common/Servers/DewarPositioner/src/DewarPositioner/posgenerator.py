"""This module implements the position generators"""
import datetime
import time
from math import sin, cos, tan, atan, radians, degrees
from IRAPy import logger
from Acspy.Common.TimeHelper import getTimeStamp


class PosGenerator(object):

    def __init__(self, zdtimeout=5):
        self.zdtimeout = zdtimeout # Timeout in case of zero division error
      
    def goto(self, position):
        yield position

    def parallactic(self, source, siteInfo, initialPosition=0):
        """Generate a parallactic...
         
        A better description...

        :param source: object with ``_get_azimuth()`` and ``_get_elevation()``
         methods
        :param siteInfo: di
        :type siteInfo: dict
        :returns:  a position
        :raises: PosGeneratorError
        """
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
                tan_p = - sin(az) / (tan(latitude)*cos(el) - sin(el)*cos(az))
                last_zerodiv_time = datetime.datetime.now()
                p = atan(tan_p)
                yield initialPosition + degrees(p)
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

    
class PosGeneratorError(Exception):
    pass

