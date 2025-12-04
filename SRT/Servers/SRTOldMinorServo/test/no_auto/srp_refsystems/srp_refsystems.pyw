import sys, os
from PyQt4.QtCore import *
from PyQt4.QtGui import *

NUM_OF_AXIS = 6
DECIMALS = 4
LIMIT = 10000.00


class Form(QDialog):

    def __init__(self, parent=None):
        super(Form, self).__init__(parent)
        self.realLabel =  [
            QLabel("<font style='color:rgb(112,23,23); font-weight:bold;'>Z1 (mm)</font>"),
            QLabel("<font style='color:rgb(112,23,23); font-weight:bold;'>Z2 (mm)</font>"),
            QLabel("<font style='color:rgb(112,23,23); font-weight:bold;'>Z3 (mm)</font>"),
            QLabel("<font style='color:rgb(112,23,23); font-weight:bold;'>Y4 (mm)</font>"),
            QLabel("<font style='color:rgb(112,23,23); font-weight:bold;'>Y5 (mm)</font>"),
            QLabel("<font style='color:rgb(112,23,23); font-weight:bold;'>X6 (mm)</font>")
        ]
        self.virtualLabel = [
            QLabel("<font style='color:rgb(50,56,131); font-weight:bold;'>TX (mm)</font>"),
            QLabel("<font style='color:rgb(50,56,131); font-weight:bold;'>TY (mm)</font>"),
            QLabel("<font style='color:rgb(50,56,131); font-weight:bold;'>TZ (mm)</font>"),
            QLabel("<font style='color:rgb(50,56,131); font-weight:bold;'>RX (deg)</font>"),
            QLabel("<font style='color:rgb(50,56,131); font-weight:bold;'>RY (deg)</font>"),
            QLabel("<font style='color:rgb(50,56,131); font-weight:bold;'>RZ (deg)</font>")
        ]

        font = QFont()
        font.setBold(True)
        vpositions = [0] * NUM_OF_AXIS 

        self.realSpinBox = [
            QDoubleSpinBox(), 
            QDoubleSpinBox(), 
            QDoubleSpinBox(), 
            QDoubleSpinBox(), 
            QDoubleSpinBox(), 
            QDoubleSpinBox()
        ]
        for item in self.realSpinBox:
            item.setRange(-LIMIT, LIMIT)
            item.setFont(font)
            item.setDecimals(DECIMALS)

        self.virtualSpinBox = [
            QDoubleSpinBox(), 
            QDoubleSpinBox(), 
            QDoubleSpinBox(), 
            QDoubleSpinBox(), 
            QDoubleSpinBox(), 
            QDoubleSpinBox()
        ]
        for item in self.virtualSpinBox:
            item.setRange(-LIMIT, LIMIT)
            item.setValue(vpositions[0])
            item.setFont(font)
            item.setDecimals(DECIMALS)

        self.r2v = QPushButton("Real Reference System")
        self.r2v.setStyleSheet("color:rgb(112,23,23); font-weight:bold;");
        self.v2r = QPushButton("Virtual Reference System")
        self.v2r.setStyleSheet("color:rgb(50,56,131); font-weight:bold;");

        grid = QGridLayout()
        grid.addWidget(self.r2v, 0, 1)
        grid.addWidget(self.v2r, 0, 2)
        for idx, item in enumerate(self.realLabel):
            grid.addWidget(item, idx + 1, 0)
        for idx, item in enumerate(self.realSpinBox):
            grid.addWidget(item, idx + 1, 1)
        for idx, item in enumerate(self.virtualSpinBox):
            item.setEnabled(False)
            grid.addWidget(item, idx + 1, 2)
        for idx, item in enumerate(self.virtualLabel):
            grid.addWidget(item, idx + 1, 3)

        self.setLayout(grid)

        for ritem, vitem in zip(self.realSpinBox, self.virtualSpinBox):
            self.connect(ritem, SIGNAL("valueChanged(double)"), self.updateVirtualUi)
            self.connect(vitem, SIGNAL("valueChanged(double)"), self.updateRealUi)

        self.connect(self.r2v, SIGNAL("clicked()"), self.real2virtual)
        self.connect(self.v2r, SIGNAL("clicked()"), self.virtual2real)
        self.v2r_pressed = True
        self.r2v_pressed = False
        self.updateRealUi()
        self.v2r_pressed = False
        self.r2v_pressed = True
        self.setWindowTitle("SRP Reference Systems")
    
    def real2virtual(self):
        for ritem, vitem in zip(self.realSpinBox, self.virtualSpinBox):
            vitem.setEnabled(False)
            ritem.setEnabled(True)
            self.r2v_pressed = True 
            self.v2r_pressed = False

    def virtual2real(self):
        for ritem, vitem in zip(self.realSpinBox, self.virtualSpinBox):
            vitem.setEnabled(True)
            ritem.setEnabled(False)
            self.r2v_pressed = False
            self.v2r_pressed = True


    def updateRealUi(self):
        if self.v2r_pressed:
            positions = [ item.value() for item in self.virtualSpinBox ]

            v2r_cmd = "./hexlib/virtual2real"
            for position in positions:
                v2r_cmd += " %s" %position
            put, out = os.popen4(v2r_cmd)
            
            positions = []
            try:
                for line in out.readlines():
                    if line.startswith('d'):
                        positions.append(float(line.split("=")[1].strip()))
            except:
                positions = [0] * NUM_OF_AXIS
            
            for idx, item in enumerate(self.realSpinBox):
                item.setValue(positions[idx])


    def updateVirtualUi(self):
        if self.r2v_pressed:
            positions = [ item.value() for item in self.realSpinBox ]

            r2v_cmd = "./hexlib/real2virtual"
            for position in positions:
                r2v_cmd += " %s" %position
            put, out = os.popen4(r2v_cmd)
            
            positions = []
            try:
                for line in out.readlines():
                    if line.startswith('x'):
                        positions.append(float(line.split("=")[1].strip()))
            except:
                positions = [0] * NUM_OF_AXIS 
            
            for idx, item in enumerate(self.virtualSpinBox):
                item.setValue(positions[idx])

app = QApplication(sys.argv)
form = Form()
form.show()
app.exec_()

