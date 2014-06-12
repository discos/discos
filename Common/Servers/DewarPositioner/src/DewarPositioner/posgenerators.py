"""This module implements the position generators"""
from IRAPy import logger


def goto(position):
    yield position


def fixed(source, site_info):
    try:
        az_obj = source._get_azimuth()
        el_obj = source._get_elevation()
        az, cmp = az_obj.get_sync() 
        el, cmp = el_obj.get_sync()
        if not cmp:
            # Compute the position
            pass
        else:
            logger.logDebug('Some problems getting the antenna coordinates: %s' %cmp)
            # raise Something
    except Exception, ex:
        logger.logDebug('Unexcpected exception getting the antenna coordinates: %s' %ex)
        raise


