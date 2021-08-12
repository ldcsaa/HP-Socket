@echo off

call %~dp0build-libs.bat %*
call %~dp0build-demos.bat
