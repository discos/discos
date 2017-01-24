#!/usr/bin/env python

# This is a python program that allow to maximize the focus for Noto subreflector
#who                                   when           what
#Andrea Orlati(a.orlati@ira.inaf.it)   24/01/2017     Creation

import getopt, sys

import Acspy.Common.Err
import maciErrType
import maciErrTypeImpl
import ClientErrorsImpl
import Antenna
import ACSLog
from Acspy.Clients.SimpleClient import PySimpleClient
from Acspy.Common.TimeHelper import getTimeStamp
from Acspy.Common.EpochHelper import EpochHelper
from Acspy.Common.DurationHelper import DurationHelper
import string

import string
import math
import time
import socket
import struct
from math import *
import os
        
def usage():
	print "subrOff [-h|--help] [-a axis] [-y val] [-z val] [-s val] [-p val]"
	print ""
	print "[-h|--help]      displays this help"
	print "[-a axis]        scu axis to be moved, currently supported Z, Y, Default Z."
	print "[-y val]         central offset of Y axis. Default 0.0"		
	print "[-z val]         central offset of Z axis. Default 0.0"
	print "[-s val]         span in millimeters. Default 40.0"
	print "[-p val]         number of samples. Default 13"

def main():
    
	try:
		opts, args = getopt.getopt(sys.argv[1:],"hy:z:c:s:p:",["help"])
	except getopt.GetoptError, err:
		print str(err)
		usage()
		sys.exit(1)
        
	pipeName="/tmp/subrPipe"
	centerY=0.0
	centerZ=0.0
	span=40.0
	points=13
	axis="Z"

	for o, a in opts:
		if o in ("-h", "--help"):
			usage()
			sys.exit()
		elif o in ("-y"):
			centerY=float(a)
		elif o in ("-z"):
			centerZ = float(a)
		elif o in ("-s"):
			span = float(a)
		elif o in ("-p"):
			points = int(a)
		elif o in ("-a"):
			axis=a

	axis.upper()
	if not((axis=="Z") or (axis=="Y")):
		print "currently supported axis are Y and Z"
		sys.exit(1)

	#get the link to the SRTmountcomponent
	simpleClient = PySimpleClient()
	AntennaCompName=""
	antType = "IDL:alma/Antenna/AntennaBoss:1.0"
	try:
		antennaComponent=simpleClient.getDefaultComponent(antType)
		antennaCompName=antennaComponent._get_name()
	except Exception , ex:
		newEx = ClientErrorsImpl.CouldntAccessComponentExImpl( exception=ex, create=1 )
		newEx.setComponentName(antType)
		newEx.log(simpleClient.getLogger(),ACSLog.ACS_LOG_ERROR)
		sys.exit(1)  
	print "Antenna boss retrieved"
	tpCompName=""
	tpInst = "BACKENDS/TotalPower"
	try:
		tpComponent=simpleClient.getComponent(tpInst)
		tpCompName=tpComponent._get_name()
	except Exception , ex:
		newEx = ClientErrorsImpl.CouldntAccessComponentExImpl( exception=ex, create=1 )
		newEx.setComponentName(tpCompName)
		newEx.log(simpleClient.getLogger(),ACSLog.ACS_LOG_ERROR)
		sys.exit(1)  
	print "Total power retrieved"

	print "Preparing pipe....."
	if not os.path.exists(pipeName):
		print "Creating pipe"
		os.mkfifo(pipeName,0777)
		print "Created"
	else:
		print "pipe already exists"

	print "Opening pipe......"
	pipeOut=open(pipeName,'w', 0)
	print "Pipe openened for writing......"

	if axis=="Y":
		bdf=-0.0036
	else:
		bdf=0.0
	
	axisPos=0.0
	startPoint=points/2
	millis=span/float(points)
	try:
		for i in range(-startPoint,startPoint):
			axisPos=center+i*millis
			try:
				if axis=="Y":
					pipeString="0.0,%lf,%lf,"%(X,axisPos,centerZ)
				else:
					pipeString="0.0,%lf,%lf,"%(X,centerY,axisPos)
				pipeOut.write(pipeString)
			except Exception, ex:
				print "Error in sending commant to scu"
				print ex
				sys.exit(1)
			try:
				antennaComponent.setOffsets(math.radians(0.0),math.radians(axisPos*bdf),Antenna.ANT_HORIZONTAL)
			except Excpetion,ex:
				print "Error  while sending offset to the telescope"
				print ex
				sys.exit(1)
			time.sleep(5)
			try:
				samples=tpComponent.getTpi()
			except:
				print "error reading total power measurement"
				print ex
				sys.exit(1)
			print "Lettura %lf,%lf"%(axisPos,samples[1])
			
	finally:
		if not (antennaCompName==""):
			simpleClient.releaseComponent(antennaCompName)
		if not (tpCompName==""):
			simpleClient.releaseComponent(tpCompName)			
		simpleClient.disconnect()
		pipeOut.close()

if __name__=="__main__":
   main()
    



