#-----------------------------------------
# Marco Buttu <mbuttu@oa-cagliari.inaf.it>
#-----------------------------------------

CDB_SCHEMAS = DewarPositioner

PY_SCRIPTS = # List of scripts to be installed in the bin directory
PY_MODULES =
PY_PACKAGES = DewarPositioner


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
	$(RM) *~ *Impl/*~ 
	$(RM) ../lib/python/site-packages/*
	@echo " . . . clean done"

clean_dist : clean_all clean_dist_all 
	@echo " . . . clean_dist done"

man   : do_man 
	@echo " . . . man page(s) done"

install : install_all
	@echo " . . . installation done"

#___oOo___

