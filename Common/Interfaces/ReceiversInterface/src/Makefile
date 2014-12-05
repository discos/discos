#*****************************************************************
# Makefile of Receivers
# =====================
# Marco Buttu <mbuttu@oa-cagliari.inaf.it>
# Andrea Orlati <a.orlati@ira.inaf.it>
#*****************************************************************

MAKE_PDF = ON

#
# Libraries (public and local)
# ----------------------------
 
CDB_SCHEMAS = Feeds ReceiversModeSetup LocalOscillatorLookUpTable \
			  NoiseMarkLookUpTable TaperLookUpTable \
			  DewarPositionerUpdatingTable DewarPositionerMapping \
			  DewarPositionerActions

# IDL Files and flags
IDL_FILES = CommonReceiverInterface ReceiversBoss \
			GenericReceiver LocalOscillatorInterface \
			DewarPositionerDefinitions GenericDerotator DewarPositioner

# list of all possible C-sources (used to create automatic dependencies)
CSOURCENAMES = \
	$(foreach exe, $(EXECUTABLES) $(EXECUTABLES_L), $($(exe)_OBJECTS)) \
	$(foreach rtos, $(RTAI_MODULES) , $($(rtos)_OBJECTS)) \
	$(foreach lib, $(LIBRARIES) $(LIBRARIES_L), $($(lib)_OBJECTS))


# INCLUDE STANDARDS
MAKEDIRTMP := $(shell searchFile include/acsMakefile)
ifneq ($(MAKEDIRTMP),\#error\#)
   MAKEDIR := $(MAKEDIRTMP)/include
   include $(MAKEDIR)/acsMakefile
endif


# TARGETS
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

