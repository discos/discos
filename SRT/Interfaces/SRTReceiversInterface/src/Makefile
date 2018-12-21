#*****************************************************************
# Makefile of Receivers
# =====================
# Marco Buttu		<mbuttu@oa-cagliari.inaf.it>
# Andrea Orlati		<a.orlati@ira.inaf.it>
# Giuseppe Carboni	<giuseppe.carboni@inaf.it
#*****************************************************************

MAKE_PDF = ON

CDB_SCHEMAS = SRTLPBandNoiseMarkLookUpTable SRTLPBandReceiverModeSetup \
			  SRTLPBandTaperLookUpTable  \
			  MarkCoefficientsSBandLookUpTable SRTIFDistributorTable #SRTIFDistributor14Table

# IDL Files and flags
IDL_FILES = SRTKBandMF SRTSBandMF SRT7GHz SRTLPBand \
			SRTKBandDerotator SRTIFDistributor #SRTIFDistributor14
SRTKBandMFStubs_LIBS = baciStubs ComponentErrorsStubs GenericReceiverStubs
SRTSBandMFStubs_LIBS = baciStubs ComponentErrorsStubs GenericReceiverStubs
SRT7GHzStubs_LIBS = baciStubs ComponentErrorsStubs GenericReceiverStubs
SRTLPBandStubs_LIBS = baciStubs ComponentErrorsStubs GenericReceiverStubs ManagmentDefinitionsStubs
SRTKBandDerotatorStubs_LIBS = baciStubs ComponentErrorsStubs GenericDerotatorStubs
SRTIFDistributorStubs_LIBS = GenericIFDistributorStubs LocalOscillatorInterfaceStubs
#SRTIFDistributor14Stubs_LIBS = GenericIFDistributorStubs


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
	$(RM) *~ ../include ../test ../*~ ../lib ../object ../rtai ../bin
	$(RM) ../doc/html
	$(RM) tmp.txt acsexmplbeans.jar ../doc/abeans.log
	@echo " . . . clean done"

clean_dist : clean_all clean_dist_all 
	@echo " . . . clean_dist done"

man   : do_man 
	@echo " . . . man page(s) done"

install : install_all
	@echo " . . . installation done"
