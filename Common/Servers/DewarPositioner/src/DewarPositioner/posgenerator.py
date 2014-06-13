"""This module implements the position generators"""
from math import sin, cos, tan, atan, radians, degrees
from Acspy.Clients.SimpleClient import PySimpleClient
from IRAPy import logger


class PosGenerator(object):
    def __init__(self):

        self.client = PySimpleClient()
        try:
            observatory = self.client.getComponent('ANTENNA/Observatory')
            lat_obj = observatory._get_latitude()
            latitude, compl = lat_obj.get_sync()
            self.latitude = radians(latitude)
        except Exception, ex:
            logger.logError("cannot get the observatory component: %s" %ex.message)
            logger.logWarning("setting a default latitude (SRT latitude)")
            self.latitude = radians(39.49304) # Set the SRT latitude

        try:
            mount_name = 'ANTENNA/Mount'
            self.mount = self.client.getComponent(mount_name)
            self.az_obj = self.mount._get_azimuth()
            self.el_obj = self.mount._get_elevation()
        except Exception, ex:
            logger.logError("cannot get the %s: %s" %(mount_name, ex.message))
            self.mount = None

    def goto(self, position):
        yield position

    def fixed(self):
        if not self.mount:
            raeson = 'mount component not available' 
            logger.logError(raeson)
            raise PosGeneratorError(raeson)

        while True:
            try:
                azd, cmpl = self.az_obj.get_sync()
                eld, cmpl = self.el_obj.get_sync()
                az = radians(azd) # Azimuth in radians
                el = radians(eld) # Elevation in radians
            except Exception:
                reason = 'cannot get the %s properties' %source._get_name()
                logger.logError(raeson)
                raise PosGeneratorError(raeson)

            if not cmpl.code:
                try:
                    tan_p = - sin(az) / (tan(self.latitude)*cos(el) - sin(el)*cos(az))
                    p = atan(tan_p)
                    yield degrees(p)
                except ZeroDivisionError:
                    logger.logWarning('zero division error computing tan(p)')
                    pass 
            else:
                reason = 'cannot get the %s coordinates' %source._get_name()
                logger.logError(raeson)
                raise PosGeneratorError(raeson)


    def optimized(source, site_info):
        yield self.fixed()

    
class PosGeneratorError(Exception):
    pass

