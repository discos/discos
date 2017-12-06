#!/usr/bin/env python

# This is a python test program that send a log message to the ACS logging service.
#who                                   when           what
#Andrea Orlati(a.orlati@ira.inaf.it)   01/12/2017     Creation

import getopt, sys
from Acspy.Clients.SimpleClient import PySimpleClient
from IRAPy import logger

def usage():
	print "emitLogMessage [-h|--help] [-m|--message=] [-e] [-n]"
	print
	print "Emits two logs, one classic ACS log and one custom"
	print
	print "[-h|--help]     displays this help"
	print "[-t|--text=]    allows to give the test message of the log"
	print "[-e]				  log an error message, if not provided a notice is logged"
    
def main():
	try:
		opts, args = getopt.getopt(sys.argv[1:],"hent:",["help","text="])
	except getopt.GetoptError, err:
		print str(err)
		usage()
		sys.exit(1)
                    
	message=""
	error=False
	for o, a in opts:
		if o in ("-h", "--help"):
			usage()
			sys.exit(1)
		elif o in ("-t", "--text"):
			message=a
		elif o in ("-e"):
			error=True
			 
	if message=="":
		print "text message is mandatory"
		print ""
		usage()
		sys.exit(1)

	simpleClient=PySimpleClient()
	
	print "emitting custom log...."
	
	if error:
		logger.logError(message)
	else:
		logger.logNotice(message)
	
	print "emitting ACS log...."
	
	if error:
		simpleClient.getLogger().logError(message)
	else:
		simpleClient.getLogger().logNotice(message)
		
	
	print "all done...."
	
	simpleClient.disconnect()
    
if __name__=="__main__":
   main()
