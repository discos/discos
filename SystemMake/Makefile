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
# Carboni   20/04/25  Parallelized code compilation with make -j

#
# this new Makefile section separates code modules between what is common
# and what is peculiar of each telescope in a modular way. Result is faster
# and there is no need to replicate common code modules for all telescopes.
#

# Documentation under Common directory
COMMON_DOC:=
# Common errors, can all be compiled in parallel
COMMON_ERRORS:=ClientErrors ComponentErrors AntennaErrors ParserErrors BackendsErrors ManagementErrors ReceiversErrors MetrologyErrors MinorServoErrors XBackendErrors ActiveSurfaceErrors
# The CommonInterface is the only interface that has no other dependency, must be compiled first
BASE_INTERFACES:=CommonInterface
# The following core interfaces has no dependency between them, they only depend on CommonInterface, therefore they can be compiled in parallel
CORE_INTERFACES:=ManagmentInterface AntennaInterface BackendsInterface MinorServoInterface WeatherStationInterface ActiveSurfaceInterface
# These common interfaces depends on some interface listed in the line above, must be compiled separately
EXTENDED_INTERFACES:=ReceiversInterface XBackendInterface
# These libraries must be compiled sequentially, since the DiscosVersion library depends on the IRALibrary which depends on the SlaLibrary
COMMON_SEQ_LIBS:=SlaLibrary IRALibrary DiscosVersion
# All these other libraries only depend on the libraries listed above, therefore they can be compiled in parallel
COMMON_LIBRARIES:=TextWindowLibrary ParserLibrary XarcosLibrary ModbusChannel ComponentProxy DiscosLocals XEphemAstroLib DiscosBackendProtocol PyTestingLibrary
# All the common servers are independend from one another, they only depend on interfaces, therefore we can compile them in parallel.
# There may be some hidden dependency between servers, but it may never come up since each core compiles a single cpp file.
# Issues may only arise if we use a very high number of cores, which is unlikely.
# If we'll ever face this problem we will split the SERVERS into more groups, with the latter one having servers which depends on some other servers in the first group.
COMMON_SERVERS:=AntennaBoss Observatory OTF PointingModel Refraction SkySource SolarSystem Moon FitsWriter Scheduler ActiveSurfaceLan ReceiversBoss ExternalClients CalibrationTool TotalPower CustomLogger \
				PyDewarPositioner ActiveSurfaceUSD Sardara Skarab PyLocalOscillator MFKBandBaseReceiver PyCalmux ActiveSurfaceBoss XBackend
# Same as servers
COMMON_CLIENTS:=AntennaBossTextClient ObservatoryTextClient GenericBackendTextClient ReceiversBossTextClient SystemTerminal CaltoolClient CustomLoggingClient SchedulerTextClient MeteoClient
# Misc do not depend from one another, compile in parallel
COMMON_MISC:=Plotter KStars Scripts getTemplateForTests PMUpdate InjectCommand UserTools
# Same as servers and clients
COMMON_SIMULATORS:= TCPGenericProtocolSim ReceiverBoardSim

# These 3 following groups contain the telescope-specific code (interfaces, libraris, servers, clients, etc).
# Whenever there are no dependencies inside the same group, we try to compile these modules in parallel with the common ones.
# Refer to the SRT ifeq section below to see what we group together and whatnot.
SRT_DOC:=SRTDox
SRT_ERRORS:=
SRT_INTERFACES:=SRTAntennaInterface SRTActiveSurfaceInterface SRTMinorServoInterface SRTReceiversInterface
SRT_LIBRARIES:=SRTMinorServoLibrary
SRT_SERVERS:=SRTMount SRTMinorServo SRTKBandMFReceiver SRT7GHzReceiver SRT5GHzReceiver SRTLPBandReceiver SRTPyIFDistributor WeatherStation
SRT_CLIENTS:=SRTActiveSurfaceGUIClient SRTMountTextClient MinorServoBossTextClient
SRT_MISC:=SRTScripts

MED_DOC:=MEDDox
MED_ERRORS:=
MED_INTERFACES:=MedicinaAntennaInterface MedicinaReceiversInterface MedicinaActiveSurfaceInterface
MED_LIBRARIES:=MedicinaVertexLibrary
MED_SERVERS:=MedicinaMount MedicinaMinorServo WeatherStation MedicinaPyDMed MedicinaKBandDualFReceiver
MED_CLIENTS:=MedicinaMountTextClient MedicinaActiveSurfaceGUIClient
MED_MISC:=MedScripts

NT_DOC:=NTDox
NT_ERRORS:=
NT_INTERFACES:=NotoAntennaInterface NotoReceiversInterface NotoWeatherStationInterface NotoActiveSurfaceInterface
NT_LIBRARIES:=
NT_SERVERS:= NotoReceivers NotoMount NotoWeatherStation NotoMinorServo NotoPyLocalOscillator NotoPyIfDistributor
NT_CLIENTS:=NotoMountTextClient NotoActiveSurfaceGUIClient
NT_MISC:=NotoScripts

ifeq ($(STATION),SRT)
	################################## man related section ##################################
	IDL_DOC:=Managment Antenna Backends SRTActiveSurface WeatherStation
	CPP_DOC:=Libraries AntennaImplementation SRTActiveSurfaceImplementation BackendsImplementation ReceiversImplementation WeatherStationImplementation MinorServoImplementation
	Managment_MODULES:=ManagmentInterface
	Antenna_MODULES:=AntennaInterface SRTAntennaInterface
	Backends_MODULES:=BackendsInterface
	WeatherStation_MODULES:=WeatherStationInterface SRTWeatherStationInterface
	Receivers_MODULES:=ReceiversInterface
	MinorServo_MODULES:=MinorServoInterface
	SRTActiveSurface_MODULES:=SRTActiveSurfaceInterface
	Libraries_MODULES:=IRALibrary SlaLibrary TextWindowLibrary ParserLibrary SRTMinorServoLibrary PyTestingLibrary
	AntennaImplementation_MODULES:=Observatory SkySource OTF Moon PointingModel Refraction AntennaBoss SRTMount
	ManagementImplementation_MODULES:=Scheduler CalibrationTool FitsWriter ExternalClients
	BackendsImplementation_MODULES:=TotalPower
	ReceiversImplementation_MODULES:=ReceiversBoss SRTKBandMFReceiver SRT7GHzReceiver SRTLPBandReceiver
	WeatherStationImplementation_MODULES:=SRTWeatherStation
	SRTActiveSurfaceImplementation_MODULES:=ActiveSurfaceBoss
	MinorServoImplementation_MODULES:=SRTMinorServo
	#########################################################################################
	################################# build groups section ##################################
	# Common and telescope errors can be compiled in parallel
	ERRORS:=$(COMMON_ERRORS) $(SRT_ERRORS)
	# The telescope interfaces must be compiled after the common ones
	TELESCOPE_INTERFACES:=$(SRT_INTERFACES)
	# Common and telescope libraries can be compiled in parallel, after the base common sequentials libraries
	LIBRARIES:=$(COMMON_LIBRARIES) $(SRT_LIBRARIES)
	# Telescope servers might depend on other common servers (i.e. the SRTKBandMFReceiver which depends on the MFKBandBaseReceiver or AS bosses which depend on USDs and LANs implementations)
	TELESCOPE_SERVERS:=$(SRT_SERVERS)
	# Common and telescope clients can be compiled in parallel
	CLIENTS:=$(COMMON_CLIENTS) $(SRT_CLIENTS)
	# Misc are independend from one another, therefore we compile common and telescope misc in parallel
	MISC:=$(COMMON_MISC) $(SRT_MISC)
	# Same as above
	SIMULATORS:=$(COMMON_SIMULATORS)
	#########################################################################################
else ifeq ($(STATION),Medicina)
	################################## man related section ##################################
	IDL_DOC:=Managment Antenna Backends WeatherStation Receivers
	CPP_DOC:=Libraries AntennaImplementation ManagementImplementation BackendsImplementation WeatherStationImplementation
	Managment_MODULES:=ManagmentInterface
	Antenna_MODULES:=AntennaInterface MedicinaAntennaInterface
	Backends_MODULES:=BackendsInterface XBackendInterface
	WeatherStation_MODULES:=WeatherStationInterface MedicinaWeatherStationInterface
	Receivers_MODULES:=ReceiversInterface
	Libraries_MODULES:=IRALibrary SlaLibrary TextWindowLibrary ParserLibrary PyTestingLibrary
	AntennaImplementation_MODULES:=Observatory SkySource OTF Moon PointingModel Refraction AntennaBoss MedicinaMount
	ManagementImplementation_MODULES:=Scheduler CalibrationTool FitsWriter ExternalClients
	BackendsImplementation_MODULES:=TotalPower XBackend
	ReceiversImplementation_MODULES:=ReceiversBoss
	WeatherStationImplementation_MODULES:=MedWeatherStation Metrology
	MedicinaActiveSurfaceImplementation_MODULES:=ActiveSurfaceBoss
	#########################################################################################
	################################# build groups section ##################################
	ERRORS:=$(COMMON_ERRORS) $(MED_ERRORS)
	TELESCOPE_INTERFACES:=$(MED_INTERFACES)
	LIBRARIES:=$(COMMON_LIBRARIES) $(MED_LIBRARIES)
	TELESCOPE_SERVERS:=$(MED_SERVERS)
	CLIENTS:=$(COMMON_CLIENTS) $(MED_CLIENTS)
	MISC:=$(COMMON_MISC) $(MED_MISC)
	SIMULATORS:=$(COMMON_SIMULATORS)
	#########################################################################################
else ifeq ($(STATION),Noto)
	################################## man related section ##################################
	IDL_DOC:=Managment Antenna Backends Metrology
	CPP_DOC:=Libraries AntennaImplementation ManagementImplementation BackendsImplementation ReceiversImplementation
	Managment_MODULES:=ManagmentInterface
	Antenna_MODULES:=AntennaInterface NotoAntennaInterface
	Backends_MODULES:=BackendsInterface
	Receivers_MODULES:=ReceiversInterface
	Libraries_MODULES:=IRALibrary SlaLibrary TextWindowLibrary ParserLibrary PyTestingLibrary
	AntennaImplementation_MODULES:=Observatory SkySource OTF Moon PointingModel Refraction AntennaBoss NotoMount
	ManagementImplementation_MODULES:=Scheduler CalibrationTool FitsWriter ExternalClients
	BackendsImplementation_MODULES:=TotalPower
	ReceiversImplementation_MODULES:=ReceiversBoss
	NotoActiveSurfaceImplementation_MODULES:=ActiveSurfaceBoss
	#########################################################################################
	################################# build groups section ##################################
	ERRORS:=$(COMMON_ERRORS) $(NT_ERRORS)
	TELESCOPE_INTERFACES:=$(NT_INTERFACES)
	LIBRARIES:=$(COMMON_LIBRARIES) $(NT_LIBRARIES)
	TELESCOPE_SERVERS:=$(NT_SERVERS)
	CLIENTS:=$(COMMON_CLIENTS) $(NT_CLIENTS)
	MISC:=$(COMMON_MISC) $(NT_MISC)
	SIMULATORS:=$(COMMON_SIMULATORS)
	#########################################################################################
else
	RESULT:=$(error STATION variable is not set correctly!)	
endif


# ******* END of Editable area *****************
CURRENT_DIR:=$(shell pwd)/..

define PathFinder
	$(shell for MOD in $(if $(filter undefined,$(origin $(1))),$(1),$($(1))); \
		do find ${CURRENT_DIR}/Common ${CURRENT_DIR}/${STATION} -name $${MOD} -not -path *ntroot* -not -path *CDB* -not -path *site-packages* \( -type d -o -type l \) -print 2>/dev/null; \
	done)
endef

#if the INTROOT varaible is not defined the make is stopped
ifndef INTROOT
	RESULT:=$(error INTROOT variable is not set!)
endif

#if the ACS_CDB variable is not set the make process is stopped
ifndef ACS_CDB
	RESULT:=$(error ACS_CDB variable is not set!)
endif

#set up the Modules file for each IDL and CPP documentation
$(foreach i, $(IDL_DOC), $(eval $(i)_MODULES_PATH:=$(call PathFinder,$($(i)_MODULES))))
$(foreach i, $(CPP_DOC), $(eval $(i)_MODULES_PATH:=$(call PathFinder,$($(i)_MODULES))))
#create the doxygen configuration files
$(foreach i, $(CPP_DOC), $(shell echo "INPUT=$($(i)_MODULES_PATH)" > /tmp/$(i)_cpp.in))
$(foreach i, $(IDL_DOC), $(shell echo "INPUT=$($(i)_MODULES_PATH)" > /tmp/$(i)_idl.in))

ifeq (X$(DOCROOT),X)
	DOC:=$(HOME)/docroot
else
	DOC:=$(DOCROOT)
endif

DOCIDL:=$(DOC)/idl
DOCCPP:=$(DOC)/cpp

# Sequential targets like COMMON_SEQ_LIBS have to be expanded in order to avoid unwanted parallelization.
# This order should not be changed in order for the build process to succeed. Simulators are disabled by default since we mostly rely on the discos-simulator suite.
TARGETS:=ERRORS BASE_INTERFACES CORE_INTERFACES EXTENDED_INTERFACES TELESCOPE_INTERFACES $(COMMON_SEQ_LIBS) LIBRARIES COMMON_SERVERS TELESCOPE_SERVERS CLIENTS MISC #SIMULATORS

# This function is used to dynamically declare the build and clean targets
define DeclareTarget
$1_DIRS:=$(addsuffix /src, $(call PathFinder,$1))
$1_BUILD_TARGETS:=$$($1_DIRS:%=build-%)
$1_CLEAN_TARGETS:=$$($1_DIRS:%=clean-%)

.PHONY: build-$1 $$($1_BUILD_TARGETS)
build-$1: $$($1_BUILD_TARGETS)
$$($1_BUILD_TARGETS):
	+$(MAKE) -C $$(@:build-%=%) all
	+$(MAKE) -C $$(@:build-%=%) install

.PHONY: clean-$1 $$($1_CLEAN_TARGETS)
clean-$1: $$($1_CLEAN_TARGETS)
$$($1_CLEAN_TARGETS):
	+$(MAKE) -C $$(@:clean-%=%) clean_dist
endef

# Main target, builds and installs all. Suggested to be launched with "make -j" in order to speed up the build process considerably.
.PHONY: all $(addprefix build-,$(TARGETS))
all:
	@for i in $(TARGETS); do \
		$(MAKE) build-$${i}; \
	done

# Clean target, calls clean_dist for each target inside the TARGETS variable. Can also be called with -j.
.PHONY: clean $(addprefix clean-,$(TARGETS))
clean: $(addprefix clean-,$(TARGETS))

# The following line dynamically declares all the make targets starting from the TARGETS variable
$(foreach t, $(TARGETS), $(eval $(call DeclareTarget,$(t))))

# This target cleans all the INTROOT so that a clean build process can be executed again
.PHONY: clean-dist
clean-dist:
	@echo -n "Cleaning INTROOT..."
	@rm -rf $(INTROOT)/app-defaults/*
	@rm -rf $(INTROOT)/bin/*
	@rm -rf $(INTROOT)/config/CDB/schemas/*
	@rm -rf $(INTROOT)/idl/*
	@rm -rf $(INTROOT)/include/*
	@rm -rf $(INTROOT)/lib/ACScomponents/*
	@rm -rf $(INTROOT)/lib/endorsed/*
	@find $(INTROOT)/lib/python -mindepth 1 ! -name 'site-packages' -exec rm -rf {} +
	@rm -rf $(INTROOT)/lib/python/site-packages/*
	@rm -rf $(INTROOT)/man/man*/*
	@rm -rf $(INTROOT)/Sources/*
	@rm -rf $(INTROOT)/templates/*
	@rm -rf $(INTROOT)/user/bin/*
	@echo "done!"

################################### DOC related targets #################################
.PHONY: stat
stat:
	@./slocc.sh -findopt "( -name *.cpp -o -name *.i -o -name *.h -o -name *.idl -o -name *.xml -o -name *.xsd -o -name *.py -o -name Makefile* ) \
	-not -path *object* -not -path *lib*" ..

.PHONY: man
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

.PHONY: clean-doc
clean-doc:
	@rm -rf $(DOC)/*
