import Metrology__POA
from Acspy.Servants.ContainerServices import ContainerServices
from Acspy.Servants.ComponentLifecycle import ComponentLifecycle
from Acspy.Servants.ACSComponent import ACSComponent
from Acspy.Common.Log import getLogger
from Metrology import  meteoparameters
import socket  # networking module
import sys  
import time
import string 

class MeteoData (Metrology__POA.MeteoData,   # CORBA STUBS
		      ACSComponent,  # IDL Interface #
		      ContainerServices,  #
                      ComponentLifecycle): #
	
	def __init__(self):
		
		ACSComponent.__init__(self)
		ContainerServices.__init__(self)

		print "initialize \n"	 		
	    	return
		
	def initialize(self):
		'''
		Override the method from ComponentLifeCycle
		'''
		self.meteoparameters=meteoparameters(0.,0.,0.,0,0,0,0,0) # structure meteoparameters from Metrology idl module
 		 
		self.timeout=0 # timeout between send and receive 
		self.host = '192.167.189.43'# sys.argv[1]  # server address
		self.port = 5002 # int(sys.argv[2])  # server port
		self.command='spettro\n'
		self.getLogger().logInfo("Initializing...")
		self.humidity = 0.0
		self.temperature = 0.0
		self.pressure = 0.0
		try:
			self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
			self.s.setblocking(1)
			self.getLogger().logDebug("Creating Socket")		    
		 
			self.s.connect((self.host, self.port))
			self.getLogger().logInfo("connecting to host  %s, port %d" % (self.host,self.port)) 

		except socket.error, (value,message): 
			
			self.getLogger().logError("socket error, host %s, port %d, message %s" % (self.host,self.port,message))
			
		except:

		 	self.getLogger().logError("Error connecting host %s, port %d" % (self.host,self.port))
	
	def receive(self):
		done=0
		msg = ""
		while not done:
			ch = self.s.recv(1)
			if ch == '':
				return ""
			if ch == '\n':
				done = 1
			msg = msg +ch
		return msg

	def sendtofs(self):
		''' 
		Send command to field system and return reply
		'''
		try:
			self.s.send(self.command)
			self.getLogger().logDebug("Sending command %s" % self.command)
			v = self.receive() #v = self.s.recv(256)
			self.getLogger().logDebug("Receiving reply command %s" % self.command)
			dsplit=v.split() # the reply is splitted into a list.
			return dsplit 
		except  socket.error, (value,message): 
			self.getLogger().logError("Error receiving data from socket")		
			self.getLogger().logError("Exception %s" % message)
			return []; #empty list

					 
	def cleanUp(self):
		'''
		Override method
		'''
		self.getLogger().logInfo("called Cleanup")
		self.s.close()
		
		return
	def getdata (self):
		'''
		Get all  the meteo parameters
		'''
		self.getLogger().logInfo("get All meteo parameters")
		dsplit=self.sendtofs()
		nn= len(dsplit)
		
		if (nn < 2):
			 self.getLogger().logError("Not enough data from socket")		 
		else:
			self.temperature = float(dsplit[nn-3])
			self.pressure = float(dsplit[nn-2])
			self.humidity = float(dsplit[nn-1]) 
		return meteoparameters(self.temperature,self.humidity,self.pressure,0,0,0,0,0)
		
	def getTemperature (self):
	 
		'''
		Get all  the temperature parameters
		'''
		self.getLogger().logInfo("get Temperature")
		dsplit=self.sendtofs() # list containing the fields of the reply from the fs
		nn= len(dsplit) # number of elements of the reply list
		if (nn < 2):
			 self.getLogger().logError("Not enough data from socket")		 
		else:
			 self.temperature = float(dsplit[nn-3])
		return self.temperature
	 
		
	
	def getHumidity (self):
	 
		'''
		Get    the Humidity  
		'''
		self.getLogger().logInfo("get Humidity")
		dsplit=self.sendtofs()
		nn= len(dsplit)
		
#		print dsplit
		if (nn < 2):
			 self.getLogger().logError("Not enough data from socket")		 
		else:
			self.humidity = float(dsplit[nn-1]) 
		return self.humidity   
	
	def getPressure (self):
	 
		'''
		Get    the Humidity  
		'''
		self.getLogger().logInfo("get Pressure")
		
		dsplit=self.sendtofs()
		nn= len(dsplit)
		
#		print dsplit
		if (nn < 2):
			 self.getLogger().logError("Not enough data from socket")		 
		else:
			self.pressure = float(dsplit[nn-2]) 
		return self.pressure   
	 
# only for debug.
		
if __name__=='__main__' :
  try:	
	a=MeteoData()    
  except:
	print "Something Wrong"
