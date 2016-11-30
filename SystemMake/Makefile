#*******************************************************************************
#
#
# Makefile 
#
# who       when      what
# --------  --------  ----------------------------------------------
# Andrea O. 26/10/07  created
# Andrea O. 21/03/09  added the target cdb in order to checkout the runtime database
# Andrea O. 15/04/11  match now the present status of the cvs
# Andrea O. 13/06/12  installation procedure now ported to svn, environment variable SVNUSER now required
# Andrea O. 13/01/13  environment variable SYSTEM_SVN_TAG now required to control which TAG has to be downloaded from svn
# Bartolini 02/02/15  added .PHONY target tall
# Bartolini 03/02/15  split away common, inspired by http://lackof.org/taggart/hacking/make-example/
# Bartolini 25/08/15  Added comments and changed default target for all

#
# this new Makefile section separates code modules between what is common
# and what is peculiar of each telescope in a modular way. Result is faster
# and there is no need to replicate common code modules for all telescopes.
#

COMMON_DOC:=
COMMON_ERRORS:=ClientErrors ComponentErrors AntennaErrors ParserErrors \
               BackendsErrors ManagementErrors ReceiversErrors \
               MetrologyErrors MinorServoErrors XBackendErrors
COMMON_INTERFACES:=CommonInterface ManagmentInterface AntennaInterface ReceiversInterface \
                   BackendsInterface MinorServoInterface \
                   WeatherStationInterface ActiveSurfaceInterface \
                   XBackendInterface
COMMON_LIBRARIES:=SlaLibrary IRALibrary TextWindowLibrary ParserLibrary \
                  XarcosLibrary ModbusChannel ComponentProxy \
				  DiscosBackendProtocol
COMMON_SERVERS:=AntennaBoss Observatory OTF PointingModel Refraction SkySource \
                Moon FitsWriter Scheduler ReceiversBoss ExternalClients \
                CalibrationTool TotalPower NoiseGenerator CustomLogger \
                XBackend PyDewarPositioner Roach #DBBC 
COMMON_CLIENTS:=AntennaBossTextClient ObservatoryTextClient \
                GenericBackendTextClient ReceiversBossTextClient \
                SystemTerminal CaltoolClient CustomLoggingClient \
                SchedulerTextClient
COMMON_MISC:=Plotter KStars Scripts getTemplateForTests
COMMON_SIMULATORS:= TCPGenericProtocolSim ReceiverBoardSim

SRT_DOC:=SRTDox
SRT_ERRORS:=SRTActiveSurfaceErrors
SRT_INTERFACES:=SRTAntennaInterface SRTActiveSurfaceInterface \
                SRTWeatherStationInterface SRTReceiversInterface
SRT_LIBRARIES:=SRTMinorServoLibrary
SRT_SERVERS:=SRTMount SRTActiveSurfaceLanServer SRTActiveSurfaceUSDServer \
             SRTActiveSurfaceBoss SRTMinorServo SRTKBandMFReceiver \
             SRTWeatherStation SRT7GHzReceiver SRTLPBandReceiver
SRT_CLIENTS:=SRTActiveSurfaceGUIClient SRTMountTextClient \
             MinorServoBossTextClient MeteoClient
SRT_MISC:=SRTScripts

MED_DOC:=MEDDox
MED_ERRORS:=
MED_INTERFACES:=MedicinaAntennaInterface MedicinaWeatherStationInterface
MED_LIBRARIES:=
MED_SERVERS:=MedicinaMount MedWeatherStation MedicinaMinorServo
MED_CLIENTS:=MedicinaMountTextClient 
MED_MISC:=MedScripts

NT_DOC:=NTDox
NT_ERRORS:=NotoActiveSurfaceErrors
NT_INTERFACES:=NotoAntennaInterface NotoReceiversInterface NotoWeatherStationInterface NotoActiveSurfaceInterface
NT_LIBRARIES:=
NT_SERVERS:= NotoReceivers NotoMount NotoWeatherStation NotoActiveSurface
NT_CLIENTS:=NotoMountTextClient 
NT_MISC:=NotoScripts

ifeq ($(TARGETSYS),SRT)
	#Add all modules included in the SRT system  in this sequence
	#Doc, (Errors, Interfacies, Libraries, Servers, Clients, Misc)
	MODULES:=SRTDox \
		ClientErrors ComponentErrors AntennaErrors ParserErrors BackendsErrors ManagementErrors ReceiversErrors MetrologyErrors MinorServoErrors XBackendErrors \
		SRTActiveSurfaceErrors \
		CommonInterface ManagmentInterface AntennaInterface ReceiversInterface BackendsInterface MinorServoInterface WeatherStationInterface ActiveSurfaceInterface XBackendInterface \
		SRTAntennaInterface SRTActiveSurfaceInterface SRTWeatherStationInterface SRTReceiversInterface \
		SlaLibrary IRALibrary TextWindowLibrary ParserLibrary XarcosLibrary SRTMinorServoLibrary ComponentProxy ModbusChannel\
		AntennaBoss Observatory OTF PointingModel Refraction SkySource Moon FitsWriter Scheduler ReceiversBoss ExternalClients CalibrationTool TotalPower NoiseGenerator DBBC CustomLogger XBackend \
		SRTMount SRTActiveSurfaceLanServer SRTActiveSurfaceUSDServer SRTActiveSurfaceBoss SRTMinorServo SRTKBandMFReceiver SRTWeatherStation SRT7GHzReceiver SRTLPBandReceiver PyDewarPositioner \
		AntennaBossTextClient ObservatoryTextClient GenericBackendTextClient ReceiversBossTextClient SystemTerminal CaltoolClient CustomLoggingClient SchedulerTextClient \
		SRTActiveSurfaceGUIClient SRTMountTextClient MinorServoBossTextClient \
		Plotter KStars SRTScripts 

	IDL_DOC:=Managment Antenna Backends SRTActiveSurface WeatherStation
	CPP_DOC:=Libraries AntennaImplementation SRTActiveSurfaceImplementation BackendsImplementation ReceiversImplementation WeatherStationImplementation MinorServoImplementation

	Managment_MODULES:=ManagmentInterface
	Antenna_MODULES:=AntennaInterface SRTAntennaInterface
	Backends_MODULES:=BackendsInterface XBackendInterface
	WeatherStation_MODULES:=WeatherStationInterface SRTWeatherStationInterface
	Receivers_MODULES:=ReceiversInterface
	MinorServo_MODULES:=MinorServoInterface
	SRTActiveSurface_MODULES:=SRTActiveSurfaceInterface
	Libraries_MODULES:=IRALibrary SlaLibrary TextWindowLibrary ParserLibrary XarcosLibrary SRTMinorServoLibrary
	AntennaImplementation_MODULES:=Observatory SkySource OTF Moon PointingModel Refraction AntennaBoss SRTMount
	ManagementImplementation_MODULES:=Scheduler CalibrationTool FitsWriter ExternalClients
	BackendsImplementation_MODULES:=TotalPower NoiseGenerator XBackend
	ReceiversImplementation_MODULES:=ReceiversBoss SRTKBandMFReceiver SRT7GHzReceiver SRTLPBandReceiver
	WeatherStationImplementation_MODULES:=SRTWeatherStation
	SRTActiveSurfaceImplementation_MODULES:=SRTActiveSurfaceLanServer SRTActiveSurfaceUSDServer SRTActiveSurfaceBoss 
	MinorServoImplementation_MODULES:=SRTMinorServo
	
	CDB_SVN_LOCATION="SRT/Configuration/CDB"
	SYSTEM_SVN_MODULES=Common SRT
else 
ifeq ($(TARGETSYS),MED)
	#Add all modules included in the Medicina system in this sequence
	#Doc, (Errors, Interfacies, Libraries, Servers, Clients, Misc)
	MODULES:=MEDDox \
		ClientErrors ComponentErrors AntennaErrors ParserErrors BackendsErrors ManagementErrors ReceiversErrors MetrologyErrors MinorServoErrors XBackendErrors\
		CommonInterface ManagmentInterface AntennaInterface ReceiversInterface BackendsInterface MinorServoInterface WeatherStationInterface ActiveSurfaceInterface XBackendInterface\
		MedicinaAntennaInterface MedicinaWeatherStationInterface \
		SlaLibrary IRALibrary TextWindowLibrary ParserLibrary ComponentProxy ModbusChannel XarcosLibrary\
		AntennaBoss Observatory OTF PointingModel Refraction SkySource Moon FitsWriter Scheduler ReceiversBoss ExternalClients CalibrationTool \
		Metrology TotalPower NoiseGenerator MedicinaMount MedWeatherStation CustomLogger XBackend MedicinaMinorServo\
		AntennaBossTextClient ObservatoryTextClient GenericBackendTextClient ReceiversBossTextClient SystemTerminal CaltoolClient SchedulerTextClient MinorServoBossTextClient\
		MedicinaMountTextClient CustomLoggingClient\
		Plotter KStars Scripts MedScripts

	IDL_DOC:=Managment Antenna Backends WeatherStation Receivers
	CPP_DOC:=Libraries AntennaImplementation ManagementImplementation BackendsImplementation WeatherStationImplementation

	Managment_MODULES:=ManagmentInterface
	Antenna_MODULES:=AntennaInterface MedicinaAntennaInterface
	Backends_MODULES:=BackendsInterface XBackendInterface
	WeatherStation_MODULES:=WeatherStationInterface MedicinaWeatherStationInterface
	Receivers_MODULES:=ReceiversInterface
	Libraries_MODULES:=IRALibrary SlaLibrary TextWindowLibrary ParserLibrary
	AntennaImplementation_MODULES:=Observatory SkySource OTF Moon PointingModel Refraction AntennaBoss MedicinaMount
	ManagementImplementation_MODULES:=Scheduler CalibrationTool FitsWriter ExternalClients
	BackendsImplementation_MODULES:=TotalPower NoiseGenerator XBackend
	ReceiversImplementation_MODULES:=ReceiversBoss
	WeatherStationImplementation_MODULES:=MedWeatherStation Metrology
	
	CDB_SVN_LOCATION="Medicina/Configuration/CDB"
	SYSTEM_SVN_MODULES=Common Medicina
else
ifeq ($(TARGETSYS),NT)
	#Add all modules included in the Noto system in this sequence
	#Doc, (Errors, Interfacies, Libraries, Servers, Clients, Misc)
	MODULES:= ClientErrors ComponentErrors AntennaErrors ParserErrors BackendsErrors ManagementErrors ReceiversErrors MetrologyErrors MinorServoErrors \
		ManagmentInterface AntennaInterface ReceiversInterface BackendsInterface MinorServoInterface WeatherStationInterface ActiveSurfaceInterface \
		NotoAntennaInterface \
		SlaLibrary IRALibrary TextWindowLibrary ParserLibrary \
		AntennaBoss Observatory OTF PointingModel Refraction SkySource Moon FitsWriter Scheduler ReceiversBoss ExternalClients CalibrationTool \
		TotalPower NotoMount CustomLogger \
		AntennaBossTextClient ObservatoryTextClient GenericBackendTextClient ReceiversBossTextClient SystemTerminal SchedulerTextClient \
		NotoMountTextClient CustomLoggingClient Scripts

	IDL_DOC:=Managment Antenna Backends Metrology
	CPP_DOC:=Libraries AntennaImplementation ManagementImplementation BackendsImplementation ReceiversImplementation

	Managment_MODULES:=ManagmentInterface
	Antenna_MODULES:=AntennaInterface NotoAntennaInterface
	Backends_MODULES:=BackendsInterface
	Receivers_MODULES:=ReceiversInterface
	Libraries_MODULES:=IRALibrary SlaLibrary TextWindowLibrary ParserLibrary
	AntennaImplementation_MODULES:=Observatory SkySource OTF Moon PointingModel Refraction AntennaBoss NotoMount
	ManagementImplementation_MODULES:=Scheduler CalibrationTool FitsWriter ExternalClients
	BackendsImplementation_MODULES:=TotalPower
	ReceiversImplementation_MODULES:=ReceiversBoss
	
	CDB_SVN_LOCATION="Noto/Configuration/CDB"
	SYSTEM_SVN_MODULES=Common Medicina Noto

else
	RESULT:=$(error TARGETSYS variable is not set correctly!)	
endif
endif
endif


# ******* END of Editable area *****************
COMMON_MODULES:=$(COMMON_ERRORS) \
                $(COMMON_INTERFACES) \
                $(COMMON_LIBRARIES) \
                $(COMMON_SERVERS) \
                $(COMMON_CLIENTS) \
                $(COMMON_MISC) \
                $(COMMON_SIMULATORS)

SRT_MODULES:=$(SRT_ERRORS) \
             $(SRT_INTERFACES) \
             $(SRT_LIBRARIES) \
             $(SRT_SERVERS) \
             $(SRT_CLIENTS) \
             $(SRT_MISC)

MED_MODULES:=$(MED_ERRORS) \
             $(MED_INTERFACES) \
             $(MED_LIBRARIES) \
             $(MED_SERVERS) \
             $(MED_CLIENTS) \
             $(MED_MISC)

NT_MODULES:=$(NT_ERRORS) \
            $(NT_INTERFACES) \
            $(NT_LIBRARIES) \
            $(NT_SERVERS) \
            $(NT_CLIENTS) \
            $(NT_MISC)

ifeq ($(TARGETSYS), SRT)
	TELESCOPE_MODULES:=$(SRT_MODULES)
else 
ifeq ($(TARGETSYS), MED)
	TELESCOPE_MODULES:=$(MED_MODULES)
else 
ifeq ($(TARGETSYS), NT)
	TELESCOPE_MODULES:=$(NT_MODULES)
else
	RESULT:=$(error TARGETSYS variable is not set correctly!)	
endif
endif
endif

CURRENT_DIR:=$(shell pwd)/..

define PathFinder
	$(shell for MOD in $(1); do find ${CURRENT_DIR} -name $${MOD} -not -path *CDB* -not -path *site-packages* \( -type d -o -type l \) -print; done)
endef

#if the INTROOT varaible is not defined the make is stopped
ifndef INTROOT
	RESULT:=$(error INTROOT variable is not set!)
endif

#if the ACS_CDB variable is not set the make process is stopped
ifndef ACS_CDB
	RESULT:=$(error ACS_CDB variable is not set!)
endif

#if the SYSLOCATION variable is not set the make process is stopped
ifndef SYSLOCATION
	RESULT:=$(error SYSLOCATION variable is not set!)
endif

#if the SVNUSER variable is not set the make process is stopped
ifndef SVNUSER
	RESULT:=$(error SVNUSER variable is not set!)
endif

#if the SYSTEM_SVN_TAG variable is not set the make process is stopped
ifndef SYSTEM_SVN_TAG
	RESULT:=$(error SYSTEM_SVN_TAG variable is not set!)
endif

#set up the Modules file for each IDL and CPP documentation
$(foreach i, $(IDL_DOC), $(eval $(i)_MODULES_PATH:=$(call PathFinder,$($(i)_MODULES))))
$(foreach i, $(CPP_DOC), $(eval $(i)_MODULES_PATH:=$(call PathFinder,$($(i)_MODULES))))
#create the doxygen configuration files
$(foreach i, $(CPP_DOC), $(shell echo "INPUT=$($(i)_MODULES_PATH)" > /tmp/$(i)_cpp.in))
$(foreach i, $(IDL_DOC), $(shell echo "INPUT=$($(i)_MODULES_PATH)" > /tmp/$(i)_idl.in))

COMPILE_MODULES_PATH:=$(call PathFinder,$(MODULES))

BUILD_DIRS:=$(foreach i, $(COMPILE_MODULES_PATH), $(i)/src)

COMMON_MODULES_PATH:=$(call PathFinder,$(COMMON_MODULES))
COMMON_MODULES_DIRS:=$(foreach i, $(COMMON_MODULES_PATH), $(i)/src)
COMMON_BUILD_TARGETS:=$(COMMON_MODULES_DIRS:%=build-%)
COMMON_CLEAN_TARGETS:=$(COMMON_MODULES_DIRS:%=clean-%)
TELESCOPE_MODULES_PATH:=$(call PathFinder,$(TELESCOPE_MODULES))
TELESCOPE_MODULES_DIRS:=$(foreach i, $(TELESCOPE_MODULES_PATH), $(i)/src)
TELESCOPE_BUILD_TARGETS:=$(TELESCOPE_MODULES_DIRS:%=build-%)
TELESCOPE_CLEAN_TARGETS:=$(TELESCOPE_MODULES_DIRS:%=clean-%)

ifeq (X$(DOCROOT),X)
	DOC:=$(HOME)/docroot
else
	DOC:=$(DOCROOT)
endif


DOCIDL:=$(DOC)/idl
DOCCPP:=$(DOC)/cpp

.PHONY: all

all: common-build telescope-build

clean: common-clean telescope-clean

legacy-all:
	@for i in $(BUILD_DIRS); do \
		make -C $${i} all install ; \
	done

.PHONY: common-build $(COMMON_BUILD_TARGETS)

common-build: $(COMMON_BUILD_TARGETS)

$(COMMON_BUILD_TARGETS):
	$(MAKE) -C $(@:build-%=%) all install

.PHONY: common-clean $(COMMON_CLEAN_TARGETS)

common-clean: $(COMMON_CLEAN_TARGETS)

$(COMMON_CLEAN_TARGETS):
	$(MAKE) -C $(@:clean-%=%) clean_dist

.PHONY: telescope-build $(TELESCOPE_BUILD_TARGETS)

telescope-build: $(TELESCOPE_BUILD_TARGETS)

$(TELESCOPE_BUILD_TARGETS):
	$(MAKE) -C $(@:build-%=%) all install

.PHONY: telescope-clean $(TELESCOPE_CLEAN_TARGETS)

telescope-clean: $(TELESCOPE_CLEAN_TARGETS)

$(TELESCOPE_CLEAN_TARGETS):
	$(MAKE) -C $(@:clean-%=%) clean_dist

compile:
	@for i in $(BUILD_DIRS); do \
		make -C $${i} all ; \
	done

install:
	@for i in $(BUILD_DIRS); do \
		make -C $${i} install ; \
	done

cdb:
	@cd $(ACS_CDB); ln -s $(SYSLOCATION)/$(SYSTEM_SVN_TAG)/$(CDB_SVN_LOCATION) CDB

sources:
	@cd $(SYSLOCATION) ; svn checkout svn+ssh://$(SVNUSER)@nuraghe-devel.oa-cagliari.inaf.it/ACS/tags/$(SYSTEM_SVN_TAG)/$${i}

man:
	@echo "creating documentation tree....."
	@if [ ! -d $(DOC) ]; then mkdir $(DOC) ; fi
	@if [ ! -d $(DOCIDL) ]; then mkdir $(DOCIDL) ; fi
	@if [ ! -d $(DOCCPP) ]; then mkdir $(DOCCPP) ; fi	
	@for i in $(IDL_DOC); do \
		if [ ! -d $(DOCIDL)/$${i} ]; then mkdir $(DOCIDL)/$${i} ; fi ; \
		cp $(INTROOT)/config/DoxyfileIDL /tmp/doxyconf ; \
		echo "OUTPUT_DIRECTORY=$(DOCIDL)/$${i}" >> /tmp/doxyconf ; \
		cat /tmp/$${i}_idl.in >> /tmp/doxyconf ; \
		echo "HTML_HEADER=$(INTROOT)/templates/DocHeader.html" >> /tmp/doxyconf ; \
	    echo "HTML_FOOTER=$(INTROOT)/templates/DocFooter.html" >> /tmp/doxyconf ; \
		doxygen /tmp/doxyconf ; \
		rm /tmp/doxyconf ; \
		rm /tmp/$${i}_idl.in ; \
	done
	@for i in $(CPP_DOC); do \
		if [ ! -d $(DOCCPP)/$${i} ]; then mkdir $(DOCCPP)/$${i} ; fi ; \
		cp $(INTROOT)/config/DoxyfileCPP /tmp/doxyconf ; \
		echo "OUTPUT_DIRECTORY=$(DOCCPP)/$${i}" >> /tmp/doxyconf ; \
		cat /tmp/$${i}_cpp.in >> /tmp/doxyconf ; \
		echo "HTML_HEADER=$(INTROOT)/templates/DocHeader.html" >> /tmp/doxyconf ; \
	    echo "HTML_FOOTER=$(INTROOT)/templates/DocFooter.html" >> /tmp/doxyconf ; \
		doxygen /tmp/doxyconf ; \
		rm /tmp/doxyconf ; \
		rm /tmp/$${i}_cpp.in ; \
	done

	@echo "documentation done in $(DOC)"

legacy-clean:
	@rm -rf $(INTROOT)/bin/*
	@rm -rf $(INTROOT)/config/CDB/schemas/*
	@rm -rf $(INTROOT)/idl/*
	@rm -rf $(INTROOT)/Sources/*
	@rm -rf $(INTROOT)/templates/*
	@rm -rf $(INTROOT)/includes/*
	@for i in $(BUILD_DIRS); do \
		make -C $${i} clean; \
	done
	@rm -rf $(DOC)/*
stat:
	@./slocc.sh -findopt "( -name *.cpp -o -name *.i -o -name *.h -o -name *.idl -o -name *.xml -o -name *.xsd -o -name *.py -o -name Makefile* ) \
	-not -path *object* -not -path *lib*" ..
