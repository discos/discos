"""This module implements the position generators"""
import datetime
import time
from math import sin, cos, tan, atan2, degrees
from IRAPy import logger
from Acspy.Common.TimeHelper import getTimeStamp


class PosGenerator(object):

    def __init__(self, zdtimeout=5):
        self.zdtimeout = zdtimeout # Timeout in case of zero division error
        self.mapping = {
                'parallactic': {
                    'getAngleFunction': PosGenerator.getParallacticAngle, 
                    'coordinateFrame': 'horizontal'
                },
                'galacticParallactic': {
                    'getAngleFunction': PosGenerator.getGalacticParallacticAngle, 
                    'coordinateFrame': 'equatorial'
                },
        }
      
    def goto(self, iStaticPos):
        yield iStaticPos

    # TODO: refactoring required, in order to put all the parallactic and
    # galacticParallactic common code in one place
    def parallactic(self, source, siteInfo):
        """Return the parallactic angle"""
        try:
            latitude = siteInfo['latitude']
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
                coordinates = source.getApparentCoordinates(t) # Values in radians
                az, el = coordinates[:2] # The first two elements are (az, el)
                position = PosGenerator.getParallacticAngle(latitude, az, el)
                yield position
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
            except GeneratorExit: # Required in Python 2.5:
                # http://www.algorithm.co.il/blogs/programming/generatorexit-another-reason-to-upgrade-to-python-2-6/
                raise 
            except Exception, ex:
                raeson = 'cannot get the %s (az, el)  values' %source._get_name()
                logger.logNotice('%s: %s' %(raeson, ex.message))
                raise PosGeneratorError(raeson)

    def galacticParallactic(self, source, siteInfo):
        """Return the galactic parallactic angle"""
        try:
            latitude = siteInfo['latitude']
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
                coordinates = source.getApparentCoordinates(t) # Values in radians
                az, el, ra, dec = coordinates[:4] 
                p = PosGenerator.getParallacticAngle(latitude, az, el)
                pg = PosGenerator.getGalacticParallacticAngle(latitude, ra, dec)
                yield p + pg
                last_zerodiv_time = datetime.datetime.now()
            except ZeroDivisionError:
                logger.logWarning('zero division error computing the galactic parallactic angle')
                zerodiv_time = datetime.datetime.now() - last_zerodiv_time
                if zerodiv_time.seconds >= self.zdtimeout:
                    raeson = 'zero division for more than %ds' %self.zdtimeout
                    logger.logError(raeson)
                    raise PosGeneratorError(raeson)
                else:
                    time.sleep(0.5)
                    continue
            except GeneratorExit: # Required in Python 2.5:
                # http://www.algorithm.co.il/blogs/programming/generatorexit-another-reason-to-upgrade-to-python-2-6/
                raise 
            except Exception, ex:
                raeson = 'cannot get the %s (az, el)  values' %source._get_name()
                logger.logNotice('%s: %s' %(raeson, ex.message))
                raise PosGeneratorError(raeson)

    @staticmethod
    def getParallacticAngle(latitude, az, el):
        """Arguments in radians"""
        p = atan2(-sin(az), tan(latitude)*cos(el) - sin(el)*cos(az))
        return degrees(p)

    @staticmethod
    def getGalacticParallacticAngle(latitude, ra, dec):
        """Arguments in radians"""
        # North celestial pole coordinates in equatorial celestial frame (j200)
        # ncp = ('12 51 26.28', '27 07 41.7') 
        # ra0 = ephem.hours(ncp[0])
        # dec0 = ephem.degrees(ncp[1])
        ra0 = 3.3660332687500043
        dec0 = 0.47347728280415174
        gp = atan2(sin(ra-ra0), cos(dec)*tan(dec0) - sin(dec)*cos(ra-ra0))
        return degrees(gp)

    
class PosGeneratorError(Exception):
    pass

