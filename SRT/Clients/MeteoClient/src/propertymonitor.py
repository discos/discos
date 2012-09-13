# to do: emit from 


from Acspy.Clients.SimpleClient import PySimpleClient # Import the acspy.PySimpleClient class

import ACS, ACS__POA                                  # Import the Python CORBA stubs for BACI
from   omniORB.CORBA import TRUE, FALSE
from time import sleep

from PyQt4 import Qt
import PyQt4.Qwt5 as Qwt
from PyQt4.Qwt5.anynumpy import *

import sys
import ACSErrTypeCommonImpl
import ACSErrTypeCommon
import ACSErrTypeOKImpl
from Acspy.Common.Err import ACSError
from Acspy.Common.Log           import acsPrintExcDebug
from numpy import concatenate,zeros

def enumList(enum, sentinel):
    '''
    '''
    return [enum(i) for i in range(sentinel)]

colorGroupList = enumList(
    Qt.QPalette.ColorGroup, Qt.QPalette.NColorGroups)
colorRoleList = enumList(
    Qt.QPalette.ColorRole, Qt.QPalette.NColorRoles)
handList  = enumList(
    Qwt.QwtAnalogClock.Hand, Qwt.QwtAnalogClock.NHands)


BUFFER = 3600 #  number of points for plot



class PropertyMonitor(ACS__POA.CBdouble,Qt.QObject):
    #------------------------------------------------------------------------------
    '''
    This class defines the method(s) that will be invoked asynchronously by the
    mount device for any monitors we may create.
    '''
    #------------------------------------------------------------------------------
    def __init__ (self,widget, propName = None,*args): 
        '''
        If the propertys name is specified, it is printed to STDOUT later on.
        '''
        if propName != None:
            self.propName = propName
        else:
            self.propName = "NoName"
	Qt.QObject.__init__(self) # 

		
	self.widget = widget
#        Qwt.QwtPlot.__init__(self))

	



        

    #------------------------------------------------------------------------------
    def __del__(self):
        '''
        Do nothing
        '''
    #------------------------------------------------------------------------------
    def working (self, value, completion, desc):
        '''
        Really this is the method that does all the work and the developer should
        be concerned with.
        Parameters: value = the double we are interested in
                    completion = completion structure
                    desc = callback struct description
        '''
        #self.count = self.count + 1
        #if self.count <= 5:
	try:
        	print "Working: ", str(self.propName), " is ", str(value)
	#	self.widget.setVal(float(str(value)))
		value=float(str(value))*3.6
		self.emit(Qt.SIGNAL("PropertyChanghed(float)"),value)
		
	except:            
		
		acsPrintExcDebug()

		
		print "exception"	
		displayMessageEx = ACSErrTypeCommonImpl.GenericErrorExImpl() 
		displayMessageEx.setErrorDesc("badMethod has thrown an UNEXPECTED exception")
	        displayMessageEx.log()


    #------------------------------------------------------------------------------
    def done (self, value, completion, desc):
        '''
        Invoked asynchronously when the DO has finished.  Normally this is invoked
        just before a monitor is destroyed.
        Parameters: value = the final value of the double we are interested in
                    completion = completion structure
                    desc = callback struct description
        '''
        print "Done: ", str(self.propName), " is ", str(value)
 #       self.widget.setValue(float(value))

    #------------------------------------------------------------------------------
    def negotiate (self, time_to_transmit, desc):
        '''
        For simplicitys sake, we always return true.  If you want more detailed,
        information on this method, please see the BACI specs.
        Parameters: See the BACI specs.
        '''
        return TRUE
#------------------------------------------------------------------------------


class TimeScaleDraw(Qwt.QwtScaleDraw):

    def __init__(self, baseTime, *args):
        Qwt.QwtScaleDraw.__init__(self, *args)
        self.baseTime = baseTime
 
    # __init__()

    def label(self, value):
        upTime = self.baseTime.addSecs(int(value))
        return Qwt.QwtText(upTime.toString())
	
HISTORY=300


class myPlot(Qwt.QwtPlot):
	
	def __init__(self,*args):


		Qwt.QwtPlot.__init__(self,*args)
	        self.setAutoReplot(False)
		
		self.t=Qt.QTime()	
		self.timestep=1. # 
					
		self.curve=Qwt.QwtPlotCurve()
		self.curve.setPen(Qt.QPen(Qt.QColor(100, 200, 150)))
		self.curve.setSymbol(Qwt.QwtSymbol(Qwt.QwtSymbol.Ellipse,Qt.QBrush(Qt.Qt.blue), Qt.QPen(Qt.Qt.yellow),Qt.QSize(4, 4)))

#		self.y=randn(1000)
		self.x=range(BUFFER)
		self.y=zeros(BUFFER) 
		self.y2=[0. for i in xrange(BUFFER)]# inizializza una lista di float 
 		
#		self.timeData = 1.0 * arange(HISTORY-1, -1, -1)-HISTORY+1	

		self.timeData = 1.0 * arange(HISTORY,0 , -1)+1

		self.setAxisScaleDraw( Qwt.QwtPlot.xBottom, TimeScaleDraw(Qt.QTime.currentTime()))
                self.setAxisScale(Qwt.QwtPlot.xBottom, 0, HISTORY)
  		self.setAxisLabelRotation(Qwt.QwtPlot.xBottom, -50.0)
		self.curve.attach(self)		
		self.setAxisAutoScale(Qwt.QwtPlot.yLeft)
	        self.setAxisLabelAlignment(Qwt.QwtPlot.xBottom, Qt.Qt.AlignLeft | Qt.Qt.AlignBottom)

#lower, upper, majorTicks, mediumTicks, minorTicks)
	
	def setVal(self,val):

        	     self.y[1:]=self.y[0:-1] # scorre a sinistra il vettore
		     self.y[0]=val
#		     self.y2[0]=self.y[self.indice]
        	     self.timeData += 1.0*self.timestep;
		     

		     self.setAxisScale(Qwt.QwtPlot.xBottom,self.timeData[-1],self.timeData[0])
    		     self.curve.setData(self.timeData,self.y)
		     self.replot()
		
		


 

class PlotProperty(Qt.QObject):
	

      def __init__(self,widget,componentname,propertyname):
	self.componentname=componentname
	self.propertyname = propertyname
        self.simpleClient = PySimpleClient()
	self.simpleClient.getLogger().logInfo("We can directly manipulate a device once we get it, which is easy!!")
	self.plotwidget= widget
#	self.windspeedw = SpeedoMeter()
      def run(self):
        
	##frame  = Qt.QFrame()
	
	##layout = Qt.QGridLayout(frame)
	##layout.addWidget(self.plotwidget, 0, 0)
 	

	
	##frame.resize(600,200)
	##frame.show()
	self.monitor()

	#return 
      
      def make(self):
	
	
	return    PlotProperty
      def monitor(self):
	component = self.simpleClient.getComponent(self.componentname)
	exec("property=component._get_"+self.propertyname+"()") #attenzione exec valuta la stringa di testo
	#	  ed inizializza un oggetto secondo quanto  contenuto nekka strubga
	
#	windProperty = meteo._get_winddir()
#	wspeedPr     = meteo._get_windspeed()
	desc = ACS.CBDescIn(0L, 0L, 0L)
	timestep=5; 	
	
	propMonitor = PropertyMonitor(self.plotwidget,self.propertyname)
 	
	cbMonServant = self.simpleClient.activateOffShoot(propMonitor)
	self.actMon = property.create_monitor(cbMonServant, desc) #attenzione l'oggetto property e' stato creato dalla funzione exec
	self.actMon.set_timer_trigger(timestep*10000000)
	
	self.connect(propMonitor, Qt.SIGNAL("PropertyChanghed(float)"),self.plotwidget.setVal)

	      
	      
      def __del__(self):
 	self.actMon.destroy()
# 	self.actMonwspeed.destroy()
	self.simpleClient.releaseComponent(self.componentname)
	self.simpleClient.disconnect()
	print "The end __oOo__"
		
		
		
def make(args):
    app = Qt.QApplication(args)
	
    demo = Qt.QWidget()
    demo.setWindowTitle("WEATHERSTATION/WeatherStation")
    
    plot = myPlot(demo)
    plot.setTitle("History")
    plot.setMargin(5)
    my=PlotProperty(plot,"WEATHERSTATION/WeatherStation","windspeed")
    
#    label = Qt.QLabel("Press the legend to en/disable a curve", demo)
    
    layout = Qt.QVBoxLayout(demo)
    layout.addWidget(plot)
#    layout.addWidget(label)

    demo.resize(1024, 800)
    demo.show()
    my.run()
    
 #   my=PlotProperty(plot,"METEO/station","temperature")
    #w=my.run()
    return app.exec_()	
	
	



 
def main(args):
	
	

	
	 
#demo,plot=make(args)
		
#	w=my.run()
    	sys.exit(make(args))
	sleep(2)
if __name__=='__main__':
	main(sys.argv)
