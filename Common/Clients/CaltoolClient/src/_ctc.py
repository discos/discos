#! /usr/bin/env python
# -*- coding: utf-8 -*-
from __future__ import print_function
from Acspy.Clients.SimpleClient import PySimpleClient
import Acspy.Common.Err
import maciErrType
import maciErrTypeImpl
import ClientErrorsImpl
import ACSLog


import ACS, ACS__POA                                  # Import the Python CORBA stubs for BACI
from PyQt4 import Qt
from PyQt4.QtCore import pyqtSlot,QThread,QMutex,QTimer
import PyQt4.Qwt as Qwt
import sys,getopt,os
from time import sleep
import math
from IRAPy import logger,userLogger



import calibrationtool_ui # file generated by pyuic4 but modified to include custom widgets.

__version__ = '$Id'


'''
@version $Id$
'''

DEFAULT_COMPONENT="MANAGEMENT/CalibrationTool"

class MyWorker(QThread):
    def __init__(self,component,parent=None):
        self.Mutex =QMutex()
        QThread.__init__(self,parent)
        self.caltool=component[0]
        self.scheduler=component[1]
        self.boss     =component[2]
        self.simpleClient=component[3]
        self.run=True;
        self.scanAxis = self.caltool._get_scanAxis()

        self.arrayDataY=self.caltool._get_arrayDataY()
        self.arrayDataX=self.caltool._get_arrayDataX()
        self.dataX=self.caltool._get_dataX()
        self.dataY=self.caltool._get_dataY()
        #self.datax_tmp=[0 for i in len(self_data)]
        self.datay_tmp=[]
        self.datax_tmp=[]
        self.projectname=self.caltool._get_projectName()
        self.observer   =self.caltool._get_observer()
        self.filename   =self.caltool._get_fileName()
        self.deviceID   =self.caltool._get_deviceID()
        self.subscan    =self.scheduler._get_subScanID()
        self.scan       =self.scheduler._get_scanID()
        self.hpbw       =self.caltool._get_hpbw()
        self.amplitude  =self.caltool._get_amplitude()
        self.peakOffset  =self.caltool._get_peakOffset()
        self.slope       =self.caltool._get_slope()
        self.offset       =self.caltool._get_offset()
        self.name        =self.boss._get_target()
        self.device      =self.caltool._get_deviceID()
        self.oldscan=0
        self.newscan=0
        self.oldsubscan=0
        self.newsubscan=0
        self.oldrecordingstatus=False
        self.newrecordingstatus=False
        self.azimuthOffset=self.boss._get_azimuthOffset()
        self.elevationOffset=self.boss._get_elevationOffset()


    def run (self):
        try:
            #arrayDataX=self.componenselft._get_arrayDataX()
            #arrayDataY=self.component._get_arrayDataY()
            while self.run:
                (scanaxis,compl)=self.scanAxis.get_sync()
                #self.scanAx
                (arraydatax,compl)=self.arrayDataX.get_sync()
                if 'SUBR' not in str(scanaxis):
                    arraydatax[:]=[x /math.pi*180. for x in arraydatax]
                self.emit(Qt.SIGNAL("arrayDataX"),arraydatax)
                (arraydatay,compl2)=self.arrayDataY.get_sync()
                self.emit(Qt.SIGNAL("arrayDataY"),arraydatay)
                (datay,compl3)=self.dataY.get_sync()
                (datax,compl3)=self.dataX.get_sync()
                if 'SUBR' not in str(scanaxis):
                    datax=datax/math.pi*180.
                (projectname,compl4)=self.projectname.get_sync()
                self.emit(Qt.SIGNAL("projectname"),projectname)
                (observername,compl5)=self.observer.get_sync()
                self.emit(Qt.SIGNAL("observer"),observername)
                (filename,compl6)=self.filename.get_sync()
                self.emit(Qt.SIGNAL("filename"),filename)
                (subscanid,compl7)=self.subscan.get_sync()
                self.emit(Qt.SIGNAL("subscan"),str(subscanid))
                (scanid,compl8)=self.scan.get_sync()
                self.emit(Qt.SIGNAL("scan"),str(scanid))
                (hpbw,compl9)=self.hpbw.get_sync()
                if 'SUBR' not in str(scanaxis):
                    hpbw= hpbw/math.pi*180*60
                self.emit(Qt.SIGNAL("hpbw"),("%5.3f" % (hpbw)))
                (amplitude,compl10)=self.amplitude.get_sync()
                self.emit(Qt.SIGNAL("amplitude"),"%5.3f" % amplitude)
                (peakOffset,compl11)=self.peakOffset.get_sync()
                if 'SUBR' not in str(scanaxis):
                    peakOffset=peakOffset/math.pi*180
                self.emit(Qt.SIGNAL("peakOffset"),"%5.3f" % (peakOffset))
                (slope,compl12)=self.slope.get_sync()
                self.emit(Qt.SIGNAL("slope"),"%5.3f" %slope)
                (offset,compl13)=self.offset.get_sync()
                self.emit(Qt.SIGNAL("offset"),"%5.3f" % offset)
                (target,compl13)=self.name.get_sync()
                self.emit(Qt.SIGNAL("target"),target)
                (device,compl13)=self.device.get_sync()
                self.emit(Qt.SIGNAL("device"),str(device))
                self.emit(Qt.SIGNAL("scanAxis"),scanaxis)
                (azOffset,_)=self.azimuthOffset.get_sync()
                (elOffset,_)=self.elevationOffset.get_sync()
                self.emit(Qt.SIGNAL("azoffset"),f'{azOffset/math.pi*180:.3f}')
                self.emit(Qt.SIGNAL("eloffset"),f'{elOffset/math.pi*180:.3f}')

                rec= self.caltool.isRecording()
                if rec==True:
                   self.datay_tmp.append(datay)
                   self.datax_tmp.append(datax)
                   self.emit(Qt.SIGNAL("DataY"),self.datay_tmp)
                   self.emit(Qt.SIGNAL("DataX"),self.datax_tmp)

                self.emit(Qt.SIGNAL("isRecording"),rec)
                self.emit(Qt.SIGNAL("scanAxis"),scanaxis)

                #print(rec)
                #if (subscanid!=self.oldsubscan and scanid !=self.oldscanid):
                    #print("scan changed",self.oldsubscan,subscanid)
                    #self.oldsubscan=subscanid
                    #self.subscan=subscanid
                    #self.datax_tmp=[]
                    #self.datay_tmp=[]

                if (rec!=self.oldrecordingstatus):
                    self.oldrecordingstatus=rec
                    self.datax_tmp=[]
                    self.datay_tmp=[]

                QThread.msleep(200)
            print("Exited from thread")

        except Exception as ex:
            newEx = ClientErrorsImpl.CouldntAccessPropertyExImpl(exception=ex, create=1)
            newEx.setPropertyName("")
            #ACS_LOG_ERROR
            newEx.log(self.simpleClient.getLogger(),ACSLog.ACS_LOG_ERROR)
            self.run=False

    def __del__(self):
        QThread.msleep(200)


class Application(Qt.QDialog,calibrationtool_ui.Ui_CalibrationToolDialog):

    def __init__(self,compname,parent=None):

        self.scheduler=None
        self.antennaBoss=None
        self.component=None
        self.componentName=DEFAULT_COMPONENT
        self.managerConnected=False
        self.thread=None

        try:
            self.simpleClient = PySimpleClient()
            self.managerConnected=True
        except Exception as ex:
            newEx = ClientErrorsImpl.CouldntLogManagerExImpl(exception=ex, create=1)
            logger.logException(newEx)
            print("Please check the system is up and running......")
            sys.exit(-1)

        try:
            self.scheduler= self.simpleClient.getDefaultComponent("IDL:alma/Management/Scheduler:1.0")
        except Exception as ex:
            newEx = ClientErrorsImpl.CouldntAccessComponentExImpl(exception=ex, create=1)
            newEx.setComponentName("IDL:alma/Management/Scheduler:1.0")
            logger.logException(newEx)
            print("Please check the system is up and running and scheduler component is alive!")
            sys.exit(-1)

        try:
            self.antennaBoss =self.simpleClient.getDefaultComponent("IDL:alma/Antenna/AntennaBoss:1.0")
        except Exception as ex:
            newEx = ClientErrorsImpl.CouldntAccessComponentExImpl(exception=ex, create=1)
            newEx.setComponentName("IDL:alma/Antenna/AntennaBoss:1.0")
            logger.logException(newEx)
            print("Please check the system is up and running and antenna boss is alive!")
            sys.exit(-1)

        #choose default recorder
        if compname=='default':
            try:
                recorder=self.scheduler._get_currentRecorder()
                (recordername,compl)=recorder.get_sync()
            except Exception as ex:
                newEx = ClientErrorsImpl.CouldntAccessPropertyExImpl(exception=ex, create=1)
                logger.logException(newEx)
                print("Please check scheduler component is alive and responsive|")
                sys.exit(-1)

            print("Starting with default component: " + DEFAULT_COMPONENT)
            if recordername!=DEFAULT_COMPONENT:
                print("Be aware that the in-use recorder is currently " + recordername)

            self.componentName=DEFAULT_COMPONENT

        else:
            self.componentName=compname
            print("Starting with component: " + self.componentName)

        try:
            self.component= self.simpleClient.getComponent(self.componentName)
        except Exception as ex:
            newEx = ClientErrorsImpl.CouldntAccessComponentExImpl(exception=ex, create=1)
            newEx.setComponentName(self.componentName)
            logger.logException(newEx)
            print("Please check the system is up and running and " + self.componentName + " is alive!")
            sys.exit(-1)

        self.thread=MyWorker([self.component,self.scheduler,self.antennaBoss,self.simpleClient])
        Qt.QDialog.__init__(self)
        self.setupUi(self)
        #self.qwtPlot_datax.setAxisScale(Qwt.QwtPlot.xBottom, 0,1000)
        self.qwtPlot_datax.setAxisAutoScale(Qwt.QwtPlot. yLeft)
        self.setWindowTitle(self.componentName)
        self.qwtPlot_datax.setAxisTitle(Qwt.QwtPlot.yLeft, "Ta(K)")
        self.qwtPlot_datay.setAxisTitle(Qwt.QwtPlot.yLeft, "Ta(K)")

        self.connect(self.thread,Qt.SIGNAL("arrayDataX"),self.qwtPlot_datay.setX)
        self.connect(self.thread,Qt.SIGNAL("arrayDataY"),self.qwtPlot_datay.setVal)

        self.connect(self.thread,Qt.SIGNAL("DataX"),self.qwtPlot_datax.setX)
        self.connect(self.thread,Qt.SIGNAL("DataY"),self.qwtPlot_datax.setVal)

        self.connect(self.thread,Qt.SIGNAL("projectname"),self.plainTextEdit_project.setPlainText)
        self.connect(self.thread,Qt.SIGNAL("observer"),self.plainTextEdit_observer.setPlainText)
        self.connect(self.thread,Qt.SIGNAL("filename"),self.plainTextEdit_filename.setPlainText)
        self.connect(self.thread,Qt.SIGNAL("subscan"),self.subscanIdLineEdit.setText)
        self.connect(self.thread,Qt.SIGNAL("scan"),self.scanIdLineEdit.setText)
        self.connect(self.thread,Qt.SIGNAL("hpbw"),self.hpbwLineEdit.setText)
        self.connect(self.thread,Qt.SIGNAL("amplitude"),self.amplitudeLineEdit.setText)
        self.connect(self.thread,Qt.SIGNAL("peakOffset"),self.peakoffsetLineEdit.setText)
        self.connect(self.thread,Qt.SIGNAL("slope"),self.slopeLineEdit.setText)
        self.connect(self.thread,Qt.SIGNAL("offset"),self.offsetLineEdit.setText)
        self.connect(self.thread,Qt.SIGNAL("target"),self.nameLineEdit.setText)
        self.connect(self.thread,Qt.SIGNAL("device"),self.deviceIdLineEdit.setText)
        self.connect(self.thread,Qt.SIGNAL("isRecording"),self.isRecording)
        self.connect(self.thread,Qt.SIGNAL("scanAxis"),self.scanAxis)
        self.connect(self.thread,Qt.SIGNAL("eloffset"),self.elOffsetlineEdit.setText)
        self.connect(self.thread,Qt.SIGNAL("azoffset"),self.azOffsetlineEdit.setText)

    @pyqtSlot(Qt.QObject,name="isRecording")
    def isRecording(self,rec):
        if rec==False:
            self.recording.setText("OFF")
            palette = self.recording.palette()
            role = self.recording.backgroundRole()
            palette.setColor(role, Qt.QColor('gray'))
            self.recording.setPalette(palette)
            self.BScanaxis.setEnabled(False)
        if rec==True:
            self.recording.setText("ON")
            palette = self.recording.palette()
            role = self.recording.backgroundRole()
            palette.setColor(role, Qt.QColor('green'))
            self.recording.setPalette(palette)
            self.BScanaxis.setEnabled(True)

    @pyqtSlot(Qt.QObject,name="scanAxis")  # decorator for the slot
    def scanAxis(self,scanaxis):
        self.BScanaxis.setText(str(scanaxis))
        if 'SUBR' not in str(scanaxis):
            self.scanAxisLabel.setText('ScanAxis - Pointing')
            xaxis_text="Direction (Deg)"
            hpbw_label_text='HPBW (arcmim)'
            peakOffsetLabel_text='PeakOffset (deg)'
        else:
            self.scanAxisLabel.setText('ScanAxis - Focus')
            xaxis_text="Distance (mm)"
            hpbw_label_text='HPBW (arcmin)'
            peakOffsetLabel_text='PeakOffset (mm)'
            self.qwtPlot_datax.setAxisTitle(Qwt.QwtPlot.xBottom, xaxis_text)
            self.qwtPlot_datay.setAxisTitle(Qwt.QwtPlot.xBottom, xaxis_text)
            self.hpbw_label.setText(hpbw_label_text)
            self.peakOffsetLabel.setText(peakOffsetLabel_text)

    @pyqtSlot(Qt.QObject,name="scalePlots")  # decorator for the slot
    def scalePlots(self,val):
        self.qwtPlot_datay.setAxisScale(QwtPlot.xBottom, min(val), max(val))

    def run(self):
        self.thread.start()

    def __del__(self):
        #__del__ apparently is called twice, I have no clue and it should be investigated
        if self.thread:
            if self.thread.isRunning:
                self.thread.quit()

        if self.managerConnected:
            try:
                if self.component:
                    self.simpleClient.releaseComponent(self.component._get_name())
                if self.scheduler:
                    self.simpleClient.releaseComponent(self.scheduler._get_name())
                if self.antennaBoss:
                    self.simpleClient.releaseComponent(self.antennaBoss._get_name())
            except Exception as ex:
                print("Error in application cleanup")


def usage():
    print("calibrationtoolclient [component name]")
    print()
    print("If no component name is provided, the default MANAGEMENT/CalibrationTool will be used")

def main(args):
    sys.tracebacklimit=0

    try:
        opts, args = getopt.getopt(sys.argv[1:],"h",["help"])
    except getopt.GetoptError as err:
        print(str(err))
        usage()
        sys.exit(1)

    for o,a in opts:
        if o in ("-h", "--help"):
            usage()
            sys.exit()

    if len(args)==0:
        componentname='MANAGEMENT/CalibrationTool'
    else:
        componentname=args[0]

    app = Qt.QApplication(args)
    a=Application(componentname) #passa il nome del component al costruttore
    sleep(1)
    a.run()
    p=a.show()
    sys.exit(app.exec_())
    sleep(2)
    print("Application closed!")


if __name__=='__main__':
    main(sys.argv)
