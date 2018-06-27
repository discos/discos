#*******************************************************************************
# PPPPPPPP
#
# "@(#) $Id: Makefile,v 1.3 2011-06-05 14:43:49 a.orlati Exp $"
#
# Makefile of ........
#
# who       when      what
# --------  --------  ----------------------------------------------
# andrea  16/07/08  created
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
EXECUTABLES     =
EXECUTABLES_L   = TestFunctions TestParser TestTypes

PY_PACKAGES  = SimpleParserPy
PY_SCRIPTS =


#
# <brief description of xxxxx program>
TestFunctions_OBJECTS   = TestFunctions	
TestFunctions_LIBS      = IRALibrary SlaLibrary ParserErrors ComponentErrors

TestParser_OBJECTS   = TestParser	
TestParser_LIBS      = IRALibrary SlaLibrary ParserErrors ComponentErrors

TestTypes_OBJECTS   = TestTypes	
TestTypes_LIBS      = IRALibrary SlaLibrary ParserErrors ComponentErrors


#
# Includes (.h) files (public only)
# ---------------------------------
INCLUDES        = SP_typeConversion.h SP_types.h SP_functor.h SP_function0.h SP_function1.h SP_function2.h SP_function3.h SP_function4.h SP_function5.h\
  SP_function6.h SP_function7.h SP_parser.h SP_parser.i pstream.h ParserConnector.h ParserConnector.i

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
