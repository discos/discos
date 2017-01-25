from ACSImpl.DevIO import DevIO

class stringDevIO(DevIO):
	def __init__(self, device, value=""):
		self.device = device
		DevIO.__init__(self, value)

	def read(self):
   	#value = self.device.getStatus()
      #return int(status, 2)
		return "empty"
