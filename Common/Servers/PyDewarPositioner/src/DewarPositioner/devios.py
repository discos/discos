from ACSImpl.DevIO import DevIO

class StatusDevIO(DevIO):
    def __init__(self, device, value=0):
        self.device = device
        DevIO.__init__(self, value)
    def read(self):
        status = self.device.getStatus()
        return int(status, 2) # Return the decimal value 
