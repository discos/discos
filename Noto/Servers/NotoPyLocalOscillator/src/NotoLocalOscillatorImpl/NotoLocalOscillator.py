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
from NotoLocalOscillatorImpl.devios import amplitudeDevIO,frequencyDevIO,GenericDevIO
import Acspy.Util.ACSCorba

import Receivers
import ComponentErrorsImpl
import ComponentErrors
import ReceiversErrors
import ReceiversErrorsImpl
 
from NotoLocalOscillatorImpl import CommandLine
from NotoLocalOscillatorImpl import CommandLineError
from IRAPy import userLogger

import socket
#IP, PORT = "192.168.201.149", 5025 #real hw

 
class NotoLocalOscillator(Receivers__POA.LocalOscillator, CharacteristicComponent, ContainerServices, ComponentLifecycle):

	def __init__(self):
		CharacteristicComponent.__init__(self)
		ContainerServices.__init__(self)
		self.cl=CommandLine.CommandLine() 
		self.freq=0.  
		self.power=0.
      
	def cleanUp(self):
		self.cl.close()

	def initialize(self):
		name=self.getName()
		try:
			dal=Acspy.Util.ACSCorba.cdb()
			dao=dal.get_DAO_Servant("alma/"+name)
			IP= dao.get_string("IP")
			PORT=int(dao.get_double("PORT"))
		except Exception:
			exc=ComponentErrorsImpl.CDBAccessExImpl()
			raise exc
		try:
			self.cl.initialize()
		except CommandLineError as ex:
			exc=ComponentErrorsImpl.SocketErrorExImpl()
			exc.setData('Reason',ex.__str__)
			raise exc
		try:
			self.cl.configure(IP,PORT)
		except CommandLineError as ex:
			msg="cannot get synthesiser IP %s with message %s" %(IP,ex.__str__)
			exc=ComponentErrorsImpl.SocketErrorExImpl()
			exc.setData('Reason',msg)
			raise exc	
		addProperty(self, 'frequency', devio_ref=frequencyDevIO(self.cl))
		addProperty(self, 'amplitude', devio_ref=amplitudeDevIO(self.cl))
		addProperty(self, 'isLocked', devio_ref=GenericDevIO(value=1))
   
	def set(self,rf_power,rf_freq):
		try:
			self.cl.setFrequency(rf_freq)
		except CommandLineError as ex:
			msg="cannot set frequency with message %s" % (ex.__str__)
			exc=ReceiversErrorsImpl.SynthetiserErrorExImpl()
			exc.setData('Details',msg);
			raise exc.getReceiversErrorsEx()
		try:
			self.cl.setPower(rf_power)
		except CommandLineError as ex:
			msg="cannot set power with message %s" % (ex.__str__)
			exc=ReceiversErrorsImpl.SynthetiserErrorExImpl()
			exc.setData('Details',msg);
			raise exc.getReceiversErrorsEx()     
		userLogger.logNotice("Synthesiser set to %f MHz at power %f dBm\n"%(rf_freq,rf_power))   	
   
	def get(self):
		txt=""
		power=0.0
		freq=0.0
		try:
			txt,power=self.cl.getPower()
			txt,freq=self.cl.getFrequency()
		except CommandLineError as ex:
			msg="cannot get readout values with message %s" % (ex.__str__)
			exc=ReceiversErrorsImpl.SynthetiserErrorExImpl()
			exc.setData('Details',msg);
			raise exc.getReceiversErrorsEx()
		return (power,freq)   	

	def rfon(self): 
		pass
		
	def rfoff(self):
		pass