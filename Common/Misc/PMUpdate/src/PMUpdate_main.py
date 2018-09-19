#! /usr/bin/env python
# -*- coding: utf-8 -*-
#
# Here we provide the necessary imports.
# The basic GUI widgets are located in QtGui module.
import sys
from PyQt4 import QtGui
import xml.etree.ElementTree
from upgrade_module import load_error_file_on_xml
from PMUpdate_gui import Ui_MainWindow

VERSION = "0.3"

# list of filed required in schedeule data section file
fields_required = {"Date__(UT)__HR:MN:SS", "R.A._(ICRF/J2000.0)", "DEC_(ICRF/J2000.0)"}
# split_file_header is used to find the schedule data section file
split_file_header = "Date__"


class Editor(QtGui.QMainWindow):

    def __init__(self):
        super(Editor, self).__init__()
        self.xml_file_status = False
        self.pdplt_file_status = False
        self.receivers_status = False
        self.ui=Ui_MainWindow()
        self.ui.setupUi(self)
        self.home()

    def home(self):
        self.ui.pushButton_exit.clicked.connect(self.menu_exitcommand)
        self.ui.pushButton_browse_xml.clicked.connect(self.browsecommand_xml)
        self.ui.pushButton_browse_error.clicked.connect(self.browsecommand_error)
        self.ui.comboBox_receivers.currentIndexChanged.connect(self.combobox_select)
        self.ui.pushButton_update.clicked.connect(self.menu_updatecommand)
        self.show()

    def menu_updatecommand(self):
        error, error_messgae = load_error_file_on_xml(errorfilename=self.ui.lineEdit_error.text(),
                               pointingxmlfilename=self.ui.lineEdit_xml.text(),
                               receivername=str(self.ui.comboBox_receivers.currentText()))

        if error>0:
            qmsg = QtGui.QMessageBox()
            qmsg.setIcon(QtGui.QMessageBox.Critical)
            qmsg.setWindowTitle("Error")
            qmsg.setText(error_messgae)
            qmsg.show()
        else:
            qmsg = QtGui.QMessageBox()
            qmsg.setIcon(QtGui.QMessageBox.Information)
            qmsg.setWindowTitle("Error")
            qmsg.setText("Update completed.")
            qmsg.show()

    def browsecommand_xml(self):

        receivers = []

        fileName = QtGui.QFileDialog()
        WindowTitle = "Open XML Pointing file"
        Directory = "."
        Filter = "XML Files (*.xml)"

        self.ui.lineEdit_xml.setText(fileName.getOpenFileName(self, WindowTitle, Directory, Filter))

        try:
            xml.etree.ElementTree.register_namespace('', 'urn:schemas-cosylab-com:PointingModel:1.0')
            xml.etree.ElementTree.register_namespace('baci', 'urn:schemas-cosylab-com:BACI:1.0')
            xml.etree.ElementTree.register_namespace('cdb', 'urn:schemas-cosylab-com:CDB:1.0')
            xml.etree.ElementTree.register_namespace('xsi', 'http://www.w3.org/2001/XMLSchema-instance')
            et = xml.etree.ElementTree.parse(self.ui.lineEdit_xml.text())

            e = et.getroot()

            for child in e:
                receivers.append(child[0].text.strip())
            if len(receivers) == 0:
                self.xml_file_status = False
                qmsg = QtGui.QMessageBox()
                qmsg.setIcon(QtGui.QMessageBox.Critical)
                qmsg.setWindowTitle("Error")
                qmsg.setText("pointingxmlfilename: no receivers found")
                qmsg.show()
        except ValueError as e:
            self.xml_file_status = False
            qmsg = QtGui.QMessageBox()
            qmsg.setIcon(QtGui.QMessageBox.Critical)
            qmsg.setWindowTitle("Error")
            qmsg.setText("pointingxmlfilename: xml parsing error {error_value}".format(error_value=e))
            qmsg.show()

        # svuota il dropdown

        self.ui.comboBox_receivers.clear()

        # aggiungi la lista dei ricevitori al drop down

        for receiver in receivers:
            self.ui.comboBox_receivers.insertItem(0, receiver)

        self.xml_file_status = True

        self.ui.comboBox_receivers.setEnabled(self.xml_file_status and self.pdplt_file_status)
        self.ui.pushButton_update.setEnabled(self.xml_file_status and self.pdplt_file_status and self.receivers_status)

    def browsecommand_error(self):

        fileName = QtGui.QFileDialog()
        fileName.setWindowTitle("Open pdplt error file")
        fileName.setDirectory(".")
        fileName.setFilter("pdplt error file (*.*)")

        self.ui.lineEdit_error.setText(fileName.getOpenFileName())

        try:
            errorfilename_rows = []
            start_section = False
            with open(self.ui.lineEdit_error.text(), 'rb') as f:
                for line in f:
                    line_strip = line.strip()
                    if start_section:
                        if line_strip == "*":
                            pass
                        else:
                            errorfilename_rows.append(line_strip.split())
                    if line_strip == "$new_model":
                        start_section = True
                else:
                    # No more lines to be read from file
                    if not start_section:
                        self.pdplt_file_status = False
                        qmsg = QtGui.QMessageBox()
                        qmsg.setIcon(QtGui.QMessageBox.Critical)
                        qmsg.setWindowTitle("Error")
                        qmsg.setText("errorfilename: new model not found")
                        qmsg.show()
            if len(errorfilename_rows) < 8:
                self.pdplt_file_status = False
                qmsg = QtGui.QMessageBox()
                qmsg.setIcon(QtGui.QMessageBox.Critical)
                qmsg.setWindowTitle("Error")
                qmsg.setText("errorfilename: new model section too short")
                qmsg.show()

            if len(errorfilename_rows[0]) <> 7 or \
                    len(errorfilename_rows[1]) <> 31 or \
                    len(errorfilename_rows[2]) <> 5 or \
                    len(errorfilename_rows[3]) <> 5 or \
                    len(errorfilename_rows[4]) <> 5 or \
                    len(errorfilename_rows[5]) <> 5 or \
                    len(errorfilename_rows[6]) <> 5 or \
                    len(errorfilename_rows[7]) <> 5:
                self.pdplt_file_status = False
                qmsg = QtGui.QMessageBox()
                qmsg.setIcon(QtGui.QMessageBox.Critical)
                qmsg.setWindowTitle("Error")
                qmsg.setText("errorfilename: not enough parameters in new model section")
                qmsg.show()

        except:
            self.pdplt_file_status = False
            qmsg = QtGui.QMessageBox()
            qmsg.setIcon(QtGui.QMessageBox.Critical)
            qmsg.setWindowTitle("Error")
            qmsg.setText("errorfilename: generic error on parameters extraction")
            qmsg.show()

        self.pdplt_file_status = True
        self.ui.comboBox_receivers.setEnabled(self.xml_file_status and self.pdplt_file_status)
        self.ui.pushButton_update.setEnabled(self.xml_file_status and self.pdplt_file_status and self.receivers_status)

    def combobox_select(self, index):
        if index == -1:
            self.receivers_status = False
        else:
            self.receivers_status = True
        self.ui.pushButton_update.setEnabled(self.xml_file_status and self.pdplt_file_status and self.receivers_status)

    def menu_exitcommand(self):
        sys.exit()


def main():
    app = QtGui.QApplication(sys.argv)
    ex = Editor()
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()
