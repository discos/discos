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
import os
import thread

offsets=[0.0,0.0,0.0,0.0,0.0]
exitFlag=False

def readPipe(pipeName):
	print "Opening pipe......"
	pipeIn=os.open(pipeName,os.O_RDONLY|os.O_NONBLOCK)
	print "Pipe openened for reading......"
	while not exitFlag:
		try:
			pipeString=os.read(pipeIn,64)
			if len(pipeString)>0:
				print "receieved offsets %s"%pipeString
				info=pipeString.split(',')
				print info
				offsets[0]=float(info[0])
				offsets[1]=float(info[1])
				offsets[2]=float(info[2])
				offsets[3]=float(info[2])
				offsets[4]=float(info[2])
				print offsets
		except Exception, ex:
			print ex
		finally:
			time.sleep(1)
	os.close(pipeIn)
        
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
	pipeName="/tmp/subrPipe"

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
		maxX=100
		minX=-100

		polY[0]=8.3689e-4
		polY[1]=0.152495
		polY[2]=20.91
		maxY=100
		minY=-100

		polZ1[0]=0.00168640  
		polZ1[1]=-0.271430
		polZ1[2]=67.55
		maxZ1=100
		minZ1=-100

		polZ2[0]=0.00168640  
		polZ2[1]=-0.271430
		polZ2[2]=84.37
		maxZ2=100
		minZ2=-100

		polZ3[0]=0.00168640  
		polZ3[1]=-0.271430
		polZ3[2]=-57.40
		maxZ3=100
		minZ3=-100

	elif code=="KKC":
		polX[0]=0.0
		polX[1]=0.0
		polX[2]=-0.5
		maxX=85
		minX=-85

		polY[0]=8.3689e-4
		polY[1]=0.152495
		polY[2]=-11.7
		maxY=85
		minY=-85

		polZ1[0]=0.00168640  
		polZ1[1]=-0.271430
		polZ1[2]=17.3
		maxZ1=85
		minZ1=-85

		polZ2[0]=0.00168640
		polZ2[1]=-0.271430
		polZ2[2]=9.8
		maxZ2=85
		minZ2=-85

		polZ3[0]=0.00168640
		polZ3[1]=-0.271430
		polZ3[2]=12.6
		maxZ3=85
		minZ3=-85

	else:
		print "Unknown code"
		sys.exit(1);

	print "Preparing pipe....."
	if not os.path.exists(pipeName):
		print "Creating pipe"
		os.mkfifo(pipeName,0777)
		print "Created"
	else:
		print "pipe already exists"

	try:
		pipeThread=thread.start_new_thread(readPipe,(pipeName, ) )
	except Exception,ex:
		print ex
		sys.exit(1)

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
			posX=posX+offsets[0]
			posY=posY+offsets[1]
			posZ1=posZ1+offsets[2]
			posZ2=posZ2+offsets[3]
			posZ3=posZ3+offsets[4]
			if posX>maxX:
				posX=maxX
			if posX<minX:
				posX=minX
			if posY>maxY:
				posY=maxY
			if posY<minY:
				posY=minY
			if posZ1>maxZ1:
				posZ1=maxZ1
			if posZ1<minZ1:
				posZ1=minZ1
			if posZ2>maxZ2:
				posZ2=maxZ2
			if posZ2<minZ2:
				posZ2=minZ2
			if posZ3>maxZ3:
				posZ3=maxZ3
			if posZ3<minZ3:
				posZ3=minZ3

			print "comando %6.2lf %6.2lf %6.2lf %6.2lf %6.2lf" % (posX,posY,posZ1,posZ2,posZ3)	
			buffer="0,1,7,%6.2lf,%6.2lf,%6.2lf,%6.2lf,%6.2lf" % (posX,posY,posZ1,posZ2,posZ3)
			#print buffer
			sendData(client_socket,buffer)
			time.sleep(1)
			data=client_socket.recv(128)
			print "risposta ", data
			time.sleep(2)
	finally:
		exitFlag=True
		pipeThread.join(timeout=5)
		if not (compName==""):
			simpleClient.releaseComponent(compName)
			simpleClient.disconnect()
		


if __name__=="__main__":
   main()
    



