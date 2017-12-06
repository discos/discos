USER_LIB    =   -lbaci \
                -lmaci \
                -lTAO_CosNotification \
        -lTAO_CosNotification_MC_Ext \
                -lRepeatGuard \
        -lacstime \
                -lexpat

#
# Libraries (public and local)
# ----------------------------
LIBRARIES       = CustomLoggerImpl
LIBRARIES_L     =
#
# <brief description of lllll library>
CustomLoggerImpl_OBJECTS   = CustomLoggerImpl expat_log_parsing
CustomLoggerImpl_LIBS   = CustomLoggerStubs baci acsnc IRALibrary ManagmentDefinitionsStubs ManagementErrors ComponentErrors

#
# Configuration Database Files
# ----------------------------
CDB_SCHEMAS = CustomLogger


SHELL = /bin/bash
# 
# IDL Files and flags
# 
IDL_FILES = 
TAO_IDLFLAGS =
USER_IDL =

SCRIPTS_L         = emitErrorLogTest

PY_SCRIPTS_L = emitLogMessage

#
# list of all possible C-sources (used to create automatic dependencies)
# ------------------------------
CSOURCENAMES = \
	$(foreach exe, $(EXECUTABLES) $(EXECUTABLES_L), $($(exe)_OBJECTS)) \
	$(foreach rtos, $(RTAI_MODULES) , $($(rtos)_OBJECTS)) \
	$(foreach lib, $(LIBRARIES) $(LIBRARIES_L), $($(lib)_OBJECTS))

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
