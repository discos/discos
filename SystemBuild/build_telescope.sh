#!/bin/bash

PRODUCTION=""
while getopts "p" opt; do
    case $opt in
      p)
        echo "compiling in production mode"
        PRODUCTION="DEBUG=''"
        ;;
    esac
done

source ~/.bashrc
if [ -d "$DISCOS_CDB"]; then
    echo "replacing $ACS_CDB with $DISCOS_CDB"
    ACS_CDB=$DISCOS_CDB
fi
if [ ! -d "$INTROOT" ]; then
    echo "cannot find INTROOT in $INTROOT"
    exit 1
fi
cd SystemMake
make $PRODUCTION telescope-clean telescope-build
