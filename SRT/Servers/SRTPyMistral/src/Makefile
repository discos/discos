#----------------------------------
# Marco Buttu <marco.buttu@inaf.it>
#----------------------------------

CDB_SCHEMAS = SRTMistral
PY_PACKAGES = SRTMistral

SRTMistralImpl_OBJECTS = \
	SRTMistralImpl \
	Configuration \
	CommandLine \
SRTMistralImpl_LIBS = \
    IRALibrary \
	SRTMistralStubs \
	TotalPowerStubs \
    GenericBackendStubs \
	ManagmentDefinitionsStubs \
	ReceiversDefinitionsStubs \
	BackendsDefinitionsStubs \
	bulkDataStubs \
	bulkDataSenderStubs \
    bulkDataReceiverStubs \
	ACSBulkDataError \
	GenericIFDistributorStubs \
	ComponentErrors \
	BackendsErrors \
	ParserErrors \
	ManagementErrors \
    DiscosBackendProtocolLib \
	DiscosVersion \



# INCLUDE STANDARDS
# -----------------
MAKEDIRTMP := $(shell searchFile include/acsMakefile)
ifneq ($(MAKEDIRTMP),\#error\#)
   MAKEDIR := $(MAKEDIRTMP)/include
   include $(MAKEDIR)/acsMakefile
endif

# TARGETS
# -------
all:	do_all
	@echo " . . . 'all' done" 

clean : clean_all
	@$(RM) *~ *Impl/*~ SRTMistral/*.pyc
	@$(RM) ../lib/python/site-packages/*
	@$(RM) $(INTROOT)/lib/python/site-packages/SRTMistral
	@echo " . . . clean done"

clean_dist : clean_all clean_dist_all 
	@echo " . . . clean_dist done"

man   : do_man 
	@echo " . . . man page(s) done"

install : install_all
	@echo " . . . installation done"

#___oOo___
