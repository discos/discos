# Change Log
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/)
and this project adheres to [Semantic Versioning](http://semver.org/

## [discos1.0] 07-11-2018
### Added

    issue #261 - Added startup procedure scripts also for Noto and Medicina. The startup command is 'discos --start'. Also command line scripts are now avialble 'discosup' and 'discosdown'. The console can be start with 'discosConsole --start'.

    issue #325 - Added command 'dmed=<config>' to control the Medicina DMed (IF distributor). The device only allows to setup a set of programmable attenuators in order to control the singal levels. Argument 'config' selects which configuration to apply between a set of predefined one.

    issue #323 - The metClient moved to common part as it is now the general client for weather stations. Also the autopark wind threshold is now read from configuration instead of being hardcoded.

    issue #324 - Full support for CHC receiver at Medicina telescope now added. The receiver will be available under two diffent configurations: 1.2GHz and 150MHz bandwidth. The respective setup are achieved by issuing the following procedures "SETUPCHC" and "SETUPCHCL".

    issue #361 - Written the PyCalMux component and integrated into the Noto line. Station procedures now execute the 'calmux=<configuration>' script in order to commute to the TotalPower noise calibration channel.

    issue #368 - Added a 'calmux' command to the OperatorInput commands list. Internally it calls the 'setup' method of the PyCalmux component.

    issue #369 - Now the Noto StationProcedures perform a default setup to the CalMux device letting it set the calibration noise mark coming from TotalPower as the default one.

    issue #419 - Completed integration and configuration of new primary focus receiver SXL, in Noto.

### Changed

    issue #316 - Several related commits. The operation releated to the Medicina K band receivers now relies on a new component directly derived from the SRT one. Large part of the code is shared between the two servants.

    issue #301 - Noto Weather station has now a generic interface. In this way the MeteoClient can be used in Noto as wll as Medicina and SRT.

### Fixed

    issue #366 - Now the PyCalmux component is kept alive after it gets instanced for the first time in order to avoid setting the default values to the controlled device every time the component is retrieved from some outside code/program.

    issue #367 - The PyCalmux component now uses a smarter way to open a socket to the CalMux device in order to avoid raising unwanted socket errors.


## [discos1.0.2] 01-08-2019
### Added
### Changed

    issue #423 - The SRT Active Surface now is monitored by 8 threads (the same threads that perform its initialization). The USDs statuses are updated inside these threads (roughly once every 15 seconds). The `usdStatus4GUI` method now returns the last read status of the given USD instead of interrogating the hardware in real time. The changes brought to the SRT Active Surface allowed its GUI to be improved. Since now it asks for the last known status of every USD without interrogating the hardware in real time, the thread is able to go from USD to USD in 10ms instead of 100ms. A complete loop of the USDs now lasts less than 15 seconds, the UI therefore appears much more responsive to the users, allowing them to send commands to the AS earlier than before (the startup time of the whole AS is approximately the same, but the UI catches up more quickly when the ASBoss is ready).

    issue #404 - Minor fixes were introduced in the SRT test CDB.

    feature - The TimeTaggedCircularArray class is now thread safe.

### Fixed

    issue #431 - The AntennaBoss WatchingThread is now free to read updated values from the mount component without starving. This was tested in the SRT environment with the ACU simulator. No tests were carried on in the Medicina/Noto environments, but due to the different socket nature of the ACU, this should not be an issue. In a simulated environment, a time gap in the coordinates set happens roughly once every 100000 readings (during source tracking).

    issue #411 - The WeatherStation component `getData` method now returns the last known set of weather parameters, without interrogating the hardware every time it was called. Previously, having N clients calling this method resulted in having N times the socket busy. One of the clients was for example the Refraction component, which introduced additional latencies and time gaps into the AntennaBoss WatchingThread.

    issue #382 - The Refraction `getCorrection` method now returns a VALUE of 0 when the antenna is pointing to the zenith (or when the elvation is encoded with more than 90 degrees). Previously, the method returned a POINTER equal to 0, which surprisingly never caused any issue other than flooding the jlog with a warning message.

    issue #402 - Fixed an issue in the ReceiversBossTui that was preventing the UI to find any derotator in the system.

## [discos1.0.3] 25-10-2019
### Added
### Changed
    issue #454 - Timestamp of the Fits now is referred to the middle of the sample according to all others metadata
    issue #455 - The SRT Active Surface GUI has been splitted into a management GUI and a user GUI
    issue #459 - The DiscosConsole layout has been reviewed. The console windows now open without overlapping
    issue #463 - X band receiver pointing model updated
    issue #469 - XBackend initialization and setup procedure improved
    some other minor refinements
### Fixed
    issue #452 - The Total Power component erroneously assigned a timestamp to the samples in case more samples were needed to reach total integration time
    issue #473 - The SRT AS USDs now retry 5 times to communicate with the hardware before setting their state to unavailable

## [discos1.0.4] 03-02-2020
### Added
    issue #505 - CALMUX integrated and tested into the SRT production line (it has been successfully tested with DBBC and DFB backends)
### Changed
    issue #477 - removed the SecureArea from some AntennaBoss methods
    issue #476 - replaced all occurrencies of DPI/2 with the defined variable DPIBY2, this should improve precision
    issue #489 - new focus curve for SRT X band receiver
    issue #506 - each SRT local oscillator now has its dedicated container
    issue #510 - fixed SRT TotalPower CDB configuration in order to use the correct IP address
### Fixed
    issue #433 - limited refraction correction values to a meaningful range in order to avoid flooding the jlog with warning messages whenever the elevation is close to 90 degrees
    issue #502 - removed last service daemon reference from Manager.xml io SRT production line
    
## [discos1.0.5] - 
## Added
	issue #504 - added credits clause (regarding INAF data ownership) to fits files headers
    issue #518 - KBand receivers cryo temperature read wrongly with connection problems. More information:
    https://github.com/discos/discos/issues/518#issuecomment-590838480

## Fixed
    issue #518 - In case of communication error we set a dummy value (100000) for the temperature properties, and the related timestamp keeps the value of the last communication timestamp.
## Changed    
