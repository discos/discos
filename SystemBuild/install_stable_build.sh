#!/bin/bash -ex
source ~/.bashrc
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
