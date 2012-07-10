from Acspy.Clients.SimpleClient import PySimpleClient # Import the acspy.PySimpleClient class
import ACS, ACS__POA                                  # Import the Python CORBA stubs for BACI
from   omniORB.CORBA import TRUE, FALSE



simpleClient = PySimpleClient()

components = simpleClient.availableComponents()

simpleClient.getLogger().logInfo("COBs available are: ")
for cob in components:
    simpleClient.getLogger().logInfo(cob.name + " of type " + cob.type)

# Do something on a device.
simpleClient.getLogger().logInfo("We can directly manipulate a device once we get it, which is easy.")

try:
	
	meteo = simpleClient.getComponent("METEO/station")
	temperatureProperty = meteo._get_temperature()
	(temp,compl)=temperatureProperty.get_sync()
	simpleClient.getLogger().logInfo("Meteo temperature: " + str(temp))
	
	
	
	
	simpleClient.releaseComponent("METEO/station")
except Exception, e:
	simpleClient.getLogger().logCritical("Sorry, I expected there to be a Mount in the system and there isn't.")
    	simpleClient.getLogger().logDebug("The exception was:" + str(e)) 
	simpleClient.disconnect()
	

