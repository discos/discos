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
import Acspy.Util.ACSCorba

import Receivers
import ComponentErrorsImpl
import ComponentErrors
 
from LocalOscillatorImpl import CommandLine
from IRAPy import logger

#IP, PORT = "192.168.201.149", 5025 #real hw

 
class LocalOscillator(Receivers__POA.LocalOscillator, CharacteristicComponent, ContainerServices, ComponentLifecycle):

   def __init__(self):
      CharacteristicComponent.__init__(self)
      ContainerServices.__init__(self)
      self.cl=CommandLine.CommandLine() 
      self.freq=0.  
      self.power=0.
      
# ___oOo___
   def cleanUp(self):
      
      self.cl.close()
   
   
   def initialize(self):
       name= self.getName()
       dal = Acspy.Util.ACSCorba.cdb()
       dao=dal.get_DAO_Servant("alma/"+name)
       IP=  dao.get_string("IP")
       PORT = int(dao.get_double("PORT"))
       
       msg = self.cl.configure(IP,PORT)
       if msg != 'OK' :
              reason = "cannot get Synthetizer IP %s component: %s" %(IP,msg)
              logger.logError(reason)
              exc = ComponentErrorsImpl.SocketErrorExImpl()
              exc.setData('reason',msg)
              raise exc.getComponentErrorsEx()
       

       addProperty(self, 'frequency', devio_ref=frequencyDevIO(self.cl))
       addProperty(self, 'amplitude', devio_ref=amplitudeDevIO(self.cl))
       addProperty(self, 'isLocked', devio_ref=isLockedDevIO(self,self.cl))
       self.cl.configure(IP,PORT)
   
   def set(self,rf_power,rf_freq):
     try:
        self.cl.setPower(rf_power)
        self.cl.setFrequency(rf_freq)
        self.freq=rf_freq
        self.power=rf_power
        logger.logNotice('SYNT FREQ  set to %f ' %self.freq)
        logger.logNotice('SYNT POWER set to %f ' %self.power)

     except CommandLine.CommandLineError,ex :
          
        logger.logError(ex,message)

       
   
   
   
   
   

   
   def get(self):
     try:
       msg,power=self.cl.getPower()
       msg,freq= self.cl.getFrequency()
       print power
       print freq
       return (power,freq)
     
     
     except CommandLine.CommandLineError,ex :
       logger.logError(ex,message)


   def rfon(self):
    
      pass
  
   def rfoff(self):
    
       pass    
   
   def getInternalFrequency(self):
       return self.freq
   