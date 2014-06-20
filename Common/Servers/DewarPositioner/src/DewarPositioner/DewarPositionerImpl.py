from multiprocessing import Queue
from Receivers__POA import DewarPositioner as POA
from Acspy.Servants.CharacteristicComponent import CharacteristicComponent as cc
from Acspy.Servants.ContainerServices import ContainerServices as services
from Acspy.Servants.ComponentLifecycle import ComponentLifecycle as lcycle
from Acspy.Util.BaciHelper import addProperty
from Acspy.Clients.SimpleClient import PySimpleClient
from maciErrType import CannotGetComponentEx

import ComponentErrorsImpl
import ComponentErrors
import DerotatorErrorsImpl
import DerotatorErrors 

from DewarPositioner.configuration import CDBConf
from DewarPositioner.positioner import Positioner, PositionerError, NotAllowedError

from IRAPy import logger

class DewarPositionerImpl(POA, cc, services, lcycle):

    def __init__(self):
        cc.__init__(self)
        services.__init__(self)
        self.cdbconf = CDBConf()
        self.positioner = Positioner()
        self._setDefaultConfiguration() 
        self.client = PySimpleClient()

    def initialize(self):
        addProperty(self, 'fooProperty')
    
    def setup(self, code):
        self.commandedSetup = code.upper()
        self.cdbconf.setup(self.commandedSetup)
        device_name = self.cdbconf.get_entry('derotator_name')
        starting_position = self.cdbconf.get_entry('starting_position') 
        try:
            device = self.client.getComponent(device_name)
        except CannotGetComponentEx, ex:
            raeson = "cannot get the %s component: %s" %(device_name, ex.message)
            logger.logError(raeson)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(raeson)
            raise exc
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setReason(ex.message)
            raise exc

        try:
            observatory = self.client.getComponent('ANTENNA/Observatory')
            lat_obj = observatory._get_latitude()
            latitude, compl = lat_obj.get_sync()
            site_info = {'latitude': latitude}
        except Exception, ex:
            logger.logWarning('cannot get the site information: %s' %ex.message)
            site_info = {}

        try:
            source_name = 'ANTENNA/Mount'
            source = self.client.getComponent(source_name)
        except Exception:
            logger.logWarning('cannot get the %s component' %source_name)
            source = None

        try:
            self.positioner.setup(site_info, source, device, starting_position)
            self.setRewindingMode('AUTO')
            self.actualSetup = self.commandedSetup
        except PositionerError, ex:
            raeson = '%s' %ex.message
            logger.logError(raeson)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(raeson)
            raise exc
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setReason(ex.message)
            raise exc

    def park(self):
        try:
            self.positioner.park()
            self._setDefaultConfiguration()
        except NotAllowedError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex.message)
            raise exc
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setReason(ex.message)
            raise exc

    def getPosition(self):
        try:
            return self.positioner.getPosition()
        except NotAllowedError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex.message)
            raise exc
        except (DerotatorErrors.CommunicationErrorEx, ComponentErrors.ComponentErrorsEx), ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason("Cannot get the derotator position")
            raise exc
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setReason(ex.message)
            raise exc

    def startUpdating(self):
        try:
            self.positioner.startUpdating()
        except PositionerError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(raeson)
            raise exc
        except NotAllowedError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(raeson)
            raise exc
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setReason(ex.message)
            raise exc

    def stopUpdating(self):
        try:
            self.positioner.stopUpdating()
        except PositionerError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(ex.message)
            raise exc
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setReason(ex.message)
            raise exc

    def isConfigured(self):
        return self.positioner.isConfigured()

    def isConfiguredForUpdating(self):
        """Return True if an updating mode has been selected"""
        return self.positioner.isConfiguredForUpdating()

    def isConfiguredForRewinding(self):
        """Return True if a rewinding mode has been selected"""
        return self.positioner.isConfiguredForRewinding()

    def isReady(self):
        try:
            return self.positioner.isReady()
        except DerotatorErrors.CommunicationErrorEx, ex:
            raeson = "cannot known if the derotator is ready" 
            logger.logError('%s: %s' %(raeson, ex.message))
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(raeson)
            raise exc
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setReason(ex.message)
            raise exc

    def isSlewing(self):
        try:
            return self.positioner.isSlewing()
        except DerotatorErrors.CommunicationErrorEx, ex:
            raeson = "cannot known if the derotator is slewing" 
            logger.logError('%s: %s' %(raeson, ex.message))
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(raeson)
            raise exc
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setReason(ex.message)
            raise exc

    def isTracking(self):
        try:
            self.positioner.isTracking()
        except DerotatorErrors.CommunicationErrorEx, ex:
            raeson = "cannot known if the derotator is tracking"
            logger.logError('%s: %s' %(raeson, ex.message))
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(raeson)
            raise exc
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setReason(ex.message)
            raise exc

    def isUpdating(self):
        return self.positioner.isUpdating()

    def setOffset(self, offset):
        try:
            self.positioner.setOffset(offset)
        except (PositionerError, NotAllowedError), ex:
            raeson = "cannot set the derotator offset" 
            logger.logError('%s: %s' %(raeson, ex.message))
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(raeson)
            raise exc
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setReason(ex.message)
            raise exc

    def clearOffset(self):
        try:
            self.positioner.clearOffset()
        except PositionerError, ex:
            raeson = "cannot set the derotator offset"
            logger.logError('%s: %s' %(raeson, ex.message))
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(raeson)
            raise exc
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setReason(ex.message)
            raise exc

    def getOffset(self):
        return self.positioner.getOffset()

    def getActualSetup(self):
        return self.actualSetup

    def getCommandedSetup(self):
        return self.commandedSetup

    def setUpdatingMode(self, mode):
        try:
            self.positioner.setUpdatingMode(mode)
        except PositionerError, ex:
            raeson = 'cannot set the updating mode: %s' %ex.message
            logger.logError(raeson)
            exc = ComponentErrorsImpl.ValidationErrorExImpl()
            exc.setReason(raeson)
            raise exc # Can happen only in case of wrong system input

    def getUpdatingMode(self):
        return self.positioner.getUpdatingMode()

    def setRewindingMode(self, mode):
        try:
            self.positioner.setRewindingMode(mode)
        except PositionerError, ex:
            raeson = 'cannot set the rewinding mode: %s' %ex.message
            logger.logError(raeson)
            exc = ComponentErrorsImpl.ValidationErrorExImpl()
            exc.setReason(raeson)
            raise exc # Can happen only in case of wrong system input

    def getRewindingMode(self):
        return self.positioner.getRewindingMode()

    def _setDefaultConfiguration(self):
        self.actualSetup = 'unknown'
        self.commandedSetup = ''

        
