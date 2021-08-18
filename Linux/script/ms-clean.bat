@echo off

set cur_path=%cd%

cd /d %~dp0..\

echo --------------------------------------------------

set all=%1

del *.sdf /f /s /q
del *.VC.db /f /s /q
del *.VC.db-shm /f /s /q
del *.VC.db-wal /f /s /q
del *.cki /f /s /q

rd /s /q "demo\Debug"

for %%i in (project lib demo) do (
	for %%j in (ipch obj .vs) do (
		for /f "delims=" %%k in ('dir /ad /b /s "%%i\%%j"') do (
		   rd /s /q "%%k"
		)
	rd /s /q "%%i\%%j"
	)
)

set /a x=0

if "%all%"=="/a" (
	set /a x+=1
)

if "%all%"=="-a" (
	set /a x+=1
)

if %x% geq 1 (
	for /f "delims=" %%i in ('dir /a-d /b /s "lib\*.so.*" ^| findstr /E /R ".*\.so\..*" ') do (
		del "%%i" /f /s /q
	)	
)

echo --------------------------------------------------
echo bye~ bye~

cd /d %cur_path%
