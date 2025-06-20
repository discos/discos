"""This module implements the position calculation functions"""
import datetime
import time
from math import sin, cos, tan, atan2, degrees, isclose
from IRAPy import logger
from Acspy.Common.TimeHelper import getTimeStamp


class PosGenerator(object):

    def __init__(self, updatingTime, trackingLeadTime):
        self.updatingTime = updatingTime
        self.trackingLeadTime = trackingLeadTime
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

    def parallactic(self, source, siteInfo, t):
        """Return the parallactic angle"""
        latitude = PosGenerator.getLatitude(siteInfo)

        try:
            coordinates = source.getApparentCoordinates(t) # Values in radians
            az, el = coordinates[:2] # The first two elements are (az, el)
            position = PosGenerator.getParallacticAngle(latitude, az, el)
            return position
        except Exception as ex:
            raeson = 'cannot get the %s (az, el)  values' %source._get_name()
            logger.logNotice('%s: %s' %(raeson, ex))
            raise PosGeneratorError(raeson)

    def galacticParallactic(self, source, siteInfo, t):
        """Return the galactic parallactic angle"""
        latitude = PosGenerator.getLatitude(siteInfo)

        try:
            coordinates = source.getApparentCoordinates(t) # Values in radians
            az, el, ra, dec = coordinates[:4]
            position = PosGenerator.getGalacticParallacticAngle(latitude, az, el, ra, dec)
            return position
        except Exception as ex:
            raeson = 'cannot get the %s (az, el)  values' %source._get_name()
            logger.logNotice('%s: %s' %(raeson, ex))
            raise PosGeneratorError(raeson)

    @staticmethod
    def getLatitude(siteInfo):
        """Return the site latitude"""
        try:
            return siteInfo['latitude']
        except (KeyError, TypeError) as ex:
            raise PosGeneratorError('cannot get the latitude: %s' %ex)
        except Exception as ex:
            raeson = 'unexpected exception getting the site latitude'
            logger.logNotice(raeson)
            raise PosGeneratorError(raeson)

    @staticmethod
    def getParallacticAngle(latitude, az, el):
        """Arguments in radians"""
        denominator = tan(latitude) * cos(el) - sin(el) * cos(az)

        # Avoid division by zero and keep continuity
        if isclose(denominator, 0, abs_tol=1e-10):
            return -90.0 if sin(az) > 0 else 90.0

        return degrees(atan2(-sin(az), denominator))

    @staticmethod
    def getGalacticAngle(ra, dec):
        """Arguments in radians"""
        # North celestial pole coordinates in equatorial celestial frame (j2000)
        # ncp = ('12 51 26.28', '27 07 41.7')
        # ra0 = ephem.hours(ncp[0])
        # dec0 = ephem.degrees(ncp[1])
        ra0 = 3.3660332687500043
        dec0 = 0.47347728280415174

        denominator = cos(dec) * tan(dec0) - sin(dec) * cos(ra-ra0)

        # Avoid division by zero and keep continuity
        if isclose(denominator, 0, abs_tol=1e-10):
            return 90.0 if sin(ra-ra0) > 0 else -90.0

        return degrees(atan2(sin(ra-ra0), denominator))

    @staticmethod
    def getGalacticParallacticAngle(latitude, az, el, ra, dec):
        """Arguments in radians"""
        p = PosGenerator.getParallacticAngle(latitude, az, el)
        g = PosGenerator.getGalacticAngle(ra, dec)
        return p + g


class PosGeneratorError(Exception):
    pass
