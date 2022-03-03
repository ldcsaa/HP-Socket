# HP-Socket for Windows
&nbsp;&nbsp;&nbsp;&nbsp;There are several projects under the *project/* directory to compile HP-Socket dynamic library or static library, and support different versions of MSVC compiler. Open the solution file (*HPSocket-20XX.sln*) with the corresponding MSVC IDE for execution.

* Project/HPSocketDLL-20XX&nbsp;&nbsp;&nbsp;&nbsp; -> HP-Socket DLL
* Project/HPSocketDLL4C-20XX -> HP-Socket 4C DLL
* Project/HPSocketLIB-20XX&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; -> HP-Socket LIB
* Project/HPSocketLIB4C-20XX&nbsp; -> HP-Socket 4C LIB

&nbsp;&nbsp;&nbsp;&nbsp;You can also run the following batch file through MSVC command prompt for MSBuild quick compilation：

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;***Script/build-libs.bat [MSVC_VERSION]***

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;*( MSVC_VERSION default value is "2010" )*