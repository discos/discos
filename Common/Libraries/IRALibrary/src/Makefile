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
INCLUDES = Definitions.h String.h IRATools.h Error.h SecureArea.h SerialPort.h Lecom.h \
 Socket.h DataField.h DBTable.h LogFilter.h Site.h DateTime.h SkySource.h TimeTaggedCircularArray.h \
 ScheduleTimer.h TimeoutSync.h MicroControllerBoard.h ReceiverControl.h MicroControllerBoardDef.h \
 SourceFlux.h IRA

#
# Libraries (public and local)
# ----------------------------
LIBRARIES       = IRALibrary
LIBRARIES_L     =
IRALibrary_OBJECTS = String IRATools Error SerialPort Lecom Socket DataField DBTable LogFilter \
 Site DateTime SkySource TimeTaggedCircularArray ScheduleTimer TimeoutSync MicroControllerBoard \
 ReceiverControl SourceFlux
IRALibrary_LIBS = acstime SlaLibrary maciClient

EXECUTABLES     =
EXECUTABLES_L   = Test1 Test2 TestDBTable TestTypes TestMicroControllerBoard TestReceiverControl

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

TestSourceFlux_OBJECTS = TestSourceFlux
TestSourceFlux_LIBS = IRALibrary

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
