from ACSImpl.DevIO import DevIO
from math import fabs
from MedicinaLocalOscillatorImpl import CommandLine
from MedicinaLocalOscillatorImpl import CommandLineError
from IRAPy import logger

SYNTH_TOLLERANCE = 1.

class GenericDevIO(DevIO):
	def __init__(self, value=0):
		DevIO.__init__(self, value)

	def read(self):
		return self.value

	def write(self, value):
		pass


class amplitudeDevIO(DevIO):
	def __init__(self,cl,value=0):
		DevIO.__init__(self,value)
		self.cl=cl

	def read(self):
		try:
			self.value=self.cl.getPower()
		except CommandLineError as ex:
			msg="cannot get values readout with message %s" % (ex.__str__)
			exc=ComponentErrors.IRALibraryResourceExImpl()
			exc.setData('Description',msg);
			raise exc.getComponentErrorsEx() 
		return self.value

	def write(self, value):
		pass
                                    
class frequencyDevIO(DevIO):
	def __init__(self,cl,value=0):
		DevIO.__init__(self,value)
		self.cl=cl

	def read(self):
		try:
			self.value=self.cl.getFrequency()
		except CommandLineError as ex:
			msg="cannot get values readout with message %s" % (ex.__str__)
			exc=ComponentErrors.IRALibraryResourceExImpl()
			exc.setData('Description',msg);
			raise exc.getComponentErrorsEx() 
		return self.value

	def write(self, value):
		pass

