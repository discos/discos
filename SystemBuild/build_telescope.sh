#!/bin/bash

source ~/.bashrc
if [ ! -d "$INTROOT" ]; then
    echo "cannot find INTROOT in $INTROOT"
    exit 1
fi
cd SystemMake
make telescope-clean telescope-build
