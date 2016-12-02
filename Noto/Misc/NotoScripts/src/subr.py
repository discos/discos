#!/usr/bin/env python

# This is a python test program that refresh the Noto minor servo system
#who                                   when           what
#Andrea Orlati(a.orlati@ira.inaf.it)   02/12/2016     Creation

import getopt, sys
import Acspy.Common.Err
import maciErrType
import maciErrTypeImpl
import ClientErrorsImpl
import ACSLog
from Acspy.Clients.SimpleClient import PySimpleClient
from Acspy.Common.TimeHelper import getTimeStamp
from Acspy.Common.EpochHelper import EpochHelper
from Acspy.Common.DurationHelper import DurationHelper
import string
import math
import time
import socket
import struct
from math import *


def usage():
	print "subr [-h|--help] [-c|--code=]"
	print ""
	print "[-h|--help]      displays this help"
	print "[-c|--code=]     select the setup code"

def sendData(socket,msg):
	length=len(msg)	
	socket.sendall(struct.pack('=i',length))
	socket.sendall(msg)

def main():
    
	try:
		opts, args = getopt.getopt(sys.argv[1:],"hc:",["help","code="])
	except getopt.GetoptError, err:
		print str(err)
		usage()
		sys.exit(1)
        
	code=""
	polX=[0.0,0.0,0.0]
	polY=[0.0,0.0,0.0]
	polZ1=[0.0,0.0,0.0]
	polZ2=[0.0,0.0,0.0]
	polZ3=[0.0,0.0,0.0]    

	for o, a in opts:
		if o in ("-h", "--help"):
			usage()
			sys.exit()
		elif o in ("-c", "--code"):
			code = a

	print "The code is %s" % code

	if code=="QQC":
		polX[0]=0.0
		polX[1]=0.0
		polX[2]=0.89

		polY[0]=8.3689e-4
		polY[1]=0.152495
		polY[2]=20.91

		polZ1[0]=0.00168640  
		polZ1[1]=-0.271430
		polZ1[2]=67.55

		polZ2[0]=0.00168640  
		polZ2[1]=-0.271430
		polZ2[2]=84.37

		polZ3[0]=0.00168640  
		polZ3[1]=-0.271430
		polZ3[2]=-57.40

	else:
		print "Unknown code"
		sys.exit(1);

	#get the link to the SRTmountcomponent
	simpleClient = PySimpleClient()
	compName=""
	compType = "IDL:alma/Antenna/AntennaBoss:1.0"
	try:
		component=simpleClient.getDefaultComponent(compType)
		compName=component._get_name()
	except Exception , ex:
		newEx = ClientErrorsImpl.CouldntAccessComponentExImpl( exception=ex, create=1 )
		newEx.setComponentName(compType)
		newEx.log(simpleClient.getLogger(),ACSLog.ACS_LOG_ERROR)
		sys.exit(1)  
		
	print "Antenna boss retrieved"

	client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	client_socket.connect(('192.167.187.92', 5003))

	print "socket connected"
	time.sleep(2)
	try:
		while 1:
			ctime=getTimeStamp().value
			try:
				az,el=component.getRawCoordinates(ctime)
			except Exception , ex:
				print "Errore nella lettura della posizione"
			delev=degrees(el)
			print el,delev
			posX=polX[0]*delev*delev+polX[1]*delev+polX[2]
			posY=polY[0]*delev*delev+polY[1]*delev+polY[2]
			posZ1=polZ1[0]*delev*delev+polZ1[1]*delev+polZ1[2]
			posZ2=polZ2[0]*delev*delev+polZ2[1]*delev+polZ2[2]
			posZ3=polZ3[0]*delev*delev+polZ3[1]*delev+polZ3[2]
			print "comando %6.2lf %6.2lf %6.2lf %6.2lf %6.2lf" % (posX,posY,posZ1,posZ2,posZ3)	
			buffer="0,1,7,%6.2lf,%6.2lf,%6.2lf,%6.2lf,%6.2lf" % (posX,posY,posZ1,posZ2,posZ3)
			#print buffer
			sendData(client_socket,buffer)
			time.sleep(1)
			data=client_socket.recv(128)
			print "risposta ", data
			time.sleep(10)
	finally:
		if not (compName==""):
        		simpleClient.releaseComponent(compName)     
    		simpleClient.disconnect()


if __name__=="__main__":
   main()
    



