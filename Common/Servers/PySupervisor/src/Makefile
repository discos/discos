#----------------------------------
# Marco Buttu <marco.buttu@inaf.it>
#----------------------------------


CDB_SCHEMAS = Supervisor

PY_MODULES =
PY_PACKAGES = Supervisor


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
	@$(RM) *~ *Impl/*~ Supervisor/*.pyc
	@$(RM) ../lib/python/site-packages/*
	@$(RM) $(INTROOT)/lib/python/site-packages/Supervisor
	@echo " . . . clean done"

clean_dist : clean_all clean_dist_all 
	@echo " . . . clean_dist done"

man   : do_man 
	@echo " . . . man page(s) done"

install : install_all
	@echo " . . . installation done"

#___oOo___
