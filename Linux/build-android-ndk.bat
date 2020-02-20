@echo off

set CURPATH=%cd%
set PROJECT_PATH=.\project\android-ndk
set LIBS_OUT=.\lib\android-ndk
set OBJ_OUT=.\lib\android-ndk\obj


cd %~dp0

rd /s /q %LIBS_OUT%

call ndk-build NDK_PROJECT_PATH=%PROJECT_PATH% NDK_LIBS_OUT=%LIBS_OUT% NDK_OUT=%OBJ_OUT% %*

call :cp_static_lib arm64-v8a
call :cp_static_lib armeabi-v7a
call :cp_static_lib x86_64
call :cp_static_lib x86

rd /s /q %OBJ_OUT%

cd %CURPATH%

exit /b 0

:cp_static_lib
	xcopy /y /s /f %OBJ_OUT%\local\%1\*.a %LIBS_OUT%\%1\
goto:eof
