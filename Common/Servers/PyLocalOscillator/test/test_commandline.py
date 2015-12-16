import unittest2
import socket

from LocalOscillatorImpl import CommandLine
#ip,port  ='192.168.200.143',5025 #simulator
ip, port = "192.168.200.149", 5025 #real hw

class TestCommandLine(unittest2.TestCase):
  
  
      def setUp(self):
          self.cl=CommandLine.CommandLine()
          self.cl.configure(ip,port)

#      def test_configure(self):
          
#         self.assertRaises(socket.error,self.cl.configure,'localhost',5555)
      #def test_query(self):
          
          #msg=self.cl.query('*IDN?')
          #print 'aa:',msg
          #self.assertEqual('0,\"No error\"\n',msg)
          
      def test_sendCmd(self):
          
          msg=self.cl.sendCmd('FREQ 2188 MHZ\n')
          print 'aa:',msg
          self.assertEqual(True,msg)
         
          
      def test_sendFrequency(self):
         msg,err=self.cl.setFrequency(2000)
         self.assertEqual('0,\"No error\"\n',msg)
         
 
      def test_getFrequency(self):
         val=0.
         msg,val=self.cl.getFrequency()
         self.assertEqual(2000,val)
      def test_sendPower(self):
         msg,err=self.cl.setPower(13)
         self.assertEqual('0,\"No error\"\n',msg)
      def test_getPower(self):
         
         msg,val=self.cl.getPower()
         self.assertEqual(13,val)
            
        
if __name__ == '__main__':
    unittest2.main()
    