#!/usr/bin/env python

# This is a python test program that send a log message to the ACS logging service.
#who                                   when           what
#Andrea Orlati(a.orlati@ira.inaf.it)   01/12/2017     Creation

import getopt, sys
from Acspy.Clients.SimpleClient import PySimpleClient
from IRAPy import logger,userLogger

def usage():
	print "emitLogMessage [-h|--help] [-t|--text=] [-u] [-e]"
	print
	print "Emits two logs, one classic ACS log and one custom"
	print
	print "[-h|--help]     displays this help"
	print "[-t|--text=]    allows to give the test message of the log"
	print "[-u]            the log will be visible to the user otherwise it will be send to acs log"
	print "[-e]			   log an error message, if not provided a notice is logged"
    
def main():
	try:
		opts, args = getopt.getopt(sys.argv[1:],"heut:",["help","text="])
	except getopt.GetoptError, err:
		print str(err)
		usage()
		sys.exit(1)
                    
	message=""
	error=False
	user=False
	for o, a in opts:
		if o in ("-h", "--help"):
			usage()
			sys.exit(1)
		elif o in ("-t", "--text"):
			message=a
		elif o in ("-e"):
			error=True
		elif o in ("-u"):
			user=True
			 
	if message=="":
		print "text message is mandatory"
		print ""
		usage()
		sys.exit(1)

	simpleClient=PySimpleClient()
	
	if user:
		if error:
			userLogger.logError(message)
		else:
			userLogger.logNotice(message)
	else:
		if error:
			logger.logError(message)
		else:
			logger.logNotice(message)	
	
	simpleClient.disconnect()
    
if __name__=="__main__":
   main()
