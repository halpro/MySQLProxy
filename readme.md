# MySQLProxy (logging database access proxy) project
### Version 1.0.1

The asynchronous logging proxy server for your MySQL database.

## Build requirements
* Linux Kernel >= 3.2 on AMD64
* C++11 compliant compiler
* Boost 1.58 and newer
* CMake 3.0 and newer

### Build steps
1. [Build your boost distribution](http://www.boost.org/build/doc/html/bbv2/installation.html). Examples:
* Build not using C++11 ABI and producing static libraries:
./b2 -j8 variant=release define=_GLIBCXX_USE_CXX11_ABI=0 --prefix={YOUR BOOST DIRECTORY HERE}/stage link=static
* Dynamic debug libraries:
./b2 -j8 variant=release define=_GLIBCXX_USE_CXX11_ABI=0 --prefix={YOUR BOOST DIRECTORY HERE}/stage

2. Edit your boost installation directory in CMakeLists.txt file of the project under:
#Set prebuilt boost libraries here:
3. Run ./build_debug.sh and ./build_release.sh for debug and release respectively.

## Description

MySQL server client-server protocol consists of encoded packets. Each packet contains a header and a body. See example:
$\000\000\000\003SELECT * FROM my.tab\nLIMIT 0, 1000\nn_compile_os'mes'EADU\307mysql_native_password....

The proxy does not interfere with the client-server data exchange, the packet from the client (actual request like SQL) is being analyzed and in the case of SQL is being logged to the log file in /tmp/daproxy folder.

## Limitations
* The proxy does not decode encrypted MySQL protocol traffic.
* The proxy does not work with large SQL requests (over 8 Kbytes).

Free use of the MySQLProxy is permitted under the guidelines and in accordance with the Boost Software License.
