import unittest
from Acspy.Clients.SimpleClient import PySimpleClient
from Acspy.Util.ACSCorba import getManager
import sys
import ComponentErrors


class test_component(unittest.TestCase):
	
	def setUp(self):
		if	getManager():
			self.client = PySimpleClient()
		else:
			print "Control software is off line, test could not be executed"
			assert False    
   
	def test_correctStartup(self):	
		"""try to load the local oscillator component"""	
		try:
			self.component=self.client.getDefaultComponent("IDL:alma/Receivers/LocalOscillator:1.0")
		except Exception as ex:
			self.fail('caught exception in component retrieval')
		self.assertIsNotNone(self.component,'component reference is null')
		try:
			self.client.releaseComponent(self.component._get_name())
		except Exception as ex:
			self.fail('caught exception in component release')		
   
	def test_setAndReadout(self):
		"""sets the values and readout to check if they match to load the local oscillator component"""
		amp=0
		freq=0	
		try:
			self.component=self.client.getDefaultComponent("IDL:alma/Receivers/LocalOscillator:1.0")
		except Exception as ex:
			self.fail('caught exception in component retrieval')
		self.assertIsNotNone(self.component,'component reference is null')
		try:
			self.component.set(12.5,9545.0)
		except ComponentErrors.ComponentErrorsEx as ex:
			self.fail('caught exception while setting the values with message %s'%(ex.message))
		except Exception:
			self.fail('caught exception while setting the values')
		try:
			amp,freq=self.component.get()
		except ComponentErrors.ComponentErrorsEx as ex:
			self.fail('caught exception while getting the values with message %s'%(ex.message))
		except Exception as ex:
			self.fail('caught exception while getting the values')
		try:
			self.client.releaseComponent(self.component._get_name())
		except Exception:
			self.fail('caught exception in component release')	
		self.assertEqual(amp,12.5)
		self.assertEqual(freq,9545.0)
		
	def test_setRfOn(self):
		"""Turn the RF on"""
		try:
			self.component=sched=self.client.getDefaultComponent("IDL:alma/Receivers/LocalOscillator:1.0")
		except Exception as ex:
			self.fail('caught exception in component retrieval')
		self.assertIsNotNone(self.component,'component reference is null')
		try:
			self.component.rfon()
		except ComponentErrors.ComponentErrorsEx as ex:
			self.fail('caught exception while setting the values with message %s'%(ex.message))
		except Exception:
			self.fail('caught exception while setting the values')
		try:
			self.client.releaseComponent(self.component._get_name())
		except Exception:
			self.fail('caught exception in component release')	
   
	def tearDown(self):
		pass


if __name__ == '__main__':
    unittest.main()