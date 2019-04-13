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
&nbsp;&nbsp;&nbsp;&nbsp;HP-Socket provides Android NDK compilation script (script directory: *project/android-ndk/*). After installing and configuring the NDK, enter the compilation script directory and execute the *ndk-build* command to compile. By default, the compilation script will compile all of ABI's dynamic library and static library libs supported by the current NDK. If you have special needs, please modify the *jni/Application.mk* file.
  
*&nbsp;&nbsp;&nbsp;&nbsp;Note: The Android NDK compilation script provided by HP-Socket disables **SSL** and **ICONV** support, so the user applications need to define the **_SSL_DISABLED** and **_ICONV_DISABLED** macros.*