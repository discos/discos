USER_LIB    =   -lbaci \
                -lmaci \
                -lTAO_CosNotification \
        -lTAO_CosNotification_MC_Ext \
                -lRepeatGuard \
        -lacstime 

#
# Libraries (public and local)
# ----------------------------
#LIBRARIES       = MedMinorServoControl #PositionQueue MedMinorServoModbus MedMinorServoTime
LIBRARIES       = MinorServoBossImpl 
LIBRARIES_L     =

#
# <brief description of lllll library>
MedMinorServoControl_OBJECTS   = MedMinorServoControl MedMinorServoGeometry \
                                 MedMinorServoTime MedMinorServoModbus \
                                 ModbusChannel PositionQueue

MedMinorServoControl_LIBS   = baci acsnc IRALibrary ManagmentDefinitionsStubs \
                              ManagementErrors \
                              ComponentErrors modbus boost_thread

MinorServoBossImpl_OBJECTS = MedMinorServoControl MedMinorServoGeometry \
                             MedMinorServoStatus MedMinorServoParameters \
                             MedMinorServoTime MedMinorServoModbus \
                             MedMinorServoScan ModbusChannel PositionQueue \
                             PositionMonitoringThread MSBossPublisher \
                             SetupThread ScanThread \
                             MSBossTracker MedMinorServoOffset MinorServoBossImpl

MinorServoBossImpl_LIBS = MedMinorServoBossStubs MinorServoBossStubs \
                          ManagmentDefinitionsStubs MinorServoDefinitionsStubs \
                          ManagementErrors \
                          IRALibrary ComponentErrors MinorServoErrors \
                          acsnc ParserErrors AntennaDefinitionsStubs \
                          MountStubs AntennaBossStubs AntennaErrors \
                          ActiveSurfaceBossStubs boost_thread acstime modbus \
                          baci

EXECUTABLES_L     = testclient testgeometry testprimary testsecondary testpassive real_to_virtual
real_to_virtual_OBJECTS = real_to_virtual testutils MedMinorServoGeometry MedMinorServoTime
real_to_virtual_LIBS = IRALibrary
testclient_OBJECTS = testclient testutils MedMinorServoModbus ModbusChannel MedMinorServoGeometry MedMinorServoTime MedMinorServoControl PositionQueue
testclient_LIBS = IRALibrary boost_thread modbus 
testprimary_OBJECTS = testprimary testutils MedMinorServoModbus ModbusChannel MedMinorServoGeometry MedMinorServoTime MedMinorServoControl PositionQueue
testprimary_LIBS = IRALibrary boost_thread modbus 
testsecondary_OBJECTS = testsecondary testutils MedMinorServoModbus ModbusChannel MedMinorServoGeometry MedMinorServoTime MedMinorServoControl PositionQueue
testsecondary_LIBS = IRALibrary boost_thread modbus 
testpassive_OBJECTS = testpassive testutils MedMinorServoModbus ModbusChannel MedMinorServoGeometry MedMinorServoTime MedMinorServoControl PositionQueue
testpassive_LIBS = IRALibrary boost_thread modbus 
testgeometry_OBJECTS = testgeometry testutils MedMinorServoGeometry MedMinorServoTime PositionQueue
testgeometry_LIBS = IRALibrary boost_unit_test_framework boost_thread modbus 

EXECUTABLES = scudisconnect
scudisconnect_OBJECTS = scudisconnect MedMinorServoModbus ModbusChannel MedMinorServoGeometry MedMinorServoTime MedMinorServoControl PositionQueue
scudisconnect_LIBS = IRALibrary modbus boost_thread

#
# Configuration Database Files
# ----------------------------
CDB_SCHEMAS = MinorServo MinorServoParameters


SHELL = /bin/bash
# 
# IDL Files and flags
# 
IDL_FILES = MedMinorServoBoss
TAO_IDLFLAGS =
USER_IDL =

#
# list of all possible C-sources (used to create automatic dependencies)
# ------------------------------
CSOURCENAMES = \
	$(foreach exe, $(EXECUTABLES) $(EXECUTABLES_L), $($(exe)_OBJECTS)) \
	$(foreach rtos, $(RTAI_MODULES) , $($(rtos)_OBJECTS)) \
	$(foreach lib, $(LIBRARIES) $(LIBRARIES_L), $($(lib)_OBJECTS))

#
# INCLUDE STANDARDS
# -----------------

MAKEDIRTMP := $(shell searchFile include/acsMakefile)
ifneq ($(MAKEDIRTMP),\#error\#)
   MAKEDIR := $(MAKEDIRTMP)/include
   include $(MAKEDIR)/acsMakefile
endif

#
# TARGETS
# -------
all:	do_all
	@echo " . . . 'all' done" 

clean : clean_all 
	@echo " . . . clean done"

clean_dist : clean_all clean_dist_all 
	@echo " . . . clean_dist done"

man   : do_man 
	@echo " . . . man page(s) done"

install : install_all
	@echo " . . . installation done"

testclient   :
	../bin/testclient

testgeometry   :
	../bin/testgeometry

testprimary   :
	../bin/testprimary

testsecondary   :
	../bin/testsecondary

#___oOo___
