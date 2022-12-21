PY_PACKAGES = DewarPositionerMockers

# CPP UNIT TESTING SETUP
#--------------
# GTEST_HOME=/usr/local/include/gtest
# GMOCK_HOME=/usr/local/include/gmock
# GTEST_LIBS=gtest gtest_main

# USER_INC=-I$(GTEST_HOME) -I$(GMOCK_HOME)
# USER_LIBS=C++ pthread
# END OF CPP UNIT TESTING SETUP
#---------------------

# DEFINE YOUR CPP UNIT TEST EXECUTABLES HERE as:
#
# EXECTUABLES_L = unittest
# unittest_OBJECTS = unittest
# unittest_LIBS = $(GTEST_LIBS) <ComponentNameImpl>

# EXECUTABLES_L = unittest
# unittest_OBJECTS = unittest
# unittest_LIBS = $(GTEST_LIBS) 

# END OF CUSTOMIZATION
# do not edit below this line
#----------------------------

CSOURCENAMES = \
	$(foreach exe, $(EXECUTABLES) $(EXECUTABLES_L), $($(exe)_OBJECTS)) \
	$(foreach rtos, $(RTAI_MODULES) , $($(rtos)_OBJECTS)) \
	$(foreach lib, $(LIBRARIES) $(LIBRARIES_L), $($(lib)_OBJECTS))

MAKEDIRTMP := $(shell searchFile include/acsMakefile)
ifneq ($(MAKEDIRTMP),\#error\#)
   MAKEDIR := $(MAKEDIRTMP)/include
   include $(MAKEDIR)/acsMakefile
endif

do_cli:
	@echo "running python cli tests"
	unit2 discover cli

do_interface:
	@echo "running python interface tests"
	unit2 discover interface

do_pyunit:
	@echo "running python unit tests"
	unit2 pyunit

clean_test:
	rm -f results/*.xml
	rm -f interface/*.pyc
	rm -f pyunit/*.pyc
	rm -rf ../lib/python/site-packages/*


cli: do_cli
	@echo " . . . 'cli' done"

interface: do_interface
	@echo " . . . 'interface' done"

py_unit: do_py_unit
	@echo " . . . 'py_unit' done"


# TARGETS
# -------
all:	do_all
	@echo " . . . 'all' done" 

clean : clean_all clean_test
	@echo " . . . clean done"

clean_dist : clean_all clean_dist_all clean_test
	@echo " . . . clean_dist done"

man   : do_man 
	@echo " . . . man page(s) done"

install : install_all
	@echo " . . . installation done"
