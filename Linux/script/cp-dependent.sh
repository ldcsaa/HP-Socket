#!/bin/bash

if [ $# -eq 0 ];then
	echo "Usage: $0" "[jemalloc|mimalloc|openssl|zlib|brotli]"
	exit 1
fi

PACKAGE_PATH=$(cd "$(dirname "$0")"; pwd)

SRC_BASE=/usr/local
DEST_BASE=$PACKAGE_PATH/../dependent

check_platform()
{
	PLATFORM=
	
	if [ "$1" == "x86_64" ]; then
		PLATFORM="x64"
	elif [[ "$1" == "x86" || "$1" == "i686" || "$1" == "i586" || "$1" == "i386" ]]; then
		PLATFORM="x86"
	elif [[ "$1" == "arm64" || "$1" == "aarch64" || "$1" =~ "armv8"  || "$1" =~ "armv9" ]]; then
		PLATFORM="arm64"
	elif [[ "$1" == "arm" || "$1" == "aarch" || "$1" =~ "armv7" ]]; then
		PLATFORM="arm"
	fi
}

do_copy()
{
	local _LIB_NAME=$1
	local _RM_FILES=
	local _LIB_FIX=
	
	if [ $_LIB_NAME == "jemalloc" ]; then
		_LIB_FIX=_pic
		_RM_FILES="$PLATFORM/lib/lib$_LIB_NAME* $PLATFORM/include/$_LIB_NAME*"
	elif [ $_LIB_NAME == "mimalloc" ]; then
		_RM_FILES="$PLATFORM/lib/lib$_LIB_NAME* $PLATFORM/include/$_LIB_NAME*"
	elif [ $_LIB_NAME == "openssl" ]; then
		_RM_FILES="$PLATFORM/lib/libssl* $PLATFORM/lib/libcrypto* $PLATFORM/include/$_LIB_NAME*"
	elif [ $_LIB_NAME == "zlib" ]; then
		_RM_FILES="$PLATFORM/lib/libz* $PLATFORM/include/zconf.h $PLATFORM/include/zlib.h"
	elif [ $_LIB_NAME == "brotli" ]; then
		_RM_FILES="$PLATFORM/lib/lib$_LIB_NAME* $PLATFORM/include/$_LIB_NAME*"
	fi
	
	if [ -n "$_RM_FILES" ]; then
		rm -rf $_RM_FILES
	fi
	
	mkdir -p $PLATFORM/lib $PLATFORM/include

	cp -rf $SRC_BASE/$_LIB_NAME/include/* $PLATFORM/include
	cp -rf $SRC_BASE/$_LIB_NAME/lib/*$_LIB_FIX.a $PLATFORM/lib
}

mkdir -p $DEST_BASE
cd $DEST_BASE

check_platform "$(arch | tr "[A-Z]" "[a-z]")"

if [ -z $PLATFORM ]; then

	check_platform "$(uname -p | tr "[A-Z]" "[a-z]")"
	
	if [ -z $PLATFORM ]; then
		printf "Can not determine Arch Platform\n"
		exit 1
	fi
fi

for i in $*
do
   do_copy $i
done
