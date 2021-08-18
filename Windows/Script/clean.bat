@echo off

set cur_path=%cd%

cd /d %~dp0..\

echo --------------------------------------------------

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

del "Other Languages\E\*.exe" /f /s /q

rd /s /q "Demo\Debug"

for %%i in (Project Lib Demo) do (
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
	del *.pdb /f /s /q
)

echo --------------------------------------------------
echo bye~ bye~

cd /d %cur_path%
