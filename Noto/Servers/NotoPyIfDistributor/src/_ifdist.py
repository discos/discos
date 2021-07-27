#!/usr/bin/env python

# This is a python script that can be used to configure the IF distributor. 
# The script is adapted from DMED. 
# PARAMETERS:
#  input1,[ingresso] per selezionare cosa si vuole in uscita su A
'''
input1,[ingresso] per selezionare cosa si vuole in uscita su A
input2,[ingresso] per selezionare cosa si vuole in uscita su B

input1 o input2 senza parametri ritorna l'ingresso selezionato come
ACK,input1=ingresso

att1,[valore] setta attenuazione A
att2,[valore] setta attenuazione B

att1 o att2 senza parametri ritorna il valore di attenuazione impostato come
ACK,att1=valore

i valori consentiti di ingresso per A sono:
0 nessun ingresso selezionato
1 ricevitore in vertex Right Pol
2 ricevitore L Right Pol
3 ricevitore X Right Pol
4 ricevitore S Right Pol
5 spare1
6 spare2

i valori consentiti di ingresso per B sono:
0 nessun ingresso selezionato
1 ricevitore in vertex Left Pol
2 ricevitore L Left Pol
3 ricevitore X Left Pol
4 ricevitore S Left Pol
5 ricevitore S Right Pol (per oss. geo)
6 spare1

i valori consentiti per att1 e att2 sono:
da 0 a 63. Ogni step corrisponde a 0.5 dB

Un comando errato ottiene in risposta
NAK
'''
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
	
	
	parameters = ['input1','input2','att1','att2']
	for i in range(0,len(argv)):
		if argv[i] != '-1':
			commandstr = parameters[i]+","+argv[i]
			userLogger.logNotice("IFDist setup according to %s command"%(commandstr))
			answer=send_command(ip,port, commandstr)
			
			if answer=="Fail":
				newEx = ComponentErrorsImpl.SocketErrorExImpl()
				add_user_message(newEx,"Unable to communicate to IFDist")
				userLogger.logException(newEx)
				simpleClient.disconnect()
				sys.exit(1)

			elif answer[0]=="NAK":				 
				newEx = ComponentErrorsImpl.NakExImpl()
				add_user_message(newEx,"IFDist command error")
				userLogger.logException(newEx)
				simpleClient.disconnect()
				sys.exit(1)

			else:
				userLogger.logNotice( "Answer: %s"%(answer))


if __name__=="__main__":
   main(sys.argv[1:])  
	