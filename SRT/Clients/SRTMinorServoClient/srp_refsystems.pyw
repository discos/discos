from Acspy.Clients.SimpleClient import PySimpleClient
from Acspy.Common.Log import acsPrintExcDebug
import ACS, ACS__POA

import sys
from PyQt4.QtCore import *
from PyQt4.QtGui import *

SAMPLING_TIME = 10000000 # 10 ** 7 is 1 second


class Monitor(ACS__POA.CBdoubleSeq, QObject):

    def __init__(self, name):
        """The `name` argument must be the property name."""
        self.property_name = name
        QObject.__init__(self)
    def working(self, value, completion, desc):
        self.emit(SIGNAL(self.property_name + "Changed(PyQt_PyObject)"), value)

    def done(self, value, completion, desc):
        pass

    def negotiate (self, time_to_transmit, desc):
        return True


class Form(QDialog):

    def __init__(self, parent=None):

        self.client = PySimpleClient()
        self.components = [
                ('SRP', self.client.getComponent("MINORSERVO/SRP")),
                ('PFP', self.client.getComponent("MINORSERVO/PFP")),
                ('GFR', self.client.getComponent("MINORSERVO/GFR")),
                ('M3R', self.client.getComponent("MINORSERVO/M3R")),
        ]

        self.actPosMonitors = {} # A dict of actPos monitors: {comp_name: monitor}
        self.actPosCbMonitors = {} # A dict of actPos Call back monitors: {comp_name: monitor}
        self.cmdPosMonitors = {} # A dict of cmdPos monitors: one item for each servo: {comp_name: monitor}
        self.cmdPosCbMonitors = {} # A dict of cmdPos Call back monitors: {comp_name: monitor}
        self.posDiffMonitors = {} # A dict of posDiff monitors: one item for each servo: {comp_name: monitor}
        self.posDiffCbMonitors = {} # A dict of posDiff Call back monitors: {comp_name: monitor}
        self.actPosTitles = {}
        self.cmdPosTitles = {}
        self.posDiffTitles = {}
        for comp_name, comp in self.components:
            actPos = comp._get_actPos()
            actPosCbMonitor = Monitor(comp_name + 'actPos') 
            actPosCbMonitorServant = self.client.activateOffShoot(actPosCbMonitor)
            actPosDesc = ACS.CBDescIn(0L, 0L, 0L) 
            actPosMonitor = actPos.create_monitor(actPosCbMonitorServant, actPosDesc)
            actPosMonitor.set_timer_trigger(SAMPLING_TIME) 
            self.actPosCbMonitors[comp_name] = actPosCbMonitor
            self.actPosMonitors[comp_name] = actPosMonitor

            cmdPos = comp._get_cmdPos()
            cmdPosCbMonitor = Monitor(comp_name + 'cmdPos') 
            cmdPosCbMonitorServant = self.client.activateOffShoot(cmdPosCbMonitor)
            cmdPosDesc = ACS.CBDescIn(0L, 0L, 0L) 
            cmdPosMonitor = cmdPos.create_monitor(cmdPosCbMonitorServant, cmdPosDesc)
            cmdPosMonitor.set_timer_trigger(SAMPLING_TIME) 
            self.cmdPosCbMonitors[comp_name] = cmdPosCbMonitor
            self.cmdPosMonitors[comp_name] = cmdPosMonitor

            posDiff = comp._get_posDiff()
            posDiffCbMonitor = Monitor(comp_name + 'posDiff') 
            posDiffCbMonitorServant = self.client.activateOffShoot(posDiffCbMonitor)
            posDiffDesc = ACS.CBDescIn(0L, 0L, 0L) 
            posDiffMonitor = posDiff.create_monitor(posDiffCbMonitorServant, posDiffDesc)
            posDiffMonitor.set_timer_trigger(SAMPLING_TIME) 
            self.posDiffCbMonitors[comp_name] = posDiffCbMonitor
            self.posDiffMonitors[comp_name] = posDiffMonitor
        
        super(Form, self).__init__(parent)
        self._order = ('SRP', 'PFP', 'GFR', 'M3R')
        self.labels = {
                'SRP': [
                     QLabel("<font style='color:black; font-weight:bold;'>SRP</font>"),
                     QLabel("<font style='color:rgb(50,56,131); font-weight:bold;'>TX (mm)</font>"),
                     QLabel("<font style='color:rgb(50,56,131); font-weight:bold;'>TY (mm)</font>"),
                     QLabel("<font style='color:rgb(50,56,131); font-weight:bold;'>TZ (mm)</font>"),
                     QLabel("<font style='color:rgb(50,56,131); font-weight:bold;'>RX (deg)</font>"),
                     QLabel("<font style='color:rgb(50,56,131); font-weight:bold;'>RY (deg)</font>"),
                     QLabel("<font style='color:rgb(50,56,131); font-weight:bold;'>RZ (deg)</font>")
                ],
                'PFP': [
                     QLabel("<font style='color:black; font-weight:bold;'>PFP</font>"),
                     QLabel("<font style='color:rgb(50,56,131); font-weight:bold;'>RY (mm)</font>"),
                     QLabel("<font style='color:rgb(50,56,131); font-weight:bold;'>TX (mm)</font>"),
                     QLabel("<font style='color:rgb(50,56,131); font-weight:bold;'>TZ (mm)</font>"),
                ],
                'GFR': [
                     QLabel("<font style='color:black; font-weight:bold;'>GFR</font>"),
                     QLabel("<font style='color:rgb(50,56,131); font-weight:bold;'>RZ (mm)</font>"),
                ],
                'M3R': [
                     QLabel("<font style='color:black; font-weight:bold;'>M3R</font>"),
                     QLabel("<font style='color:rgb(50,56,131); font-weight:bold;'>RZ (mm)</font>"),
                ]
        }
        for qlabels in self.labels.values():
            qlabels[0].setFrameStyle(QFrame.StyledPanel);
            qlabels[0].setStyleSheet("background-color: rgb(104,198,66)");
        

        self.actPosLabels =  {}
        self.cmdPosLabels =  {}
        self.posDiffLabels =  {}
        grid = QGridLayout()
        row = 0
        for comp_name in self._order:
            # Titles
            actPosTitle = QLabel("<font style='color:black; font-weight:bold;'>Actual Postion</font>")
            actPosTitle.setFrameStyle(QFrame.StyledPanel);
            actPosTitle.setStyleSheet("background-color: rgb(104,198,66)");
            self.actPosTitles[comp_name] = actPosTitle
            cmdPosTitle = QLabel("<font style='color:black; font-weight:bold;'>Commanded Position</font>")
            cmdPosTitle.setFrameStyle(QFrame.StyledPanel);
            cmdPosTitle.setStyleSheet("background-color: rgb(104,198,66)");
            self.cmdPosTitles[comp_name] = cmdPosTitle
            posDiffTitle = QLabel("<font style='color:black; font-weight:bold;'>Position Difference</font>")
            posDiffTitle.setFrameStyle(QFrame.StyledPanel);
            posDiffTitle.setStyleSheet("background-color: rgb(104,198,66)");
            self.posDiffTitles[comp_name] = posDiffTitle
            # Labels
            labels = self.labels[comp_name]
            self.actPosLabels[comp_name] = []
            self.cmdPosLabels[comp_name] = []
            self.posDiffLabels[comp_name] = []
            for label in labels:
                # Actual Position
                item = QLabel("<font style='color:black; font-weight:bold;'>0.0</font>")
                item.setFrameStyle(QFrame.StyledPanel);
                item.setStyleSheet("background-color: rgb(206,224,223)");
                self.actPosLabels[comp_name].append(item)
                # Commanded Position
                item = QLabel("<font style='color:black; font-weight:bold;'>0.0</font>")
                item.setFrameStyle(QFrame.StyledPanel);
                item.setStyleSheet("background-color: rgb(206,224,223)");
                self.cmdPosLabels[comp_name].append(item)
                # Position Difference
                item = QLabel("<font style='color:black; font-weight:bold;'>0.0</font>")
                item.setFrameStyle(QFrame.StyledPanel);
                item.setStyleSheet("background-color: rgb(206,224,223)");
                self.posDiffLabels[comp_name].append(item)

            grid.addWidget(self.actPosTitles[comp_name], row, 1)
            grid.addWidget(self.cmdPosTitles[comp_name], row, 2)
            grid.addWidget(self.posDiffTitles[comp_name], row, 3)
            for idx, item in enumerate(self.labels[comp_name]):
                grid.addWidget(item, idx + row, 0)
            for idx, item in enumerate(self.actPosLabels[comp_name][1:]):
                grid.addWidget(item, idx + row + 1, 1)
            for idx, item in enumerate(self.cmdPosLabels[comp_name][1:]):
                grid.addWidget(item, idx + row + 1, 2)
            for idx, item in enumerate(self.posDiffLabels[comp_name][1:]):
                grid.addWidget(item, idx + row + 1, 3)
            row += len(self.labels[comp_name])

        self.setLayout(grid)
        for comp_name, monitor in self.actPosCbMonitors.items():
            method = getattr(self, 'updateActPos%sUi' %comp_name)
            self.connect(monitor, SIGNAL(comp_name + "actPosChanged(PyQt_PyObject)"), method)
        for comp_name, monitor in self.cmdPosCbMonitors.items():
            method = getattr(self, 'updateCmdPos%sUi' %comp_name)
            self.connect(monitor, SIGNAL(comp_name + "cmdPosChanged(PyQt_PyObject)"), method)
        for comp_name, monitor in self.posDiffCbMonitors.items():
            method = getattr(self, 'updatePosDiff%sUi' %comp_name)
            self.connect(monitor, SIGNAL(comp_name + "posDiffChanged(PyQt_PyObject)"), method)

        self.setWindowTitle("Minor Servo System Monitoring")

    def __del__(self):
        for monitor in self.actPosMonitors.values():
            monitor.destroy()
        for monitor in self.cmdPosMonitors.values():
            monitor.destroy()
        for monitor in self.posDiffMonitors.values():
            monitor.destroy()
        self.client.releaseComponent("MINORSERVO/SRP")
        self.client.releaseComponent("MINORSERVO/PFP")
        self.client.releaseComponent("MINORSERVO/GFR")
        self.client.releaseComponent("MINORSERVO/M3R")
        self.client.disconnect()

    def updateActPosSRPUi(self, value):
        for idx, item in enumerate(self.actPosLabels['SRP'][1:]):
            item.setText("<font style='color:black; font-weight:bold;'>%0.4f</font>" %value[idx])
    def updateActPosPFPUi(self, value):
        for idx, item in enumerate(self.actPosLabels['PFP'][1:]):
            item.setText("<font style='color:black; font-weight:bold;'>%0.4f</font>" %value[idx])
    def updateActPosGFRUi(self, value):
        for idx, item in enumerate(self.actPosLabels['GFR'][1:]):
            item.setText("<font style='color:black; font-weight:bold;'>%0.4f</font>" %value[idx])
    def updateActPosM3RUi(self, value):
        for idx, item in enumerate(self.actPosLabels['M3R'][1:]):
            item.setText("<font style='color:black; font-weight:bold;'>%0.4f</font>" %value[idx])
    def updateCmdPosSRPUi(self, value):
        for idx, item in enumerate(self.cmdPosLabels['SRP'][1:]):
            item.setText("<font style='color:black; font-weight:bold;'>%0.4f</font>" %value[idx])
    def updateCmdPosPFPUi(self, value):
        for idx, item in enumerate(self.cmdPosLabels['PFP'][1:]):
            item.setText("<font style='color:black; font-weight:bold;'>%0.4f</font>" %value[idx])
    def updateCmdPosGFRUi(self, value):
        for idx, item in enumerate(self.cmdPosLabels['GFR'][1:]):
            item.setText("<font style='color:black; font-weight:bold;'>%0.4f</font>" %value[idx])
    def updateCmdPosM3RUi(self, value):
        for idx, item in enumerate(self.cmdPosLabels['M3R'][1:]):
            item.setText("<font style='color:black; font-weight:bold;'>%0.4f</font>" %value[idx])
    def updatePosDiffSRPUi(self, value):
        for idx, item in enumerate(self.posDiffLabels['SRP'][1:]):
            item.setText("<font style='color:black; font-weight:bold;'>%0.4f</font>" %value[idx])
    def updatePosDiffPFPUi(self, value):
        for idx, item in enumerate(self.posDiffLabels['PFP'][1:]):
            item.setText("<font style='color:black; font-weight:bold;'>%0.4f</font>" %value[idx])
    def updatePosDiffGFRUi(self, value):
        for idx, item in enumerate(self.posDiffLabels['GFR'][1:]):
            item.setText("<font style='color:black; font-weight:bold;'>%0.4f</font>" %value[idx])
    def updatePosDiffM3RUi(self, value):
        for idx, item in enumerate(self.posDiffLabels['M3R'][1:]):
            item.setText("<font style='color:black; font-weight:bold;'>%0.4f</font>" %value[idx])

if __name__ == '__main__':
    app = QApplication(sys.argv)
    form = Form()
    form.show()
    app.exec_()

