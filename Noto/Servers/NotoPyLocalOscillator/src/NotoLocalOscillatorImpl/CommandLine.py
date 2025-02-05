from __future__ import print_function
import socket
import time
import decimal
from NotoLocalOscillatorImpl import CommandLineError
import ReceiversErrorsImpl

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
		return self.create()   
   
	def configure(self,ip,port):
		'''
		Connect to the HW
		Clear query error
		'''  
		self.ip=ip
		self.port=port
		#raises an error.....
		return self.connect() 
			
	def create(self):
		try:
			if self.sock==None:
				self.sock=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
				print ("socket creato")
		except socket.error as msg:
			self.sock=None
			return False
		return True
			
	def close(self):
		if self.connected:
			if self.sock!=None:
				self.sock.close()
		self.connected=False
		self.sock=None   

	def connect(self):
		try:
			if self.connected==False:
				#print ("PRovo la connessione")
				self.sock.connect((self.ip,self.port))
				#print ("Conessione ok")
				self.connected=True
				cmd="*CLS\n;SYST:ERR?\n++read\n"
				#print ("mando stringa di inizializzazione")
				self.sock.sendall(cmd.encode())
				#print ("mandata")
				msg=self.sock.recv(1024)
				#print ("ricevuta risposta :")
				#print (msg)
		except socket.error as msg:
			self.close()
			return False
		return True
			   
	def check(self):
		if (self.create() and self.connect()):
			return True
		else:
			return False
			
	def setPower(self,power):
		cmd="POW:LEV "+str(power)+"\n"
		#can raise an error......
		res,msg=self.command(cmd)   
		return res,msg

	def getPower(self):
		cmd="POW:LEV?\n"
		now=time.time()
		if now-30<self.powerTime:
			return True,"Read from internal memory",self.power
		#can raise an error.....
		res,msg=self.query(cmd)
		if res:
			self.power=float(msg)
			self.powerTime=time.time()
			return True,"Read from device",self.power
#			commands=msg.split(';')
#			if len(commands)>1:
#				self.power=int(decimal.Decimal(commands[0]))
#				self.powerTime=time.time()
#				err_msg=commands[1]
#				return True,err_msg,self.power
#			else:
#				self.power=-1.0
#				err_msg='Communication Error with synth'  
#				return False,err_msg,self.power
		else:
			self.freq=-1.0
			err_msg=msg
			return False,err_msg,self.power
   
	def setFrequency(self,freq):
		cmd='FREQ '+str(freq)+ 'MHZ\n'
		#can raise an error......
		res,msg=self.command(cmd)
		return res,msg   
   
	def getFrequency(self):       
		cmd='FREQ?'
		now=time.time()
		if now-30<self.freqTime:
			return True,"Read from internal memory",self.freq
		#can raise an error.....
		res,msg=self.query(cmd)
		if res:
			self.freq=float(msg)/1e6
			self.powerTime=time.time()
			return True,"Read from device",self.freq
#			commands=msg.split(';')
#			if len(commands)>1:
#				self.freq=int(decimal.Decimal(commands[0]))/1e6
#				self.freqTime=time.time()
#				err_msg=commands[1]
#				return True,err_msg,self.freq
#			else:	
#				self.freq=-1.0
#				err_msg='Communication Error with synth'
#				return False,err_msg,self.freq
		else:
			self.freq=-1.0
			err_msg=msg
			return False,err_msg,self.freq
			
   
	def readStatus(self):
		''' 
		Query the error code of the synt.      
		'''       
		try:
           
			msg=self.query(QUERYERROR)
			print("query err",msg)
			if msg != '0,\"No error\"\n': 
					print("exception",msg)
					raise CommandLineError(msg)   
			return msg
		except socket.error as msg:
			print("connect error: " ,msg)
			return msg
          
	def rfOn(self):
		pass
   
	def rfOff(self):
		pass
	
	def command(self,cmd):
		msg=""
		if self.check():			
			try:
				self.sock.sendall(cmd.encode())
			except socket.error as msg:
				self.close()
				return False,msg
			return True,msg				        
		else:
			return False,"connection is not available"

	def query(self,cmd):
		msg=""
		if self.check():			
			try:
				buff=cmd+'\n++read\n'
				self.sock.sendall(cmd.encode())
			except socket.error as msg:
				self.close()
				return False,msg
			try:
				msg=self.sock.recv(1024)
			except socket.error as msg:
				self.close()
				return False,msg
			if	len(msg)==0:
				self.close()
				return False,"disconnection from remote side"
			return True,msg				        
		else:
			return False,"connection is not available"
       
       
      
     
