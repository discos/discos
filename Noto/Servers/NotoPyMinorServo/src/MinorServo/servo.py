import datetime
import time
import threading
import socket, errno

import Management
import Antenna
import MinorServoErrors
import MinorServoErrorsImpl
import ComponentErrorsImpl
import ManagementErrorsImpl
import ACSLog

from Acspy.Servants import ContainerServices
from Acspy.Common.TimeHelper import getTimeStamp
from IRAPy import logger
from math import *


class focus(object):
	primary=0
	secondary=1

class Servo(object):

	classLock=threading.Lock()

	def __init__(self,service):
		"""Initialize the `servo` 
		"""
		self.services=service
		self.client_socket=None
		self.antennaBoss=None
		self.currentConf = ""
		self.focus=focus.primary
		self.axisNumber=2
		self.polX=[0.0,0.0,0.0]
		self.polY=[0.0,0.0,0.0]
		self.polZ1=[0.0,0.0,0.0]
		self.polZ2=[0.0,0.0,0.0]
		self.polZ3=[0.0,0.0,0.0]  
		self.position=[0.0,0.0,0.0,0.0,0.0]
		self.offset=[0.0,0.0,0.0,0.0,0.0]
		self.upperLimit=[0.0,0.0,0.0,0.0,0.0]
		self.lowerLimit=[0.0,0.0,0.0,0.0,0.0]
		self.primaryAxisCode={'Y': 0, 'Z': 1}
		self.secondaryAxisCode={'X': 0, 'Y': 1, 'Z1': 2, 'Z2': 3, 'Z3': 4}
		self.primaryAxisName={0: 'Y', 1: 'Z'}
		self.secondaryAxisName={0: 'X',1: 'Y', 2: 'Z1', 3: 'Z2', 4: 'Z3'}
		""" Bussiness logic parameters """
		self.configured=False
		self.connected=False;

	def __del__(self):
		self.disconnect()

	def connect(self):
		try:
			self.client_socket= socket.socket(socket.AF_INET, socket.SOCK_STREAM)
			self.client_socket.connect(('192.167.187.92',5003))
			self.connected=True
		except socket.error, ex:
			newEx=ComponentErrorsImpl.SocketErrorExImpl(exception=ex,create=1)
			newEx.log(self.services.getLogger(),ACSLog.ACS_LOG_DEBUG)
			raise newEx
		try:
			self.antennaBoss=self.services.getComponent('ANTENNA/Boss')
		except CannotGetComponentEx, ex:
			newEx=ComponentErrorsImpl.CouldntGetComponentExImpl(exception=ex,create=1)
			newEx.setComponentName('ANTENNA/Boss')
			newEx.log(self.services.getLogger(),ACSLog.ACS_LOG_DEBUG)
			raise newEx
		except Exception, ex:
			newEx=ComponentErrorsImpl.UnexpectedExImpl(exception=ex,create=1)
			newEx.log(self.services.getLogger(),ACSLog.ACS_LOG_DEBUG)
			raise newEx

	def disconnect(self):
		if self.connected:
			self.client_socket.close()
			self.connected=False
		if self.antennaBoss!=None:
			try:
				self.services.releaseComponent('ANTENNA/Boss')
			except Exception,ex:
				pass
			finally:
				self.antennaBoss=None
		self.currentConf=""
		
	def updatePosition(self):
		print "Updating position......."
		if (self.configured) and (self.currentConf!="") and (self.antennaBoss!=None):
			ctime=getTimeStamp().value
			try:
				az,el=self.antennaBoss.getRawCoordinates(ctime)
			except Exception , ex:
				newEx=ComponentErrorsImpl.CouldntCallOperationExImpl(exception=ex,create=1)
				newEx.setOperationName('getRawCoordinates')
				newEx.log(self.services.getLogger(),ACSLog.ACS_LOG_DEBUG)
				raise newEx
			self._computePosition(el)
			buffer=self._prepareMessage()
			try:
				self._sendData(buffer)
				time.sleep(1)
				data=self.client_socket.recv(128)
			except Exception,ex:
				pass
			
			
	def _prepareMessage(self):
		if self.focus==focus.primary:
			buffer="0,%1d,7,%6.2lf,%6.2lf,%6.2lf,%6.2lf,%6.2lf" % (self.focus,self.position[0],self.position[1],self.position[2],self.position[3],self.position[4])
		else:
			buffer="0,%1d,7,%6.2lf,%6.2lf" % (self.focus,self.position[0],self.position[1])
		return buffer

	def _sendData(self,msg):
		length=len(msg)	
		if self.connected:
			self.client_socket.sendall(struct.pack('=i',length))
			self.client_socket.sendall(msg)
	
	def setup(self, code):
		"""Configure the servo.
		a `setup()` loads the parameters related to a condifuration code and starts the servo conifugration.
		:param code: mnemonic code of the configuration.
		"""
		Servo.classLock.acquire()
		try:
			code.upper()
			if code=="QQC":
				self.focus=focus.secondary
				self.axisNumber=5
				self.upperLimit=[100,100,100,100,100]
				self.lowerLimit=[-100,-100,-100,-100,-100]
				self.offset=[0.0,0.0,0.0,0.0,0.0]
				self.polX[0]=0.0
				self.polX[1]=0.0
				self.polX[2]=0.89
				self.polY[0]=8.3689e-4
				self.polY[1]=0.152495
				self.polY[2]=20.91
				self.polZ1[0]=0.00168640  
				self.polZ1[1]=-0.271430
				self.polZ1[2]=67.55
				self.polZ2[0]=0.00168640  
				self.polZ2[1]=-0.271430
				self.polZ2[2]=84.37
				self.polZ3[0]=0.00168640  
				self.polZ3[1]=-0.271430
				self.polZ3[2]=-57.40
			elif code=="KKC":
				self.focus=focus.secondary
				self.axisNumber=5
				self.upperLimit=[85,100,85,85,85]
				self.lowerLimit=[-85,-85,-85,-85,-85]
				self.offset=[0.0,0.0,0.0,0.0,0.0]
				self.polX[0]=0.0
				self.polX[1]=0.0
				self.polX[2]=-1
				self.polY[0]=8.3689e-4
				self.polY[1]=0.152495
				self.polY[2]=-10.4
				self.polZ1[0]=0.00128  
				self.polZ1[1]=-0.13644
				self.polZ1[2]=9.4
				self.polZ2[0]=0.00128  
				self.polZ2[1]=-0.23394
				self.polZ2[2]=10.6
				self.polZ3[0]=0.00128  
				self.polZ3[1]=-0.23394
				self.polZ3[2]=13.4
			else:
				newEx=ManagementErrorsImpl.ConfigurationErrorExImpl()
				newEx.log(self.services.getLogger(),ACSLog.ACS_LOG_DEBUG)
				raise newEx
			self.currentConf=code
			"""
			*****************************************
			it should be set when the migration /primary/secondary is completed
			"""
			self.configured=True
		finally:
			Servo.classLock.release()
		
	def _computePosition(self,el):
		delev=degrees(el)
		if self.focus==focus.secondary:
			self.position[0]=self.polX[0]*delev*delev+self.polX[1]*delev+self.polX[2]
			self.position[1]=self.polY[0]*delev*delev+self.polY[1]*delev+self.polY[2]
			self.position[2]=self.polZ1[0]*delev*delev+self.polZ1[1]*delev+self.polZ1[2]
			self.position[3]=self.polZ2[0]*delev*delev+self.polZ2[1]*delev+self.polZ2[2]
			self.position[4]=self.polZ3[0]*delev*delev+self.polZ3[1]*delev+self.polZ3[2]
		else:
			self.position[0]=self.polY[0]*delev*delev+self.polY[1]*delev+self.polY[2]
			self.position[1]=self.polZ[0]*delev*delev+self.polZ[1]*delev+self.polZ[2]
		for i in range(self.axisNumber):
			self.position[i]=self.position[i]+self.offset[i]
			if self.position[i]>self.upperLimit[i]:
				self.position[i]=self.upperLimit[i]
				logger.logWarning('servo axis against upper soft limit: %s' % self.getAxisName(i))
			if self.position[i]<self.lowerLimit[i]:
				self.position[i]=self.lowerLimit[i]
				logger.logWarning('servo axis against lower soft limit: %s' % self.getAxisName(i))	
	
	def getAxisName(self,index):
		if self.focus==focus.secondary:
			return self.secondaryAxisName[index]
		else:
			return self.primaryAxisName[index]
		
