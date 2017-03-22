#!/usr/bin/env python

# This is a python client that can be used as a terminal to the system, It issues the command
# action of the connected sub-system boss.
#who                                       when           what
#andrea orlati(a.orlati@ira.inaf.it)       22/08/2008     Creation
#andrea orlati(a.orlati@ira.inaf.it)       25/09/2009   
#andrea orlati(a.orlati@ira.inaf.it)       23/07/2010     If no component name is given it logs into the scheduler
#andrea orlati(a.orlati@ira.inaf.it)       26/07/2010     Added support for command history
#andrea orlati(a.orlati@ira.inaf.it)       24/09/2010     Exceptions coming from the component command are now logged as it should be by a client application
#andrea orlati(a.orlati@ira.inaf.it)       20/07/2011     Logging exceptions now works also in ACS 8.2
#Marco Buttu (mbuttu@oa-cagliari.inaf.it)  28/05/2013     Tab completion from a file of commands, help command
#andrea orlati(a.orlati@ira.inaf.it)       04/09/2013     Implemented support for external termination signal
#Marco Buttu (mbuttu@oa-cagliari.inaf.it)  16/12/2013     New answer format, showing the values in different lines
#Marco Buttu (mbuttu@oa-cagliari.inaf.it)  16/12/2013     Commands immediatly saved in the history file
#Marco Buttu (mbuttu@oa-cagliari.inaf.it)  22/10/2014     help() without argument lists all the commands
#Marco Buttu (mbuttu@oa-cagliari.inaf.it)  28/10/2014     control of the success value returned by the command

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
import signal
from Acspy.Clients.SimpleClient import PySimpleClient
from nuraghe_commands import commands

stopAll=False

def usage():
    print "systerm [-h|--help] [ComponentName]"
    print "ComponentName         The name of a component that implements the CommandInterpreter interface"
    print "                      If not given the Scheduler component is used."
    print "[-h|--help]           displays this help"

def handler(num, stack):
    global stopAll
    stopAll=True

def get_history_items():
    return [readline.get_history_item(i) for i in xrange(1,readline.get_current_history_length()+1)]


class HistoryCompleter(object):

    def __init__(self):
        self.matches=[]
        return

    def complete(self, text, state):
        response=None
        if state==0:
            # history_values=get_history_items()
            if text:
                self.matches=sorted(c for c in commands if c and c.startswith(text))
            else:
                self.matches=[]
        try:
            response=self.matches[state]
        except IndexError:
            response=None
        return response
    
    
def main():
    
    global stopAll
   
    #handler for the external request of termination 
    signal.signal(signal.SIGUSR1, handler)

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
    
    cmdCounter=0
    
    while not stopAll:
        try:
            #sys.stdout.write("<%d> "%cmdCounter)
            cmd=''
            try:
                #cmd=sys.stdin.readline()
                cmd=raw_input("<%d> "%cmdCounter)
            except IOError:
                cmd='exit'
                pass
            cmdCounter=cmdCounter+1
            cmd=cmd.strip()
            if cmd=="exit":
                stopAll=True
            elif cmd == 'help()':
                print '\t' + '\n\t'.join(sorted(commands.keys()))
            elif cmd.startswith('help('):
                start = cmd.find('(') + 1
                end = cmd.find(')')
                arg = cmd[start:end] # help(setupCCB) -> arg=setupCCB
                if arg not in commands:
                    print "`%s` is not a valid command" %(arg if arg else cmd)
                else:
                    print commands['help'](arg)
            elif cmd:
                readline.write_history_file(historyFile)            
                try:
                    success, res = component.command(cmd)
                    idx = res.find('\\')
                    cmd_name, response = res[:idx+1], res[idx+1:].rstrip('\\')
                    if success and response:
                        groups = response.split(',')
                        for group in groups:
                            values = group.split(';')
                            if len(values) > 1:
                                print cmd_name
                                for i, value in enumerate(values):
                                    print '%02d) %s' %(i, value)
                            elif res:
                                print res
                    elif res:
                        print res
                except Exception, ex:
                    newEx = ClientErrorsImpl.CouldntPerformActionExImpl(exception=ex, create=1)
                    newEx.setAction("command()")
                    newEx.setReason(ex.message)
                    newEx.log(simpleClient.getLogger(),ACSLog.ACS_LOG_ERROR) 
        except KeyboardInterrupt:
            stopAll=True
            
    readline.write_history_file(historyFile)            
    simpleClient.releaseComponent(compName)     
    simpleClient.disconnect()
            
if __name__=="__main__":
   main()
