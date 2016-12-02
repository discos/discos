import unittest2
import socket

from LocalOscillatorImpl import CommandLine
#ip,port  ='192.168.200.143',5025 #simulator
ip, port = "192.167.187.200", 1234 #real hw

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
      #def test_1_sendFrequency(self):
         #msg=self.cl.setFrequency(4600)
         #self.assertEqual('+0,\"No error\"\n',msg)
      #def test_1_sendPower(self):
         #msg=self.cl.setPower(16)
         #self.assertEqual('+0,\"No error\"\n',msg)
      def test_2_getPower(self):
         msg=self.cl.setPower(16)
         msg,val=self.cl.getPower()
         msg=self.cl.setFrequency(4600)
         msg,val=self.cl.getFrequency()
         self.assertEqual(4600,val)
      
      def test_3_getfrequency(self):
         val=0.
         msg,val=self.cl.getFrequency()
         self.assertEqual(4600,val)
      
           
            
         
if __name__ == '__main__':
    unittest2.main()
    