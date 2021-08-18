@echo off

for /f "delims=" %%i in ('dir /a-d /b /s "%~dp0..\Demo\*.sln"') do (
  for %%j in (Win32 x64) do (
    MSBuild -m:4 -t:Rebuild -p:Configuration=Release -p:Platform="%%j" "%%i"
  )
)
