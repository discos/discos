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
from IRAPy import logger


class DewarPositionerImpl(POA, cc, services, lcycle):
    def __init__(self):
        cc.__init__(self)
        services.__init__(self)
        # self.positioner = Positioner() # TODO: Give it the parameters!
        self.cdbconf = CDBConf()
        self.reset() # Set the offset, actualSetup and commandedSetup vaules

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
            self._setPosition(0) # The method adds both offset and starting position 
        except (DerotatorErrors.PositioningErrorEx, DerotatorErrors.CommunicationErrorEx), ex:
            raeson = "cannot set the derotator position"
            logger.logError('%s: %s' %(raeson, ex.message))
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(raeson)
            raise exc

        self.actualSetup = self.commandedSetup


    def isConfigured(self):
        return self.commandedSetup == self.actualSetup

    def getActualSetup(self):
        return self.actualSetup

    def getCommandedSetup(self):
        return self.commandedSetup

    def park(self):
        self._checkConfiguration() # Raises NotAllowedEx if the check fails
        self._setPosition(0) # The method adds both offset and starting position 
        self.reset()

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
        self.positioner.start() # Start the daemon

    def stopTracking(self):
        self.positioner.terminate()

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
        self.offset = offset

    def getOffset(self):
        return self.offset
    
    def reset(self):
        self.actualSetup = 'unknown'
        self.commandedSetup = ''
        self.setOffset(0.0)

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

