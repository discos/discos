#*******************************************************************************
# PPPPPPPP
#
# "@(#) $Id$"
#
# Makefile of ........
#
# who       when      what
# --------  --------  ----------------------------------------------
# andrea  16/09/11  created
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
USER_LIB =  -lACE \
			-lbaci \
			-lmaci \
			-lmaciClient \
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
			-lacserr \
			-lm \
			-lloki \
			-lACSErrTypeCommon \
			-lncurses

#
# MODULE CODE DESCRIPTION:
# ------------------------
# As a general rule:  public file are "cleaned" and "installed"  
#                     local (_L) are not "installed".

#
# C programs (public and local)
# -----------------------------
EXECUTABLES     = _tui_MinorServoBossTextClient
EXECUTABLES_L   = 

#
# <brief description of xxxxx program>
_tui_MinorServoBossTextClient_OBJECTS   = MinorServoBossTextClient
#xxxxx_LDFLAGS   =
_tui_MinorServoBossTextClient_LIBS      = MinorServoBossStubs IRALibrary ManagmentDefinitionsStubs \
         TextWindowLibrary ClientErrors ComponentErrors ManagementErrors MinorServoErrors AntennaDefinitionsStubs \
		 MinorServoDefinitionsStubs


SCRIPTS         = minorservoBossTui

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
