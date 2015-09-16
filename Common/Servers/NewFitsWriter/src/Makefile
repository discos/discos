#*******************************************************************************
# PPPPPPPP
#
# "@(#) $Id: Makefile,v 1.16 2011-06-21 16:38:41 a.orlati Exp $"
#
# Makefile of ........
#
# who       when      what
# --------  --------  ----------------------------------------------
# simona  10/11/08  created
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
USER_INC = -I/usr/local/include/CCfits 
USER_LIB = -lCCfits 

#
# MODULE CODE DESCRIPTION:
# ------------------------
# As a general rule:  public file are "cleaned" and "installed"  
#                     local (_L) are not "installed".

EXECUTABLES_L     =

# Libraries (public and local)
# ----------------------------
LIBRARIES       =
LIBRARIES_L     = FitsModule

FitsModule_OBJECTS = FitsTools File
FitsModule_LIBS = IRALibrary
#
# <brief description of lllll library>
#NewFitsWriterImpl_OBJECTS   = FitsWriterImpl 
#NewFitsWriterImpl_LIBS = baci maci FitsWriterStubs DataReceiverStubs ComponentErrors ManagementErrors ReceiversErrors bulkDataReceiverStubs\
                 ManagmentDefinitionsStubs ACSBulkDataError bulkDataStubs IRALibrary\
                 bulkDataCallback AntennaErrors AntennaDefinitionsStubs AntennaBossStubs MinorServoDefinitionsStubs \
                 EphemGeneratorStubs BackendsDefinitionsStubs ObservatoryStubs MinorServoBossStubs MinorServoErrors\
                 GenericWeatherStationStubs ReceiversBossStubs ReceiversDefinitionsStubs CommonReceiverInterfaceStubs SchedulerStubs ManagmentDefinitionsStubs




#
# Configuration Database Files
# ----------------------------
CDB_SCHEMAS = 

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
