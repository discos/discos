import socket
import time
from MedicinaLocalOscillatorImpl import CommandLineError


FREQ_CMD="set LOFREQ %f\n"
FREQ_ANS_OK="ACK\n"
AMP_CMD="set LOAMP %f\n"
AMP_ANS_OK="ACK\n"
RF_ON_CMD="set LORF ON\n"
RF_ON_ANS_OK="ACK\n"
RF_OFF_CMD="set LORF OFF\n"
RF_OFF_ANS_OK="ACK\n"

FREQ_QUERY="get LOFREQ\n"
FREQ_QUERY_NOK="NAK"
AMP_QUERY="get LOAMP\n"
AMP_QUERY_NOK="NAK"

class CommandLine:
	def __init__(self):
		self.sock=None
		self.connected=False;
		self.ip=""
		self.port=0
		self.power=0.0
		self.freq=0.0
		self.powerTime=0
		self.freqTime=0
		
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
		cmd=AMP_CMD%power
		#can rise an error....
		answer=self.sendCmd(cmd)
		if answer!=AMP_ANS_OK:
			nak,err=answer.split()
			message="cannot set the power, the error code is %d"%err
			exc=CommandLineError(message);
			raise exc
		else:
			#force next readout
			self.powerTime=0

	def getPower(self):
		cmd=AMP_QUERY
		now=time.time()
		if now<self.powerTime:
			return self.power		
		#can rise an error....
		answer=self.sendCmd(cmd)
		if AMP_QUERY_NOK in answer:
			nak,err=answer.split()
			message="cannot read power, the error code is %d"%err
			exc=CommandLineError(message);
			raise exc
		else:
			self.power=float(answer)
			#sets the time of next readout within 30 seconds
			self.powerTime=time.time()+30
			return self.power

	def setFrequency(self,freq):
		cmd=FREQ_CMD%freq
		#can rise an error....
		answer=self.sendCmd(cmd)
		if answer!=FREQ_ANS_OK:
			nak,err=answer.split()
			message="cannot set frequency, the error code is %d"%err
			exc=CommandLineError(message);
			raise exc
		else:
			#force next readout
			self.freqTime=0
		
	def getFrequency(self):
		cmd=FREQ_QUERY
		now=time.time()
		if now<self.freqTime:
			return self.freq
		#can rise an error....
		answer=self.sendCmd(cmd)
		if FREQ_QUERY_NOK in answer:
			nak,err=answer.split()
			message="cannot read frequency, the error code is %d"%err
			exc=CommandLineError(message);
			raise exc
		else:
			self.freq=float(answer)
			#sets the time of next readout within 30 seconds
			self.freqTime=time.time()+30
			return self.freq
       
	def rfOn(self):
		cmd=RF_ON_CMD
		#can rise an error....
		answer=self.sendCmd(cmd)
		print answer
		if answer!=RF_ON_ANS_OK:
			nak,err=answer.split()
			message="an error occurred, the code is %d"%err
			exc=CommandLineError(message);
			raise exc
  
	def rfOff(self):
		cmd=RF_OFF_CMD
		#can rise an error....
		answer=self.sendCmd(cmd)
		if answer!=RF_OFF_ANS_OK:
			nak,err=answer.split()
			message="an error occurred, the code is %d"%err
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
       
       
      
     