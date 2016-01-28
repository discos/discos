#*******************************************************************************
# PPPPPPPP
#
# "@(#) $Id$"
#
# Makefile of ComponentProxy
#
# who       when      what
# --------  --------  ----------------------------------------------
# mbartolini  17/02/15  created
#

EXECUTABLES_L   = compilation_test
compilation_test_OBJECTS    = compilation_test
compilation_test_LIBS       = IRALibrary \
                              ManagmentDefinitionsStubs \
                              AntennaDefinitionsStubs \
                              MinorServoDefinitionsStubs \
                              ReceiversDefinitionsStubs \
                              CommonReceiverInterfaceStubs \
                              GenericReceiverStubs \
                              DewarPositionerDefinitionsStubs \
                              BackendsDefinitionsStubs \
                              CustomLoggerStubs \
                              ExternalClientsStubs \
                              SchedulerStubs \
                              FitsWriterStubs \
                              CalibrationToolStubs \
                              AntennaBossStubs \
                              BackendsBossStubs \
                              DBBCStubs \
                              HolographyStubs \
                              NoiseGeneratorStubs \
                              TotalPowerStubs \
                              ActiveSurfaceBossStubs \
                              MinorServoBossStubs \
                              MinorServoStubs \
                              ReceiversBossStubs \
                              DewarPositionerStubs \
                              LocalOscillatorInterfaceStubs \
                              GenericWeatherStationStubs \
                              DataReceiverStubs \
                              GenericBackendStubs \
                              CommonReceiverInterfaceStubs \
                              bulkDataStubs bulkDataReceiverStubs \
                              bulkDataCallback bulkDataSenderStubs \
			      MountStubs \
                              ManagementErrors \
                              ComponentErrors \
                              AntennaErrors \
                              MinorServoErrors \
                              ReceiversErrors \
                              BackendsErrors 

#
# Includes (.h) files (public only)
# ---------------------------------
INCLUDES        = ActiveSurfaceProxy.h  \
                  BackendsProxy.h   \
                  ComponentProxy.h \
                  MinorServoProxy.h  \
                  WeatherProxy.h \
                  AntennaProxy.h \
                  ComponentProxies  \
                  ManagementProxy.h  \
                  ReceiversProxy.h \
		  MountProxy.h
		  


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
