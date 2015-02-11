#!/bin/bash

source ~/.bashrc
rm -rf $INTROOT
mkdir -p $INTROOT
getTemplateForDirectory INTROOT $INTROOT
cd SystemMake
make common-clean common-build
