# Change Log
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/)
and this project adheres to [Semantic Versioning](http://semver.org/

## [discos1.0.2] 01-08-2019
### Added
### Changed

    - The TimeTaggedCircularArray class is now thread safe.
    - The SRT Active Surface now is monitored by 8 threads (the same threads that perform its initialization). The USDs statuses are updated inside these threads (roughly once every 15 seconds). The `usdStatus4GUI` method now returns the last read status of the given USD instead of interrogating the hardware in real time.
    - The changes brought to the SRT Active Surface allowed its GUI to be improved. Since now it asks for the last known status of every USD without interrogating the hardware in real time, the thread is able to go from USD to USD in 10ms instead of 100ms. A complete loop of the USDs now lasts less than 15 seconds, the UI therefore appears much more responsive to the users, allowing them to send commands to the AS earlier than before (the startup time of the whole AS is approximately the same, but the UI catches up more quickly when the ASBoss is ready).
    - Minor fixes were introduced in the SRT test CDB.

### Fixed

    - The AntennaBoss WatchingThread is now free to read updated values from the mount component without starving. This was tested in the SRT environment with the ACU simulator. No tests were carried on in the Medicina/Noto environments, but due to the different socket nature of the ACU, this should not be an issue. In a simulated environment, a time gap in the coordinates set happens roughly once every 100000 readings (during source tracking).
    - The WeatherStation component `getData` method now returns the last known set of weather parameters, without interrogating the hardware every time it was called. Previously, having N clients calling this method resulted in having N times the socket busy. One of the clients was for example the Refraction component, which introduced additional latencies and time gaps into the AntennaBoss WatchingThread.
    - The Refraction `getCorrection` method now returns a VALUE of 0 when the antenna is pointing to the zenith (or when the elvation is encoded with more than 90 degrees). Previously, the method returned a POINTER equal to 0, which surprisingly never caused any issue other than flooding the jlog with a warning message.
    - Fixed an issue in the ReceiversBossTui that was preventing the UI to find any derotator in the system.

##
