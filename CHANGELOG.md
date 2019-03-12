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

  issue #324 - full support for CHC receiver at Medicina telescope now added.The receiver will be avaiable 
	under two diffent configurations: 1.2 GHz and 150MHz bandwidth. The respective setup are achived by issueing
	the following procedures "SETUPCHC" and "SETUPCHCL".  

### Changed

	issue #316 - several related commits. The operation releated to the Medicina K band receivers now relys on a
	new componet which is derived directly form the SRT one. larg part of the code is shared between the two
	servants.

### Fixed 

## 


