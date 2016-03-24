#!/bin/bash

PRODUCTION=""
while getopts "p" opt; do
    case $opt in
      p)
        echo "compiling in production mode"
        PRODUCTION="OPTIMIZE=0"
        ;;
    esac
done

source ~/.bashrc
rm -rf $INTROOT
mkdir -p $INTROOT
getTemplateForDirectory INTROOT $INTROOT
cd SystemMake
make $PRODUCTION common-clean common-build
