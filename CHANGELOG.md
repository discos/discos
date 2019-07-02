# Change Log
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/)
and this project adheres to [Semantic Versioning](http://semver.org/

## [discos1.0] 07-11-2018
### Added
	
	issue #261 - Added startup procedure scripts also for Noto and Medicina. The startup command is 
	'discos --start'. Also command line scripts are now avialble 'discosup' and 'discosdown'. The console
	can be start with 'discosConsole --start'. 
	
	issue #325 - added command 'dmed=config' to control the Medicina DMed (IF distributor). The device just 
	allows to setup a set of programmable attenuators in order to control the singal levels. config select
	which configuration to apply between a set of predefined one.
	
	issue #323 - The metClient moved to common part as it is now the general cleint for weather stations. 
	Also the autopark wind threshld is now read from confiugration and not hardcoded.

	issue #324 - full support for CHC receiver at Medicina telescope now added.The receiver will be available
	under two diffent configurations: 1.2 GHz and 150MHz bandwidth. The respective setup are achived by issueing
	the following procedures "SETUPCHC" and "SETUPCHCL".  

	issue #361 - written the PyCalMux component and integrated into the Noto line. Station procedures now execute
	the `calmux=<configuration>` script in order to commute to the TotalPower noise calibration channel.

	issue #368 - added a `calmux` command to the OperatorInput commands list. Internally it calls the `setup`
	method of the PyCalmux component.

	issue #369 - Now the Noto StationProcedures performs a default setup to the CalMux device letting it set the
	calibration noise mark coming from TotalPower as the default one.

    issue #419 - Completed integration and configuration of new primary focus receiver SXL, in Noto.

### Changed

	issue #316 - several related commits. The operation releated to the Medicina K band receivers now relys on a
	new componet which is derived directly form the SRT one. larg part of the code is shared between the two
	servants.
	
	issue #301 - Noto Weather station has now Generic interface. in this way the MeteoClient can be used in Noto
	as wll as Medicina and SRT.

### Fixed 

	issue #366 - Now the PyCalmux component is kept alive after it gets instanced for the first time in order to
	avoid setting the default values to the controlled device every time the component is retrieved from some
	outside code/program.

	issue #367 - The PyCalmux component now uses a smarter way to open a socket to the CalMux device in order to
	avoid raising unwanted socket errors.

## 
