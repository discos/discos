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
        self.actualSetup = ''
        self._configured = False

    def initialize(self):
        addProperty(self, 'fooProperty')

    def setup(self, code):
        code = code.upper()
        self.commandedSetup = code
        # Assign the derotator name
        comp_name = self.cdbconf.derotator_name(code)
        # If there is not a derotator related to this code, raise an exception
        if not comp_name:
            raeson = "code %s unknown" %code
            logger.logError(raeson)
            exc = ComponentErrorsImpl.ValidationErrorExImpl()
            exc.setReason(raeson)
            raise exc

        try:
            self.derotator = self.getComponent('RECEIVERS/' + comp_name)
        except CannotGetComponentEx, ex:
            logger.logDebug(ex.message)
            raise
        except Exception:
            logger.logError('Unexpected exception loading %s' %comp_name)
            raise ComponentErrorsImpl.UnexpectedExImpl()
        try:
            self.derotator.setup()
            self.actualSetup = code
            self._configured = True
        except (DerotatorErrorsImpl.ConfigurationErrorEx, 
                ComponentErrorsImpl.ComponentErrorsEx), ex:
            raeson = "Cannot perform the %s setup" %comp_name
            logger.logError(raeson)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(raeson)
            raise exc

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

        self.derotator.setPosition(0)
        self.actualSetup = ''
        self.commandedSetup = ''
        self._configured = False

    def getPosition(self):
        if not self.isReady():
            raeson = "DewarPositioner not ready. A setup() is required."
            logger.logError(raeson)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(raeson)
            raise exc

        try:
            return self.derotator.getActPosition()
        except (DerotatorErrors.CommunicationErrorEx, 
                ComponentErrors.ComponentErrorsEx), ex:
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
            return self._configured and self.derotator.isReady()
        except AttributeError:
            return False

    def isSlewing(self):
        try:
            return self._configured and self.derotator.isSlewing()
        except AttributeError:
            return False


    def isTracking(self):
        try:
            return self._configured and self.derotator.isTracking()
        except AttributeError:
            return False


