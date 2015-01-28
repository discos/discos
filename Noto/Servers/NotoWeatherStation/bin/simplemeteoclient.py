#!/usr/bin/env python

#

from Acspy.Clients.SimpleClient import PySimpleClient
from Acspy.Common.Log import getLogger
import time
simpleClient=PySimpleClient()
logger=getLogger()


try:
	meteo=simpleClient.getComponent("WEATHERSTATION/WeatherStation")
	logger.logInfo('getting component METEO')
	meteoparam=meteo.getData()
#	print meteo.getHumidity()
	print meteoparam
	time.sleep(1)
	print "temperature= ", meteo.getTemperature()
	time.sleep(1)
	print "humidity= ", meteo.getHumidity()
	time.sleep(1)
	print "Pressure= ", meteo.getPressure()

except Exception,e:

	print "The exception was:", e

	
	
