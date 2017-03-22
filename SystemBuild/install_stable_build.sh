#!/bin/bash -ex
source ~/.bashrc
if [ -d "$DISCOS_CDB" ]; then
    echo "replacing $ACS_CDB with $DISCOS_CDB"
    ACS_CDB=$DISCOS_CDB
fi
NEW_INTROOT=${INTROOT_PREFIX}/${TARGETSYS}/${REPO_VERSION}/STABLE
NEW_CDB=${CDB_PREFIX}/${TARGETSYS}/${REPO_VERSION}/STABLE
rm -rf $NEW_INTROOT
rm -rf $NEW_CDB
mkdir -p $NEW_INTROOT
chmod -R u+rwx $NEW_INTROOT
cp -r $INTROOT/* $NEW_INTROOT/
mkdir -p $NEW_CDB
chmod -R u+rwx $NEW_CDB
cp -r ${ACS_CDB}/CDB ${NEW_CDB}/
echo "Installed new stable version of $TARGETSYS"
