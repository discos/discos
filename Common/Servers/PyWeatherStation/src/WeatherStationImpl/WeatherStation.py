#! /usr/bin/env python


import time
from math import radians
import Receivers__POA
from Acspy.Servants.CharacteristicComponent import CharacteristicComponent
from Acspy.Servants.ContainerServices import ContainerServices 
from Acspy.Servants.ComponentLifecycle import ComponentLifecycle 
from Acspy.Util.BaciHelper import addProperty
from Acspy.Clients.SimpleClient import PySimpleClient
from Acspy.Nc.Supplier import Supplier
from Acspy.Common.TimeHelper import getTimeStamp
from maciErrType import CannotGetComponentEx
from ACSErrTypeCommonImpl import CORBAProblemExImpl
from LocalOscillatorImpl.devios import amplitudeDevIO,frequencyDevIO,isLockedDevIO
import Acspy.Util.ACSCorba

import Receivers
import ComponentErrorsImpl
import ComponentErrors
 
from LocalOscillatorImpl import CommandLine
from IRAPy import logger


class WeatherStation (Weather__POA.LocalOscillator, CharacteristicComponent, ContainerServices, ComponentLifecycle):

       def __init__(self):
          pass
       def cleanUp(self):
          pass


       def initialize(self): 
           addProperty(self, 'temperature')
           
           addProperty(self, 'winddir')
           addProperty(self, 'pressure')
           addProperty(self, 'pressure')


       def getData(self):
            pass
       def getWindSpeedAverage(self):

            pass
       def getWindspeedPeak(self):

            pass
       def getWindDir(self):
            
            pass

       def getHumidity(self):
            pass
       

       def getPressure(self):

            pass
 

       
