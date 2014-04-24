from Receivers__POA import DewarPositioner as POA
from Acspy.Servants.CharacteristicComponent import CharacteristicComponent as cc
from Acspy.Servants.ContainerServices import ContainerServices as services
from Acspy.Servants.ComponentLifecycle import ComponentLifecycle as lcycle
from Acspy.Util.BaciHelper import addProperty
from maciErrTypeImpl import CannotGetComponentExImpl

from ComponentErrorsImpl import CouldntGetComponentExImpl, ValidationErrorExImpl
from DewarPositioner import configuration
from IRAPy import logger


class DewarPositionerImpl(cc, POA, services, lcycle):
    def __init__(self):
        cc.__init__(self)
        services.__init__(self)
        self.configured = False
        self.actualSetup = ''

    def initialize(self):
        addProperty(self, 'fooProperty')

    def setup(self, code):
        code = code.upper()
        try:
            # Verify if there is a derotator associated to this code
            comp_name = configuration.derotator[code]
        except KeyError, ex:
            logger.logError("code %s unknown" %ex.message)
            ex = ValidationErrorExImpl("code %s unknown" %ex.message)
            raise ex.getComponentErrorsEx()

        try:
            self.derotator = self.getComponent(comp_name)
            self.actualSetup = code
            self.configured = True
        except CannotGetComponentExImpl, ex:
            logger.logDebug(ex.message)
            ex = CouldntGetComponentExImpl(ex.message)
            raise ex.getComponentErrorsEx()
        except Exception, ex:
            logger.logDebug(ex.message)
            ex = CouldntGetComponentExImpl('Unexpected exception loading %s' %comp_name)
            raise ex.getComponentErrorsEx()

    def isReady(self):
        try:
            return self.configured and self.derotator.isReady()
        except AttributeError, ex:
            return False


    def getActualSetup(self):
        return self.actualSetup



