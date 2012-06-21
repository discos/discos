#!/usr/bin/env python

# This is a python client that can be used as a terminal to the system, It issues the command
# action of the connected sub-system boss.
#who                                   when           what
#andrea orlati(a.orlati@ira.inaf.it)   22/08/2008     Creation
#andrea orlati(a.orlati@ira.inaf.it)   25/09/2009   
#andrea orlati(a.orlati@ira.inaf.it)   23/07/2010     If no component name is given it logs into the scheduler
#andrea orlati(a.orlati@ira.inaf.it)   26/07/2010     Added support for command history
#andrea orlati(a.orlati@ira.inaf.it)   24/09/2010     Exceptions coming from the component command are now logged as it should be by a client application
#andrea orlati(a.orlati@ira.inaf.it)   20/07/2011     Logging exceptions now works also in ACS 8.2

import getopt, sys
import Acspy.Common.Err
import maciErrType
import maciErrTypeImpl
import ClientErrorsImpl
import ManagementErrors
import ManagementErrorsImpl
import ACSLog
import string
import readline
import os
from Acspy.Clients.SimpleClient import PySimpleClient

def usage():
    print "systerm [-h|--help] [ComponentName]"
    print "ComponentName         The name of a component that implements the CommandInterpreter interface"
    print "                      If not given the Scheduler component is used."
    print "[-h|--help]           displays this help"


def get_history_items():
    return [readline.get_history_item(i) for i in xrange(1,readline.get_current_history_length()+1)]

class HistoryCompleter(object):

    def __init__(self):
        self.matches=[]
        return

    def complete(self, text, state):
        response=None
        if state==0:
            history_values=get_history_items()
            if text:
                self.matches=sorted(h for h in history_values if h and h.startswith(text))
            else:
                self.matches=[]
        try:
            response=self.matches[state]
        except IndexError:
            response=None
        return response

def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:],"h",["help"])
    except getopt.GetoptError, err:
        print str(err)
        usage()
        sys.exit(1)
        
    for o, a in opts:
        if o in ("-h", "--help"):
            usage()
            sys.exit()
    
    simpleClient = PySimpleClient()
    compName=""
    
    if args == []:
        compType = "IDL:alma/Management/Scheduler:1.0"
        try:
            component=simpleClient.getDefaultComponent(compType)
            compName=component._get_name()
        except Exception , ex:
            newEx = ClientErrorsImpl.CouldntAccessComponentExImpl( exception=ex, create=1 )
            newEx.setComponentName(compType)
            newEx.log(simpleClient.getLogger(),ACSLog.ACS_LOG_ERROR)
            sys.exit(1)        
    else:
        compName = args[0]    
        try:
            component=simpleClient.getComponent(compName)
        except Exception , ex:
            newEx = ClientErrorsImpl.CouldntAccessComponentExImpl( exception=ex, create=1 )
            newEx.setComponentName(compName)
            newEx.log(simpleClient.getLogger(),ACSLog.ACS_LOG_ERROR)
            sys.exit(1)
    
    userHome = os.getenv('HOME')
    historyFile = None
    if userHome == None:
        historyFile = '/tmp/.oprin_cmd.hist'
    else:
        historyFile = userHome+'/.oprin_cmd.hist'
    
    if os.path.exists(historyFile):
        readline.read_history_file(historyFile)
        
    readline.set_completer(HistoryCompleter().complete)
    readline.parse_and_bind('tab: complete')
    
    stop=False
    cmdCounter=0
    while not stop:
        cmd=raw_input("<%d> "%cmdCounter)
        cmdCounter=cmdCounter+1
        if cmd=="exit":
            stop=True
        elif cmd:
            try:
                out=component.command(cmd)
                print out
                # this is thrown in case an error occurs during parsing or command execution, we do not want to do nothing since 
                # the error is reported to the user throught the returned string
            except ManagementErrors.CommandLineErrorEx, cmdEx:
                cmdError = ManagementErrorsImpl.CommandLineErrorExImpl(exception= cmdEx, create=0)
                out=cmdError.getErrorMessage()
                cmdError.log(simpleClient.getLogger(),ACSLog.ACS_LOG_ERROR)
                print out
            except Exception, ex:
                newEx = ClientErrorsImpl.CouldntPerformActionExImpl( exception=ex, create=1 )
                newEx.setAction("command()")
                newEx.setReason("comunication error to component server")
                newEx.log(simpleClient.getLogger(),ACSLog.ACS_LOG_ERROR) 
    simpleClient.releaseComponent(compName)     
    simpleClient.disconnect()
    readline.write_history_file(historyFile)            
            
if __name__=="__main__":
   main()