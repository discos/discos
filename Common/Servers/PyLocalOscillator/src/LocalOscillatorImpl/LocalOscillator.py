#! /usr/bin/env python
#*******************************************************************************
# ALMA - Atacama Large Millimiter Array
# (c) UNSPECIFIED - FILL IN, 2015 
# 
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
# 
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
#
# "@(#) $Id$"
#
# who       when      what
# --------  --------  ----------------------------------------------
# spoppi  2015-10-14  created
#

#************************************************************************
#   NAME
# 
#   SYNOPSIS
# 
#   DESCRIPTION
#
#   FILES
#
#   ENVIRONMENT
#
#   RETURN VALUES
#
#   CAUTIONS
#
#   EXAMPLES
#
#   SEE ALSO
#
#   BUGS     
#
#------------------------------------------------------------------------
#

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

import Receivers
import ComponentErrorsImpl
import ComponentErrors
 
class LocalOscillator(Receivers__POA.LocalOscillator, CharacteristicComponent, ContainerServices, ComponentLifecycle):

   def __init__(self):
      CharacteristicComponent.__init__(self)
      ContainerServices.__init__(self)
#
# ___oOo___
   def cleanUp(self):
  
     
       pass
   
   def initialize(self):
       addProperty(self, 'frequency', devio_ref=frequencyDevIO())
       addProperty(self, 'amplitude', devio_ref=amplitudeDevIO())
       addProperty(self, 'isLocked', devio_ref=isLockedDevIO())

       pass
   
   def set(self,rf_power,rf_freq):
       pass
   
   
   def get(self,rf_power,rf_freq):
     
      pass
  
   def rfon(self):
    
      pass
  
   def rfoff(self):
    
       pass    