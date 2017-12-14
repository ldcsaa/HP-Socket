# HP-Socket for Linx
&nbsp;&nbsp;&nbsp;&nbsp;The exported API of HP-Socket for Linux is consistent with HP-Socket for Windows, but the implementation code is completely separated. HP-Socket for Linux uses the features of the *C++14* standard, and thus needs to be compiled by a compiler which version is *GCC 5.x* and above.  
&nbsp;&nbsp;&nbsp;&nbsp;The distribution packages of HP-Socket contains a series of Visual Studio Solutions, which were used to compile HP-Socket components and demos, you can compile them by yourself after install and configure the [*Visual C++ for Linux Development*](https://blogs.msdn.microsoft.com/vcblog/2016/03/30/visual-c-for-linux-development/) plug-in. Of course, you can write your own Makefile file to compile these codes.  
&nbsp;&nbsp;&nbsp;&nbsp;Note: the compilation and execution environments of HP-Socket requires Linux kernel version *2.6.32* and above.
## Installation
```
$ sudo ./install.sh -h
Usage: install.sh [...O.P.T.I.O.N.S...]
-----------------+------------------------------------------------
  -p|--prefix    : install/uninstall path (default: /usr/local)
  -l|--libdir    : lib dir (default: x86 -> lib, x64 -> lib64)
  -d|--with-demo : install demos or not (default: true)
  -u|--uninstall : execute uninstall operation from install path
  -v|--version   : print installing hp-socket version
  -h|--help      : print this usage message
-----------------+------------------------------------------------
```