import socket
import time
from MedicinaLocalOscillatorImpl import CommandLineError

class CommandLine:
	def __init__(self, p_cmd_dict):
		"""
		Init Command line comm interface
		It needs command strings to be sent to 
		the translator server ( it routes commands to synths )

		Some assumptions on command strings are done, 
		no protocol string checks are provided, hence with a 
		malformed command string synth drive may fail!
		"""
		self.m_cmd_dict= p_cmd_dict
		self.sock=None
		self.connected=False;
		self.ip=""
		self.port=0
		self.power=0.0
		self.freq=0.0
		self.powerTime=0
		self.freqTime=0
		self.last_answer=""
		
	def initialize(self):
		#raises an error.....
		self.create()

	def __del__(self):
		self.close()	
   
	def configure(self,ip,port):
		self.ip=ip
		self.port=port
		#raises an error......
		self.connect()

	def setPower(self,power):
		cmd=self.m_cmd_dict['AMP_CMD']%power
		#can rise an error....
		answer=self.sendCmd(cmd)
		self.last_answer= sanswer
		if answer!=self.m_cmd_dict['AMP_ANS_OK']:
			nak,err=answer.split()
			message="cannot set the power, the error code is %s" % err
			exc=CommandLineError(message);
			raise exc
		else:
			#force next readout
			self.powerTime=0

	def getPower(self):
		cmd=self.m_cmd_dict['AMP_QUERY']
		now=time.time()		
		if now<self.powerTime:
			return self.power		
		#can rise an error....
		answer=self.sendCmd(cmd)
		self.last_answer= sanswer
		if self.m_cmd_dict['AMP_QUERY_NOK'] in answer:
			nak,err=answer.split()
			message="cannot read power, the error code is %s" % err
			exc=CommandLineError(message);
			raise exc
		else:
			self.power=float(answer)
			#sets the time of next readout within 30 seconds
			self.powerTime=time.time()+30
			return self.power

	def setFrequency(self,freq):
		cmd=self.m_cmd_dict['FREQ_CMD']%freq
		#can rise an error....
		answer=self.sendCmd(cmd)
		self.last_answer= sanswer
		if answer!=self.m_cmd_dict['FREQ_ANS_OK']:
			nak,err=answer.split()
			message="cannot set frequency, the error code is %s" % err
			exc=CommandLineError(message);
			raise exc
		else:
			#force next readout
			self.freqTime=0
		
	def getFrequency(self):
		cmd=self.m_cmd_dict['FREQ_QUERY']
		now=time.time()
		if now<self.freqTime:
			return self.freq
		#can rise an error....
		answer=self.sendCmd(cmd)
		self.last_answer= sanswer
		if self.m_cmd_dict['FREQ_QUERY_NOK'] in answer:
			nak,err=answer.split()
			message="cannot read frequency, the error code is %s" % err
			exc=CommandLineError(message);
			raise exc
		else:
			self.freq=float(answer)
			#sets the time of next readout within 30 seconds
			self.freqTime=time.time()+30
			return self.freq
       
	def rfOn(self):
		cmd=self.m_cmd_dict['RF_ON_CMD']
		#can rise an error....
		answer=self.sendCmd(cmd)
		print answer
		if answer!= self.m_cmd_dict['RF_ON_ANS_OK']:
			nak,err=answer.split()
			message="an error occurred, the code is %s" % err
			exc=CommandLineError(message);
			raise exc
  
	def rfOff(self):
		cmd= self.m_cmd_dict['RF_OFF_CMD']
		#can rise an error....
		answer=self.sendCmd(cmd)
		if answer!= self.m_cmd_dict['RF_OFF_ANS_OK']:
			nak,err=answer.split()
			message="an error occurred, the code is %s " % err
			exc=CommandLineError(message);
			raise exc
   
	def sendCmd(self,msg):
		ans=""
		if self.check():
			try:
				self.sock.sendall(msg)
			except socket.error,msg:
				slef.close()
				exc=CommandLineError(msg);
				raise exc
			try:
				ans=self.sock.recv(128)
				return ans
			except socket.error,msg:
				self.close()
				exc=CommandLineError(msg);
				raise exc		
		else:
			exc=CommandLineError("connection is not available");
			raise exc
	
	def create(self):
		try:
			if self.sock==None:
				self.sock=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		except socket.error,msg:
			exc=CommandLineError(msg);
			self.sock=None
			raise exc	
	
	def close(self):
		if self.connected:
			if self.sock!=None:
				self.sock.close()
		self.connected=False
		self.sock=None
		
	def connect(self):
		try:
			if self.connected==False:
				self.sock.connect((self.ip,self.port))
				self.connected=True
		except socket.error, msg:
			exc=CommandLineError(msg);
			raise exc		

	def check(self):
		try:
			self.create()
			self.connect()
			return True
		except Exception:
			return False
       
       
      
     