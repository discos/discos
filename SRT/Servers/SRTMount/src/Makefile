#*******************************************************************************
# PPPPPPPP
#
# "@(#) $Id: Makefile,v 1.6 2011-06-03 18:02:49 a.orlati Exp $"
#
# Makefile of ........
#
# who       when      what
# --------  --------  ----------------------------------------------
# andrea  28/09/09  created
#

#*******************************************************************************
# This Makefile follows VLT Standards (see Makefile(5) for more).
#*******************************************************************************
# REMARKS
#    None
#------------------------------------------------------------------------

MAKE_PDF=ON

#
# user definable C-compilation flags
#USER_CFLAGS = 

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
EXECUTABLES_L     = testACU statusLineTest

testACU_OBJECTS = ACUProtocol CommandSocket StatusSocket Configuration CommonData testACU 
testACU_LIBS = IRALibrary ComponentErrors AntennaErrors

statusLineTest_OBJECTS = ACUProtocol StatusSocket Configuration CommonData statusLineTest 
statusLineTest_LIBS = IRALibrary ComponentErrors


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
LIBRARIES       = SRTMountImpl
#LIBRARIES_L     =
SRTMountImpl_OBJECTS = ACUProtocol CommandSocket StatusSocket Configuration CommonData SRTMountImpl WatchDog
SRTMountImpl_LIBS = SRTMountStubs MountStubs IRALibrary AntennaDefinitionsStubs ComponentErrors AntennaErrors ManagementErrors ManagmentDefinitionsStubs

#
# Configuration Database Files
# ----------------------------
CDB_SCHEMAS = SRTMount

#
# Python stuff (public and local)
# ----------------------------
PY_SCRIPTS         =
PY_SCRIPTS_L       = testTracking setTime stressTest

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
