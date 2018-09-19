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
			  DewarPositionerActions \
			  LocalOscillator LocalOscillatorEthernet

# IDL Files and flags
IDL_FILES = CommonReceiverInterface ReceiversBoss \
			GenericReceiver LocalOscillatorInterface \
			DewarPositionerDefinitions GenericDerotator \
			DewarPositioner GenericIFDistributor


CommonReceiverInterfaceStubs_LIBS = baciStubs ComponentErrorsStubs ManagementErrorsStubs \
    ReceiversErrorsStubs
ReceiversBossStubs_LIBS = baciStubs ComponentErrorsStubs ReceiversErrorsStubs \
    ManagmentDefinitionsStubs CommonReceiverInterfaceStubs ReceiversDefinitionsStubs \
    AntennaDefinitionsStubs
GenericReceiverStubs_LIBS = baciStubs ComponentErrorsStubs ReceiversErrorsStubs \
    ReceiversDefinitionsStubs ManagmentDefinitionsStubs CommonReceiverInterfaceStubs
LocalOscillatorInterfaceStubs_LIBS = baciStubs ComponentErrorsStubs ReceiversErrorsStubs
DewarPositionerDefinitionsStubs_LIBS = baciStubs AntennaDefinitionsStubs
GenericDerotatorStubs_LIBS = baciStubs DerotatorErrorsStubs ComponentErrorsStubs \
    ManagmentDefinitionsStubs
DewarPositionerStubs_LIBS =  baciStubs maciErrTypeStubs ComponentErrorsStubs \
    ManagmentDefinitionsStubs AntennaDefinitionsStubs DewarPositionerDefinitionsStubs
GenericIFDistributorStubs_LIBS = baciStubs maciErrTypeStubs ComponentErrorsStubs


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
	$(RM) $(INTROOT)/lib/python/site-packages/DewarPositioner*_idl*.py*
	$(RM) $(INTROOT)/lib/python/site-packages/libDewarPositionerStubs.*
	$(RM) $(INTROOT)/lib/python/site-packages/GenericIFDistributor*_idl*.py*
	$(RM) $(INTROOT)/lib/python/site-packages/libGenericIFDistributorStubs.*
	@echo " . . . clean done"

clean_dist : clean_all clean_dist_all 
	@echo " . . . clean_dist done"

man   : do_man 
	@echo " . . . man page(s) done"

install : install_all
	@echo " . . . installation done"

