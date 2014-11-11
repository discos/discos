######################################################################
# Qt-qmake QtGraphicsLibrary
######################################################################


TARGET = QtGraphicsLibrary

TEMPLATE = lib 
DESTDIR = $(INTROOT)/lib
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += IRA_box.h\
           IRA_label.h\
           IRA_datapoint.h\
           IRA_plotter.h\
           IRA_curve.h\
           IRA_histogram.h\
           IRA_boxscope.h\
           IRA_monitor.h\
           IRA_image.h

SOURCES += IRA_box.cpp\
           IRA_label.cpp\
           IRA_datapoint.cpp\
           IRA_plotter.cpp\
           IRA_curve.cpp\
           IRA_histogram.cpp\
           IRA_boxscope.cpp\
           IRA_monitor.cpp\
           IRA_image.cpp
                     
LIBS    +=

unix:install.path = $(INTROOT)/include
unix:install.files = *.h
INSTALLS += install 
