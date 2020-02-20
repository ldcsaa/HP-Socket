#!/bin/bash

set -e

cd $(cd "$(dirname "$0")"; pwd)

PROJECT_PATH=./project/android-ndk
LIBS_OUT=./lib/android-ndk
OBJ_OUT=./lib/android-ndk/obj

function cp_static_lib
{
	cp -f $OBJ_OUT/local/$1/*.a $LIBS_OUT/$1
}

rm -rf $LIBS_OUT

ndk-build NDK_PROJECT_PATH=$PROJECT_PATH NDK_LIBS_OUT=$LIBS_OUT NDK_OUT=$OBJ_OUT $@

cp_static_lib arm64-v8a
cp_static_lib armeabi-v7a
cp_static_lib x86_64
cp_static_lib x86

rm -rf $OBJ_OUT
