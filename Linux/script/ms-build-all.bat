@echo off

call %~dp0ms-build-libs.bat %*
call %~dp0ms-build-demos.bat
