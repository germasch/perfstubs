#!/bin/bash -e
# Copyright (c) 2019-2020 University of Oregon
# Distributed under the BSD Software License
# (See accompanying file LICENSE.txt)

# where is this script?
if [ -z ${scriptdir} ] ; then
    scriptdir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
fi

workdir="$( dirname "${scriptdir}" )"
echo $workdir

UNAME=`uname`
FORTRAN_COMPILER=""
if [ ${UNAME} != "Darwin" ] ; then
    FORTRAN_COMPILER="-DCMAKE_Fortran_COMPILER=`which gfortran`"
fi

export CFLAGS="-Wall -Werror"
export CXXFLAGS="-Wall -Werror"

do_build() {
    echo "Removing build_${linktype}_${buildtype} and install_${linktype}_${buildtype}"
    rm -rf ${workdir}/build_${linktype}_${buildtype} ${workdir}/install_${linktype}_${buildtype}
    mkdir ${workdir}/build_${linktype}_${buildtype}
    cd ${workdir}/build_${linktype}_${buildtype}

    set -x
    cmake \
    -DCMAKE_C_COMPILER=`which gcc` \
    -DCMAKE_CXX_COMPILER=`which g++` \
    ${FORTRAN_COMPILER} \
    -DCMAKE_BUILD_TYPE=${buildtype} \
    -DPERFSTUBS_SANITIZE=${sanitize} \
    -DPERFSTUBS_USE_STATIC=${staticflag} \
    ..
    set +x
    make -j
}

buildtype=Debug
sanitize=ON
linktype=dynamic
staticflag=OFF
do_build
buildtype=Release
sanitize=OFF
do_build

buildtype=Debug
sanitize=ON
linktype=static
staticflag=ON
do_build
buildtype=Release
sanitize=OFF
do_build


