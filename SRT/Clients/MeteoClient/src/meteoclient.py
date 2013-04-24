# to do: emit from 
#! /usr/bin/env python


from Acspy.Clients.SimpleClient import PySimpleClient # Import the acspy.PySimpleClient class

import ACS, ACS__POA                                  # Import the Python CORBA stubs for BACI
from   omniORB.CORBA import TRUE, FALSE
from time import sleep
import time
from PyQt4 import Qt, QtCore, QtGui
import PyQt4.Qwt5 as Qwt
from PyQt4.Qwt5.anynumpy import *

import sys
import ACSErrTypeCommonImpl
import ACSErrTypeCommon
import ACSErrTypeOKImpl
from Acspy.Common.Err import ACSError
from Acspy.Common.Log           import acsPrintExcDebug
from numpy import concatenate,zeros
from Acspy.Common.TimeHelper import getTimeStamp,TimeUtil

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

TIMESTEP=5 # seconds
HISTORY=500 #



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
		value=float(str(value))
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

class SpeedoMeter(Qwt.QwtDial):

    def __init__(self,parent=None):
        Qwt.QwtDial.__init__(self, parent)
        self.__label = 'km/h'
        self.setWrapping(False)
        self.setReadOnly(True)

        self.setOrigin(135.0)
        self.setScaleArc(0.0, 270.0)

        self.setNeedle(Qwt.QwtDialSimpleNeedle(Qwt.QwtDialSimpleNeedle.Arrow,True,Qt.QColor(Qt.Qt.red),Qt.QColor(Qt.Qt.gray).light(130)))

        self.setScaleOptions(Qwt.QwtDial.ScaleTicks | Qwt.QwtDial.ScaleLabel)
        self.setScaleTicks(0, 4, 8)
        self.setRange(0.0,120.0)
        self.setScale(-1, 2, 20)
        self.setFrameShadow(Qwt.QwtDial.Sunken)

    # __init__()
    
    def setLabel(self, text):
        self.__label = text
        self.update()

    # setLabel()
    
    def label(self):
        return self.__label

    # label()
    
    def drawScaleContents(self, painter, center, radius):
        rect = Qt.QRect(0, 0, 2 * radius, 2 * radius - 10)
        rect.moveCenter(center)
        painter.setPen(self.palette().color(Qt.QPalette.Text))
        painter.drawText(rect, Qt.Qt.AlignBottom | Qt.Qt.AlignHCenter, self.__label)
    def setVal(self,value=0.):
        self.setValue(value)
    # drawScaleContents

# class SpeedoMeter





class TimeScaleDraw(Qwt.QwtScaleDraw):

    def __init__(self, baseTime, *args):
        Qwt.QwtScaleDraw.__init__(self, *args)
        self.baseTime = baseTime
 
    # __init__()

    def label(self, value):
        upTime = self.baseTime.addSecs(int(value)*TIMESTEP)
        return Qwt.QwtText(upTime.toString())
	




class Background(Qwt.QwtPlotItem):

    def __init__(self):
        Qwt.QwtPlotItem.__init__(self)
        self.setZ(0.0)

    # __init__()

    def rtti(self):
        return Qwt.QwtPlotItem.Rtti_PlotUserItem

    # rtti()

    def draw(self, painter, xMap, yMap, rect):
        c = Qt.QColor(Qt.Qt.gray)
        r = Qt.QRect(rect)

        for i in range(60, 0, -10):
            r.setBottom(yMap.transform(i - 10))
            r.setTop(yMap.transform(i))
            painter.fillRect(r, c)
            c = c.dark(110)

    # draw()

# class Background

class WindWidget(Qt.QWidget):

    def __init__(self,parent=None):
        Qt.QWidget.__init__(self,parent)        
   
        self.compass=Qwt.QwtCompass(self)
        #self.lcd=Qt.QLCDNumber()        
        self.lcd =QtGui.QLineEdit('0')
        
        palette = Qt.QPalette( )
        palette.setColor(Qt.QPalette.Foreground,Qt.Qt.black)
        
        newPalette = self.compass.palette()
        rose=Qwt.QwtSimpleCompassRose(4,1)
        self.compass.setRose(rose)
        self.compass.setNeedle(Qwt.QwtCompassWindArrow(Qwt.QwtCompassWindArrow.Style2))
    #    self.lcd.setSegmentStyle(Qt.QLCDNumber.Filled)#        
        lcdpalette = Qt.QPalette( )
        lcdpalette.setBrush(Qt.QPalette.Background,Qt.Qt.red);          
#        palette.setColor(Qt.QPalette.Foreground,Qt.Qt.black)
 #       lcdpalette.setColor(Qt.QPalette.Active,Qt.QPalette.Background, Qt.Qt.red);
      #  self.lcd.setStyleSheet("* { background-color: white; color: red; }")
 #       self.lcd.setPalette( lcdpalette );
# 
        layout = Qt.QGridLayout(self)
        layout.addWidget(self.compass,0,0,9,0) 
      #  layout.setColumnStrecth(10) 
        layout.addWidget(self.lcd,10,0)
        self.setLayout(layout)  
        
    def __del__(self):
        ''' none'''
    def setVal(self,value=0.):
        self.compass.setValue(value)
        self.lcd.setText(Qt.QString(str(value))+" deg.")
        
class WindSpeed(Qt.QWidget):
    def __init__ (self,parent=None):
        Qt.QWidget.__init__(self,parent)        
        
        self.speed=SpeedoMeter(self)
        self.speed.setRange(0.0,120.0)
        self.speed.setReadOnly(True)
        self.setGeometry(0,0,255,255)
     
    def setVal(self,value=0.):
        self.speed.setValue(value)
        
        
class Temperature(Qt.QWidget):
      def __init__ (self,parent=None):
          Qt.QWidget.__init__(self,parent)
          self.lcd=Qt.QLineEdit()        

  
          self.thermo = Qwt.QwtThermo(self)
          self.thermo.setRange(-5., 45.0)
          self.thermo.setFillColor(Qt.Qt.red)
          self.thermo.setPipeWidth(12)
          self.thermo.setOrientation(Qt.Qt.Vertical, Qwt.QwtThermo.LeftScale)
          label = Qt.QLabel("Air Temp", self)
  
          label.setAlignment(Qt.Qt.AlignTop | Qt.Qt.AlignLeft)
          layout = Qt.QVBoxLayout(self)
          layout.addWidget(self.thermo)
          layout.addWidget(label)
          layout.addWidget(self.lcd)


          layout.setMargin(0)

      
      
      def setVal(self,value=0.):
           self.thermo.setValue(value)
           lcdpalette = Qt.QPalette( )
 #  palette->setColor(QPalette::Background,Qt::red);          
#        palette.setColor(Qt.QPalette.Foreground,Qt.Qt.black)
           lcdpalette.setColor(Qt.QPalette.Background, Qt.Qt.red);
           self.lcd.setPalette( lcdpalette );
           self.lcd.setText(Qt.QString("%1").arg(value,3,'f',1))
           #QString("%1").arg(123.2349102,8,'f',4);

class Values(Qt.QFrame):
        def __init__(self,*args):
             Qt.QFrame.__init__(self,*args)
             



class myPlot(Qwt.QwtPlot):

	def __init__(self,*args):
		Qwt.QwtPlot.__init__(self,*args)
	        self.setAutoReplot(False)
		self.t=Qt.QTime()
		ts=getTimeStamp()
		epoch=TimeUtil()
		pydate=epoch.epoch2py(ts)
		t=time.gmtime(pydate) # convert to 
		h,m,s=t.tm_hour,t.tm_min,t.tm_sec  
		self.t=Qt.QTime(h,m,s)
#		self.timestep=5. # 
					
		self.curve=Qwt.QwtPlotCurve()
		self.curve.setPen(Qt.QPen(Qt.Qt.blue,2))
#                 self.curve.setBrush(Qt.Qt.blue)

#		self.curve.setSymbol(Qwt.QwtSymbol(Qwt.QwtSymbol.Ellipse,Qt.QBrush(Qt.Qt.blue), Qt.QPen(Qt.Qt.yellow),Qt.QSize(4, 4)))
#                self.curve.setZ(2);
                
                background = Background()
                background.attach(self)
                
                
                self.curveWSpeedPeak=Qwt.QwtPlotCurve()
                self.curveWSpeedPeak.setPen(Qt.QPen(Qt.Qt.green,2))
#                self.curveWSpeedPeak.setBrush(Qt.Qt.blue)
#                self.curveWSpeedPeak.setSymbol(Qwt.QwtSymbol(Qwt.QwtSymbol.Ellipse,Qt.QBrush(Qt.Qt.blue), Qt.QPen(Qt.Qt.yellow),Qt.QSize(4, 4)))
         #       self.curveWSpeedPeak.setZ(1)
                
#		self.y=randn(1000)
		self.x=range(BUFFER)
		self.y=zeros(BUFFER) 
                self.windSpeedPeak=zeros(BUFFER)
                
		self.y2=[0. for i in xrange(BUFFER)]# inizializza una lista di float 
 		
		self.timeData = 1.0 * arange(0,-HISTORY , -1)+1
		
		currTime=Qt.QTime.currentTime();	
		currTime.addSecs(-HISTORY*TIMESTEP);
		
		self.setAxisScaleDraw( Qwt.QwtPlot.xBottom, TimeScaleDraw(currTime))
                self.setAxisScale(Qwt.QwtPlot.xBottom, 0, HISTORY*TIMESTEP)
  		self.setAxisLabelRotation(Qwt.QwtPlot.xBottom, -50.0)
		self.curve.attach(self)
                self.curveWSpeedPeak.attach(self)		
#    		self.setAxisAutoScale(Qwt.QwtPlot.yLeft)
                self.setAxisScale(Qwt.QwtPlot.yLeft,0,60,10)
	        self.setAxisTitle(Qwt.QwtPlot.yLeft, "Wind Speed (Km/s)")

                self.setAxisLabelAlignment(Qwt.QwtPlot.xBottom, Qt.Qt.AlignLeft | Qt.Qt.AlignBottom)

#lower, upper, majorTicks, mediumTicks, minorTicks)
	
	def setVal(self,val):

        	     self.y[1:]=self.y[0:-1] # scorre a sinistra il vettore
		     self.y[0]=val
#		     self.y2[0]=self.y[self.indice]
		
	def setWindSpeedPeak(self,val):
                     self.timeData += 1.0;
                     

                     self.setAxisScale(Qwt.QwtPlot.xBottom,self.timeData[-1],self.timeData[0])
                     self.curve.setData(self.timeData,self.y)

                     self.windSpeedPeak[1:]=self.windSpeedPeak[0:-1] 
                     self.windSpeedPeak[0]=val
                     self.curveWSpeedPeak.setData(self.timeData,self.windSpeedPeak)
#                     self.curveWSpeedPeak.setBaseline(self.y[0]) # baseline set to win 
                     self.replot()

class PlotProperty(Qt.QFrame):
      
      def __init__(self,componentname,propertyname):
        Qt.QFrame.__init__(self) # you must initialize the superclass
        self.componentname=componentname
        self.propertyname = propertyname
        self.simpleClient = PySimpleClient()
        self.simpleClient.getLogger().logInfo("We can directly manipulate a device once we get it, which is easy!!")
        
        self.plotwidget=     myPlot()  
        self.windspeedw = SpeedoMeter()
        self.winddirw= WindWidget()
        self.temperaturew= Temperature()
        self.plotwidget.setTitle("WeatherStation")
        self.plotwidget.setMargin(5)
        mainLayout = Qt.QVBoxLayout(self)
        mainLayout.addWidget(self.plotwidget,2)

#        layout  = Qt.QGridLayout()
        
        
        
        hlayout = Qt.QHBoxLayout()
 #       hvlayout= Qt.QForm
        hlayout.setAlignment(Qt.Qt.AlignHCenter)
#        hlayout.setSpacing(150)
#        hlayout.addSpacing(10)
        hlayout.addStretch(50)
        hlayout.addWidget(self.temperaturew)
        hlayout.addStretch(50)
        hlayout.addWidget(self.windspeedw)
        hlayout.addWidget(self.winddirw)
        hlayout.addStretch(50)
        mainLayout.addLayout(hlayout)

        #layout.addWidget(self.plotwidget, 0, 0, 9, 3)
 #       layout.addWidget(self.plotwidget, 1,0)
        

        

#        self.setLayout(layout)  # 
	
	#	self.windspeedw = SpeedoMeter()
      def run(self):
	self.monitor()
      def monitor(self):
	component = self.simpleClient.getComponent(self.componentname)
#	exec("property=component._get_"+self.propertyname+"()") #attenzione exec valuta la stringa di testo
	#	  ed inizializza un oggetto secondo quanto  contenuto nekka strubga
        windSpeadAvgPr=component._get_windspeed()
        windSpeedPeakProperty =component._get_windspeedpeak()
#        windDir = component._get_winddir()
	tempPr  = component._get_temperature()
        windDirPr  = component._get_winddir()
        
        
        
        
#	windProperty = meteo._get_winddir()
#	wspeedPr     = meteo._get_windspeed()
	desc = ACS.CBDescIn(0L, 0L, 0L)
            
     
        
        
	propMonitor = PropertyMonitor(self.plotwidget,self.propertyname)
        cbMonServant = self.simpleClient.activateOffShoot(propMonitor)
	self.actMon = windSpeadAvgPr.create_monitor(cbMonServant, desc) #attenzione l'oggetto property e' stato creato dalla funzione exec
	self.actMon.set_timer_trigger(TIMESTEP*10000000)
        #self.connect(propMonitor, Qt.SIGNAL("PropertyChanghed(float)"),self.plotwidget.setVal)
        
        windSpeedPeakMon=PropertyMonitor(self.plotwidget,"windspeedpeak")
        cbMonServant_wsppeak = self.simpleClient.activateOffShoot(windSpeedPeakMon)
        self.WSpeedPeakactMon = windSpeedPeakProperty.create_monitor(cbMonServant_wsppeak, desc) #attenzione l'oggetto property e' stato creato dalla funzione exec
        self.WSpeedPeakactMon.set_timer_trigger(TIMESTEP*10000000)
        self.connect(windSpeedPeakMon, Qt.SIGNAL("PropertyChanghed(float)"),self.plotwidget.setWindSpeedPeak)
        self.connect(windSpeedPeakMon, Qt.SIGNAL("PropertyChanghed(float)"),self.windspeedw.setVal)
        
        windDirPeakMon = PropertyMonitor(self.plotwidget,"wind dir peak")
        cbMonServant = self.simpleClient.activateOffShoot(windDirPeakMon)
        self.WDiractMon = windDirPr.create_monitor(cbMonServant, desc) #attenzione l'oggetto property e' stato creato dalla funzione exec
        self.WDiractMon.set_timer_trigger(TIMESTEP*10000000)
        self.connect(windDirPeakMon, Qt.SIGNAL("PropertyChanghed(float)"),self.winddirw.setVal)

        temperatureMon = PropertyMonitor(self.plotwidget,"Temperature:")
        cbMonServant = self.simpleClient.activateOffShoot(temperatureMon)
        self.temperatureactMon = tempPr.create_monitor(cbMonServant, desc) #attenzione l'oggetto property e' stato creato dalla funzione exec
        self.temperatureactMon.set_timer_trigger(TIMESTEP*10000000*4) 
        self.connect(temperatureMon, Qt.SIGNAL("PropertyChanghed(float)"),self.temperaturew.setVal)
        

        
      
      
      
      def __del__(self):
 	self.actMon.destroy()
        self.WDiractMon.destroy()
        self.temperatureactMon.destroy()
        self.WSpeedPeakactMon.destroy()
	self.actMonwspeed.destroy()
	self.simpleClient.releaseComponent(self.componentname)
	self.simpleClient.disconnect()
	print "The end __oOo__"
		
			
		
def make( ):
	
    plot = PlotProperty("WEATHERSTATION/WeatherStation","windspeed")
    plot.resize(800, 600)
    plot.run()
    plot.show() 

    return plot	
	
	
 
def main(args):
	
        app = Qt.QApplication(args)
	plot=make()
	

#demo,plot=make(args)
		
#	w=my.run()
    	sys.exit(app.exec_())
	sleep(2)
if __name__=='__main__':
	main(sys.argv)
