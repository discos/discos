#*******************************************************************************
# 
#
# "@(#) $Id: Makefile,v 1.3 2009-05-04 14:32:42 a.orlati Exp $"
#
# Makefile of ........
#
# who       when      what
# --------  --------  ----------------------------------------------
# andrea  03/01/06  created
#

#*******************************************************************************
# This Makefile follows VLT Standards (see Makefile(5) for more).
#*******************************************************************************
# REMARKS
#    None
#------------------------------------------------------------------------

MAKE_PDF = ON

#
# user definable C-compilation flags
#USER_CFLAGS = -g

#
# additional include and library search paths
#USER_INC = 
USER_LIB = -lncurses

#
# MODULE CODE DESCRIPTION:
# ------------------------
# As a general rule:  public file are "cleaned" and "installed"  
#                     local (_L) are not "installed".

#
# C programs (public and local)
# -----------------------------
EXECUTABLES     =
EXECUTABLES_L   = 

#
# <brief description of xxxxx program>
xxxxx_OBJECTS   =	
xxxxx_LDFLAGS   =
xxxxx_LIBS      =

#
# special compilation flags for single c sources
#yyyyy_CFLAGS   = 

#
# Includes (.h) files (public only)
# ---------------------------------
INCLUDES        = TW_Frame.h TW_Tools.h TW_PropertyText.h TW_PropertyText.i TW_Label.h TW_InputCommand.h TW_PropertyComponent.h TW_PropertyComponent.i \
				  TW_FrameComponent.h TW_Callbacks.h TW_Callbacks.i TW_PropertyLedDisplay.h TW_PropertyLedDisplay.i TW_PropertyStatusBox.h TW_PropertyStatusBox.i TW_Text.h TW_StatusBox.h \
				  TW_StatusBox.i TW_LedDisplay.h TW_PropertyBitField.h TW_PropertyBitField.i

#
# Libraries (public and local)
# ----------------------------
LIBRARIES       = TextWindowLibrary
LIBRARIES_L     = 
TextWindowLibrary_OBJECTS = Tools Frame FrameComponent Label InputCommand Callbacks PropertyComponent PropertyText PropertyLedDisplay PropertyStatusBox \
	Text StatusBox LedDisplay PropertyBitField
TextWindowLibrary_LIBS = IRALibrary

#
# <brief description of lllll library>
lllll_OBJECTS   =

#
# Scripts (public and local)
# ----------------------------
SCRIPTS         = getTextClientTemplate
SCRIPTS_L       =

#
# TCL scripts (public and local)
# ------------------------------
TCL_SCRIPTS     =
TCL_SCRIPTS_L   =

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
# <brief description of tttttt tcl-script>
tttttt_OBJECTS  =
tttttt_TCLSH    = 
tttttt_LIBS     = 

#
# TCL libraries (public and local)
# ------------------------------
TCL_LIBRARIES   =
TCL_LIBRARIES_L =

#
# <brief description of tttlll library>
tttlll_OBJECTS  = 

#
# Configuration Database Files
# ----------------------------
CDB_SCHEMAS = 

# 
# IDL Files and flags
# 
IDL_FILES =
IDL_TAO_FLAGS =
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
INSTALL_FILES = ../templates/Client.cpp.template ../templates/Client.h.template

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
	@chmod 700 $(INTROOT)/bin/getTextClientTemplate
	@echo " . . . installation done"


#___oOo___
