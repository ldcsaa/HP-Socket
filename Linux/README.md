# HP-Socket for Linx
&nbsp;&nbsp;&nbsp;&nbsp;The exported API of HP-Socket for Linux is consistent with HP-Socket for Windows, but the implementation code is completely separated. HP-Socket for Linux uses the features of the *C++14* standard, and thus need to be compiled by a compiler which version is *GCC 5.x* and above, and the compilation and execution environments of HP-Socket requires Linux kernel version *2.6.32* and above.   
## Compilation
&nbsp;&nbsp;&nbsp;&nbsp;Binary library files and sample executable files are available in the HP-Socket distribution. These files were compiled with GCC 6.3.1 under Linux kernel version 2.6.32. Alternatively, you can compile these files using the following two options:
1. **[*Visual C++ for Linux Development*](https://github.com/Microsoft/VSLinux) plug-in:** The HP-Socket distribution contains a series of Visual Studio Solutions, which were used to compile HP-Socket library and demos, you can compile them by yourself after install and configure the *Visual C++ for Linux Development* plug-in.  
2. ***compile.sh*:** You can compile HP-Socket library files yourself using *compile.sh* shell script. But *compile.sh* only compile library files and will not compile sample executable files.  
```
$ ./compile.sh -h
Usage: compile.sh [...O.P.T.I.O.N.S...]
----------------------+-------------------------------------------------
  -d|--with-debug-lib : compile debug libs (default: true)
  -j|--use-jemalloc   : use jemalloc in release libs
                      : (x86/x64 default: true, ARM default: false)
  -u|--udp-enabled    : enable UDP components (default: true)
  -t|--http-enabled   : enable HTTP components (default: true)
  -s|--ssl-enabled    : enable SSL components (default: true)
  -z|--zlib-enabled   : enable ZLIB related functions (default: true)
  -i|--iconv-enabled  : enable ICONV related functions (default: true)
  -c|--compiler       : compiler (default: g++)
  -p|--platform       : platform: x86 / x64 / ARM
                      : (default: current machine arch platform)  
  -e|--clean          : clean compilation intermediate temp files
  -r|--remove         : remove all compilation target files
  -v|--version        : print hp-socket version
  -h|--help           : print this usage message
----------------------+-------------------------------------------------
```
## Installation
&nbsp;&nbsp;&nbsp;&nbsp;Use *install.sh* shell script to install HP-Socket library files and sample executable files to your system.  
```
$ sudo ./install.sh -h
Usage: install.sh [...O.P.T.I.O.N.S...]
-----------------+------------------------------------------------------
  -p|--prefix    : install/uninstall path (default: /usr/local)
  -l|--libdir    : lib dir (x86/ARM default: 'lib', x64 default: 'lib64')
  -d|--with-demo : install demos or not (default: false)
  -u|--uninstall : execute uninstall operation from install path
  -v|--version   : print hp-socket version
  -h|--help      : print this usage message
-----------------+------------------------------------------------------
```
  
*&nbsp;&nbsp;&nbsp;&nbsp;Note: The **complle.sh** build script depends on the **script/**, **src/**, **include/**, and **dependent/** directories of the distribution; the **install.sh** install script depends on the **script/**, **include/**, and **lib/** directories of the distribution, if you want to install the sample executable files, it also need to depends on the **demo/Release/** directory.*
## Android NDK
&nbsp;&nbsp;&nbsp;&nbsp;HP-Socket provides Android NDK build script __*build-android-ndk.sh*__ (__*build-android-ndk.bat*__ on Windows). After installing and configuring the NDK, execute *build-android-ndk.sh* to build it. By default, the build script will build all of ABI's dynamic libraries and static libraries supported by the current NDK, and output the library files to *lib/android-ndk/* directory. If you have special needs, please set the corresponding command line parameters for *build-android-ndk.sh*.  

&nbsp;&nbsp;&nbsp;&nbsp;Build HP-Socket libraries use default settings:
```
$ cd HP-Socket/Linux
$ ./build-android-ndk.sh
```
### ABIs
&nbsp;&nbsp;&nbsp;&nbsp;By default, the build script generates libraries for all non-deprecated ABIs. You can modify the **APP_ABI** parameter for *build-android-ndk.sh* to generate libraries for specific ABIs.  

&nbsp;&nbsp;&nbsp;&nbsp;(for example: only generate *armeabi-v7a* and *x86* libraries):
```
$ ./build-android-ndk.sh APP_ABI=armeabi-v7a,x86
```
### Features
&nbsp;&nbsp;&nbsp;&nbsp;By default, all optional features (*UDP, SSL, HTTP, ZLIB, ICONV*) were enabled. You can set one or more ***_XXX_DISABLED=true*** parameters for *build-android-ndk.sh* to disable corresponding features.  
  - *_UDP_DISABLED=true*&nbsp;&nbsp;&nbsp;(disable UDP)
  - *_SSL_DISABLED=true*&nbsp;&nbsp;&nbsp;(disable SSL)
  - *_HTTP_DISABLED=true*&nbsp;&nbsp;(disable HTTP)
  - *_ZLIB_DISABLED=true*&nbsp;&nbsp;(disable ZLIB)
  - *_ICONV_DISABLED=true*&nbsp;(disable ICONV)

&nbsp;&nbsp;&nbsp;&nbsp;(for example: disable *SSL* and *ICONV* features):
```
$ ./build-android-ndk.sh _SSL_DISABLED=true _ICONV_DISABLED=true
```
*&nbsp;&nbsp;&nbsp;&nbsp;Note: If you disable one or more features, you need to define corresponding macros while compiling your application. for example: If you disable the **SSL** and **ICONV** features of HP-Socket library, when you compile the application that uses this library, need to define macro like: __-D_SSL_DISABLED -D_ICONV_DISABLED__.*  
### Other Options
&nbsp;&nbsp;&nbsp;&nbsp;For more details of *build-android-ndk.sh* command, please refer to the [*ndk-build official documentation*](https://developer.android.com/ndk/guides/ndk-build).  
&nbsp;&nbsp;&nbsp;&nbsp;The following example demonstrates: building *armeabi-v7a* and *x86_64* ABI targets; disabling *UDP*, *ZLIB*, and *ICONV* features; library files output to *lib/android-ndk/* directory; obj files output to *lib/Android-ndk/obj/* directory:
```
$ ./build-android-ndk.sh APP_ABI=armeabi-v7a,x86_64 \
                         _UDP_DISABLED=true \
                         _ZLIB_DISABLED=true \
                         _ICONV_DISABLED=true \
                         NDK_LIBS_OUT=./lib/android-ndk \
                         NDK_OUT=./lib/android-ndk/obj
```