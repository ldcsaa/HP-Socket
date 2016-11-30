*** HP-Socket 源码编译 ***
----------------------------------------

1、项目工程（编译环境： VS2010）：

	Project/HPSocketDLL		-> HP-Socket DLL
	Project/HPSocketDLL4C	-> HP-Socket 4C DLL
	Project/HPSocketLIB		-> HP-Socket LIB
	Project/HPSocketLIB4C	-> HP-Socket 4C LIB
	
2、VS2015 编译方法：

	1) 复制项目工程（如：把 Project/HPSocketDLL4C 复制为 Project/HPSocketDLL4C-2015）
	2) 用 VS2015 打开复制出来的工程，根据提示确认升级工程文件
	3) 修改项目属性
	
		（Configuration Properties -> VC++ Directories）
		
			* Platform: Win32
			* Configuration: Debug-SSL,uDebug-SSL,Release-SSL,uRelease-SSL
				> Include Directories => ..\..\..\Common\Lib\openssl\$(VisualStudioVersion)\x86\include;$(VC_IncludePath);$(WindowsSDK_IncludePath);
				> Library Directories => ..\..\..\Common\Lib\openssl\$(VisualStudioVersion)\x86\lib;$(VC_LibraryPath_x86);$(WindowsSDK_LibraryPath_x86);$(NETFXKitsDir)Lib\um\x86

			* Platform: x64
			* Configuration: Debug-SSL,uDebug-SSL,Release-SSL,uRelease-SSL
				> Include Directories => ..\..\..\Common\Lib\openssl\$(VisualStudioVersion)\x64\include;$(VC_IncludePath);$(WindowsSDK_IncludePath);
				> Library Directories => ..\..\..\Common\Lib\openssl\$(VisualStudioVersion)\x64\lib;$(VC_LibraryPath_x64);$(WindowsSDK_LibraryPath_x64);$(NETFXKitsDir)Lib\um\x64
