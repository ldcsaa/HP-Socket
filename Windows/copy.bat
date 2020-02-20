@echo off

xcopy /y /s /f Bin\HPSocket\x86\HPSocket_U.dll Demo\Release\x86\
xcopy /y /s /f Bin\HPSocket\x64\HPSocket_U.dll Demo\Release\x64\

xcopy /y /s /f Bin\HPSocket4C\x86\HPSocket4C_U.dll Demo\Release\x86\
xcopy /y /s /f Bin\HPSocket4C\x64\HPSocket4C_U.dll Demo\Release\x64\

xcopy /y /s /f Bin\HPSocket4C\x86\HPSocket4C.dll "Other Languages\E\Ä£¿é\Module\"
xcopy /y /s /f Bin\HPSocket4C\x86\HPSocket4C.dll "Other Languages\E\Ä£¿é\Demo\"

xcopy /y /s /f Bin\HPSocket4C\x86\HPSocket4C_U.dll "Other Languages\Delphi\Lib\"
