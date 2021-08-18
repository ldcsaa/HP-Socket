@echo off

for /f "delims=" %%i in ('dir /a-d /b /s "%~dp0..\demo\*.sln"') do (
  for %%j in (x86 x64) do (
    MSBuild -m:4 -t:Rebuild -p:Configuration=Release -p:Platform="%%j" "%%i"
  )
)
