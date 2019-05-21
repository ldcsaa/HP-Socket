#!/bin/bash

cd $(cd "$(dirname "$0")"; pwd)

ndk-build NDK_PROJECT_PATH=./project/android-ndk NDK_LIBS_OUT=./lib/android-ndk NDK_OUT=./lib/android-ndk/obj $@
