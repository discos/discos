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


def usage():
	print "subr [-h|--help] [-c|--code=]"
	print ""
	print "[-h|--help]      displays this help"
	print "[-c|--code=]     select the setup code"

def main():
    
	try:
		opts, args = getopt.getopt(sys.argv[1:],"hc:",["help","code="])
	except getopt.GetoptError, err:
		print str(err)
		usage()
		sys.exit(1)
        
	code=""
	pol=[0.0,0.0,0.0]
    
	for o, a in opts:
		if o in ("-h", "--help"):
			usage()
			sys.exit()
		elif o in ("-c", "--code"):
			code = a

	print "The code is %s" % code

	if code=="QQC":
		pol[0]=1.0
		pol[1]=0.0
		pol[2]=0.0
	else:
		print "Unknown code"
		sys.exit(1);

	#get the link to the SRTmountcomponent
	simpleClient = PySimpleClient()
	compName=""
	compType = "IDL:alma/Antenna/AntennaBoss:1.0"
	try:
		pass
		#component=simpleClient.getDefaultComponent(compType)
		#compName=component._get_name()
	except Exception , ex:
		newEx = ClientErrorsImpl.CouldntAccessComponentExImpl( exception=ex, create=1 )
		newEx.setComponentName(compType)
		newEx.log(simpleClient.getLogger(),ACSLog.ACS_LOG_ERROR)
		sys.exit(1)  
		
	print "Antenna boss retrieved"

	client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	client_socket.connect(('192.167.187.92', 5003))

	print "socket connected"

	while 1:
		buffer="1\n"
		client_socket.send(buffer)
		time.sleep(1)
		data=client_socket.recv(512)
		print data	


if __name__=="__main__":
   main()
    



