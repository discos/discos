# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'calibrationToolClient.ui'
#
# Created: Wed Jun 29 15:41:47 2011
#      by: PyQt4 UI code generator 4.8.1
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui
from customwidgets import PlotProperty
try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_CalibrationToolDialog(object):
    def setupUi(self, CalibrationToolDialog):
        CalibrationToolDialog.setObjectName(_fromUtf8("CalibrationToolDialog"))
        CalibrationToolDialog.resize(897, 762)
        self.buttonBox = QtGui.QDialogButtonBox(CalibrationToolDialog)
        self.buttonBox.setGeometry(QtCore.QRect(340, 730, 341, 32))
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName(_fromUtf8("buttonBox"))
        self.verticalLayoutWidget = QtGui.QWidget(CalibrationToolDialog)
        self.verticalLayoutWidget.setGeometry(QtCore.QRect(20, 130, 661, 521))
        self.verticalLayoutWidget.setObjectName(_fromUtf8("verticalLayoutWidget"))
        self.verticalLayout = QtGui.QVBoxLayout(self.verticalLayoutWidget)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.qwtPlot_datay = PlotProperty("arrayDataY",self.verticalLayoutWidget)
        self.qwtPlot_datay.setObjectName(_fromUtf8("qwtPlot_datay"))
        self.verticalLayout.addWidget(self.qwtPlot_datay)
        self.qwtPlot_datax = PlotProperty("arrayDataX",self.verticalLayoutWidget)
        self.qwtPlot_datax.setObjectName(_fromUtf8("qwtPlot_datax"))
        self.verticalLayout.addWidget(self.qwtPlot_datax)
        self.label = QtGui.QLabel(CalibrationToolDialog)
        self.label.setGeometry(QtCore.QRect(20, 50, 60, 18))
        self.label.setObjectName(_fromUtf8("label"))
        self.label_2 = QtGui.QLabel(CalibrationToolDialog)
        self.label_2.setGeometry(QtCore.QRect(240, 50, 60, 18))
        self.label_2.setObjectName(_fromUtf8("label_2"))
        self.label_3 = QtGui.QLabel(CalibrationToolDialog)
        self.label_3.setGeometry(QtCore.QRect(460, 50, 60, 18))
        self.label_3.setObjectName(_fromUtf8("label_3"))
        self.plainTextEdit_project = QtGui.QPlainTextEdit(CalibrationToolDialog)
        self.plainTextEdit_project.setGeometry(QtCore.QRect(18, 70, 210, 31))
        self.plainTextEdit_project.setObjectName(_fromUtf8("plainTextEdit_project"))
        self.plainTextEdit_filename = QtGui.QPlainTextEdit(CalibrationToolDialog)
        self.plainTextEdit_filename.setGeometry(QtCore.QRect(457, 70, 221, 31))
        self.plainTextEdit_filename.setObjectName(_fromUtf8("plainTextEdit_filename"))
        self.plainTextEdit_observer = QtGui.QPlainTextEdit(CalibrationToolDialog)
        self.plainTextEdit_observer.setGeometry(QtCore.QRect(240, 70, 211, 31))
        self.plainTextEdit_observer.setObjectName(_fromUtf8("plainTextEdit_observer"))
        self.groupBox = QtGui.QGroupBox(CalibrationToolDialog)
        self.groupBox.setGeometry(QtCore.QRect(690, 130, 181, 201))
        self.groupBox.setAlignment(QtCore.Qt.AlignLeading|QtCore.Qt.AlignLeft|QtCore.Qt.AlignTop)
        self.groupBox.setFlat(False)
        self.groupBox.setCheckable(False)
        self.groupBox.setObjectName(_fromUtf8("groupBox"))
        self.formLayout = QtGui.QFormLayout(self.groupBox)
        self.formLayout.setFieldGrowthPolicy(QtGui.QFormLayout.AllNonFixedFieldsGrow)
        self.formLayout.setObjectName(_fromUtf8("formLayout"))
        self.formLayout_3 = QtGui.QFormLayout()
        self.formLayout_3.setFieldGrowthPolicy(QtGui.QFormLayout.AllNonFixedFieldsGrow)
        self.formLayout_3.setFormAlignment(QtCore.Qt.AlignLeading|QtCore.Qt.AlignLeft|QtCore.Qt.AlignVCenter)
        self.formLayout_3.setHorizontalSpacing(16)
        self.formLayout_3.setObjectName(_fromUtf8("formLayout_3"))
        self.label_4 = QtGui.QLabel(self.groupBox)
        self.label_4.setObjectName(_fromUtf8("label_4"))
        self.formLayout_3.setWidget(0, QtGui.QFormLayout.FieldRole, self.label_4)
        self.doubleSpinBox_hpbw = QtGui.QDoubleSpinBox(self.groupBox)
        self.doubleSpinBox_hpbw.setObjectName(_fromUtf8("doubleSpinBox_hpbw"))
        self.formLayout_3.setWidget(0, QtGui.QFormLayout.LabelRole, self.doubleSpinBox_hpbw)
        self.doubleSpinBox_amplitude = QtGui.QDoubleSpinBox(self.groupBox)
        self.doubleSpinBox_amplitude.setObjectName(_fromUtf8("doubleSpinBox_amplitude"))
        self.formLayout_3.setWidget(1, QtGui.QFormLayout.LabelRole, self.doubleSpinBox_amplitude)
        self.doubleSpinBox_peakOffset = QtGui.QDoubleSpinBox(self.groupBox)
        self.doubleSpinBox_peakOffset.setObjectName(_fromUtf8("doubleSpinBox_peakOffset"))
        self.formLayout_3.setWidget(2, QtGui.QFormLayout.LabelRole, self.doubleSpinBox_peakOffset)
        self.doubleSpinBox_slope = QtGui.QDoubleSpinBox(self.groupBox)
        self.doubleSpinBox_slope.setObjectName(_fromUtf8("doubleSpinBox_slope"))
        self.formLayout_3.setWidget(3, QtGui.QFormLayout.LabelRole, self.doubleSpinBox_slope)
        self.label_5 = QtGui.QLabel(self.groupBox)
        self.label_5.setObjectName(_fromUtf8("label_5"))
        self.formLayout_3.setWidget(1, QtGui.QFormLayout.FieldRole, self.label_5)
        self.label_6 = QtGui.QLabel(self.groupBox)
        self.label_6.setObjectName(_fromUtf8("label_6"))
        self.formLayout_3.setWidget(2, QtGui.QFormLayout.FieldRole, self.label_6)
        self.label_7 = QtGui.QLabel(self.groupBox)
        self.label_7.setObjectName(_fromUtf8("label_7"))
        self.formLayout_3.setWidget(3, QtGui.QFormLayout.FieldRole, self.label_7)
        self.formLayout.setLayout(4, QtGui.QFormLayout.FieldRole, self.formLayout_3)

        self.retranslateUi(CalibrationToolDialog)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("accepted()")), CalibrationToolDialog.accept)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("rejected()")), CalibrationToolDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(CalibrationToolDialog)

    def retranslateUi(self, CalibrationToolDialog):
        CalibrationToolDialog.setWindowTitle(QtGui.QApplication.translate("CalibrationToolDialog", "Calibration Tool Client", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("CalibrationToolDialog", "Project", None, QtGui.QApplication.UnicodeUTF8))
        self.label_2.setText(QtGui.QApplication.translate("CalibrationToolDialog", "Observer", None, QtGui.QApplication.UnicodeUTF8))
        self.label_3.setText(QtGui.QApplication.translate("CalibrationToolDialog", "Filename", None, QtGui.QApplication.UnicodeUTF8))
        self.groupBox.setTitle(QtGui.QApplication.translate("CalibrationToolDialog", "Fit parameters", None, QtGui.QApplication.UnicodeUTF8))
        self.label_4.setText(QtGui.QApplication.translate("CalibrationToolDialog", "HPBW", None, QtGui.QApplication.UnicodeUTF8))
        self.label_5.setText(QtGui.QApplication.translate("CalibrationToolDialog", "Amplitude", None, QtGui.QApplication.UnicodeUTF8))
        self.label_6.setText(QtGui.QApplication.translate("CalibrationToolDialog", "PeakOffset", None, QtGui.QApplication.UnicodeUTF8))
        self.label_7.setText(QtGui.QApplication.translate("CalibrationToolDialog", "Slope", None, QtGui.QApplication.UnicodeUTF8))

from PyQt4 import Qwt5
