#*******************************************************************************
#
# "@(#) $Id: Makefile,v 1.6 2011-06-21 16:39:05 a.orlati Exp $"
#
# Makefile of ........
#
# who       when      what
# --------  --------  ----------------------------------------------
# andrea  16/01/06  created
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

MAKE_PDF = ON

#
# additional include and library search paths
#USER_INC = 
#USER_LIB = libThirdPartyLibrary.a


#
# Libraries (public and local)
# ----------------------------
LIBRARIES       = ObservatoryImpl
LIBRARIES_L     =

#
# <brief description of lllll library>
ObservatoryImpl_OBJECTS   =  ObservatoryImpl
ObservatoryImpl_LIBS = ObservatoryStubs IRALibrary \
					   ComponentErrors AntennaDefinitionsStubs ManagmentDefinitionsStubs ManagementErrors

#
# Configuration Database Files
# ----------------------------
CDB_SCHEMAS = Observatory 

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
