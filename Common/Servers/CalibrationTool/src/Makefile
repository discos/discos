#*******************************************************************************
# PPPPPPPP
#
# "@(#) $Id: Makefile,v 1.5 2011-06-21 16:38:30 a.orlati Exp $"
#
# Makefile of ........
#
# who           when      what
# --------      --------  ----------------------------------------------
# Carlo Migoni  23/11/10  created
#

#*******************************************************************************
# This Makefile follows VLT Standards (see Makefile(5) for more).
#*******************************************************************************
# REMARKS
#    None
#------------------------------------------------------------------------

#
# user definable C-compilation flags
USER_CFLAGS = -g -u MAIN__

#
# additional include and library search paths
#USER_INC =
USER_LIB = -lm

#
# MODULE CODE DESCRIPTION:
# ------------------------
# As a general rule:  public file are "cleaned" and "installed"  
#                     local (_L) are not "installed".

EXECUTABLES_L     =

# Libraries (public and local)
# ----------------------------
LIBRARIES       = CalibrationToolImpl 
LIBRARIES_L     =

#
# <brief description of lllll library>
CalibrationToolImpl_OBJECTS   = CalibrationToolImpl ReceiverCallback DataCollection EngineThread Configuration CommonTools \
								CollectorThread fit2 matin fgaus
CalibrationToolImpl_LIBS = baci maci CalibrationToolStubs DataReceiverStubs ComponentErrors ManagementErrors bulkDataReceiverStubs\
						   ManagmentDefinitionsStubs ACSBulkDataError bulkDataStubs IRALibrary \
						   bulkDataCallback AntennaErrors AntennaDefinitionsStubs AntennaBossStubs \
						   EphemGeneratorStubs BackendsDefinitionsStubs SchedulerStubs ManagmentDefinitionsStubs \
						   ObservatoryStubs MinorServoBossStubs MinorServoErrors

#
# Configuration Database Files
# ----------------------------
CDB_SCHEMAS = CalibrationTool

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
	@echo " . . . installation done"


#___oOo___
