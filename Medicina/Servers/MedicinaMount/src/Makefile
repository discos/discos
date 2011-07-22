#*******************************************************************************
# E.S.O. - ACS project
#
# "@(#) $Id: Makefile,v 1.5 2009-11-26 17:03:59 a.orlati Exp $"
#
# Makefile of ........
#
# who       						when      what
# --------  						--------  ----------------------------------------------
# Andrea Orlati(aorlati@ira.inaf.it) 02/01/05  created
# Andrea Orlati(aorlati@ira.inaf.it) 16/06/05  Modified according to ACS 4.1
# Andrea Orlati(aorlati@ira.inaf.it) 03/10/06  Change the public include files list
# Andrea Orlati(aorlati@ira.inaf.it) 10/10/06  Added the creation of PDF documentation

#*******************************************************************************
# This Makefile follows VLT Standards (see Makefile(5) for more).
#*******************************************************************************
# REMARKS
#    None
#------------------------------------------------------------------------

MAKE_PDF = ON

#
# user definable C-compilation flags
#USER_CFLAGS = 

#
# additional include and library search paths
#USER_INC = 
#USER_LIB = 

#
# MODULE CODE DESCRIPTION:


#
# Includes (.h) files (public only)
# ---------------------------------
#INCLUDES        =

LIBRARIES       = MedicinaMountImpl
LIBRARIES_L     =
MedicinaMountImpl_OBJECTS = MedicinaMountImpl MedicinaMountSocket ACUData MedicinaMountThread ACUInterface Configuration
MedicinaMountImpl_LIBS = MedicinaMountStubs MountStubs acsnc IRALibrary \
    ComponentErrors AntennaErrors ManagementErrors AntennaDefinitionsStubs ManagmentDefinitionsStubs

#
# Configuration Database Files
# ----------------------------
CDB_SCHEMAS = MedicinaMount

# 
# IDL Files and flags
# 
IDL_FILES =
IDL_TAO_FLAGS =
USER_IDL =

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
