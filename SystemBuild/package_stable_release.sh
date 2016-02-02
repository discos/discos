#!/bin/bash -ex
BASENAME=`basename ${REPO_VERSION}`
BASEDIR=discos/${TARGETSYS}/${BASENAME}
BUILDINFO=${BASEDIR}/buildinfo.txt
BASHRC=${BASEDIR}/bashrc
source ~/.bashrc
if [ -d "$DISCOS_CDB" ]; then
    echo "replacing $ACS_CDB with $DISCOS_CDB"
    ACS_CDB=$DISCOS_CDB
fi
rm -rf discos
mkdir -p ${BASEDIR}/introot
mkdir -p ${BASEDIR}/cdb
chown -R manager:acs ${BASEDIR}
cp -r ${INTROOT_PREFIX}/${TARGETSYS}/${REPO_VERSION}/STABLE/* ${BASEDIR}/introot/
cp -r ${CDB_PREFIX}/${TARGETSYS}/${REPO_VERSION}/STABLE/* ${BASEDIR}/cdb/
touch ${BUILDINFO}
printf "build id:\t${BUILD_ID}\n" >> ${BUILDINFO}
printf "build number:\t${BUILD_DISPLAY_NAME}\n" >> ${BUILDINFO}
printf "build tag:\t${BUILD_TAG}\n" >> ${BUILDINFO}
printf "svn revision:\t${SVN_REVISION}\n" >> ${BUILDINFO}
printf "svn url:\t${SVN_URL}\n" >> ${BUILDINFO}
printf "******************************************************\n" >> ${BUILDINFO}
printf "* Azdora build parameters                            *\n" >> ${BUILDINFO}
printf "******************************************************\n" >> ${BUILDINFO}
printf "INTROOT_PREFIX:\t${INTROOT_PREFIX}\n" >> ${BUILDINFO}
printf "CDB_PREFIX:\t${CDB_PREFIX}\n" >> ${BUILDINFO}
printf "TARGETSYS:\t${TARGETSYS}\n" >> ${BUILDINFO}
printf "REPO_VERSION:\t${REPO_VERSION}\n" >> ${BUILDINFO}
touch ${BASHRC}
printf "export INTROOT=/${BASEDIR}/introot\n" >> ${BASHRC}
printf "export ACS_CDB=/${BASEDIR}/cdb\n" >> ${BASHRC}
printf "export TARGETSYS=${TARGETSYS}\n" >> ${BASHRC}
printf "export REPO_VERSION=${REPO_VERSION}\n" >> ${BASHRC}
tar czvf discos-${TARGETSYS}-${BASENAME}-b${BUILD_NUMBER}.tar.gz discos
