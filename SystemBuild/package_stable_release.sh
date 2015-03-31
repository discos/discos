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
touch buildinfo.txt
echo "build id:\t${BUILD_ID}\n" >> buildinfo.txt
echo "build number:\t${BUILD_DISPLAY_NAME}\n" >> buildinfo.txt
echo "build tag:\t${BUILD_TAG}\n" >> buildinfo.txt
echo "svn revision:\t${SVN_REVISION}\n" >> buildinfo.txt
echo "svn url:\t${SVN_URL}\n" >> buildinfo.txt
echo "******************************************************\n" >> buildinfo.txt
echo "* Azdora build parameters                            *\n" >> buildinfo.txt
echo "******************************************************\n" >> buildinfo.txt
echo "INTROOT_PREFIX:\t${INTROOT_PREFIX}\n" >> buildinfo.txt
echo "CDB_PREFIX:\t${CDB_PREFIX}\n" >> buildinfo.txt
echo "TARGETSYS:\t${TARGETSYS}\n" >> buildinfo.txt
echo "REPO_VERSION:\t${REPO_VERSION}\n" >> buildinfo.txt
tar czvf discos-${TARGETSYS}-${BASENAME}-b${BUILD_NUMBER}.tar.gz discos buildinfo.txt
