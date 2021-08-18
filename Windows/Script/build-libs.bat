@echo off

set version=%1

if "%version%"=="" (
	set version=2010
)

set prj_dir=%~dp0..\Project

for %%i in (Debug Release uDebug uRelease) do (
  for %%j in (Win32 x64) do (
    MSBuild -m:4 -t:Rebuild -p:Configuration=%%i -p:Platform="%%j" %prj_dir%\HPSocket-%version%.sln
  )
)

call %~dp0copy.bat
