#*******************************************************************************
# E.S.O. - ACS project
#
# "@(#) $Id: Makefile,v 1.13 2011-06-21 16:31:50 a.orlati Exp $"
#
# Makefile of ........
#
# who       when      what
# --------  --------  ----------------------------------------------
# andrea  12/08/04  created
# andrea  29/11/04  Added AsyncSocket
# marco   24/05/12  Added CustomLoggerUtils
# 

#*******************************************************************************
# This Makefile follows VLT Standards (see Makefile(5) for more).
#*******************************************************************************
# REMARKS
#    None
#------------------------------------------------------------------------

MAKE_PDF = ON

#
# user definable C-compilation flags
USER_CFLAGS =

#
# additional include and library search paths
#USER_INC = 
#USER_LIB = 

#
# Includes (.h) files (public only)
# ---------------------------------
INCLUDES = Definitions.h String.h IRATools.h Error.h SecureArea.h SerialPort.h Lecom.h FastQueue.h\
 Socket.h DataField.h DBTable.h LogFilter.h Site.h DateTime.h Timer.h SkySource.h TimeTaggedCircularArray.h \
 ScheduleTimer.h TimeoutSync.h MicroControllerBoard.h ReceiverControl.h MicroControllerBoardDef.h \
 SourceFlux.h CustomLoggerUtils.h LogDike.h FrequencyTracking.h BaseConverter.h Cplusplus11Helper.h IRA

#
# Libraries (public and local)
# ----------------------------
LIBRARIES       = IRALibrary
LIBRARIES_L     =
IRALibrary_OBJECTS = String IRATools Error SerialPort Lecom Socket DataField DBTable LogFilter \
 Site DateTime Timer SkySource TimeTaggedCircularArray ScheduleTimer TimeoutSync MicroControllerBoard \
 ReceiverControl SourceFlux CustomLoggerUtils LogDike FrequencyTracking BaseConverter
IRALibrary_LIBS = acstime SlaLibrary maciClient boost_system boost_regex

PY_PACKAGES  = IRAPy
PY_SCRIPTS =

EXECUTABLES     = 
EXECUTABLES_L   = Test1 Test2 TestDBTable TestTypes TestMicroControllerBoard TestReceiverControl TestLogDike TestTimeTaggedCircularArray  \
				  TestFlux TestTimer TestRounding TestCommandRXBoard

#
# <brief description of xxxxx program>
Test1_OBJECTS   = TestSiderealToUTC	
Test1_LIBS      = IRALibrary SlaLibrary maciClient

Test2_OBJECTS   = TestScheduleTimer ScheduleTimer
Test2_LIBS      = IRALibrary

TestDBTable_OBJECTS = TestDBTable
TestDBTable_LIBS = IRALibrary maciClient ClientErrors

TestTypes_OBJECTS   = TestTypeConversion
TestTypes_LIBS      = IRALibrary

TestMicroControllerBoard_OBJECTS   = TestMicroControllerBoard
TestMicroControllerBoard_LIBS      = IRALibrary

TestReceiverControl_OBJECTS   = TestReceiverControl
TestReceiverControl_LIBS      = IRALibrary

TestCommandRXBoard_OBJECTS   = TestCommandRXBoard
TestCommandRXBoard_LIBS      = IRALibrary


TestLogDike_OBJECTS = TestDike
TestLogDike_LIBS      = IRALibrary ClientErrors

TestTimeTaggedCircularArray_OBJECTS = TestTTCArray
TestTimeTaggedCircularArray_LIBS = IRALibrary

TestFlux_OBJECTS = TestSourceFlux
TestFlux_LIBS = IRALibrary

TestTimer_OBJECTS = TestTimer
TestTimer_LIBS = IRALibrary

#FTrack_OBJECTS = TestFrequency FrequencyTracking 
#FTrack_LIBS = SlaLibrary IRALibrary

#STrack_OBJECTS = TestSkySource SkySource DateTime String
#STrack_LIBS = IRALibrary

TestRounding_OBJECTS = TestFloatRounding IRATools
TestRounding_LIBS = IRALibrary

# 
# IDL Files and flags
# 
IDL_FILES = 
IDL_TAO_FLAGS =
USER_IDL =

#
# list of all possible C-sources (used to create automatic dependencies)
# ------------------------------
CSOURCENAMES = \
	$(foreach exe, $(EXECUTABLES) $(EXECUTABLES_L), $($(exe)_OBJECTS)) \
	$(foreach rtos, $(RTAI_MODULES) , $($(rtos)_OBJECTS)) \
	$(foreach lib, $(LIBRARIES) $(LIBRARIES_L), $($(lib)_OBJECTS))

#
#>>>>> END OF standard rules

#
# INCLUDE STANDARDS
# -----------------
ifdef ACSROOT
    MAKEDIR  = $(shell if [ -f $(INTROOT)/include/acsMakefile ];  then \
                     echo $(INTROOT)/include; \
	           else \
	             echo $(ACSROOT)/include; \
		   fi;)
    include $(MAKEDIR)/acsMakefile
else
    MAKEDIR  = $(shell if [ -f $(INTROOT)/include/vltMakefile ];  then \
                     echo $(INTROOT)/include; \
	           else \
	             echo $(VLTROOT)/include; \
		   fi;)
    include $(MAKEDIR)/vltMakefile
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

db : db_all
	@echo " . . . ../DB done"


#___oOo___
