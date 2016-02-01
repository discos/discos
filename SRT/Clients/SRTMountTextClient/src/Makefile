#*******************************************************************************
# PPPPPPPP
#
# "@(#) $Id: Makefile,v 1.2 2011-06-01 18:25:35 a.orlati Exp $"
#
# Makefile of ........
#
# who       when      what
# --------  --------  ----------------------------------------------
# andrea  30/11/10  created
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
# MODULE CODE DESCRIPTION:
# ------------------------
# As a general rule:  public file are "cleaned" and "installed"  
#                     local (_L) are not "installed".

#
# C programs (public and local)
# -----------------------------
EXECUTABLES     = _tui_SRTMountTextClient
EXECUTABLES_L   = 

#
# <brief description of xxxxx program>
_tui_SRTMountTextClient_OBJECTS = SRTMountTextClient
_tui_SRTMountTextClient_LIBS = MountStubs SRTMountStubs IRALibrary TextWindowLibrary ComponentErrors AntennaErrors ManagementErrors ClientErrors AntennaDefinitionsStubs ManagmentDefinitionsStubs

SCRIPTS         = mountTui
SCRIPTS_L       =
#
# special compilation flags for single c sources
#yyyyy_CFLAGS   = 

#
# Includes (.h) files (public only)
# ---------------------------------
INCLUDES        =

#
# Python stuff (public and local)
# ----------------------------
PY_SCRIPTS         =
PY_SCRIPTS_L       =

PY_MODULES         =
PY_MODULES_L       =

PY_PACKAGES        =
PY_PACKAGES_L      =
pppppp_MODULES	   =

#
# Configuration Database Files
# ----------------------------
CDB_SCHEMAS = 

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
