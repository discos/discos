import socket
import time
import decimal

QUERYERROR="SYST:ERR? \n"

FREQCMD="FREQ "
QUERYFREQ="FREQ?;"+QUERYERROR
QUERYPOWER="POW?\n"
RFONCMD="OUTP:STAT ON"
RFOFFCMD="OUTP:STAT OFF"
QUERYRF="OUTP:STAT?"
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
       except socket.error , msg:
          print msg
          

   def __del__(self):
       pass
   
   def configure(self,ip,port):
      '''
      Connect to the HW
      Clear query error
      
      '''
        
   
      try:
         self.ip=ip
         self.port=port
         
         self.sock.connect((ip,port))
         msg_ok ='OK' 
         self.sock.sendall('*CLS\n;SYST:ERR?\n++read\n')
         msg = self.sock.recv(1024)

         return msg_ok
      except socket.error , msg:
         print msg
         print "connect error: " ,msg
         return msg
   
   def init(self,reply):
     
     pass
   
   def setPower(self,power):
       cmd= "POW " + str(power) + "DBM;SYST:ERR?\n"
 
       try:
           msg=self.query(cmd)   

           return  msg
           
       except socket.error , msg:
          print "connect error: " ,msg
          return msg
   
   def getPower(self):
       QUERYPOWER="POW?;SYST:ERR?\n"
       cmd=QUERYPOWER
       try:
          msg=self.query(cmd)
          commands=msg.split(';')
          
          if len(commands)>1:
             val=int(decimal.Decimal(commands[0]))# unit is MHZ,
             err_msg=commands[1]
          else:
             val=-1
             err_msg='Communication Error with synth'
           
          return err_msg,val
   
       except socket.error , msg:
          print "connect error: " ,msg
          return msg,-1
       except CommandLineError,msg:
          raise
       except ValueError,msg:
          raise CommandLineError(msg)   
   
     
   def setFrequency(self,freq):

       cmd=  'FREQ '+str(freq)+ 'MHZ;SYST:ERR?'

 
       try:
           msg=self.query(cmd)
           return msg
           
       except socket.error , msg:
          print "connect error: " ,msg
          return msg
   
   def getFrequency(self):
         
        cmd= 'FREQ?;SYST:ERR?'
        try:
           
          msg=self.query(cmd)
          commands=msg.split(';')
          if len(commands)>1:
              val=int(decimal.Decimal(commands[0]))/1e6 # unit is MHZ,
              err_msg=commands[1]
          else:
              val=-1
              err_msg='Communication Error with synth'
          print "Error Messager from Synt:",msg
          #if err_msg != '0,\"No error\"\n': 
                #print "exception",err_msg
                #raise CommandLineError(err_msg)   
          return err_msg,val
   
        except socket.error , msg:
          print "connect error: " ,msg
          return msg,-1
          
        except CommandLineError,msg:
          raise
        except ValueError,msg:
          raise CommandLineError(msg)   
   
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
      
   def close(self):
     
       self.sock.close()

   def query(self,cmd):
       try:
           self.sock.sendall(cmd+'\n++read\n') 
           msg = self.sock.recv(1024) 
           if len(msg)==0:
             
                 self.sock.close()
                 
                 self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

                 self.configure(self.ip,self.port)
                 
           return msg
       
       except socket.error , msg:
          print "connect error: " ,msg
          print 'len msg received',len(msg)
          
          self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
          self.configure(self.ip,self.port)
  
          
          return msg
   
       
       
      
     