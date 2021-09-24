from ACSImpl.DevIO import DevIO
from math import fabs
from NotoLocalOscillatorImpl import CommandLine
from NotoLocalOscillatorImpl import CommandLineError
from IRAPy import logger
import ReceiversErrorsImpl

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
		txt=""
		self.value=0.0
		res=True
		res,txt,self.value=self.cl.getPower()
		if not res:		
			ex=ReceiversErrorsImpl.SynthetiserErrorExImpl()
			ex.setData('Details',txt)
			raise ex
		return self.value

	def write(self, value):
		pass

class frequencyDevIO(DevIO):
	def __init__(self,cl,value=0):
		DevIO.__init__(self,value)
		self.cl=cl

	def read(self):
		txt=""
		self.value=0.0
		res=True
		res,txt,self.value=self.cl.getFrequency()
		if not res:		
			ex=ReceiversErrorsImpl.SynthetiserErrorExImpl()
			ex.setData('Details',txt)
			raise ex
		return self.value


	def write(self, value):
		pass

