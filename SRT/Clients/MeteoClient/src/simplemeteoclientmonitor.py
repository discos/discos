from Acspy.Clients.SimpleClient import PySimpleClient # Import the acspy.PySimpleClient class
import ACS, ACS__POA                                  # Import the Python CORBA stubs for BACI
from   omniORB.CORBA import TRUE, FALSE
from time import sleep

from PyQt4 import Qt
import PyQt4.Qwt5 as Qwt
import sys
import ACSErrTypeCommonImpl
import ACSErrTypeCommon
import ACSErrTypeOKImpl
from Acspy.Common.Err import ACSError
from Acspy.Common.Log           import acsPrintExcDebug

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



class WindMonitor(ACS__POA.CBdouble):
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
	
		
	self.widget = widget
        



        

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
		self.widget.setVal(float(str(value)))
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
	self.setValue(value*3.6)
    # drawScaleContents

# class SpeedoMeter



class WindWidget(Qt.QWidget):

    def __init__(self,parent=None):
   	Qt.QWidget.__init__(self,parent)	
   
   	#w=parent.width()
  	#h=parent.heigth()
	
	self.compass=Qwt.QwtCompass()
 	self.lcd=Qt.QLCDNumber()        

	
	palette = Qt.QPalette( )
        palette.setColor(Qt.QPalette.Foreground,Qt.Qt.lightGray)
	newPalette = self.compass.palette()
	rose=Qwt.QwtSimpleCompassRose(4,1)
	self.compass.setRose(rose)
	self.compass.setNeedle(Qwt.QwtCompassWindArrow(Qwt.QwtCompassWindArrow.Style2))	
	layout = Qt.QGridLayout(self)
	layout.addWidget(self.compass,0,0,9,0)	
	layout.addWidget(self.lcd,10,0)
	self.lcd.setSegmentStyle(Qt.QLCDNumber.Filled)

	self.setLayout(layout)	
	
    def __del__(self):
	''' none'''
    def setVal(self,value=0.):
 	self.compass.setValue(value)
	self.lcd.display(value)		      
	
class WindSpeed(Qt.QWidget):
    def __init__ (self,parent=None):
	self.speed=SpeedoMeter()
	self.speed.setRange(0.0,120.0)
	self.speed.setReadOnly(True)
	     
    def setVal(self,value=0.):
	self.setValue(value)
	
	


class ShowMeteo():
	

      def __init__(self):
        self.simpleClient = PySimpleClient()
	self.simpleClient.getLogger().logInfo("We can directly manipulate a device once we get it, which is easy!!")
	self.winddirw= WindWidget()
	self.windspeedw = SpeedoMeter()
      def run(self):
        
	frame  = Qt.QFrame()
	
	layout = Qt.QGridLayout(frame)
	layout.addWidget(self.winddirw, 0, 0)
	layout.addWidget(self.windspeedw, 0, 1)
 	

	
	frame.resize(600,200)
	frame.show()
	self.monitor()

	return frame
      
      def make(self):
 	#frame=Qt.QTabWidget()
	#frame.addTab(windWidget,"Wind");
	return    windWidget
      def monitor(self):
	meteo = self.simpleClient.getComponent("WEATHERSTATION/WeatherStation")	
	windProperty = meteo._get_winddir()
	wspeedPr     = meteo._get_windspeed()
	desc = ACS.CBDescIn(0L, 0L, 0L)
	
	
	windMon = WindMonitor(self.winddirw,"winddir")
 	cbMonServant = self.simpleClient.activateOffShoot(windMon)
	self.actMon = windProperty.create_monitor(cbMonServant, desc)
	self.actMon.set_timer_trigger(10000000)
	
	windSpeedMon =WindMonitor(self.windspeedw,"windspeed")

        cbMonServant2 = self.simpleClient.activateOffShoot(windSpeedMon)
	self.actMonwspeed = wspeedPr.create_monitor(cbMonServant2, desc)
	self.actMonwspeed.set_timer_trigger(20000000)
	
	
	
	      
	      
      def __del__(self):
 	self.actMon.destroy()
 	self.actMonwspeed.destroy()
	self.simpleClient.releaseComponent("METEO/station")
	self.simpleClient.disconnect()
	print "The end __oOo__"
	



 
def main(args):

	
	app = Qt.QApplication(args)
	my=ShowMeteo()
	
	w=my.run()
    	sys.exit(app.exec_())
	sleep(2)
if __name__=='__main__':
	main(sys.argv)
