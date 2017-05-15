#!/usr/bin/env python

# This is a python program that can be used insert a RAL command inot the control system 
#who                                   when           what
#andrea orlati(a.orlati@ira.inaf.it)   06/03/2017     Creation

from Acspy.Clients.SimpleClient import PySimpleClient
import ACSLog
import maciErrType
import maciErrTypeImpl
import ClientErrorsImpl
import sys
from Acspy.Util.ACSCorba import getManager

def main():

	if	getManager():
		simpleClient = PySimpleClient()
	else:
		print "Control software is off line"
		sys.exit(1)

	command=""
	
	try:
		command=sys.argv[1]
	except Exception, ex:
		newEx = ClientErrorsImpl.InvalidArgumentExImpl( exception=ex, create=1 )
		newEx.log(simpleClient.getLogger(),ACSLog.ACS_LOG_ERROR)
		sys.exit(1)

	try:
		sched=simpleClient.getDefaultComponent("IDL:alma/Management/Scheduler:1.0")
	except Exception, ex:
		newEx = ClientErrorsImpl.CouldntAccessComponentExImpl(exception=ex,create=1)
		newEx.log(simpleClient.getLogger(),ACSLog.ACS_LOG_ERROR)
		sys.exit(1)
    
	try:
		answer=sched.command(command)
	except Exception, ex:
		newEx = ClientErrorsImpl.CORBAProblemExImpl(exception=ex,create=1)
		newEx.log(simpleClient.getLogger(),ACSLog.ACS_LOG_ERROR)
		sys.exit(1)

	print answer[1]

	if sched!=None:
		try:
			simpleClient.releaseComponent(sched.getName())
		except Exception,ex:
			pass
		finally:
			sched=None            
	
	if not answer[0]:
		sys.exit(1)
	else:
		sys.exit(0)
    
if __name__=="__main__":
   main()    
