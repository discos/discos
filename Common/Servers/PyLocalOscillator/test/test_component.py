import time
import unittest2
from ComponentErrors import ComponentErrorsEx, ComponentErrorsEx
from Acspy.Clients.SimpleClient import PySimpleClient
from LocalOscillatorImpl import CommandLine

class TestCommandLine(unittest2.TestCase):


      def setUp(self):
         client = PySimpleClient()
         self.lo = client.getComponent('RECEIVERS/LO_LP')
         

      def test_get(self):
         self.lo.set(13,2000)
         power,freq=self.lo.get()
         self.assertEqual(2000,freq)
         
 



if __name__ == '__main__':
    unittest2.main()
