#*******************************************************************************
# PPPPPPPP
#
# "@(#) $Id: Makefile,v 1.13 2011-06-21 16:39:52 a.orlati Exp $"
#
# Makefile of ........
#
# who       when      what
# --------  --------  ----------------------------------------------
# andrea  12/12/08  created
#

#*******************************************************************************
# This Makefile follows VLT Standards (see Makefile(5) for more).
#*******************************************************************************
# REMARKS
#    None
#------------------------------------------------------------------------

#
# user definable C-compilation flags
#USER_CFLAGS = 

#
# additional include and library search paths
#USER_INC = 
#USER_LIB = 

RESTRICT_PERMS=_tp_agc

#
# MODULE CODE DESCRIPTION:
# ------------------------
# As a general rule:  public file are "cleaned" and "installed"  
#                     local (_L) are not "installed".

#
# C programs (public and local)
# -----------------------------
EXECUTABLES     = scheduleChecker
EXECUTABLES_L   = testSched testSchedules testLSTtoUT

#
# <brief description of xxxxx program>
testSched_OBJECTS   = testSchedule Schedule SubScanBinder Schedule_ScanList Configuration
testSched_LIBS      = IRALibrary ComponentErrors

testSchedules_OBJECTS   = testSchedules Schedule SubScanBinder Schedule_ScanList Configuration
testSchedules_LIBS      = IRALibrary ComponentErrors boost_system boost_filesystem

testLSTtoUT_OBJECTS = testLST2UT
testLSTtoUT_LIBS = IRALibrary ComponentErrors

scheduleChecker_OBJECTS   = ScheduleChecker Schedule SubScanBinder Schedule_ScanList Configuration
scheduleChecker_LIBS      = IRALibrary ComponentErrors

#
# special compilation flags for single c sources
#yyyyy_CFLAGS   = 

#
# Includes (.h) files (public only)
# ---------------------------------
INCLUDES        =

#
# Libraries (public and local)
# ----------------------------
LIBRARIES       = SchedulerImpl
#LIBRARIES_L     =

SchedulerImpl_OBJECTS = Schedule SchedulerImpl Configuration Core ScheduleExecutor SubScanBinder Schedule_ScanList ScheduleReport
SchedulerImpl_LIBS = IRALibrary AntennaDefinitionsStubs ManagmentDefinitionsStubs SchedulerStubs acsnc \
EphemGeneratorStubs AntennaBossStubs ObservatoryStubs FitsWriterStubs DataReceiverStubs CustomLoggerStubs GenericWeatherStationStubs\
GenericBackendStubs ComponentErrors ManagementErrors AntennaErrors BackendsErrors ParserErrors ReceiversErrors MinorServoErrors\
bulkDataStubs bulkDataReceiverStubs bulkDataCallback bulkDataSenderStubs CommonReceiverInterfaceStubs ReceiversBossStubs ReceiversDefinitionsStubs \
MinorServoStubs MinorServoBossStubs MinorServoDefinitionsStubs ActiveSurfaceBossStubs DiscosVersion DiscosLocals

#
# <brief description of lllll library>
lllll_OBJECTS   =

#
# Scripts (public and local)
# ----------------------------
SCRIPTS         =
SCRIPTS_L       =

#
# TCL scripts (public and local)
# ------------------------------
TCL_SCRIPTS     =
TCL_SCRIPTS_L   =

#
# Python stuff (public and local)
# ----------------------------
PY_SCRIPTS         = _tp_agc
PY_SCRIPTS_L       =

PY_MODULES         =
PY_MODULES_L       =

PY_PACKAGES        =
PY_PACKAGES_L      =
pppppp_MODULES	   =

#
# <brief description of tttttt tcl-script>
tttttt_OBJECTS  =
tttttt_TCLSH    = 
tttttt_LIBS     = 

#
# TCL libraries (public and local)
# ------------------------------
TCL_LIBRARIES   =
TCL_LIBRARIES_L =

#
# <brief description of tttlll library>
tttlll_OBJECTS  = 

#
# Configuration Database Files
# ----------------------------
CDB_SCHEMAS = Scheduler

# 
# IDL Files and flags
# 
IDL_FILES =
TAO_IDLFLAGS =
USER_IDL =
#
# Jarfiles and their directories
#
JARFILES= 
jjj_DIRS=
jjj_EXTRAS= 
#
# java sources in Jarfile on/off
DEBUG= 
#
# ACS XmlIdl generation on/off
#
XML_IDL= 
#
# Java Component Helper Classes generation on/off
#
COMPONENT_HELPERS=
#
# Java Entity Classes generation on/off
#
XSDBIND=
#
# Schema Config files for the above
#
XSDBIND_INCLUDE=
# man pages to be done
# --------------------
MANSECTIONS =
MAN1 =
MAN3 =
MAN5 =
MAN7 =
MAN8 =

#
# local man pages
# ---------------
MANl =

#
# ASCII file to be converted into Framemaker-MIF
# --------------------
ASCII_TO_MIF = 

#
# other files to be installed
#----------------------------
INSTALL_FILES =

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
	@echo $(RESTRICT_PERMS)" has been restcicted to system manager"
	@chmod g-x $(INTROOT)/bin/$(RESTRICT_PERMS)
	@chmod o-x $(INTROOT)/bin/$(RESTRICT_PERMS)
	@echo "scheduleChecker made available to users...."
	@if [ -d $(INTROOT)/user/bin ]; then cp -p $(INTROOT)/bin/scheduleChecker $(INTROOT)/user/bin/; fi;
	@echo " . . . installation done"


#___oOo___
