#! /usr/bin/env python
#
# "@(#) $Id$"
#
# who       when        what
# --------  --------    ----------------------------------------------
# aorlati  	2019-01-23  created
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
from MedicinaLocalOscillatorImpl.devios import amplitudeDevIO,frequencyDevIO,GenericDevIO
import Acspy.Util.ACSCorba

import Receivers
import ComponentErrorsImpl
import ComponentErrors
import ReceiversErrors
import ReceiversErrorsImpl
 
from MedicinaLocalOscillatorImpl import CommandLine
from MedicinaLocalOscillatorImpl import CommandLineError
from IRAPy import userLogger

 
class MedicinaLocalOscillator(Receivers__POA.LocalOscillator, CharacteristicComponent, ContainerServices, ComponentLifecycle):

	def __init__(self):
		CharacteristicComponent.__init__(self)
		ContainerServices.__init__(self)
		self.freq=0.0
		self.power=0.0
      
# ___oOo___
	def cleanUp(self):
		self.cl.close()
   
	def initialize(self):
		name=self.getName()
		try:
			dal=Acspy.Util.ACSCorba.cdb()
			dao=dal.get_DAO_Servant("alma/"+name)
			IP=dao.get_string("IP")
			PORT=int(dao.get_double("PORT"))
		except Exception:
			exc=ComponentErrorsImpl.CDBAccessExImpl()
			raise exc
		self.cl=CommandLine.CommandLine()
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
		try:
			power=self.cl.getPower()
			freq=self.cl.getFrequency()
		except CommandLineError as ex:
			msg="cannot get values readout with message %s" % (ex.__str__)
			exc=ReceiversErrorsImpl.SynthetiserErrorExImpl()
			exc.setData('Details',msg);
			raise exc.getReceiversErrorsEx()
		return (power,freq)
  
	def rfon(self):
		try:
			self.cl.rfOn()
		except CommandLineError as ex:
			msg="cannot turn RF on with message %s" % (ex.__str__)
			exc=ReceiversErrorsImpl.SynthetiserErrorExImpl()
			exc.setData('Details',msg);
			raise exc.getReceiversErrorsEx()
  
	def rfoff(self):
		try:
			self.cl.rfOff()
		except CommandLineError as ex:
			msg="cannot turn RF off with message %s" % (ex.__str__)
			exc=ReceiversErrorsImpl.SynthetiserErrorExImpl()
			exc.setData('Details',msg);
			raise exc.getReceiversErrorsEx()
   