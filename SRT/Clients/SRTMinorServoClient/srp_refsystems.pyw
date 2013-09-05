from Acspy.Clients.SimpleClient import PySimpleClient
from Acspy.Common.Log import acsPrintExcDebug
import ACS, ACS__POA

import sys
from PyQt4.QtCore import *
from PyQt4.QtGui import *

NUM_OF_AXES = 6
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
        self.srp = self.client.getComponent("MINORSERVO/SRP")

        actPos = self.srp._get_actPos()
        actPosCbMonitor = Monitor('actPos') # The name of the property
        actPosCbMonitorServant = self.client.activateOffShoot(actPosCbMonitor)
        actPosDesc = ACS.CBDescIn(0L, 0L, 0L) 
        self.actPosMonitor = actPos.create_monitor(actPosCbMonitorServant, actPosDesc)
        self.actPosMonitor.set_timer_trigger(SAMPLING_TIME) 
        
        actElongation = self.srp._get_actElongation()
        actElongationCbMonitor = Monitor('actElongation')
        actElongationCbMonitorServant = self.client.activateOffShoot(actElongationCbMonitor)
        actElongationDesc = ACS.CBDescIn(0L, 0L, 0L) 
        self.actElongationMonitor = actElongation.create_monitor(actElongationCbMonitorServant, actElongationDesc)
        self.actElongationMonitor.set_timer_trigger(SAMPLING_TIME)
 
        cmdPos = self.srp._get_cmdPos()
        cmdPosCbMonitor = Monitor('cmdPos')
        cmdPosCbMonitorServant = self.client.activateOffShoot(cmdPosCbMonitor)
        cmdPosDesc = ACS.CBDescIn(0L, 0L, 0L) 
        self.cmdPosMonitor = cmdPos.create_monitor(cmdPosCbMonitorServant, cmdPosDesc)
        self.cmdPosMonitor.set_timer_trigger(SAMPLING_TIME)
 
        posDiff = self.srp._get_posDiff()
        posDiffCbMonitor = Monitor('posDiff')
        posDiffCbMonitorServant = self.client.activateOffShoot(posDiffCbMonitor)
        posDiffDesc = ACS.CBDescIn(0L, 0L, 0L) 
        self.posDiffMonitor = posDiff.create_monitor(posDiffCbMonitorServant, posDiffDesc)
        self.posDiffMonitor.set_timer_trigger(SAMPLING_TIME)

        super(Form, self).__init__(parent)
        self.realLabel =  [
            QLabel("<font style='color:rgb(80,23,23); font-weight:bold;'>RRS</font>"),
            QLabel("<font style='color:rgb(122,23,23); font-weight:bold;'>Z1 (mm)</font>"),
            QLabel("<font style='color:rgb(122,23,23); font-weight:bold;'>Z2 (mm)</font>"),
            QLabel("<font style='color:rgb(122,23,23); font-weight:bold;'>Z3 (mm)</font>"),
            QLabel("<font style='color:rgb(122,23,23); font-weight:bold;'>Y4 (mm)</font>"),
            QLabel("<font style='color:rgb(122,23,23); font-weight:bold;'>Y5 (mm)</font>"),
            QLabel("<font style='color:rgb(122,23,23); font-weight:bold;'>X6 (mm)</font>")
        ]
        self.virtualLabel = [
            QLabel("<font style='color:rgb(25,35,85); font-weight:bold;'>VRS</font>"),
            QLabel("<font style='color:rgb(50,56,131); font-weight:bold;'>TX (mm)</font>"),
            QLabel("<font style='color:rgb(50,56,131); font-weight:bold;'>TY (mm)</font>"),
            QLabel("<font style='color:rgb(50,56,131); font-weight:bold;'>TZ (mm)</font>"),
            QLabel("<font style='color:rgb(50,56,131); font-weight:bold;'>RX (deg)</font>"),
            QLabel("<font style='color:rgb(50,56,131); font-weight:bold;'>RY (deg)</font>"),
            QLabel("<font style='color:rgb(50,56,131); font-weight:bold;'>RZ (deg)</font>")
        ]
        
        self.elongationLabels =  []
        self.actPosLabels =  []
        self.cmdPosLabels =  []
        self.posDiffLabels =  []
        for i in range(NUM_OF_AXES):
            # Elongations
            item = QLabel("<font style='color:black; font-weight:bold;'>0.0</font>")
            item.setFrameStyle(QFrame.StyledPanel);
            item.setStyleSheet("background-color: rgb(243,217,210)");
            self.elongationLabels.append(item)
            # Actual Position
            item = QLabel("<font style='color:black; font-weight:bold;'>0.0</font>")
            item.setFrameStyle(QFrame.StyledPanel);
            item.setStyleSheet("background-color: rgb(206,224,223)");
            self.actPosLabels.append(item)
            # Commanded Position
            item = QLabel("<font style='color:black; font-weight:bold;'>0.0</font>")
            item.setFrameStyle(QFrame.StyledPanel);
            item.setStyleSheet("background-color: rgb(206,224,223)");
            self.cmdPosLabels.append(item)
            # Position Difference
            item = QLabel("<font style='color:black; font-weight:bold;'>0.0</font>")
            item.setFrameStyle(QFrame.StyledPanel);
            item.setStyleSheet("background-color: rgb(206,224,223)");
            self.posDiffLabels.append(item)

        self.actElongationTitle = QLabel("<font style='color:black; font-weight:bold;'>Actual Elongation</font>")
        self.actElongationTitle.setFrameStyle(QFrame.StyledPanel);
        self.actElongationTitle.setStyleSheet("background-color: rgb(249,184,184)");
        self.actPosTitle = QLabel("<font style='color:black; font-weight:bold;'>Actual Position</font>")
        self.actPosTitle.setFrameStyle(QFrame.StyledPanel);
        self.actPosTitle.setStyleSheet("background-color: rgb(178,198,213)");
        self.cmdPosTitle = QLabel("<font style='color:black; font-weight:bold;'>Commanded Position</font>")
        self.cmdPosTitle.setFrameStyle(QFrame.StyledPanel);
        self.cmdPosTitle.setStyleSheet("background-color: rgb(178,198,213)");
        self.posDiffTitle = QLabel("<font style='color:black; font-weight:bold;'>Position Difference</font>")
        self.posDiffTitle.setFrameStyle(QFrame.StyledPanel);
        self.posDiffTitle.setStyleSheet("background-color: rgb(178,198,213)");

        grid = QGridLayout()
        grid.addWidget(self.actElongationTitle, 0, 1)
        grid.addWidget(self.actPosTitle, 0, 2)
        grid.addWidget(self.cmdPosTitle, 0, 3)
        grid.addWidget(self.posDiffTitle, 0, 4)
        for idx, item in enumerate(self.realLabel):
            grid.addWidget(item, idx, 0)
        for idx, item in enumerate(self.elongationLabels):
            grid.addWidget(item, idx + 1, 1)
        for idx, item in enumerate(self.actPosLabels):
            grid.addWidget(item, idx + 1, 2)
        for idx, item in enumerate(self.cmdPosLabels):
            grid.addWidget(item, idx + 1, 3)
        for idx, item in enumerate(self.posDiffLabels):
            grid.addWidget(item, idx + 1, 4)
        for idx, item in enumerate(self.virtualLabel):
            grid.addWidget(item, idx, 5)

        self.setLayout(grid)

        self.connect(actPosCbMonitor, SIGNAL("actPosChanged(PyQt_PyObject)"), self.updateActPosUi)
        self.connect(actElongationCbMonitor, SIGNAL("actElongationChanged(PyQt_PyObject)"), self.updateActElongationUi)
        self.connect(cmdPosCbMonitor, SIGNAL("cmdPosChanged(PyQt_PyObject)"), self.updateCmdPosUi)
        self.connect(posDiffCbMonitor, SIGNAL("posDiffChanged(PyQt_PyObject)"), self.updatePosDiffUi)

        self.setWindowTitle("SRP Reference Systems")

    def __del__(self):
        self.actPosMonitor.destroy()
        self.actElongationMonitor.destroy()
        self.cmdPosMonitor.destroy()
        self.posDiffMonitor.destroy()
        self.client.releaseComponent("MINORSERVO/SRP")
        self.client.disconnect()

    def updateActElongationUi(self, value):
            for idx, item in enumerate(self.elongationLabels):
                item.setText("<font style='color:black; font-weight:bold;'>%0.4f</font>" %value[idx])
    
    def updateActPosUi(self, value):
            for idx, item in enumerate(self.actPosLabels):
                item.setText("<font style='color:black; font-weight:bold;'>%0.4f</font>" %value[idx])

    def updateCmdPosUi(self, value):
            for idx, item in enumerate(self.cmdPosLabels):
                item.setText("<font style='color:black; font-weight:bold;'>%0.4f</font>" %value[idx])

    def updatePosDiffUi(self, value):
            for idx, item in enumerate(self.posDiffLabels):
                item.setText("<font style='color:black; font-weight:bold;'>%0.4f</font>" %value[idx])


if __name__ == '__main__':
    app = QApplication(sys.argv)
    form = Form()
    form.show()
    app.exec_()

