#-------------------------------------------------
# Giuseppe Carboni  <giuseppe.carboni@inaf.it>
#-------------------------------------------------


CDB_SCHEMAS = PyCalmux

PY_SCRIPTS = _calmux
PY_MODULES =
PY_PACKAGES = Calmux


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
	$(RM) *~ *Impl/*~ Calmux/*.pyc
	$(RM) ../lib/python/site-packages/*
	$(RM) $(INTROOT)/lib/python/site-packages/Calmux
	@echo " . . . clean done"

clean_dist : clean_all clean_dist_all 
	@echo " . . . clean_dist done"

man   : do_man 
	@echo " . . . man page(s) done"

install : install_all
	@chmod 700 $(INTROOT)/bin/_calmux
	@echo " . . . installation done"

#___oOo___

