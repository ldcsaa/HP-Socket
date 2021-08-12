@echo off

set version=%1

if "%version%"=="" (
	set version=2019
)

set prj_dir=%~dp0..\project

for %%i in (Debug Release) do (
  for %%j in (x86 x64) do (
    MSBuild -m:4 -t:Rebuild -p:Configuration=%%i -p:Platform="%%j" %prj_dir%\hpsocket-%version%.sln
  )
)
