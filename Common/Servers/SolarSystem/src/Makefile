#*******************************************************************************
# PPPPPPPP
#
# "@(#) $Id$"
#
# Makefile of ........
#
# who       when      what
# --------  --------  ----------------------------------------------
# spoppi  03/05/17  created
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
# C programs (public and local)
# -----------------------------
 EXECUTABLES     = testastrolib
# EXECUTABLES_L   = 



#
# special compilation flags for single c sources
#yyyyy_CFLAGS   = 

#
# Includes (.h) files (public only)
# ---------------------------------
# INCLUDES        = 

#
# Libraries (public and local)
# ----------------------------
LIBRARIES       = SolarSystemBodyImpl
LIBRARIES_L     =

#
# <brief description of lllll library>

SolarSystemBodyImpl_OBJECTS   = SolarSystemBodyImpl libastrowrapper
SolarSystemBodyImpl_LIBS      = EphemGeneratorStubs SolarSystemBodyStubs baci maci IRALibrary \
                                SlaLibrary ObservatoryStubs \
				ComponentErrors AntennaErrors \
				AntennaDefinitionsStubs ManagmentDefinitionsStubs \
				ManagementErrors astro

testastrolib_OBJECTS  = 	 libastrowrapper 	mainastro		
testastrolib_LIBS  =		astro		
#
# Scripts (public and local)
# ----------------------------
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
