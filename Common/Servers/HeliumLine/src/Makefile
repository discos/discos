#*******************************************************************************
# PPPPPPPP
#
# "@(#) $Id$"
#
# Makefile of HeliumLine
#
# who       when      what
# --------  --------  ----------------------------------------------
# spoppi  14/02/14  created
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
USER_LIB =               -lACE \
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
			-lmodbus 

#
# MODULE CODE DESCRIPTION:
# ------------------------
# As a general rule:  public file are "cleaned" and "installed"  
#                     local (_L) are not "installed".

#
# C programs (public and local)
# -----------------------------
EXECUTABLES_L   = testCommandLine

#
# <brief description of xxxxx program>
HeliumLineImpl_OBJECTS   =	HeliumLineImpl CommandLine testCommandLine
HeliumLineImpl_LIBS      = ReceiversErrors ComponentErrors HeliumLineInterfaceStubs IRALibrary

testCommandLine_OBJECTS    = CommandLine testCommandLine


# ----------------------------
LIBRARIES       = HeliumLineImpl


#
# Configuration Database Files
# ----------------------------
CDB_SCHEMAS = HeliumLine

# 
# IDL Files and flags
# 
IDL_FILES =
TAO_IDLFLAGS =
USER_IDL =
#
# Jarfiles and their directories
#
JARFILES= 
jjj_DIRS=
jjj_EXTRAS= 
#
# java sources in Jarfile on/off
DEBUG= 
#
# ACS XmlIdl generation on/off
#
XML_IDL= 
#
# Java Component Helper Classes generation on/off
#
COMPONENT_HELPERS=
#
# Java Entity Classes generation on/off
#
XSDBIND=
#
# Schema Config files for the above
#
XSDBIND_INCLUDE=
# man pages to be done
# --------------------
MANSECTIONS =
MAN1 =
MAN3 =
MAN5 =
MAN7 =
MAN8 =

#
# local man pages
# ---------------
MANl =

#
# ASCII file to be converted into Framemaker-MIF
# --------------------
ASCII_TO_MIF = 

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
