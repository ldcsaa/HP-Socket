*** HP-Socket Դ����� ***
----------------------------------------

1����Ŀ���̣����뻷���� VS2010����

	Project/HPSocketDLL		-> HP-Socket DLL
	Project/HPSocketDLL4C	-> HP-Socket 4C DLL
	Project/HPSocketLIB		-> HP-Socket LIB
	Project/HPSocketLIB4C	-> HP-Socket 4C LIB
	
2��VS2015 ���뷽����

	1) ������Ŀ���̣��磺�� Project/HPSocketDLL4C ����Ϊ Project/HPSocketDLL4C-2015��
	2) �� VS2015 �򿪸��Ƴ����Ĺ��̣�������ʾȷ�����������ļ�
	3) �޸���Ŀ����
	
		��Configuration Properties -> VC++ Directories��
		
			* Platform: Win32
			* Configuration: Debug,uDebug,Release,uRelease
				> Include Directories => ..\..\Common\Lib\openssl\$(VisualStudioVersion)\x86\include;$(VC_IncludePath);$(WindowsSDK_IncludePath);
				> Library Directories => ..\..\Common\Lib\openssl\$(VisualStudioVersion)\x86\lib;$(VC_LibraryPath_x86);$(WindowsSDK_LibraryPath_x86);$(NETFXKitsDir)Lib\um\x86

			* Platform: x64
			* Configuration: Debug,uDebug,Release,uRelease
				> Include Directories => ..\..\Common\Lib\openssl\$(VisualStudioVersion)\x64\include;$(VC_IncludePath);$(WindowsSDK_IncludePath);
				> Library Directories => ..\..\Common\Lib\openssl\$(VisualStudioVersion)\x64\lib;$(VC_LibraryPath_x64);$(WindowsSDK_LibraryPath_x64);$(NETFXKitsDir)Lib\um\x64
	
2��VS2017 ���뷽����

	1) ������Ŀ���̣��磺�� Project/HPSocketDLL4C ����Ϊ Project/HPSocketDLL4C-2017��
	2) �� VS2017 �򿪸��Ƴ����Ĺ��̣�������ʾȷ�����������ļ�
	3) �޸���Ŀ����
	
		��Configuration Properties -> VC++ Directories��
		
			* Platform: Win32
			* Configuration: Debug,uDebug,Release,uRelease
				> Include Directories => ..\..\Common\Lib\openssl\$(VisualStudioVersion)\x86\include;$(VC_IncludePath);$(WindowsSDK_IncludePath);
				> Library Directories => ..\..\Common\Lib\openssl\$(VisualStudioVersion)\x86\lib;$(VC_LibraryPath_x86);$(WindowsSDK_LibraryPath_x86);$(NETFXKitsDir)Lib\um\x86

			* Platform: x64
			* Configuration: Debug,uDebug,Release,uRelease
				> Include Directories => ..\..\Common\Lib\openssl\$(VisualStudioVersion)\x64\include;$(VC_IncludePath);$(WindowsSDK_IncludePath);
				> Library Directories => ..\..\Common\Lib\openssl\$(VisualStudioVersion)\x64\lib;$(VC_LibraryPath_x64);$(WindowsSDK_LibraryPath_x64);$(NETFXKitsDir)Lib\um\x64
