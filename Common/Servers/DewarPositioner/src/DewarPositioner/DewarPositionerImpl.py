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
from DewarPositioner.positioner import Positioner, PositionerError
from DewarPositioner.posgenerators import goto

from IRAPy import logger


class DewarPositionerImpl(POA, cc, services, lcycle):

    modes = {
            'tracking': ('FIXED', 'OPTIMIZED'),
            'rewinding': ('AUTO', 'MANUAL')
    }

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
        except PositionerError, ex:
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
        except PositionerError, ex:
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


    def startTracking(self):
        if not self.getTrackingMode():
            raeson = "tracking mode not selected: call setTrackingMode()"
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
        else:
            # Sistema configurato: OK tracking Mode, OK rewinding Mode
            pass
        # Se e' in tracking, un nuovo startTracking cosa fa?

        # self.positioner = Positioner(self, 1, 2, 3) # Passo i vari argomenti
        # self.positioner.start() # Start the daemon
        raise NotImplementedError('method not yet implemented')


    def stopTracking(self):
        try:
            self.positioner.stop()
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


    def setOffset(self, offset):
        try:
            self.positioner.setOffset(offset)
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


    def setTrackingMode(self, mode):
        self._setMode('tracking', mode)


    def getTrackingMode(self):
        return self.trackingMode


    def setRewindingMode(self, mode):
        self._setMode('rewinding', mode)


    def getRewindingMode(self):
        return self.rewindingMode


    def _setDefaultConfiguration(self):
        self.actualSetup = 'unknown'
        self.commandedSetup = ''
        self.rewindingMode = ''
        self.trackingMode = ''


    def _setMode(self, mode_type, mode):
        try:
            modes = DewarPositionerImpl.modes[mode_type]
        except KeyError:
            raeson = "_setMode(): mode type %s not in %s" %(
                    mode_type, DewarPositionerImpl.modes.keys())
            logger.logDebug(raeson)
            exc = ComponentErrorsImpl.ValidationErrorExImpl()
            exc.setReason(raeson)
            raise exc # Can happen only in case of wrong system input
        if mode not in modes:
            raeson = '%s mode %s unknown. Allowed modes: %s' %(mode_type, mode, modes)
            logger.logError(raeson)
            exc = ComponentErrorsImpl.ValidationErrorExImpl()
            exc.setReason(raeson)
            raise exc
        else:
            setattr(self, mode_type + 'Mode', mode)


