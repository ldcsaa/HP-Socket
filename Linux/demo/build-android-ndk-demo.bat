@echo off

set CURPATH=%cd%
cd %~dp0

call ndk-build NDK_PROJECT_PATH=. NDK_APPLICATION_MK=./Application.mk NDK_LIBS_OUT=./Release/android-ndk NDK_OUT=./Release/android-ndk/obj %*

cd %CURPATH%
