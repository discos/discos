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
        self.__reset()

    def initialize(self):
        addProperty(self, 'fooProperty')

    def setup(self, code):
        self.commandedSetup = code.upper()
        self.cdbconf.setup(self.commandedSetup)
        comp_name = self.cdbconf.get_entry('derotator_name')
            
        try:
            self.derotator = self.getComponent('RECEIVERS/' + comp_name)
        except CannotGetComponentEx, ex:
            logger.logDebug(ex.message)
            raise
        except Exception:
            logger.logError('unexpected exception loading %s' %comp_name)
            raise ComponentErrorsImpl.UnexpectedExImpl()
        try:
            self.derotator.setup()
        except (DerotatorErrors.ConfigurationErrorEx, ComponentErrors.ComponentErrorsEx), ex:
            raeson = "cannot perform the %s setup" %comp_name
            logger.logError('%s: %s' %(raeson, ex.message))
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(raeson)
            raise exc

        position = self.cdbconf.get_entry('starting_position')

        try:
            self.derotator.setPosition(position)
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
        if not self.isReady():
            raeson = "DewarPositioner not ready. A setup() is required."
            logger.logError(raeson)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(raeson)
            raise exc

        position = self.cdbconf.get_entry('starting_position')

        try:
            self.derotator.setPosition(position)
        except (DerotatorErrors.PositioningErrorEx, DerotatorErrors.CommunicationErrorEx), ex:
            raeson = "cannot set the derotator position"
            logger.logError('%s: %s' %(raeson, ex.message))
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(raeson)
            raise exc
        self.__reset()

    def getPosition(self):
        if not self.isReady():
            raeson = "DewarPositioner not ready. A setup() is required."
            logger.logError(raeson)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(raeson)
            raise exc

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
    
    def __reset(self):
        self.actualSetup = 'unknown'
        self.commandedSetup = ''



