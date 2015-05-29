#*******************************************************************************
# PPPPPPPP
#
# "@(#) $Id: Makefile,v 1.5 2011-06-21 16:38:57 a.orlati Exp $"
#
# Makefile of ........
#
# who       when      what
# --------  --------  ----------------------------------------------
# rashmi  19/03/08  created
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
# Libraries (public and local)
# ----------------------------
LIBRARIES       = MoonImpl 


EXECUTABLES_L = test_moon


#	
# <brief description of lllll library>
#lllll_OBJECTS   =
MoonImpl_OBJECTS = MoonImpl
MoonImpl_LIBS = EphemGeneratorStubs MoonStubs baci maci IRALibrary SlaLibrary ObservatoryStubs   \
                 ComponentErrors AntennaErrors AntennaDefinitionsStubs ManagmentDefinitionsStubs ManagementErrors

test_moon_OBJECTS = test_moon
test_moon_LIBS = baci IRALibrary SlaLibrary EphemGeneratorStubs ComponentErrors AntennaErrors   \
                     AntennaDefinitionsStubs ManagmentDefinitionsStubs ObservatoryStubs ManagementErrors
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
