#!/bin/sh
# Echo only in interactive shell
if tty -s
then 
  echo "Nuraghe Version: " $NURAGHEVERSION
  echo "CDB set to" $ACS_CDB
  echo "introot set to" $INTROOT
fi

