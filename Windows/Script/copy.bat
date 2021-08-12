@echo off

xcopy /y /s /f %~dp0..\Lib\HPSocket\x86\HPSocket_U.dll %~dp0..\Demo\Release\x86\
xcopy /y /s /f %~dp0..\Lib\HPSocket\x64\HPSocket_U.dll %~dp0..\Demo\Release\x64\

xcopy /y /s /f %~dp0..\Lib\HPSocket4C\x86\HPSocket4C_U.dll %~dp0..\Demo\Release\x86\
xcopy /y /s /f %~dp0..\Lib\HPSocket4C\x64\HPSocket4C_U.dll %~dp0..\Demo\Release\x64\
