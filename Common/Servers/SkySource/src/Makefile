#*******************************************************************************
#
# "@(#) $Id: Makefile,v 1.2 2008-02-22 16:38:17 a.orlati Exp $"
#
# Makefile of ........
#
# who       when      what
# --------  --------  ----------------------------------------------
# andrea  12/09/07  created
#

#
# user definable C-compilation flags
#USER_CFLAGS = 

#
# additional include and library search paths
#USER_INC = 
USER_LIB =      -lACE \
		 		-lTAO \
				-lTAO_DsLogAdmin \
				-lTAO_CosNaming \
				-lTAO_IORTable \
				-lTAO_PortableServer \
				-lTAO_Svc_Utils \
				-lTAO_CosTrading \
                -lTAO_CosNotification \
				-lTAO_DynamicAny \
				-lTAO_IFR_Client \
                -lTAO_CosProperty \
				-lacsutil \
				-lcdb \
				-llogging \
				-lacscomponent \
				-lbaci \
				-lmaci \
				-lmaciClient \
				-lacserr \
				-lm \
				-lloki \
				-lncurses 

#
# C programs (public and local)
# -----------------------------
#EXECUTABLES     =
EXECUTABLES_L   = SkySourceTextDisplay

#
# <brief description of xxxxx program>
SkySourceTextDisplay_OBJECTS = TextClient
#xxxxx_LDFLAGS   =
SkySourceTextDisplay_LIBS = EphemGeneratorStubs SkySourceStubs IRALibrary \
    TextWindowLibrary ComponentErrors AntennaErrors ClientErrors \
    AntennaDefinitionsStubs
#
# special compilation flags for single c sources
#yyyyy_CFLAGS   = 

#
# Includes (.h) files (public only)
# ---------------------------------
#INCLUDES        =

#
# Libraries (public and local)
# ----------------------------
LIBRARIES       = SkySourceImpl
#LIBRARIES_L     =

#
# <brief description of lllll library>
SkySourceImpl_OBJECTS   = SkySourceImpl
SkySourceImpl_LIBS = EphemGeneratorStubs SkySourceStubs ObservatoryStubs \
					 SiteInformationStubs IRALibrary ComponentErrors \
					 AntennaErrors AntennaDefinitionsStubs


#
# Configuration Database Files
# ----------------------------
CDB_SCHEMAS = SourceCatalog

# 
# IDL Files and flags
# 
#IDL_FILES =
#IDL_TAO_FLAGS =
#USER_IDL =

#
# other files to be installed
#----------------------------
#INSTALL_FILES =

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
