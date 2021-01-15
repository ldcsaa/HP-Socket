#!/bin/bash

cd $(cd "$(dirname "$0")"; pwd)

ndk-build NDK_PROJECT_PATH=. NDK_APPLICATION_MK=./Application.mk NDK_LIBS_OUT=./Release/android-ndk NDK_OUT=./Release/android-ndk/obj $@

