#!/usr/bin/env python

# " $Id: configureBackend.py,v 1.2 2009-02-11 14:02:57 a.orlati Exp $ "
# This is a python script that allows to configure a backend via command line 
# component
#who                                   when           what
#andrea orlati(a.orlati@ira.inaf.it)   30/01/2009     Creation


import getopt, sys
import Acspy.Common.Err
import maciErrType
import maciErrTypeImpl
import ClientErrorsImpl
import time
import string
import Backends
from Acspy.Clients.SimpleClient import PySimpleClient

def usage():
    print "configureBackend [-h|--help] -c|--channels= val1,val2  [-w|--bandwidth= val] [-s|--samplerate= val]" 
    print "[-a|--attenuation= val] [-i|--integration= val] [-f|--frequency= val] "
    print "[-p|--polarization= val] [-b|--bins= val] BackendInstance"
    print "\n"
    print "It allows to configure a backend compliant to the genericBackend interface"
    print "\n"
    print "[-h|--help]               displays this help"
    print "-c|--channels= val1,val2  It allows to enter the numeric identifier of the backend input"
    print "                          the configuration refers to. The entered value could be simple integer or" 
    print "                          a range of channels. For example 0,13 means identifiers from 0 to 13 included"
    print "                          If not provided the configuration is done for all the inputs"
    print "[-w|--bandwidth= val]     It allows to give the bandwidth for the backend channel (MHz)."
    print "                          If not provided the previous value is retained" 
    print "[-s|--sample=val]         It allows to give the sample rate in MHz for each channel."
    print "                          If not provided the previous one is retained."
    print "[-a|--attenuation= val]   It allows to give the attenuation level for each backend channel (db)."
    print "                          If not provided the previous one is retained."
    print "[-i|--integration= val]   It allows to configure the integration in milliseconds."
    print "                          If not provided the previous one is retained."
    print "[-f|--frequency= val]     It allows to configure the star frequency of backend input channel (MHz)."
    print "                          If not provided the previous one is retained."
    print "[-p|--polarization= val]  If the backend supports this configuration, it allows to configure the"
    print "                          kind of polarization the input can deal with. The given value could be"
    print "                          LCP, RCP or FULLSTOKE."
    print "                          If not provided the previous one is retained."    
    print "[-b|--bins= val]          It allows to configure the number of bins per channel exported by the backend."
    print "                          If not provided the previous one is retained." 
    print "BackendInstance           Instance of the backend to be configured."
   
def error(errMsg):
    print errMsg
    print
    usage()
    sys.exit(-1)
           

def main():
    bw= -1.0
    sampleRate= -1.0
    att= -1.0
    freq= -1.0
    pol = -1
    integr= -1
    backend = None
    sChannel= -1
    fChannel= -1
    bins = -1
    
    try:
        opts, args = getopt.getopt(sys.argv[1:],"hc:w:s:a:i:f:p:b:",
        ["help","channels=","bandwidth=","samplerate=","attenuation=","integration=","frequency=","polarization=","bins="])
    except getopt.GetoptError, err:
        error(str(err))

    for o, a in opts:
        if o in ("-h", "--help"):
            usage()
            sys.exit(0)
        elif o in ("-c", "--channels"):
            ch=a.split(",",2)
            if len(ch)==1:
                try:
                    sChannel = string.atoi(ch[0])
                    fChannel = sChannel + 1
                except:
                    error("Wrong channel value")
            elif len(ch)==2:
                try:
                    sChannel= string.atoi(ch[0])
                    fChannel= string.atoi(ch[1]) + 1
                except:
                    error("Wrong channel value")
            else:
                error("Invalid channel range")
        elif o in ("-w", "--bandwidth"):
            try:
                bw = string.atof(a)
            except:
                error("Wrong bandwidth value")
        elif o in ("-s", "--samplerate"):
            try:
                sampleRate = string.atof(a)
            except:
                error("Wrong samplerate value")
        elif o in ("-a", "--attenuation"):
            try:
                att=string.atof(a)
            except:
                error("Wrong attenuation value")
        elif o in ("-i", "--integration"):
            try:
                integr = string.atoi(a)
            except:
                error("Wrong integration value")
        elif o in ("-f", "--frequency"):
            try:
                freq=string.atof(a)
            except:
                error("Wrong frequency value")
        elif o in ("-p", "--polarization"):
            if a=="LCP":
                pol=Backends.BKND_LCP
            elif a=="RCP":
                pol=Backends.BKND_RCP
            elif a=="FULLSTOKE":
                pol=Backends.BKND_FULL_STOKES
            else:
                error("Wrong frequency value")
        elif o in ("-b", "--bins"):
            try:
                bins=string.atoi(a)
            except:
                error("Wrong bins value")
                        
    if len(args[0])==0:
        error("Backend instance must be provided")
        
    simpleClient = PySimpleClient()
    
    simpleClient.getLogger().logNotice("configureBackend")
       
    try:
        backend=simpleClient.getComponent(args[0])
    except Exception , ex:
        newEx = ClientErrorsImpl.CouldntAccessComponentExImpl( exception=ex, create=1 )
        newEx.setComponentName(args[0])
        newEx.log()
        simpleClient.disconnect()
        sys.exit(-1)

    if bw != -1.0 or sampleRate != -1.0 or att != -1.0 or freq != -1.0 or pol != -1 or bins != -1:
        if not fChannel==-1:            
            try:
                for i in range(sChannel,fChannel):
                    backend.setChannel(i,freq,bw,sampleRate,att,pol,bins)
            except Exception, ex:
                newEx = ClientErrorsImpl.CouldntPerformActionExImpl( exception=ex, create=1 )
                newEx.setAction("setChannel()")
                newEx.log()
                simpleClient.releaseComponent(args[0])
                simpleClient.disconnect()
                sys.exit(-1)
        else:
            try:
                backend.setAllChannel(freq,bw,sampleRate,att,pol,bins)
            except Exception, ex:
                newEx = ClientErrorsImpl.CouldntPerformActionExImpl( exception=ex, create=1 )
                newEx.setAction("setChannel()")
                newEx.log()
                simpleClient.releaseComponent(args[0])
                simpleClient.disconnect()
                sys.exit(-1)
            
    if integr>=0:
        try:
            backend.setIntegration(integr)
        except Exception, ex:
            newEx = ClientErrorsImpl.CouldntPerformActionExImpl( exception=ex, create=1 )
            newEx.setAction("setIntegration()")
            newEx.log()
            simpleClient.releaseComponent(args[0])
            simpleClient.disconnect()
            sys.exit(-1)

    simpleClient.releaseComponent(args[0])     
    simpleClient.disconnect()

if __name__=="__main__":
   main()