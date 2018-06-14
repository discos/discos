#!/usr/bin/env python

# This is a python test program that logs an exception message to the ACS logging service.
#who                               when           what
#Andrea Orlati(andrea.orlati@inaf.it)   23/06/2018     Creation

import getopt, sys
from Acspy.Clients.SimpleClient import PySimpleClient
from IRAPy import logger,userLogger,_add_user_message
from AntennaErrorsImpl import ConnectionExImpl
from AntennaErrorsImpl import AntennaBusyExImpl


def usage():
	print "sendException [-h|--help] [-u] [-s] [-t|--text==]"
	print
	print "send exceptions to the logging system"
	print
	print "[-h|--help]     displays this help"
	print "[-u]            the log will be visible to the user otherwise it will be send to acs log"
	print "[-s]			   send a stacked exception otherwise a simple one"
	print "[-t|--text==]   additional text to be appended, this will be show from the operator input"
    
def main():
	try:
		opts, args = getopt.getopt(sys.argv[1:],"hust:",["help","text="])
	except getopt.GetoptError, err:
		print str(err)
		usage()
		sys.exit(1)
                    
	user=False
	stacked=False
	message=""
	for o, a in opts:
		if o in ("-h", "--help"):
			usage()
			sys.exit(1)
		elif o in ("-u"):
			user=True
		elif o in ("-s"):
			stacked=True
		elif o in ("-t","--text"):
			message=a
			 
	simpleClient=PySimpleClient()
	
	ex1=None	
	
	if stacked==True:
		ex=ConnectionExImpl()
		ex1=AntennaBusyExImpl(exception=ex)
	else:
		ex1=AntennaBusyExImpl()
		
	if message!="":
		_add_user_message(ex1,message)
	
	if user==True:
		userLogger.logException(ex1)
	else:
		logger.logException(ex1)
	
	print "all done...."
	
	simpleClient.disconnect()
    
if __name__=="__main__":
   main()
