import socket

FREQCMD="FREQ "
QUERYFREQ="FREQ? \n"
QUERYPOWER="POW? \n"
RFONCMD="OUTP:STAT ON"
RFOFFCMD="OUTP:STAT OFF"
QUERYRF="OUTP:STAT?"
FREQUNIT=" MHZ\n"
POWERUNIT=" dBM\n"
QUERYERROR="SYST:ERR?\n"
class CommandLine:
   
    
   def __init__(self):
     
       try: 
          self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
       except socket.error , msg:
          print msg
          self.sock=None
       

   def __del__(self):
       pass
   
   def configure(self,ip,port):
     
   
      try:
         self.sock.connect((ip,port))
         msg ='OK'
         
         return msg
      except socket.error , msg:
         print msg
         print "connect error: " ,msg
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
           
       except socket.error , msg:
          print "connect error: " ,msg
          return msg,False
          self.sock=None
   
   def getPower(self):
       QUERYPOWER="POWER? \n"
       cmd=QUERYPOWER
       try:

           msg=self.query(cmd)
           val=int(msg.strip()) # unit is MHZ, 
           print msg,val
           return msg,val
       
   
       except socket.error , msg:
          print "connect error: " ,msg
          return msg,-1
          self.sock=None
   
     
   def setFrequency(self,freq):

       cmd= FREQCMD + str(freq) + FREQUNIT
 
       try:

           err=self.sendCmd(cmd)
           print "send freq"
           
           msg=self.query(QUERYERROR)
           print "query err"
           
           return msg,err
           
       except socket.error , msg:
          print "connect error: " ,msg
          return msg,False
          self.sock=None
   
   def getFrequency(self):
         
         
       cmd= QUERYFREQ
       try:

           msg=self.query(cmd)
           val=int(msg)/1e6 # unit is MHZ, 
           print msg,val
           return msg,val
       
   
       except socket.error , msg:
          print "connect error: " ,msg
          return msg,-1
          self.sock=None
         
    
   def rfOn(self):
     
       pass
   
   def rfOff(self):
      
      
      pass
   
   def sendCmd(self,msg):
     
       try:
           self.sock.sendall(msg)
           return True
       
       except socket.error , msg:
          print "connect error: " ,msg
          raise msg
          self.sock=None
          return False
   
   def close(self):
     
       self.sock.disconnect()

   def query(self,cmd):
       try:
           self.sock.sendall(cmd) 
           msg = self.sock.recv(1024) 
           print 'query:received:',msg
           return msg
       
       except socket.error , msg:
          print "connect error: " ,msg
          raise
          return msg
   
       
       
      
     