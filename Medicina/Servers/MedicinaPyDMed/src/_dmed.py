#!/usr/bin/env python

# This is a python script that can be used to configure the DMED IF distributor.
# The only available parameter is the attenuation level which can be selected 
# not as a single channel but a whole configuration
#who                                   when           what
#andrea orlati(andrea.orlati@inaf.it)  18/02/2019     Creation



from Acspy.Clients.SimpleClient import PySimpleClient
import ACSLog
import maciErrType
import cdbErrType
import maciErrTypeImpl
import ManagementErrorsImpl
from IRAPy import logger,userLogger
from SimpleParserPy import add_user_message
import xml.etree.ElementTree as ET
import sys,getopt
import os
import socket
import time
from collections import namedtuple

def send_command(ip, port, command):
	s=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	try:
		s.connect((ip,int(port)))
	except socket.error:
		return "Fail"
	s.sendall(command)
	time.sleep(0.1)
	response=s.recv(1024)
	s.close()
	response=response.strip().split('\n')
	return response

def cdb(xmlstr):
	data=[]
	record=()
	confname=""
	boardnumber=0
	attenuatorsNumber=[]
	attenuatorsValue=[]
	root=ET.fromstring(xmlstr)
	configurations=root.findall('{urn:schemas-cosylab-com:DMedConfiguration:1.0}Configuration')
	for conf in configurations:
		confname=conf.items()[0][1]
		boards=conf.findall('{urn:schemas-cosylab-com:DMedConfiguration:1.0}Board')
		for board in boards:
			boardnumber=board.items()[0][1]
			attenuators=board.findall('{urn:schemas-cosylab-com:DMedConfiguration:1.0}Attenuator')
			for att in attenuators:
				attenuatorsNumber.append(att.items()[0][1])
				attenuatorsValue.append(att.items()[1][1])
			record=(confname,boardnumber,attenuatorsNumber,attenuatorsValue)
			data.append(record)
			attenuatorsNumber=[]
			attenuatorsValue=[]
	return root.items()[0][1],root.items()[1][1],data
	
def main(argv):
	#check we are at the Medicina
	station=os.environ['STATION']
	dataPath="alma/DataBlock/DMed"
	xmlstring=""    
	data=[]
	found=False
	if station!="Medicina":
		newEx = ManagementErrorsImpl.UnsupportedOperationExImpl()
		add_user_message(newEx,"The command is not available or it is not supported")
		userLogger.logExcpetion(newEx)
		simpleClient.disconnect()
		sys.exit(1)
    
	simpleClient = PySimpleClient()
	
	try:
		xmlstring=simpleClient.getCDBRecord(dataPath)
	except cdbErrType.CDBRecordDoesNotExistEx as ex:
		newEx = ComponentErrorsImpl.CDBAccessExImpl( exception=ex, create=1 )
		add_user_message(newEx,"Unable to load DMed configuration")
		userLogger.logException(newEx)
		simpleClient.disconnect()
		sys.exit(1)
	
	try:
		ip,port,data=cdb(xmlstring)
	except:
		newEx = ComponentErrorsImpl.CDBFieldFormatExImpl()
		add_user_message(newEx,"Unable to parse DMed configuration")
		userLogger.logException(newEx)
		simpleClient.disconnect()
		sys.exit(1)
	
	for i in range(0,len(data)):
		if argv[0] in data[i]:
			found=True
			for j in range(0,len(data[i][2])):
				commandstr="A %s %s %s\n" % (data[i][1],data[i][2][j],data[i][3][j])
				answer=send_command(ip,port,commandstr)
				if answer=="Fail":
					newEx = ComponentErrorsImpl.SocketErrorExImpl()
					add_user_message(newEx,"Unable to communicate to DMed")
					userLogger.logException(newEx)
					simpleClient.disconnect()
					sys.exit(1)
										
				if answer=="nak":				 
					newEx = ComponentErrorsImpl.NakExImpl()
					add_user_message(newEx,"Dmed configuration error")
					userLogger.logException(newEx)
					simpleClient.disconnect()
					sys.exit(1)
	
	userLogger.logNotice("DMed setup according to %s configuration"%(argv[0]))	
				
	if not found:
		newEx = ComponentErrorsImpl.ValidationErrorExImpl()
		add_user_message(newEx,"The required DMed configuration is not known")
		userLogger.logException(newEx)
		simpleClient.disconnect()
		sys.exit(1)
	
if __name__=="__main__":
   main(sys.argv[1:])  
	