#!/bin/sh

unset ACS_CDB
unset INTROOT

# Unlock the tag
if [ -f $ACS_TMP/nuraghetag.lock ] 
   then
   rm $ACS_TMP/nuraghetag.lock
   touch $ACS_TMP/nuraghetrunk.lock
fi

# Unlock the branch
if [ -f $ACS_TMP/nuraghebranch.lock ] 
   then
   rm $ACS_TMP/nuraghebranch.lock
   touch $ACS_TMP/nuraghetrunk.lock
fi

export INTROOT=$NURAGHEROOT/introotTrunk
export NURAGHEVERSION=TRUNK
source $HOME/.acs/.bash_profile.acs
export ACS_CDB=$NURAGHEROOT/ACS/trunk/SRT/Configuration

# Print the actual setup
source $NURAGHESETUPBIN/printNuragheSetup.sh
