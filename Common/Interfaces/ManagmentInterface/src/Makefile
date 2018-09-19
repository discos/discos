#*******************************************************************************
# PPPPPPPP
#
# "@(#) $Id: Makefile,v 1.7 2010-11-03 16:21:20 a.orlati Exp $"
#
# Makefile of ........
#
# who       when      what
# --------  --------  ----------------------------------------------
# andrea  23/01/08  created
#

#*******************************************************************************
# This Makefile follows VLT Standards (see Makefile(5) for more).
#*******************************************************************************
# REMARKS
#    None
#------------------------------------------------------------------------

#
# Configuration Database Files
# ----------------------------
CDB_SCHEMAS = ProcedureFile TelescopeEquipment

#
# IDL Files and flags
#
IDL_FILES = DataReceiver FitsWriter MBFitsWriter Scheduler ExternalClients CalibrationTool CustomLogger
IDL_TAO_FLAGS =
USER_IDL =

DataReceiverStubs_LIBS = baciStubs maciStubs ManagmentDefinitionsStubs ManagementErrorsStubs \
						 bulkDataReceiverStubs ComponentErrorsStubs
FitsWriterStubs_LIBS = baciStubs maciStubs ManagmentDefinitionsStubs ManagementErrorsStubs \
					   DataReceiverStubs
MBFitsWriterStubs_LIBS = baciStubs maciStubs ManagmentDefinitionsStubs ManagementErrorsStubs \
						 DataReceiverStubs
SchedulerStubs_LIBS = baciStubs maciStubs ManagmentDefinitionsStubs ManagementErrorsStubs \
					  AntennaDefinitionsStubs ComponentErrorsStubs DataReceiverStubs
ExternalClientsStubs_LIBS = baciStubs maciStubs ManagmentDefinitionsStubs ManagementErrorsStubs \
							ComponentErrorsStubs
CalibrationToolStubs_LIBS = baciStubs maciStubs DataReceiverStubs
CustomLoggerStubs_LIBS = baciStubs maciStubs ManagmentDefinitionsStubs ManagementErrorsStubs \
						 ComponentErrorsStubs

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
	@echo " . . . installation done"


#___oOo___
