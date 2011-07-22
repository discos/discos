#*******************************************************************************
# PPPPPPPP
#
# "@(#) $Id: Makefile,v 1.14 2009-11-26 13:39:44 a.orlati Exp $"
#
# Makefile of ........
#
# who       when      what
# --------  --------  ----------------------------------------------
# andrea  22/11/07  created
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

#
# MODULE CODE DESCRIPTION:
# ------------------------
# As a general rule:  public file are "cleaned" and "installed"  
#                     local (_L) are not "installed".


#
# special compilation flags for single c sources
#yyyyy_CFLAGS   = 

EXECUTABLES	= CoordinateGrabber
EXECUTABLES_L = SlewTimeCheck

CoordinateGrabber_OBJECTS = coordinateGrabber
CoordinateGrabber_LIBS = AntennaDefinitionsStubs AntennaBossStubs EphemGeneratorStubs \
	ObservatoryStubs SiteInformationStubs ManagmentDefinitionsStubs \
	IRALibrary ComponentErrors ClientErrors AntennaErrors ManagementErrors\
	baci maci maciClient acserr loki ACSErrTypeCommon

SlewTimeCheck_OBJECTS   =	TestClient SlewCheck
SlewTimeCheck_LIBS      = IRALibrary 

#
# Includes (.h) files (public only)
# ---------------------------------
INCLUDES        =

#
# Libraries (public and local)
# ----------------------------
LIBRARIES       = AntennaBossImpl
LIBRARIES_L     =

#
# <brief description of lllll library>
AntennaBossImpl_OBJECTS   = AntennaBossImpl BossCore Configuration \
	WorkingThread WatchingThread Callback SlewCheck
AntennaBossImpl_LIBS = AntennaDefinitionsStubs AntennaBossStubs MountStubs ObservatoryStubs \
   EphemGeneratorStubs SiteInformationStubs ManagmentDefinitionsStubs IRALibrary ComponentErrors ManagementErrors \
   AntennaErrors ParserErrors PointingModelStubs RefractionStubs SkySourceStubs OTFStubs MoonStubs acsnc

#
# Configuration Database Files
# ----------------------------
CDB_SCHEMAS = AntennaBoss

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
