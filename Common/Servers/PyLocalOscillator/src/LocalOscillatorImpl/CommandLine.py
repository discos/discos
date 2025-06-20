from __future__ import print_function
import socket
import time


QUERYERROR="SYST:ERR? \n"

FREQCMD="FREQ "
QUERYFREQ="FREQ?;"+QUERYERROR
QUERYPOWER="POW?\n"
RFONCMD="OUTP:STAT ON\n"
RFOFFCMD="OUTP:STAT OFF\n"
QUERYRF="OUTP:STAT?\n"
FREQUNIT=" MHZ\n"
POWERUNIT=" dBM\n"

class CommandLineError(Exception):
   def __init__(self, value):

      self.value = value
   def __str__(self):
      return repr(self.value)


class CommandLine:
   
   def __init__(self):
       try: 
          self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
       except socket.error as msg:
          print(msg)
          self.sock=None

   def __del__(self):
       pass
   
   def configure(self,ip,port):
      '''
      Connect to the HW
      Clear query error
      
      '''
      try:
         self.sock.connect((ip,port))
         msg ='OK' 
         self.sendCmd('*CLS\n')
         return msg
      except socket.error as msg:
         print(msg)
         print("connect error: " ,msg)
         return msg
   
   def init(self,reply):
     pass
   
   def setPower(self,power):
       POWERCMD="POWER "
       cmd= POWERCMD + str(power) + POWERUNIT
       try:
           err=self.sendCmd(cmd)
           msg=self.query(QUERYERROR)
           return msg,err
       except socket.error as msg:
          print("connect error: " ,msg)
          return msg,False
          self.sock=None
   
   def getPower(self):
       QUERYPOWER="POWER?;SYST:ERR?\n"
       cmd=QUERYPOWER
       try:
          msg=self.query(cmd)
          commands=msg.split(';')
          val=int(commands[0])# unit is MHZ,
          err_msg=commands[1]
          print("query err",msg)
          if err_msg != '0,\"No error\"\n': 
                print("exception",err_msg)
                raise CommandLineError(err_msg)   
          return err_msg,val
       except socket.error as msg:
          print("connect error: " ,msg)
          return msg,-1
          self.sock=None
       except CommandLineError as msg:
          raise
       except ValueError as msg:
          raise CommandLineError(msg)   
     
   def setFrequency(self,freq):
       cmd= FREQCMD + str(freq) + FREQUNIT
 
       try:
           err=self.sendCmd(cmd)
           msg=self.query(QUERYERROR)
           return msg,err
       except socket.error as msg:
          print("connect error: " ,msg)
          return msg,False
          self.sock=None
   
   def getFrequency(self):
        cmd= QUERYFREQ
        try:
          msg=self.query(cmd)
          commands=msg.split(';')
          val=int(commands[0])/1e6 # unit is MHZ,
          err_msg=commands[1]
          print("query err",msg)
          if err_msg != '0,\"No error\"\n': 
                print("exception",err_msg)
                raise CommandLineError(err_msg)   
          return err_msg,val
   
        except socket.error as msg:
          print("connect error: " ,msg)
          return msg,-1
          self.sock=None
        except CommandLineError as msg:
          raise
        except ValueError as msg:
          raise CommandLineError(msg)   
   
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
       cmd= RFONCMD
       try:
           err=self.sendCmd(cmd)
           msg=self.query(QUERYERROR)
           return msg,err
       except socket.error as msg:
          print("connect error: " ,msg)
          return msg,False
          self.sock=None
   
   def rfOff(self):
       cmd= RFOFFCMD
       try:
           err=self.sendCmd(cmd)
           msg=self.query(QUERYERROR)
           return msg,err
       except socket.error as msg:
          print("connect error: " ,msg)
          return msg,False
          self.sock=None

   def isRfOn(self):
       cmd=QUERYRF
       try:
           return bool(int(self.query(cmd).strip()))
       except socket.error as msg:
          print("connect error: " ,msg)
          self.sock=None
          raise msg
       except:
          print("wrong answer")
          raise
   
   def sendCmd(self,msg):
       try:
           self.sock.sendall(msg.encode())
           return True
       
       except socket.error as msg:
          print("connect error: " ,msg)
          raise msg
          self.sock=None
          return False
   
   def close(self):
       self.sock.close()

   def query(self,cmd):
       try:
           self.sock.sendall(cmd.encode())
           msg = self.sock.recv(1024).decode()
           print('query:received:',msg)
           return msg
       
       except socket.error as msg:
          print("connect error: " ,msg)
          raise
          return msg
