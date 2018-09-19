#-------------------------------------------------
# Giuseppe Carboni  <gcarboni@oa-cagliari.inaf.it>
#-------------------------------------------------


CDB_SCHEMAS = SRTIFDistributor

PY_SCRIPTS = _ifd
PY_MODULES =
PY_PACKAGES = IFDistributor


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
	$(RM) *~ *Impl/*~ IFDistributor/*.pyc
	$(RM) ../lib/python/site-packages/*
	$(RM) $(INTROOT)/lib/python/site-packages/IFDistributor
	@echo " . . . clean done"

clean_dist : clean_all clean_dist_all 
	@echo " . . . clean_dist done"

man   : do_man 
	@echo " . . . man page(s) done"

install : install_all
	@chmod 700 $(INTROOT)/bin/_ifd
	@echo " . . . installation done"

#___oOo___

