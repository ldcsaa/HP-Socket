@echo off

set CURPATH=%cd%
cd %~dp0

call ndk-build NDK_PROJECT_PATH=./project/android-ndk NDK_LIBS_OUT=./lib/android-ndk NDK_OUT=./lib/android-ndk/obj %*

cd %CURPATH%
