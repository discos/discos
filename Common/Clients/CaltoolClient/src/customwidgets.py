from __future__ import print_function
from PyQt4 import Qt
from PyQt4.QtCore import   pyqtSlot,QThread
import PyQt4.Qwt as Qwt
'''
Custom widgets classes.

  The file generated by the compilation of the .ui file with pyuic4
  must be changed.
  For example,  in the calibrationtool_ui.py the self.qwtPlot_datax widget
  must inherit from  PlotProperty class instead of its parent QwtPlot.
  The output of  compilation of the .ui file thus must be changed accordingly.

'''

class PlotProperty(Qwt.QwtPlot):
    def __init__(self,parent,*args):
        Qwt.QwtPlot.__init__(self,parent,*args)
        #self.paint=Qt.QPainter(self) # you must initialize the superclass
        self.timeData= [i for i in range (1000)]
        self.y=[]
        self.curve=Qwt.QwtPlotCurve('aaaa')
        self.curve.attach(self)
        p = Qt.QPalette()
        p.setColor(self.backgroundRole(), Qt.QColor(30, 30, 50))
        self.setPalette(p)
        self.curve.setPen(Qt.QPen(Qt.Qt.red))

#void Widget::drawCanvas(QPainter* p)
#{
#    QwtPlot::drawCanvas( p );  // <<---

#    QStaticText txt("number");
#    p->drawStaticText ( 0, 0, txt);
#}

    def suspend(self):
        pass

    @pyqtSlot(Qt.QObject,name="setval")  # decorator for the slot, Qt.Object is necessary for lists and dictionarys
    def setVal(self,val):
        value_decimated=[0 for i in range (len(val))]

        step=len(val)/1000.
        if step<1:
            step=1  #step 0 not allowed
        for i in range(0,min(len(val),1000)):
            value_decimated[int(i)] =val[int(i*step-1)]
        #self.curve.setSamples(self.timeData, value_decimated)
        self.curve.setSamples(self.timeData, val)
        self.replot()

    @pyqtSlot(Qt.QObject,name="setX")  # decorator for the slot
    def setX(self,val):
        self.timeData=[i for i in val]

    def setDataY(self,val):
        value_decimated=[0 for i in range (1000)]

        step=len(val)/1000.
        if step<1:
            step=1  #step 0 not allowed
        for i in  range(0,min(len(val),1000)):
            value_decimated[int(i)] =val[int(i*step)-1]
        #self.curve.setSamples(self.timeData, value_decimated)
        self.curve.setSamples(self.timeData, val)
        self.replot()

    def setDataX(self,val):
        pass

    def setCurveColor(self,val):
        pass

    def __del__(self):
        #self.actMonwspeed.destroy()
        print("The end __oOo__")
