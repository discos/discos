#*******************************************************************************
# This Makefile follows VLT Standards (see Makefile(5) for more).
#*******************************************************************************
# REMARKS
#    None
#------------------------------------------------------------------------

#
# user definable C-compilation flags
#USER_CFLAGS = 

#
# additional include and library search paths
#USER_INC =
USER_LIB = /usr/lib64/libzmq.so

#
# MODULE CODE DESCRIPTION:
# ------------------------
# As a general rule:  public file are "cleaned" and "installed"  
#                     local (_L) are not "installed".

#
# C programs (public and local)
# -----------------------------
EXECUTABLES =
EXECUTABLES_L =


PY_PACKAGES = ZMQLibrary
#PY_SCRIPTS =


#
# <brief description of xxxxx program>


#
# Includes (.h) files (public only)
# ---------------------------------
INCLUDES = ZMQLibrary.hpp ZMQDictionary.hpp ZMQPublisher.hpp PyZMQPublisher.hpp ZMQTimeStamp.hpp PyZMQTimeStamp.hpp

#
# Libraries (public and local)
# ----------------------------
LIBRARIES = ZMQLibrary PyZMQPublisher PyZMQTimeStamp
LIBRARIES_L =

ZMQLibrary_OBJECTS = ZMQPublisher ZMQTimeStamp
ZMQLibrary_LIBS = IRALibrary

PyZMQPublisher_OBJECTS = PyZMQPublisher
PyZMQPublisher_LIBS = ZMQLibrary boost_python3

PyZMQTimeStamp_OBJECTS = PyZMQTimeStamp
PyZMQTimeStamp_LIBS = ZMQLibrary boost_python3


#
# list of all possible C-sources (used to create automatic dependencies)
# ------------------------------
CSOURCENAMES = \
	$(foreach exe, $(EXECUTABLES) $(EXECUTABLES_L), $($(exe)_OBJECTS)) \
	$(foreach rtos, $(RTAI_MODULES) , $($(rtos)_OBJECTS)) \
	$(foreach lib, $(LIBRARIES) $(LIBRARIES_L), $($(lib)_OBJECTS))

#
#>>>>> END OF standard rules

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
