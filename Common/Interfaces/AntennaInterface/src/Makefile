#*******************************************************************************
# PPPPPPPP
#
# "@(#) $Id: Makefile,v 1.14 2011-03-11 12:56:13 c.migoni Exp $"
#
# Makefile of ........
#
# who       when      what
# --------  --------  ----------------------------------------------
# Andrea    06/04/07  created
# Andrea    24/04/07  Added PointingModule by Carlo Migoni
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

CDB_SCHEMAS = Mount Station PointingModel

# 
# IDL Files and flags
# 
IDL_FILES = Observatory Mount PointingModel EphemGeneratorMACRO_include \
EphemGenerator SkySource OTF Refraction AntennaBoss Moon
IDL_TAO_FLAGS =
USER_IDL =
ObservatoryStubs_LIBS = baciStubs maciStubs AntennaDefinitionsStubs ComponentErrorsStubs
MountStubs_LIBS = baciStubs maciStubs ManagmentDefinitionsStubs ManagementErrorsStubs \
                  AntennaErrorsStubs AntennaDefinitionsStubs ComponentErrorsStubs
PointingModelStubs_LIBS = baciStubs maciStubs AntennaErrorsStubs acscomponentStubs
EphemGeneratorStubs_LIBS = baciStubs maciStubs ManagmentDefinitionsStubs ManagementErrorsStubs \
                           AntennaErrorsStubs AntennaDefinitionsStubs
SkySourceStubs_LIBS = baciStubs maciStubs EphemGeneratorStubs ComponentErrorsStubs \
                      AntennaErrorsStubs AntennaDefinitionsStubs
OTFStubs_LIBS = baciStubs maciStubs EphemGeneratorStubs ComponentErrorsStubs \
                AntennaErrorsStubs AntennaDefinitionsStubs
RefractionStubs_LIBS = baciStubs maciStubs acscomponentStubs AntennaErrorsStubs
AntennaBossStubs_LIBS = baciStubs maciStubs ManagmentDefinitionsStubs ManagementErrorsStubs \
                        EphemGeneratorStubs ComponentErrorsStubs AntennaErrorsStubs \
                        AntennaDefinitionsStubs
MoonStubs_LIBS = baciStubs maciStubs EphemGeneratorStubs


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
	@cp ../images/Antenna.png $(INTROOT)/bitmaps/
	@chmod 644 $(INTROOT)/bitmaps/Antenna.png
	@echo " . . . installation done"


#___oOo___
