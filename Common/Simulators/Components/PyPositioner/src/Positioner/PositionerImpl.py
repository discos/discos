from __future__ import print_function
import time

from Acspy.Servants.CharacteristicComponent import CharacteristicComponent as cc
from Acspy.Servants.ContainerServices import ContainerServices as services
from Acspy.Servants.ComponentLifecycle import ComponentLifecycle as lcycle
from Acspy.Util.BaciHelper import addProperty
from Acspy.Common.Log import getLogger

from TestNamespaceInterface__POA import Positioner as POA


from Positioner.devios import GenericDevIO, PositionDevIO, SequenceDevIO

__copyright__ = "Marco Buttu <marco.buttu@inaf.it>"


logger = getLogger()

class PositionerImpl(POA, cc, services, lcycle):
 
    def __init__(self):
        cc.__init__(self)
        services.__init__(self)
        self.position = 0
        self.sequence = [1.1, 2.2, 3.3]

    def initialize(self):
        addProperty(self, 'status', devio_ref=GenericDevIO())
        addProperty(self, 'position', devio_ref=PositionDevIO(self))
        addProperty(self, 'current', devio_ref=GenericDevIO())
        addProperty(self, 'seq', devio_ref=SequenceDevIO(self))
        print('Component initialized')

    def setPosition(self, position):
        print('Setting the position to ', position)
        self.position = position

    def getPosition(self):
        return self.position

    def setSequence(self, sequence):
        self.sequence = sequence

    def getSequence(self):
        return self.sequence
