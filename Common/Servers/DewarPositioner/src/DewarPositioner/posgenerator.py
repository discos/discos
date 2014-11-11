"""This module implements the position generators"""
import datetime
import time
from math import sin, cos, tan, atan, radians, degrees
from IRAPy import logger


class PosGenerator(object):

    def __init__(self, zdtimeout=5):
        self.zdtimeout = zdtimeout # Timeout in case of zero division error
      
    def goto(self, position):
        yield position

    def parallactic(self, source, site_info, initial_position=0):
        """Generate a parallactic...
         
        A better description...

        :param source: object with ``_get_azimuth()`` and ``_get_elevation()``
         methods
        :param site_info: di
        :type site_info: dict
        :returns:  a position
        :raises: PosGeneratorError
        """
        try:
            latitude = radians(site_info['latitude'])
            az_obj = source._get_azimuth()
            el_obj = source._get_elevation()
        except (KeyError, TypeError), ex:
            raise PosGeneratorError('cannot get the latitude: %s' %ex.message)
        except Exception, ex:
            raeson = 'cannot get the %s property objects' %source._get_name()
            logger.logDebug('%s: %s' %raeson)
            raise PosGeneratorError(raeson)

        last_zerodiv_time = datetime.datetime.now()
        while True:
            try:
                azd, cmpl_az = az_obj.get_sync()
                eld, cmpl_el = el_obj.get_sync()
                az = radians(azd) # Azimuth in radians
                el = radians(eld) # Elevation in radians
            except Exception, ex:
                raeson = 'cannot get the %s property values' %source._get_name()
                logger.logDebug('%s: %s' %(raeson, ex.message))
                raise PosGeneratorError(raeson)

            if not cmpl_az.code and not cmpl_el.code:
                try:
                    tan_p = - sin(az) / (tan(latitude)*cos(el) - sin(el)*cos(az))
                    last_zerodiv_time = datetime.datetime.now()
                    p = atan(tan_p)
                    yield initial_position + degrees(p)
                except ZeroDivisionError:
                    logger.logWarning('zero division error computing tan(p)')
                    zerodiv_time = datetime.datetime.now() - last_zerodiv_time
                    if zerodiv_time.seconds >= self.zdtimeout:
                        raeson = 'zero division for more than %ds' %self.zdtimeout
                        logger.logError(raeson)
                        raise PosGeneratorError(raeson)
                    else:
                        time.sleep(0.5)
                        continue
            else:
                raeson = 'cannot get the %s coordinates' %source._get_name()
                logger.logDebug(raeson)
                raise PosGeneratorError(raeson)

    
class PosGeneratorError(Exception):
    pass

