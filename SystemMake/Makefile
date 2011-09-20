#*******************************************************************************
#
# "@(#) $Id: Makefile,v 1.67 2011-06-24 08:14:23 c.migoni Exp $"
#
# Makefile 
#
# who       when      what
# --------  --------  ----------------------------------------------
# Andrea O. 26/10/07  created
# Andrea O. 21/03/09  added the target cdb in order to checkout the runtime database
# Andrea O. 15/04/11  match now the present status of the cvs

ifeq ($(TARGETSYS),SRT)
	#Add all modules included in the SRT system  in this sequence
	#Doc, (Errors, Interfacies, Libraries, Servers, Clients, Misc)
	MODULES:=SRTDox \
		ClientErrors ComponentErrors AntennaErrors ParserErrors BackendsErrors ManagementErrors ReceiversErrors MetrologyErrors MinorServoErrors \
		SRTActiveSurfaceErrors \
		ManagmentInterface AntennaInterface ReceiversInterface BackendsInterface MinorServoInterface WeatherStationInterface \
		SRTAntennaInterface SRTActiveSurfaceInterface SRTWeatherStationInterface SRTReceiversInterface \
		SlaLibrary IRALibrary TextWindowLibrary ParserLibrary SRTMinorServoLibrary \
		AntennaBoss Observatory OTF PointingModel Refraction SkySource Moon FitsWriter Scheduler ReceiversBoss ExternalClients CalibrationTool \
		TotalPower NoiseGenerator DBBC SRTMount SRTActiveSurfaceLanServer SRTActiveSurfaceUSDServer SRTActiveSurfaceBoss SRTMinorServo SRTKBandMFReceiver SRTWeatherStation \
		SRT7GHzReceiver \
		AntennaBossTextClient ObservatoryTextClient GenericBackendTextClient ReceiversBosstextClient SystemTerminal \
		SRTActiveSurfaceGUIClient SRTMountTextClient \
		Plotter KStars Scripts \

	IDL_DOC:=Managment Antenna Backends SRTActiveSurface
	CPP_DOC:=Libraries AntennaImplementation SRTActiveSurfaceImplementation

	Managment_MODULES:=ManagmentInterface
	Antenna_MODULES:=AntennaInterface SRTAntennaInterface
	Backends_MODULES:=BackendsInterface
	WeatherStation_MODULES:=WeatherStationInterface SRTWeatherStationInterface
	Receivers_MODULES:=ReceiversInterface
	MinorServo_MODULES:=MinorServoInterface
	SRTActiveSurface_MODULES:=SRTActiveSurfaceInterface
	Libraries_MODULES:=IRALibrary SlaLibrary TextWindowLibrary ParserLibrary SRTMinorServoLibrary
	AntennaImplementation_MODULES:=Observatory SkySource OTF Moon PointingModel Refraction AntennaBoss SRTMount
	ManagementImplementation_MODULES:=Scheduler CalibrationTool FitsWriter ExternalClients
	BackendsImplementation_MODULES:=TotalPower NoiseGenerator
	ReceiversImplementation_MODULES:=ReceiversBoss SRTKBandMFReceiver SRT7GHzReceiver DBBC
	WeatherStationImplementation_MODULES:=SRTWeatherStation
	SRTActiveSurfaceImplementation_MODULES:=SRTActiveSurfaceLanServer SRTActiveSurfaceUSDServer SRTActiveSurfaceBoss 
	MinorServoImplementation_MODULES:=SRTMinorServo
	
	CDB_LOCAL_LOCATION="SRT/CDB"
	SYSTEM_CVS_MODULES=Common SRT 
else 
ifeq ($(TARGETSYS),MED)
	#Add all modules included in the Medicina system in this sequence
	#Doc, (Errors, Interfacies, Libraries, Servers, Clients, Misc)
	MODULES:=MEDDox \
		ClientErrors ComponentErrors AntennaErrors ParserErrors BackendsErrors ManagementErrors ReceiversErrors MetrologyErrors MinorServoErrors\
		ManagmentInterface AntennaInterface ReceiversInterface BackendsInterface MinorServoInterface WeatherStationInterface \
		MedicinaAntennaInterface MedicinaWeatherStationInterface \
		SlaLibrary IRALibrary TextWindowLibrary ParserLibrary \
		AntennaBoss Observatory OTF PointingModel Refraction SkySource Moon FitsWriter Scheduler ReceiversBoss ExternalClients CalibrationTool \
		Metrology TotalPower NoiseGenerator HolographyBackend DBBC MedicinaMount MedWeatherStation \
		AntennaBossTextClient ObservatoryTextClient GenericBackendTextClient ReceiversBosstextClient SystemTerminal \
		MedicinaMountTextClient MedicinaMountGUIClient MedicinaGeneralControlSystemGUIClient \
		Plotter KStars Scripts \
		MedScripts

	IDL_DOC:=Managment Antenna Backends Metrology
	CPP_DOC:=Libraries AntennaImplementation 

	Managment_MODULES:=ManagmentInterface
	Antenna_MODULES:=AntennaInterface MedicinaAntennaInterface
	Backends_MODULES:=BackendsInterface
	WeatherStation_MODULES:=WeatherStationInterface MedicinaWeatherStationInterface
	Receivers_MODULES:=ReceiversInterface
	Libraries_MODULES:=IRALibrary SlaLibrary TextWindowLibrary ParserLibrary
	AntennaImplementation_MODULES:=Observatory SkySource OTF Moon PointingModel Refraction AntennaBoss MedicinaMount
	ManagementImplementation_MODULES:=Scheduler CalibrationTool FitsWriter ExternalClients
	BackendsImplementation_MODULES:=TotalPower NoiseGenerator HolographyBackend DBBC
	ReceiversImplementation_MODULES:=ReceiversBoss
	WeatherStationImplementation_MODULES:=MedWeatherStation Metrology
	
	CDB_LOCAL_LOCATION="Medicina/CDB"
	SYSTEM_CVS_MODULES=Common Medicina
else
	RESULT:=$(error TARGETSYS variable is not set correctly!)	
endif
endif


# ******* END of Editable area *****************

CURRENT_DIR:=$(shell pwd)/..

define PathFinder
	$(shell for MOD in $(1); do find ${CURRENT_DIR} -name $${MOD} -not -path *CDB* -not -path *site-packages* -type d -print; done)
endef

#if the INTROOT varaible is not defined the make is stopped
ifndef INTROOT
	RESULT:=$(error INTROOT variable is not set!)
endif

#if the ACS_CDB variable is not set the make process is stopped
ifndef ACS_CDB
	RESULT:=$(error ACS_CDB variable is not set!)
endif

#if the ACS_CDB variable is not set the make process is stopped
ifndef SYSLOCATION
	RESULT:=$(error SYSLOCATION variable is not set!)
endif

#set up the Modules file for each IDL and CPP documentation
$(foreach i, $(IDL_DOC), $(eval $(i)_MODULES_PATH:=$(call PathFinder,$($(i)_MODULES))))
$(foreach i, $(CPP_DOC), $(eval $(i)_MODULES_PATH:=$(call PathFinder,$($(i)_MODULES))))
#create the doxygen configuration files
$(foreach i, $(CPP_DOC), $(shell echo "INPUT=$($(i)_MODULES_PATH)" > /tmp/$(i)_cpp.in))
$(foreach i, $(IDL_DOC), $(shell echo "INPUT=$($(i)_MODULES_PATH)" > /tmp/$(i)_idl.in))

COMPILE_MODULES_PATH:=$(call PathFinder,$(MODULES))

BUILD_DIRS:=$(foreach i, $(COMPILE_MODULES_PATH), $(i)/src)

ifeq (X$(DOCROOT),X)
	DOC:=$(HOME)/docroot
else
	DOC:=$(DOCROOT)
endif


DOCIDL:=$(DOC)/idl
DOCCPP:=$(DOC)/cpp

all:
	@for i in $(BUILD_DIRS); do \
		make -C $${i} all install ; \
	done

compile:
	@for i in $(BUILD_DIRS); do \
		make -C $${i} all ; \
	done

install:
	@for i in $(BUILD_DIRS); do \
		make -C $${i} install ; \
	done

cdb:
	@cd $(ACS_CDB); cvs -d $(CVSROOT) co -P -d "CDB" -r "RUNTIME" $(CDB_LOCAL_LOCATION)

sources:
	@cd $(SYSLOCATION) ; for i in $(SYSTEM_CVS_MODULES) ; do \
		cvs -d $(CVSROOT) -r co -P $${i} ; \
	done

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

clean:
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
