@echo off

echo --------------------------------------------------

set all=%1

del *.ilk /f /s /q
del *.exp /f /s /q
del *.idb /f /s /q
del *.ipdb /f /s /q
del *.iobj /f /s /q
del *.sdf /f /s /q
del *.VC.db /f /s /q
del *.aps /f /s /q
del *.cki /f /s /q
del *.exe.config /f /s /q

for /f "delims=" %%i in ('dir /ad /b /s "obj"') do (
   rd /s /q "%%i"
)

rd /s /q "Demo\Debug"

rd /s /q "Demo\Other Languages Demos\C#\HPSocketCS\bin\Debug"
rd /s /q "Demo\Other Languages Demos\C#\HPSocketCS\bin\Release"

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

del "Demo\Other Languages Demos\E\*.exe" /f /s /q

call copy.bat

echo --------------------------------------------------
echo bye~ bye~
