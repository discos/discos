#!/bin/sh

unset ACS_CDB
unset INTROOT

# Unlock the trunk
if [ -f $ACS_TMP/nuraghetrunk.lock ] 
   then
   rm $ACS_TMP/nuraghetrunk.lock
   touch $ACS_TMP/nuraghetag.lock
fi

# Unlock the branch
if [ -f $ACS_TMP/nuraghebranch.lock ] 
   then
   rm $ACS_TMP/nuraghebranch.lock
   touch $ACS_TMP/nuraghetag.lock
fi

export INTROOT=$NURAGHEROOT/introot
export NURAGHEVERSION=TAG-$NURAGHETAG
source $HOME/.acs/.bash_profile.acs
export ACS_CDB=$NURAGHEROOT/ACS/tags/$NURAGHETAG/SRT/Configuration

# Print the actual setup
source $NURAGHESETUPBIN/printNuragheSetup.sh

