#! /bin/bash

ssh -f -l discos manager "/bin/bash --login -c 'acsStartContainer -cpp \
AntennaBossContainer > /dev/null 2>&1 &'"
echo "...AntennaBossContainer!"

ssh -f -l discos manager "/bin/bash --login -c 'acsStartContainer -cpp \
AntennaContainer > /dev/null 2>&1 &'"
echo "...AntennaContainer!"

ssh -f -l discos manager "/bin/bash --login -c 'acsStartContainer -cpp \
ManagementContainer > /dev/null 2>&1 &'"
echo "...ManagementContainer!"

ssh -f -l discos manager "/bin/bash --login -c 'acsStartContainer -cpp \
MountContainer > /dev/null 2>&1 &'"
echo "...MountContainer!"

ssh -f -l discos manager "/bin/bash --login -c 'acsStartContainer -cpp \
WeatherStationContainer > /dev/null 2>&1 &'"
echo "...WeatherStationContainer!"

ssh -f -l discos manager "/bin/bash --login -c 'acsStartContainer -py \
SupervisorContainer > /dev/null 2>&1 &'"
echo "...SupervisorContainer!"
