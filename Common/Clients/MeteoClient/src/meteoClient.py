#! /usr/bin/env python
from __future__ import print_function
from Acspy.Clients.SimpleClient import PySimpleClient # Import the acspy.PySimpleClient class

import ACS, ACS__POA                                  # Import the Python CORBA stubs for BACI
from omniORB.CORBA import TRUE, FALSE
from time import sleep
import time
from PyQt4 import Qt, QtCore, QtGui
import PyQt4.Qwt as Qwt

import sys
import ACSErrTypeCommonImpl
import ACSErrTypeCommon
import ACSErrTypeOKImpl
import ClientErrorsImpl
from IRAPy import logger
from Acspy.Common.Err import ACSError
from Acspy.Common.Log import acsPrintExcDebug
from numpy import concatenate, zeros, arange
from Acspy.Common.TimeHelper import getTimeStamp, TimeUtil


def enumList(enum, sentinel):
    return [enum(i) for i in range(sentinel)]


colorGroupList = enumList(
    Qt.QPalette.ColorGroup, Qt.QPalette.NColorGroups)
colorRoleList = enumList(
    Qt.QPalette.ColorRole, Qt.QPalette.NColorRoles)
handList  = enumList(
    Qwt.QwtAnalogClock.Hand, Qwt.QwtAnalogClock.NHands)

BUFFER = 3600  # number of points for plot

TIMESTEP = 5  # seconds
HISTORY = 500


class PropertyMonitor(ACS__POA.CBdouble,Qt.QObject):
    #------------------------------------------------------------------------------
    '''
    This class defines the method(s) that will be invoked asynchronously by the
    mount device for any monitors we may create.
    '''
    def __init__ (self, widget, propName = None, *args):
        '''
        If the propertys name is specified, it is printed to STDOUT later on.
        '''
        if propName != None:
            self.propName = propName
        else:
            self.propName = "NoName"
        Qt.QObject.__init__(self)

        self.widget = widget
        #Qwt.QwtPlot.__init__(self))

    def __del__(self):
        '''
        Do nothing
        '''
        pass

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
            #print("Working: ", str(self.propName), " is ", str(value))
            #self.widget.setVal(float(str(value)))
            value = float(str(value))
            self.emit(Qt.SIGNAL("PropertyChanghed(float)"), value)

        except:
            acsPrintExcDebug()
            print("exception")
            displayMessageEx = ACSErrTypeCommonImpl.GenericErrorExImpl()
            displayMessageEx.setErrorDesc("badMethod has thrown an UNEXPECTED exception")
            displayMessageEx.log()

    def done (self, value, completion, desc):
        '''
        Invoked asynchronously when the DO has finished.  Normally this is invoked
        just before a monitor is destroyed.
        Parameters: value = the final value of the double we are interested in
                        completion = completion structure
                        desc = callback struct description
        '''
        print("Done: ", str(self.propName), " is ", str(value))
        #self.widget.setValue(float(value))

    def negotiate (self, time_to_transmit, desc):
        '''
        For simplicitys sake, we always return true.  If you want more detailed,
        information on this method, please see the BACI specs.
        Parameters: See the BACI specs.
        '''
        return TRUE


class SpeedoMeter(Qwt.QwtDial):

    def __init__(self, limit, parent=None):
        Qwt.QwtDial.__init__(self, parent)
        self.limit = limit
        self.__label = 'km/h'
        self.setWrapping(False)
        self.setReadOnly(True)

        self.setOrigin(135.0)
        self.setScaleArc(0.0, 270.0)

        self.setNeedle(Qwt.QwtDialSimpleNeedle(Qwt.QwtDialSimpleNeedle.Arrow, True, Qt.QColor(Qt.Qt.blue), Qt.QColor(Qt.Qt.gray).light(130)))

        #self.setScaleOptions(Qwt.QwtDial.ScaleTicks | Qwt.QwtDial.ScaleLabel)
        #self.setScaleTicks(0, 4, 8)
        #self.setMinScaleArc(0.0)
        #self.setMaxScaleArc(80.0)
        #self.setRange(0.0,80.0)
        self.setScale(0.0, 80.0)
        self.setFrameShadow(Qwt.QwtDial.Sunken)

    def setLabel(self, text):
        self.__label = text
        self.update()

    def label(self):
        return self.__label

    def drawScaleContents(self, painter, center, radius):
        radius = int(radius)
        rect = Qt.QRect(0, 0, 2 * radius, 2 * radius - 10)
        rect.moveCenter(center.toPoint())
        painter.setPen(self.palette().color(Qt.QPalette.Text))
        painter.drawText(rect, int(Qt.Qt.AlignBottom | Qt.Qt.AlignHCenter), self.__label)
        rect = Qt.QRect(-1000, -1000, 2 * radius - 30, 2 * radius - 30)
        rect.moveCenter(center.toPoint())

        unit = 270. / 80

        pen = Qt.QPen()
        pen.setCapStyle(Qt.Qt.FlatCap)
        pen.setWidth(8)
        pen.setColor(Qt.Qt.red)
        painter.setPen(pen)
        painter.drawArc(rect, 16 * -45, int(16 * (80 - self.limit) * unit))

        pen.setColor(Qt.Qt.green)
        painter.setPen(pen)
        painter.drawArc(rect, int(16 * (-45 + (80 - self.limit) * unit)), int(16 * self.limit * unit))

    def setVal(self, value=0.):
        self.setValue(value)


class Background(Qwt.QwtPlotItem):

    def __init__(self, limit):
        Qwt.QwtPlotItem.__init__(self)
        self.setZ(0.0)
        self.limit = limit

    def rtti(self):
        return Qwt.QwtPlotItem.Rtti_PlotUserItem

    def draw(self, painter, xMap, yMap, rect):
        c = Qt.QColor(Qt.Qt.gray)
        r = Qt.QRect(rect.toAlignedRect())
        red = Qt.QColor(Qt.Qt.red)
        yellow = Qt.QColor(Qt.Qt.yellow)

        r.setBottom(int(yMap.transform(self.limit)))
        r.setTop(int(yMap.transform(self.limit)))
        painter.fillRect(r, red)


class WindWidget(Qt.QWidget):

    def __init__(self,parent = None):
        Qt.QWidget.__init__(self,parent)

        self.compass = Qwt.QwtCompass(self)
        self.lcd = QtGui.QLineEdit('0')

        palette = Qt.QPalette()
        palette.setColor(Qt.QPalette.Foreground, Qt.Qt.black)

        newPalette = self.compass.palette()
        rose = Qwt.QwtSimpleCompassRose(4, 1)
        self.compass.setRose(rose)
        self.compass.setNeedle(Qwt.QwtCompassWindArrow(Qwt.QwtCompassWindArrow.Style2, Qt.QColor(0xCCCCFF)))
        #self.lcd.setSegmentStyle(Qt.QLCDNumber.Filled)
        lcdpalette = Qt.QPalette()
        lcdpalette.setBrush(Qt.QPalette.Background, Qt.Qt.red)

        layout = Qt.QGridLayout(self)
        layout.addWidget(self.compass, 0, 0, 9, 0)
        #layout.setColumnStrecth(10)
        layout.addWidget(self.lcd, 10, 0)
        self.setLayout(layout)

    def __del__(self):
        pass

    def setVal(self, value=0.):
        self.compass.setValue(value)
        self.lcd.setText(str(value) + " deg.")


class WindSpeed(Qt.QWidget):

    def __init__ (self, parent=None):
        Qt.QWidget.__init__(self, parent)

        self.speed = SpeedoMeter(self)
        self.speed.setRange(0.0, 120.0)
        self.speed.setReadOnly(True)
        self.setGeometry(0, 0, 255, 255)

    def setVal(self, value=0.):
        self.speed.setValue(value)


class Temperature(Qt.QWidget):

        def __init__ (self, parent=None):
            Qt.QWidget.__init__(self, parent)
            self.lcd = Qt.QLineEdit()
            self.thermo = Qwt.QwtThermo()
            self.thermo.setLowerBound(-5.)
            self.thermo.setUpperBound(45.0)
            self.thermo.setFillBrush(QtGui.QBrush(Qt.Qt.red))
            self.thermo.setPipeWidth(12)
            self.thermo.setOrientation(Qt.Qt.Vertical)
            self.thermo.setScalePosition(Qwt.QwtThermo.TrailingScale)
            #self.thermo.setOrientation(Qt.Qt.Vertical, Qwt.QwtThermo.LeftScale)
            label = Qt.QLabel("Air Temp", self)

            label.setAlignment(Qt.Qt.AlignTop | Qt.Qt.AlignCenter)
            layout = Qt.QVBoxLayout()
            layout.setAlignment(Qt.Qt.AlignHCenter)
            layout.addWidget(self.thermo, Qt.Qt.AlignCenter)
            layout.addWidget(label)
            layout.addWidget(self.lcd, 1, Qt.Qt.AlignRight)
            layout.setMargin(0)
            self.setLayout(layout)

        def setVal(self, value=0.):
            self.thermo.setValue(value)
            lcdpalette = Qt.QPalette()
            lcdpalette.setColor(Qt.QPalette.Background, Qt.Qt.red);
            self.lcd.setPalette(lcdpalette);
            self.lcd.setText("%.1f" % value)


class Values(Qt.QFrame):

        def __init__(self, *args):
            Qt.QFrame.__init__(self, *args)


class PlotWindWidget(Qwt.QwtPlot):

    def __init__(self, limit, *args):
        Qwt.QwtPlot.__init__(self, *args)
        self.setAutoReplot(False)
        self.t = Qt.QTime()
        ts = getTimeStamp()
        epoch = TimeUtil()
        pydate = epoch.epoch2py(ts)
        t = time.gmtime(pydate) # convert to
        h, m, s = t.tm_hour, t.tm_min, t.tm_sec
        self.t = Qt.QTime(h, m, s)
        #self.timestep = 5.

        #self.curve = Qwt.QwtPlotCurve()
        #self.curve.setPen(Qt.QPen(Qt.Qt.blue, 2))
        #self.curve.setBrush(Qt.Qt.blue)

        #self.curve.setSymbol(Qwt.QwtSymbol(Qwt.QwtSymbol.Ellipse, Qt.QBrush(Qt.Qt.blue), Qt.QPen(Qt.Qt.yellow), Qt.QSize(4, 4)))
        #self.curve.setZ(2);

        background = Background(limit)
        background.attach(self)

        grid = Qwt.QwtPlotGrid()
        grid.attach(self)
        grid.enableX(False)

        grid.setPen(Qt.QPen(Qt.Qt.black, 0, Qt.Qt.DotLine))

        self.setCanvasBackground(Qt.Qt.white)
        self.curveWSpeedPeak = Qwt.QwtPlotCurve()
        self.curveWSpeedPeak.setPen(Qt.QPen(Qt.Qt.blue, 2))
        #self.curveWSpeedPeak.setBrush(Qt.Qt.blue)
        #self.curveWSpeedPeak.setSymbol(Qwt.QwtSymbol(Qwt.QwtSymbol.Ellipse, Qt.QBrush(Qt.Qt.blue), Qt.QPen(Qt.Qt.yellow), Qt.QSize(4, 4)))
        #self.curveWSpeedPeak.setZ(1)

        #self.y = randn(1000)
        self.windSpeedPeak = zeros(BUFFER)

        self.y2 = [0. for i in range(BUFFER)]  # inizializza una lista di float

        a = Qwt.QwtDateScaleDraw()
        a.setDateFormat(Qwt.QwtDate.Second, 'hh:mm:ss')
        self.setAxisScaleDraw(Qwt.QwtPlot.xBottom, a)

        now = time.time() * 1000
        self.timeData = arange(now, now - BUFFER * 1000, -1000)
        self.setAxisScale(Qwt.QwtPlot.xBottom, self.timeData[-1], self.timeData[0])
        #self.setAxisScale(Qwt.QwtPlot.xBottom, 0, HISTORY * TIMESTEP)
        self.setAxisLabelRotation(Qwt.QwtPlot.xBottom, -50.0)
        #self.curve.attach(self)
        self.curveWSpeedPeak.attach(self)
        #self.setAxisAutoScale(Qwt.QwtPlot.yLeft)
        self.setAxisScale(Qwt.QwtPlot.yLeft, 0, 80, 10)
        self.setAxisTitle(Qwt.QwtPlot.yLeft, "Wind Speed (Km/h)")

        self.setAxisLabelAlignment(Qwt.QwtPlot.xBottom, Qt.Qt.AlignLeft | Qt.Qt.AlignBottom)

    def setWindSpeedPeak(self, val):
        self.timeData[1:] = self.timeData[0:-1]
        self.timeData[0] = time.time() * 1000
        self.setAxisScale(Qwt.QwtPlot.xBottom, self.timeData[-1], self.timeData[0])
        #self.curve.setData(self.timeData, self.y)
        self.windSpeedPeak[1:] = self.windSpeedPeak[0:-1]
        self.windSpeedPeak[0] = val
        self.curveWSpeedPeak.setSamples(self.timeData.tolist(), self.windSpeedPeak.tolist())
        #self.curveWSpeedPeak.setBaseline(self.y[0])  # baseline set to win
        self.replot()


class PlotProperty(Qt.QFrame):

    def __init__(self,componentname,propertyname):
        Qt.QFrame.__init__(self)  # you must initialize the superclass
        self.componentname = componentname
        self.propertyname = propertyname
        try:
            self.simpleClient = PySimpleClient()
            component = self.simpleClient.getComponent(self.componentname)
            self.limit = component._get_autoparkThreshold().get_sync()[0]
        except Exception as ex:
            newEx = ClientErrorsImpl.CouldntAccessPropertyExImpl(exception=ex, create=1)
            newEx.setPropertyName("autoparkThreshold")
            raise newEx

        self.simpleClient.getLogger().logInfo("Wheather station client startup")

        self.plotwidget = PlotWindWidget(self.limit)
        self.windspeedw = SpeedoMeter(self.limit)
        self.winddirw = WindWidget()
        self.temperaturew = Temperature()
        #self.plotwidget.setTitle("WeatherStation")
        #self.plotwidget.setMargin(5)
        mainLayout = Qt.QVBoxLayout(self)
        mainLayout.addWidget(self.plotwidget, 2)

        #layout = Qt.QGridLayout()

        hlayout = Qt.QHBoxLayout()
        #hvlayout = Qt.QForm
        hlayout.setAlignment(Qt.Qt.AlignHCenter)
        #hlayout.setSpacing(150)
        #hlayout.addSpacing(10)
        hlayout.addStretch(50)
        hlayout.addWidget(self.temperaturew)
        hlayout.addStretch(50)
        hlayout.addWidget(self.windspeedw)
        hlayout.addStretch(50)
        hlayout.addWidget(self.winddirw)
        hlayout.addStretch(50)
        mainLayout.addLayout(hlayout)

        #layout.addWidget(self.plotwidget, 0, 0, 9, 3)
        #layout.addWidget(self.plotwidget, 1, 0)
        #self.setLayout(layout)
        #self.windspeedw = SpeedoMeter()

    def run(self):
        self.monitor()

    def monitor(self):
        component = self.simpleClient.getComponent(self.componentname)
        windSpeadAvgPr = component._get_windspeed()
        windSpeedPeakProperty = component._get_windspeedpeak()
        tempPr = component._get_temperature()
        windDirPr = component._get_winddir()
        desc = ACS.CBDescIn(0, 0, 0)

        propMonitor = PropertyMonitor(self.plotwidget, self.propertyname)
        cbMonServant = self.simpleClient.activateOffShoot(propMonitor)
        self.actMon = windSpeadAvgPr.create_monitor(cbMonServant, desc)
        self.actMon.set_timer_trigger(TIMESTEP * 10000000)

        windSpeedPeakMon = PropertyMonitor(self.plotwidget, "windspeedpeak")
        cbMonServant_wsppeak = self.simpleClient.activateOffShoot(windSpeedPeakMon)
        self.WSpeedPeakactMon = windSpeedPeakProperty.create_monitor(cbMonServant_wsppeak, desc)
        self.WSpeedPeakactMon.set_timer_trigger(TIMESTEP * 10000000)
        self.connect(windSpeedPeakMon, Qt.SIGNAL("PropertyChanghed(float)"), self.plotwidget.setWindSpeedPeak)
        self.connect(windSpeedPeakMon, Qt.SIGNAL("PropertyChanghed(float)"), self.windspeedw.setVal)

        windDirPeakMon = PropertyMonitor(self.plotwidget, "wind dir peak")
        cbMonServant = self.simpleClient.activateOffShoot(windDirPeakMon)
        self.WDiractMon = windDirPr.create_monitor(cbMonServant, desc)  # attenzione l'oggetto property e' stato creato dalla funzione exec
        self.WDiractMon.set_timer_trigger(TIMESTEP * 10000000)
        self.connect(windDirPeakMon, Qt.SIGNAL("PropertyChanghed(float)"), self.winddirw.setVal)

        temperatureMon = PropertyMonitor(self.plotwidget, "Temperature:")
        cbMonServant = self.simpleClient.activateOffShoot(temperatureMon)
        self.temperatureactMon = tempPr.create_monitor(cbMonServant, desc) #attenzione l'oggetto property e' stato creato dalla funzione exec
        self.temperatureactMon.set_timer_trigger(TIMESTEP * 10000000 * 4)
        self.connect(temperatureMon, Qt.SIGNAL("PropertyChanghed(float)"), self.temperaturew.setVal)

    def __del__(self):
        self.simpleClient.disconnect()


def make():
    try:
        plot = PlotProperty("WEATHERSTATION/WeatherStation", "windspeed")
    except ClientErrorsImpl.CouldntAccessPropertyExImpl as ex:
        logger.logException(ex)
        sys.exit(-1)

    plot.resize(800, 600)
    plot.run()
    plot.show()
    return plot


def main(args):
    app = Qt.QApplication(args)
    plot = make()

    #demo, plot = make(args)

    #w = my.run()
    sys.exit(app.exec_())
    sleep(2)


if __name__=='__main__':
    main(sys.argv)
