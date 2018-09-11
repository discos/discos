"""
This module provides support for python code to be used in components and client for 
interacting with the user and the user console (operator input) via the Simple Parser
"""
import sys

def exceptionToUser(ex):
	err=ex.getErrorTrace()
	output=''
	temp='(type:%ld code:%ld): %s' % (err.errorType,err.errorCode,getMessage(err))
	output=temp
	while err.previousError!=[]:
		err=err.previousError[0]
		temp='(type:%ld code:%ld): %s' % (err.errorType,err.errorCode,getMessage(err))
		output=output+'\n'+temp
	return output
	
def getMessage(tr):
	l=len(tr.data)
	descr=''
	count=0
	descr=tr.shortDescription
	while (count<l):
		if tr.data[count].name=="Message-to-User":
			descr=tr.data[count].value
		count=count+1
	return descr
"""
This permits to add an additional message to the error to be shown to the user. 
"""		
def add_user_message(ex,message,directLink=True):
	ex.addData("Message-to-User",message)
	#print it into the standard error to pass the message to the calling parser object
	#the string error means an error has occurred
	if directLink:
		out='error '+exceptionToUser(ex)
		sys.stderr.write(out)
	
"""
This function allows to send a message to the user console. It works only on directly isseud by the parser 
"""
def send_message(message):
	sys.stderr.write(message)
