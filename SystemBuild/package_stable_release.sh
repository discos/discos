#!/bin/bash -ex
BASENAME=`basename ${REPO_VERSION}`
BASEDIR=discos/${TARGETSYS}/${BASENAME}
BUILDINFO=${BASEDIR}/buildinfo.txt
BASHRC=${BASEDIR}/bashrc
source ~/.bashrc
rm -rf discos
mkdir -p ${BASEDIR}/introot
mkdir -p ${BASEDIR}/cdb
chown -R manager:acs ${BASEDIR}
cp -r ${INTROOT_PREFIX}/${TARGETSYS}/${REPO_VERSION}/STABLE/* ${BASEDIR}/introot/
cp -r ${CDB_PREFIX}/${TARGETSYS}/${REPO_VERSION}/STABLE/* ${BASEDIR}/cdb/
touch ${BUILDINFO}
echo "build id:\t${BUILD_ID}\n" >> ${BUILDINFO}
echo "build number:\t${BUILD_DISPLAY_NAME}\n" >> ${BUILDINFO}
echo "build tag:\t${BUILD_TAG}\n" >> ${BUILDINFO}
echo "svn revision:\t${SVN_REVISION}\n" >> ${BUILDINFO}
echo "svn url:\t${SVN_URL}\n" >> ${BUILDINFO}
echo "******************************************************\n" >> ${BUILDINFO}
echo "* Azdora build parameters                            *\n" >> ${BUILDINFO}
echo "******************************************************\n" >> ${BUILDINFO}
echo "INTROOT_PREFIX:\t${INTROOT_PREFIX}\n" >> ${BUILDINFO}
echo "CDB_PREFIX:\t${CDB_PREFIX}\n" >> ${BUILDINFO}
echo "TARGETSYS:\t${TARGETSYS}\n" >> ${BUILDINFO}
echo "REPO_VERSION:\t${REPO_VERSION}\n" >> ${BUILDINFO}
touch ${BASHRC}
echo "export INTROOT=/${BASEDIR}/introot" >> ${BASHRC}
echo "export ACS_CDB=/${BASEDIR}/cdb" >> ${BASHRC}
echo "export TARGETSYS=${TARGETSYS}" >> ${BASHRC}
echo "export REPO_VERSION=${REPO_VERSION}" >> ${BASHRC}
tar czvf discos-${TARGETSYS}-${BASENAME}-b${BUILD_NUMBER}.tar.gz discos
