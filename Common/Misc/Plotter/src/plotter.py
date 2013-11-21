#!/usr/bin/env python

# This is a python client that can be used to display value read from properties of a given 
# component
#who                                   when           what
#andrea orlati(a.orlati@ira.inaf.it)   08/05/2008     Creation
#andrea orlati(a.orlati@ira.inaf.it)   22/05/2008     Fixed a bug, the component was not released
#andrea orlati(a.orlati@ira.inaf.it)   11/06/2008     Added the possibility to save data into a file
#andrea orlati(a.orlati@ira.inaf.it)   16/07/2008     now values can be read also from the output parameters of methods 


import getopt, sys
import Acspy.Common.Err
import maciErrType
import maciErrTypeImpl
import ClientErrorsImpl
import time
import string
import Gnuplot
import threading
from Acspy.Clients.SimpleClient import PySimpleClient
from Acspy.Common.TimeHelper import getTimeStamp
from Acspy.Common.EpochHelper import EpochHelper
from Acspy.Common.DurationHelper import DurationHelper

class ThreadData:
    def __init__(self):
        self.sleepTime = 1.0
        self.plot = None
        self.Y1 = None 
        self.Y2 = None 
        self.Y3 = None
        self.X = None
        self.call = None
        self.Xparameter = -1
        self.Yparameter = [-1,-1,-1]
        self.stop = False
        self.pause = False
        self.clear = False
        self.maxData = 300
        self.yscale1 = 1.0
        self.yscale2 = 1.0
        self.yscale3 = 1.0
        self.xscale = 1.0
        self.xscaleTime= False
        self.xLabel = ""
        self.yLabel = ""
        self.lock = threading.Lock();
        self.saveToFile = False
        self.fileName = ""
        self.timeGap = 0

class GraphThread(threading.Thread):
    
    def __init__(self,data):
        threading.Thread.__init__(self)
        self.data=data
        self.dy1 = None
        self.dy2 = None
        self.dy3 = None
        self.y1 = []
        self.y2 = []
        self.y3 = []
        self.x = []
        self.functionResult = None
    
    def saveFile(self):
        xLab = ""
        yLab = []
        tmp = ""
        file = open (self.data.fileName,"w")
        if (self.data.xLabel==""):
            xLab = "counter\t"
        else:
            if self.data.xscaleTime:
                xLab = "milliseconds from midnight (" + str(len(self.x)) + ")"
            else:
                xLab = self.data.xLabel +" (" + str(len(self.x)) + ")"
        yLab = self.data.yLabel.split(",")
        for i in range(0,len(yLab)):
            tmp = tmp + yLab[i] + "\t"
        outstr = xLab + "\t" + tmp +"\n"
        file.write(outstr)
        outstr = ""
        for i in range(0,len(self.x)):
            if not (self.y3 == []):
                outstr = "%lf \t %lf \t %lf \t %lf \n" % (self.x[i],self.y1[i],self.y2[i],self.y3[i],)
            elif not (self.y2 == []):
                outstr = "%lf \t %lf \t %lf \n" % (self.x[i],self.y1[i],self.y2[i],)
            elif not (self.y1 == []):
                outstr = "%lf \t %lf \n" % (self.x[i],self.y1[i],)  
            file.write(outstr);
        file.close()
    
    def run(self):
        inc = 0.0
        dataCounter = 0
        while not self.data.stop:
            self.data.lock.acquire();
            if not self.data.pause:
                if self.data.clear:
                    inc = 0.0
                    dataCounter = 0
                    self.y1 = []
                    self.y2 = []
                    self.y3 = []
                    self.x = []
                    self.data.clear = False
                    
                if self.data.saveToFile == True:
                    self.saveFile()
                    self.data.saveToFile = False
                    
                if not (self.data.call == None):
                    par = []
                    self.functionResult = self.data.call(*par)
                
                if not (self.data.call == None):
                    if self.data.Xparameter == -1:
                        inc = inc + 1.0
                        self.x.append(inc)
                    else:
                        value = string.atof(self.functionResult[self.data.Xparameter])
                        if self.data.xscaleTime:
                            value = (value - float(self.data.timeGap))/10000.0
                            self.x.append(value)
                        else:
                            self.x.append(value*self.data.xscale)
                elif self.data.X == None:
                    inc = inc + 1.0
                    self.x.append(inc)
                else:
                    value=self.data.X.get_sync()[0]
                    if self.data.xscaleTime:
                        value = (value - float(self.data.timeGap))/10000.0
                        self.x.append(value)
                    else:    
                        self.x.append(value*self.data.xscale)
                if dataCounter > self.data.maxData:
                    self.x = self.x[1:]
                                
                if not (self.data.call == None):
                    if not(self.data.Yparameter[0] == -1):
                        value = self.functionResult[self.data.Yparameter[0]]
                        self.y1.append(value*self.data.yscale1)  
                elif not (self.data.Y1 == None):
                    self.y1.append(self.data.Y1.get_sync()[0]*self.data.yscale1)
                if dataCounter > self.data.maxData:
                    self.y1 = self.y1[1:]

                if not (self.data.call == None):
                    if not(self.data.Yparameter[1] == -1):
                        value = self.functionResult[self.data.Yparameter[1]]
                        self.y2.append(value*self.data.yscale2)  
                elif not (self.data.Y2 == None):
                    self.y2.append(self.data.Y2.get_sync()[0]*self.data.yscale2)
                if dataCounter > self.data.maxData:
                    self.y2 = self.y2[1:]

                if not (self.data.call == None):
                    if not(self.data.Yparameter[2] == -1):
                        value = self.functionResult[self.data.Yparameter[2]]
                        self.y3.append(value*self.data.yscale3)  
                elif not (self.data.Y3 == None):
                    self.y3.append(self.data.Y3.get_sync()[0]*self.data.yscale3)
                if dataCounter > self.data.maxData:
                    self.y3 = self.y3[1:]

                if dataCounter <= self.data.maxData:
                    dataCounter = dataCounter + 1
                
                if not (self.y3 == []):
                    self.dy3 = Gnuplot.PlotItems.Data(self.x,self.y3)
                    self.dy2 = Gnuplot.PlotItems.Data(self.x,self.y2)
                    self.dy1 = Gnuplot.PlotItems.Data(self.x,self.y1)
                    self.data.plot.plot(self.dy1,self.dy2,self.dy3)
                elif not (self.y2 == []):
                    self.dy2 = Gnuplot.PlotItems.Data(self.x,self.y2)
                    self.dy1 = Gnuplot.PlotItems.Data(self.x,self.y1)
                    self.data.plot.plot(self.dy1,self.dy2)
                elif not (self.y1 == []):
                    self.dy1 = Gnuplot.PlotItems.Data(self.x,self.y1)
                    self.data.plot.plot(self.dy1)
                    
            self.data.lock.release();                        
            time.sleep(self.data.sleepTime) 

def usage():
    print "plotter [-h|--help] [-l|--list] [-s|--sample val] [-g|--gap val] [-c|--call=val]"
    print  "       [-Y|--yscale=val] [-X|--xscale=val]  -y|--yaxe=val ComponentName"
    print "       "
    print "[-h|--help]           displays this help"
    print "[-l|--list]           list all the available components, then the program exits"
    print "[-s|--sample=val]     maximum number of samples in the plots, default is 300"
    print "[-g|--gap=val]        gap in milliseconds between two sample,"
    print "                      default value is 1000 milliseconds"
    print "[-c|--call=val]       name of an action of the component from which the values to be"
    print "                      plotted are taken from. The pure name is required, no parentesis or"
    print "                      anything else. The action must be of type void and so cannot retrurns values back."
    print "                      Just one name can be provided at a time."
    print "-y|--yaxe=val         comma separated list of the names of component attributes"
    print "                      that must be plotted along the Y axe. At least one item must be"
    print "                      provided, but a maximum of three will be considered. if the -c option"
    print "                      is provided this must be a comma separeted list of integers. Each integer"
    print "                      indicates a parameter in the argument sequence of the function given with -c."
    print "                      The value of that parameter is drawn in the plot. 0 represents the first parameter."
    print "                      A maximum of three can be given."
    print "[-x|--xaxe=val]       the name of the component attribute that will be the value"
    print "                      for the X axe. If not specified a sample counter will be adopted. In case of"
    print "                      option -c an integer is expected as for -y option."
    print "[-Y|--yscale=val]     the value set here will be used as a scale factor for attributes "
    print "                      plotted along the Y axis. The list must be comma separated, a maximum" 
    print "                      of three values is considered"       
    print "[-X|--xscale=val]     the value set here will be used a scale factor for the X axis. If the special token"
    print "                      'time' is provided then the value is treated as time and it is formatted as milliseconds "
    print "                      midnight of the the current day."
    print "ComponentName         this is the name of the component that must be loaded"

def onlineUsage():
    print "help                displays this help"
    print "stop                stops the automatic update of the plot"
    print "start               starts the automatic update of the plot"
    print "clear               clears the current plot"
    print "exit                exits this application"
    print "save [filename]     saves the plot into a ps file in the current working directory"
    print "savefile [filename] saves the data into a text file in the current working directory"
    print ""
    print "Then all Gnuplot commands are accepted for example: "
    print "set xrange [{x1}:{x2]] - changes the X axis ranges"
    print "set yyange [{y1}:{y2}] - changes the Y axis ranges"
    print "set autoscale - lets Gnuplot to choose the better scale"
    print "set logscale - sets the logaritmic scale"
    print "set ytics {value} - sets the spaces between tics in the Y axis"
    print "set ytics auto -  sets the spaces between tics automatically"
    print "set xlabel ""{label}"" - sets the label for the X axis"
    print "set style data {lines}{linespoints}{dots}{points} - sets the drawing style for the plot"
    print ""
    print "show ytics - shows the current configuration for the ytics"
    print "unset ytics - resets the current configuration for the ytics"
    

def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:],"hls:g:y:x:Y:X:c:",
        ["help","list","sample=","gap=","yaxe=","xaxe=","yscale=","xscale=","call="])
    except getopt.GetoptError, err:
        print str(err)
        usage()
        sys.exit(1)

    list = False
    sleepTime = 1000
    xAttribute = ""
    yAttribute = []
    xscale = 1.0
    yscale = [1.0, 1.0, 1.0]
    xscaleTime = False
    data = ThreadData()
    samples = 300
    yLabel = ""
    xLabel = ""
    functionToCall  = ""
    
    for o, a in opts:
        if o in ("-h", "--help"):
            usage()
            sys.exit()
        elif o in ("-l", "--list"):
            list = True
        elif o in ("-s", "--sample"):
            samples = string.atoi(a)
        elif o in ("-g", "--gap"):
            sleepTime=string.atoi(a)
        elif o in ("-c", "--call"):
            functionToCall = a
        elif o in ("-y", "--yaxe"):
            yAttribute = a.split(",",3)
            yLabel = a
        elif o in ("-x", "--xaxe"):
            xAttribute = a
            xLabel = a
        elif o in ("-Y", "--yscale"):
            tmp = a.split(",",3)
            for ii in range(0,len(tmp)):
                yscale[ii] = string.atof(tmp[ii])
        elif o in ("-X", "--xscale"):
            if ((a=="time") or (a=="TIME")):
                xscaleTime = True
            else:
                xscale = string.atof(a)

    if list==False and yAttribute == []:
        print("At least one Y axis item must be provided")
        usage()
        sys.exit(1)
     
    simpleClient = PySimpleClient()

    if list == True:  #Lists all available components
        components = simpleClient.availableComponents()
        for cob in components:
            print cob.name + " of type "+ cob.type
        a = sys.__stdin__.readline()
        sys.exit(0)
    elif not len(args)==0: #if one component has been set... then run
        try:
            component=simpleClient.getComponent(args[0])
        except Exception , ex:
            newEx = ClientErrorsImpl.CouldntAccessComponentExImpl( exception=ex, create=1 )
            newEx.setComponentName(args[0])
            newEx.log(ACS_LOG_ERROR)
            sys.exit(1)
        data.plot = Gnuplot.Gnuplot()
        
        if not (functionToCall == ""):
            data.call = getattr (component,functionToCall)
            data.Xparameter = int(xAttribute)
            for i in range(0,len(yAttribute)):
                data.Yparameter[i]=int(yAttribute[i])
        else:
            f1 = None
            f2 = None
            f3 = None
            fx = None
            if len(yAttribute) >= 1:
                f1 = getattr(component,"_get_" + yAttribute[0])
            if len(yAttribute) >= 2:
                f2 = getattr(component,"_get_" + yAttribute[1])
            if len(yAttribute) >= 3:
                f3 = getattr(component,"_get_" + yAttribute[2])
            if not (xAttribute == ""):
                fx = getattr(component,"_get_" + xAttribute) 
            
            try:
                if not (f1 == None):
                    data.Y1 = f1()
                if not (f2 == None):
                    data.Y2 = f2()
                if not (f3 == None):
                    data.Y3 = f3()
                if not (fx == None):
                    data.X = fx()
            except Exception, ex:
                newEx = ClientErrorsImpl.CouldntAccessPropertyExImpl( exception=ex, create=1 )
                newEx.setPropertyName("")
                newEx.log(ACS_LOG_ERROR)
                sys.exit(1)
                
        data.sleepTime = sleepTime/1000.0
        data.maxData = samples
        data.plot.ylabel(yLabel)
        data.plot.xlabel(xLabel)
        data.xLabel = xLabel
        data.yLabel = yLabel
        data.yscale1 = yscale [0]
        data.yscale2 = yscale [1]
        data.yscale3 = yscale [2]
        data.xscale = xscale
        data.xscaleTime = xscaleTime
        now=getTimeStamp().value
        timeHelp=EpochHelper(now)
        #get midnight of the current day
        timeHelp.hour(0)
        timeHelp.microSecond(0)
        timeHelp.minute(0)
        timeHelp.second(0)
        data.timeGap=timeHelp.value().value
        loop = GraphThread(data)
        loop.start()
        stop = False
        print ""
        while not stop:
            command = raw_input("Command ( ? for help): ")
            a=command.split(" ")
            if a[0] == "exit":
                data.lock.acquire();
                data.stop=True;
                stop = True
                data.lock.release()
                time.sleep(1.5)
            elif a[0] == "stop":
                data.lock.acquire();
                data.pause = True
                data.lock.release()
            elif a[0] == "start":
                data.lock.acquire();
                data.pause = False
                data.lock.release()
            elif a[0] == "clear":
                data.lock.acquire();
                data.clear = True
                data.lock.release()
            elif a[0] == "savefile":
                data.lock.acquire()
                data.saveToFile = True
                if not (a[1]==""):
                    data.fileName  = a[1]
                else:
                    data.fileName = "out.txt"
                data.lock.release()
            elif a[0] == "save":
                data.lock.acquire();
                if not (a[1]==""):
                    data.plot.hardcopy(a[1], mode='eps')
                else:
                    data.plot.hardcopy("out.ps", mode='eps')
                data.lock.release()
            elif a[0] == "help" or a[0] == "?" :
                onlineUsage()
            elif not (command==""):
                data.lock.acquire();
                data.plot(command)
                data.lock.release()
                
    simpleClient.releaseComponent(args[0])     
    simpleClient.disconnect()


if __name__=="__main__":
   main()
