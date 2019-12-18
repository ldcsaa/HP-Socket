@echo off

echo --------------------------------------------------

call copy.bat

set all=%1

del *.ilk /f /s /q
del *.exp /f /s /q
del *.idb /f /s /q
del *.ipdb /f /s /q
del *.iobj /f /s /q
del *.sdf /f /s /q
del *.VC.db /f /s /q
del *.VC.db-shm /f /s /q
del *.VC.db-wal /f /s /q
del *.aps /f /s /q
del *.cki /f /s /q
del *.exe.config /f /s /q

for /f "delims=" %%i in ('dir /ad /b /s "obj"') do (
   rd /s /q "%%i"
)

for /f "delims=" %%i in ('dir /ad /b /s "netstandard2.0"') do (
   rd /s /q "%%i"
)

rd /s /q "Demo\Debug"

set /a x=0

if "%all%"=="/a" (
	set /a x+=1
)

if "%all%"=="-a" (
	set /a x+=1
)

if %x% geq 1 (
	for /f "delims=" %%i in ('dir /ad /b /s "static"') do (
		rd /s /q "%%i"
	)
	
	del *.pdb /f /s /q
)

del "Other Languages\E\*.exe" /f /s /q

echo --------------------------------------------------
echo bye~ bye~
