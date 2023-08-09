#! /bin/bash

ssh -f -l discos manager "/bin/bash --login -c 'acsStopContainer \
SupervisorContainer > /dev/null 2>&1 &'"
echo "Stopping SupervisorContainer"

ssh -f -l discos manager "/bin/bash --login -c 'acsStopContainer \
WeatherStationContainer > /dev/null 2>&1 &'"
echo "Stopping WeatherStationContainer"

ssh -f -l discos manager "/bin/bash --login -c 'acsStopContainer \
MountContainer > /dev/null 2>&1 &'"
echo "Stopping MountContainer"

ssh -f -l discos manager "/bin/bash --login -c 'acsStopContainer \
ManagementContainer > /dev/null 2>&1 &'"
echo "Stopping ManagementContainer"

ssh -f -l discos manager "/bin/bash --login -c 'acsStopContainer \
AntennaContainer > /dev/null 2>&1 &'"
echo "Stopping AntennaContainer"

ssh -f -l discos manager "/bin/bash --login -c 'acsStopContainer \
AntennaBossContainer > /dev/null 2>&1 &'"
echo "Stoping AntennaBossContainer"
