#!/bin/bash -ex
BASEDIR=discos/${TARGETSYS}/${REPO_VERSION}
BASENAME=`basename ${REPO_VERSION}`
source ~/.bashrc
rm -rf discos
mkdir -p ${BASEDIR}/introot
mkdir -p ${BASEDIR}/cdb
chown -R manager:acs ${BASEDIR}
cp -r ${INTROOT_PREFIX}/${TARGETSYS}/${REPO_VERSION}/STABLE/* ${BASEDIR}/introot/
cp -r ${CDB_PREFIX}/${TARGETSYS}/${REPO_VERSION}/STABLE/* ${BASEDIR}/cdb/
touch ${BASEDIR}/buildinfo.txt
cat "build id:\t${BUILD_ID}\n" >> ${BASEDIR}/buildinfo.txt
cat "build number:\t${BUILD_DISPLAY_NAME}\n" >> ${BASEDIR}/buildinfo.txt
cat "build tag:\t${BUILD_TAG}\n" >> ${BASEDIR}/buildinfo.txt
cat "svn revision:\t${SVN_REVISION}\n" >> ${BASEDIR}/buildinfo.txt
cat "svn url:\t${SVN_URL}\n" >> ${BASEDIR}/buildinfo.txt
cat "******************************************************\n"
cat "* Azdora build parameters                            *\n"
cat "******************************************************\n"
cat "INTROOT_PREFIX:\t${INTROOT_PREFIX}\n" >> ${BASEDIR}/buildinfo.txt
cat "CDB_PREFIX:\t${CDB_PREFIX}\n" >> ${BASEDIR}/buildinfo.txt
cat "TARGETSYS:\t${TARGETSYS}\n" >> ${BASEDIR}/buildinfo.txt
cat "REPO_VERSION:\t${REPO_VERSION}\n" >> ${BASEDIR}/buildinfo.txt
tar czvf discos-${TARGETSYS}-${BASENAME}-b${BUILD_NUMBER}.tar.gz introot cdb buildinfo.txt
