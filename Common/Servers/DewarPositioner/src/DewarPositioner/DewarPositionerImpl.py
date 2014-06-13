from multiprocessing import Queue
from Receivers__POA import DewarPositioner as POA
from Acspy.Servants.CharacteristicComponent import CharacteristicComponent as cc
from Acspy.Servants.ContainerServices import ContainerServices as services
from Acspy.Servants.ComponentLifecycle import ComponentLifecycle as lcycle
from Acspy.Util.BaciHelper import addProperty
from maciErrType import CannotGetComponentEx

import ComponentErrorsImpl
import ComponentErrors
import DerotatorErrorsImpl
import DerotatorErrors 

from DewarPositioner.configuration import CDBConf
from DewarPositioner.positioner import Positioner, PositionerError, NotAllowedError
from DewarPositioner.posgenerators import goto

from IRAPy import logger


class DewarPositionerImpl(POA, cc, services, lcycle):

    def __init__(self):
        cc.__init__(self)
        services.__init__(self)
        self.cdbconf = CDBConf()
        self.positioner = Positioner()
        self._setDefaultConfiguration() 

    def initialize(self):
        addProperty(self, 'fooProperty')
    
    def setup(self, code):
        self.commandedSetup = code.upper()
        self.cdbconf.setup(self.commandedSetup)
        device_name = self.cdbconf.get_entry('derotator_name')
        starting_position = self.cdbconf.get_entry('starting_position') 
        try:
            self.positioner.setup(device_name, starting_position)
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
        # lancio positioner.startUpdating e catturo le dovute eccezioni
        if not self.getUpdatingMode():
            raeson = "updating mode not selected: call setUpdatingMode()"
            logger.logError(raeson)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(raeson)
            raise exc
        elif not self.positioner.isConfigured():
            raeson = "system not yet configured: a setup() is required"
            logger.logError(raeson)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(raeson)
            raise exc
        elif self.positioner.isUpdating():
            raeson = "the positioner is busy: a stopUpdating() is required"
            logger.logError(raeson)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(raeson)
            raise exc
        else:
            # Sistema configurato: OK updating Mode, OK rewinding Mode
            pass
        # Se sta aggiornando, un nuovo startUpdating cosa fa?

        # self.positioner = Positioner(self, 1, 2, 3) # Passo i vari argomenti
        # self.positioner.start() # Start the daemon
        raise NotImplementedError('method not yet implemented')

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
            raeson = "cannot known if %s is ready" %device_name
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
            raeson = "cannot known if %s is slewing" %device_name
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
            raeson = "cannot known if %s is tracking" %device_name
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
            raeson = "cannot set the %s offset" %device_name
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
            raeson = "cannot set the %s offset" %device_name
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

        
