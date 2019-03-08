#*******************************************************************************
# PPPPPPPP
#
# "@(#) $Id: Makefile,v 1.4 2010-12-06 12:46:20 a.orlati Exp $"
#
# Makefile of ........
#
# who       when      what
# --------  --------  ----------------------------------------------
# andrea  19/11/09  created
#

#*******************************************************************************
# This Makefile follows VLT Standards (see Makefile(5) for more).
#*******************************************************************************
# REMARKS
#    None
#------------------------------------------------------------------------

#
# user definable C-compilation flags
# a quick comment on this: in order to control receivers a t med the boss is in charge of doing much more things that it has to. So
# different code is required until new specific component to control the receivers for Medicina are written
ifeq ($(STATION),Medicina)
	USER_CFLAGS = -U 'COMPILE_TARGET_STANDARD' -U 'COMPILE_TARGET_NT' -D 'COMPILE_TARGET_MED'
else ifeq ($(STATION),Noto)
	USER_CFLAGS = -U 'COMPILE_TARGET_MED' -U 'COMPILE_TARGET_STANDARD' -D 'COMPILE_TARGET_NT'
else
	USER_CFLAGS = -U 'COMPILE_TARGET_MED' -U 'COMPILE_TARGET_NT' -D 'COMPILE_TARGET_STANDARD'
endif

#
# additional include and library search paths
#USER_INC = 
#USER_LIB = 

#
# MODULE CODE DESCRIPTION:
# ------------------------
# As a general rule:  public file are "cleaned" and "installed"  
#                     local (_L) are not "installed".

#
# C programs (public and local)
# -----------------------------
EXECUTABLES     =
EXECUTABLES_L   = TestMedicinaSXKL

TestMedicinaSXKL_OBJECTS = TestMedicinaSXKL
TestMedicinaSXKL_LIBS = IRALibrary


#
# <brief description of xxxxx program>
ReceiversBossImpl_OBJECTS   = ReceiversBossImpl	RecvBossCore Configuration WatchingThread
ReceiversBossImpl_LDFLAGS   =
ifeq ($(STATION),Noto)
	ReceiversBossImpl_LIBS      = IRALibrary ComponentErrors CommonReceiverInterfaceStubs ReceiversBossStubs ReceiversDefinitionsStubs AntennaDefinitionsStubs GenericReceiverStubs \
				      DewarPositionerStubs ManagmentDefinitionsStubs DewarPositionerDefinitionsStubs ManagementErrors ParserErrors ReceiversErrors acsnc \
				      BackendsDefinitionsStubs TotalPowerStubs GenericBackendStubs BackendsErrors bulkDataStubs bulkDataReceiverStubs \
				      bulkDataCallback bulkDataSenderStubs DiscosVersion
else
	ReceiversBossImpl_LIBS      = IRALibrary ComponentErrors CommonReceiverInterfaceStubs ReceiversBossStubs ReceiversDefinitionsStubs AntennaDefinitionsStubs GenericReceiverStubs \
				      DewarPositionerStubs ManagmentDefinitionsStubs DewarPositionerDefinitionsStubs ManagementErrors ParserErrors ReceiversErrors acsnc DiscosVersion
endif

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
LIBRARIES       = ReceiversBossImpl
LIBRARIES_L     =


#
# Configuration Database Files
# ----------------------------
CDB_SCHEMAS = ReceiversBoss

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
