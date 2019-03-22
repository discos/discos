import socket
import time
import decimal
from NotoLocalOscillatorImpl import CommandLineError

QUERYERROR="SYST:ERR? \n"

FREQCMD="FREQ "
QUERYFREQ="FREQ?;"+QUERYERROR
QUERYPOWER="POW?\n"
RFONCMD="OUTP:STAT ON"
RFOFFCMD="OUTP:STAT OFF"
QUERYRF="OUTP:STAT?"
FREQUNIT=" MHZ\n"
POWERUNIT=" dBM\n"

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
          
	def __del__(self):
		self.close()
   
	def initialize(self):
		#raises an error.....
		self.create()   
   
	def configure(self,ip,port):
		'''
		Connect to the HW
		Clear query error
		'''  
		self.ip=ip
		self.port=port
		#raises an error.....
		self.connect() 
			
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
				self.sock.sendall('*CLS\n;SYST:ERR?\n++read\n')
				msg=self.sock.recv(1024)
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
			
	def setPower(self,power):
		cmd="POW "+str(power)+"DBM;SYST:ERR?\n"
		#can raise an error......
		msg=self.query(cmd)   
		return msg

	def getPower(self):
		QUERYPOWER="POW?;SYST:ERR?\n"
		cmd=QUERYPOWER
		now=time.time()
		if now<self.powerTime:
			return "Read from internal memory",self.power
		#can raise an error.....
		msg=self.query(cmd)
		commands=msg.split(';')
		if len(commands)>1:
			self.power=int(decimal.Decimal(commands[0]))# unit is MHZ
			self.powerTime=time.time()+30
			err_msg=commands[1]
		else:
			self.power=-1.0
			err_msg='Communication Error with synth'  
		return err_msg,self.power
   
	def setFrequency(self,freq):
		cmd='FREQ '+str(freq)+ 'MHZ;SYST:ERR?'
		#can raise an error......
		msg=self.query(cmd)
		return msg   
   
	def getFrequency(self):       
		cmd='FREQ?;SYST:ERR?'
		now=time.time()
		if now<self.freqTime:
			return "Read from internal memory",self.freq
		#can raise an error.....
		msg=self.query(cmd)
		commands=msg.split(';')
		if len(commands)>1:
			self.freq=int(decimal.Decimal(commands[0]))/1e6
			self.freqTime=time.time()+30
			err_msg=commands[1]
		else:
			self.freq=-1.0
			err_msg='Communication Error with synth'
		return err_msg,self.freq
   
	def readStatus(self):
		''' 
		Query the error code of the synt.      
		'''       
		try:
           
			msg=self.query(QUERYERROR)
			print "query err",msg
			if msg != '0,\"No error\"\n': 
					print "exception",msg
					raise CommandLineError(msg)   
			return msg
		except socket.error , msg:
			print "connect error: " ,msg
			return msg
          
	def rfOn(self):
		pass
   
	def rfOff(self):
		pass

	def query(self,cmd):
		msg=""
		if self.check():			
			try:
				self.sock.sendall(cmd+'\n++read\n')
			except socket.error,msg:
				self.close()
				exc=CommandLineError(msg);
				raise exc
			try:
				msg=self.sock.recv(1024)
			except socket.error,msg:
				self.close()
				exc=CommandLineError(msg);
				raise exc
			if	len(msg)==0:
				self.sock.close()
				raise CommandLineError("disconnection from remote side"); 					        
			return msg
		else:
			exc=CommandLineError("connection is not available");
			raise exc   
       
       
      
     