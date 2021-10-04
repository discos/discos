#!/usr/bin/env python

# This is a python script that can be used to configure the IF distributor. 
# The script is adapted from DMED. 
# PARAMETERS:
#    ifdist=input,pol,att

#who                                   when           what
#fabio vitello (fabio.vitello@inaf.it)  28/06/2021     Creation

from Acspy.Clients.SimpleClient import PySimpleClient
import ACSLog
import maciErrType
import cdbErrType
import maciErrTypeImpl
import ManagementErrorsImpl
import ComponentErrorsImpl
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
	root=ET.fromstring(xmlstr)
	return root.items()[0][1],root.items()[1][1]
	
def main(argv):
	#check we are at the Noto
	station=os.environ['STATION']
	dataPath="alma/DataBlock/IFDist"
	xmlstring=""    
	data=[]
	found=False
	if station!="Noto":
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
		add_user_message(newEx,"Unable to load IFDist configuration")
		userLogger.logException(newEx)
		simpleClient.disconnect()
		sys.exit(1)
	
	try:
		ip,port=cdb(xmlstring)
	except:
		newEx = ComponentErrorsImpl.CDBFieldFormatExImpl()
		add_user_message(newEx,"Unable to parse IFDist configuration")
		userLogger.logException(newEx)
		simpleClient.disconnect()
		sys.exit(1)
	
	commandstr=[]

	if int(argv[0]) not in range (1,3):
		userLogger.logError("Enter input channel number (allowed 1 or 2), your input %s" %(argv[0]))
		simpleClient.disconnect()
		sys.exit(1)
	
	selectedInput='input'+argv[0]

	if int(argv[1]) not in range (-1,7):
		userLogger.logError("Enter an allowed value for pol (-1 to not change the pol value or 0-6 to set pol), your input %s" %(argv[0]))
		simpleClient.disconnect()
		sys.exit(1)
	
	selectedPol=argv[1]
	
	if float(argv[2]) not in range (-1,64):
		userLogger.logError("Enter an allowed value for att (-1 to get att value or 0-63 to set att), your input %s" %(argv[0]))
		simpleClient.disconnect()
		sys.exit(1)
	
	selectedAtt=argv[2]

	#if selectedPol and selectedAtt are -1 print information on current pol and att
	if int(selectedPol) == -1 and float(selectedAtt)==-1:
		commandstr=[selectedInput,"att"+argv[0]]
	#if selectedPol is -1 and selectedAtt is not -1 change att for selected input and current pol
	elif int(selectedPol) == -1 and float(selectedAtt) !=-1:
		commandstr=["att"+argv[0]+","+selectedAtt]
	#if selectedPol is not -1 and selectedAtt is -1 change pol for selected input
	elif int(selectedPol) != -1 and float(selectedAtt) ==-1:
		commandstr=[selectedInput+","+selectedPol]
	#if selectedPol is not -1 and selectedAtt is not -1 change pol and att for selected input
	elif int(selectedPol) != -1 and float(selectedAtt) !=-1:
		commandstr=[selectedInput+","+selectedPol, "att"+argv[0]+","+selectedAtt]

	for i in range(0,len(commandstr)):
		userLogger.logNotice("IFDist setup according to %s command"%(commandstr[i]))

		answer=send_command(ip,port, commandstr[i])
		if answer=="Fail":
				newEx = ComponentErrorsImpl.SocketErrorExImpl()
				add_user_message(newEx,"Unable to communicate to IFDist")
				userLogger.logError(newEx)
				simpleClient.disconnect()
				sys.exit(1)

		elif answer[0]=="NAK":				 
				newEx = ComponentErrorsImpl.NakExImpl()
				add_user_message(newEx,"IFDist command error")
				userLogger.logError(newEx)
				simpleClient.disconnect()
				sys.exit(1)
		else:
				userLogger.logNotice( "Answer: %s"%(answer))

if __name__=="__main__":
   main(sys.argv[1:])  