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
if [ ! -d "$INTROOT" ]; then
    echo "cannot find INTROOT in $INTROOT"
    exit 1
fi
cd SystemMake
make $PRODUCTION telescope-clean telescope-build
