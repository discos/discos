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
from DewarPositioner.positioner import Positioner

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
        self._setDefaultConfiguration() 

    def initialize(self):
        addProperty(self, 'fooProperty')
    

    def setup(self, code):
        self.commandedSetup = code.upper()

        self.cdbconf.setup(self.commandedSetup)
        self.comp_name = self.cdbconf.get_entry('derotator_name')
        self.starting_position = self.cdbconf.get_entry('starting_position') 
            
        try:
            self.derotator = self.getComponent('RECEIVERS/' + self.comp_name)
        except CannotGetComponentEx, ex:
            logger.logDebug(ex.message)
            raise
        except Exception:
            logger.logError('unexpected exception loading %s' %self.comp_name)
            raise ComponentErrorsImpl.UnexpectedExImpl()
        try:
            self.derotator.setup()
        except (DerotatorErrors.ConfigurationErrorEx, ComponentErrors.ComponentErrorsEx), ex:
            raeson = "cannot perform the %s setup" %self.comp_name
            logger.logError('%s: %s' %(raeson, ex.message))
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(raeson)
            raise exc

        try:
            self._clearOffset()
            self._setPosition(0) # The method adds both offset and starting position 
        except (DerotatorErrors.PositioningErrorEx, DerotatorErrors.CommunicationErrorEx), ex:
            raeson = "cannot set the derotator position"
            logger.logError('%s: %s' %(raeson, ex.message))
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(raeson)
            raise exc

        self.setRewindingMode('AUTO')
        self.actualSetup = self.commandedSetup

    def setTrackingMode(self, mode):
        self._setMode('tracking', mode)

    def getTrackingMode(self):
        return self.trackingMode

    def setRewindingMode(self, mode):
        self._setMode('rewinding', mode)

    def getRewindingMode(self):
        return self.rewindingMode

    def isConfigured(self):
        return self.commandedSetup == self.actualSetup

    def getActualSetup(self):
        return self.actualSetup

    def getCommandedSetup(self):
        return self.commandedSetup

    def park(self):
        self.stopTracking()
        self._checkConfiguration() # Raises NotAllowedEx if the check fails
        self._setDefaultConfiguration()
        self._setPosition(0) # The method adds both offset and starting position 

    def getPosition(self):
        self._checkConfiguration() # Raises NotAllowedEx if the check fails
        try:
            return self.derotator.getActPosition()
        except (DerotatorErrors.CommunicationErrorEx, ComponentErrors.ComponentErrorsEx), ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason("Cannot get the derotator position")
            raise exc
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason("Cannot get the derotator position")
            raise exc

    def startTracking(self):
        if not self.getTrackingMode():
            raeson = "tracking mode not selected: call setTrackingMode()"
            logger.logError(raeson)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(raeson)
            raise exc
        elif not self.isConfigured():
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
        # self.positioner.terminate()
        pass

    def isReady(self):
        try:
            return self.isConfigured() and self.derotator.isReady()
        except AttributeError:
            return False

    def isSlewing(self):
        try:
            return self.isConfigured() and self.derotator.isSlewing()
        except AttributeError:
            return False


    def isTracking(self):
        try:
            return self.isConfigured() and self.derotator.isTracking()
        except AttributeError:
            return False

    def setOffset(self, offset):
        act_position = self.getPosition()
        self._setOffset(offset)
        self._setPosition(act_position)

    def clearOffset(self):
        act_position = self.getPosition()
        self._clearOffset()
        self._setPosition(act_position)

    def getOffset(self):
        return self.offset

    def _setDefaultConfiguration(self):
        self.positioner = None
        self.actualSetup = 'unknown'
        self.commandedSetup = ''
        self.rewindingMode = ''
        self.trackingMode = ''
        self._clearOffset()

    def _setPosition(self, position):
        if not self.derotator.isReady():
            raeson = "derotator not ready to move: a setup() is required."
            logger.logError(raeson)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(raeson)
            raise exc

        target = self.starting_position + self.offset + position
        if self.derotator.getMinLimit() < target < self.derotator.getMaxLimit():
            try:
                self.derotator.setPosition(target)
            except (DerotatorErrors.PositioningErrorEx, DerotatorErrors.CommunicationErrorEx), ex:
                raeson = "cannot set the derotator position"
                logger.logError('%s: %s' %(raeson, ex.message))
                exc = ComponentErrorsImpl.OperationErrorExImpl()
                exc.setReason(raeson)
                raise exc
            except Exception, ex:
                raeson = "unknown exception setting the derotator position"
                logger.logError('%s: %s' %(raeson, ex.message))
                exc = ComponentErrorsImpl.OperationErrorExImpl()
                exc.setReason(raeson)
                raise exc

        else:
            raeson = "position %.2f out of range (%.2f, %.2f)" %(target, 
                     self.derotator.getMinLimit(), self.derotator.getMaxLimit())
            logger.logError(raeson)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(raeson)
            raise exc

    def _checkConfiguration(self):
        """Raises NotAllowedEx if the dewar positioner is not yet configured"""
        if not self.isConfigured():
            raeson = "DewarPositioner not configured. A setup() is required."
            logger.logError(raeson)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(raeson)
            raise exc

    def _clearOffset(self):
        self._setOffset(0.0)
    
    def _setOffset(self, offset):
        self.offset = offset

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


