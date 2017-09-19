Ubuntu or Debian Linux
======================

## Setup ##

This project uses CMake (minimum version 3.0), you can download it [here](http://www.cmake.org/download/).

Make sure to update git submodules before you build:

```bash
git submodule update --init
```

Currently, builds were tested under MinGW-TDM and MS Build Tools 2015 (recommended). You will need to install/build zlib and curl.

The demo application uses the Mapzen vector tile service, so you will need a Mapzen API key to build and run the demo. 

 1. Visit https://mapzen.com/documentation/overview/#get-started-developing-with-mapzen to get an API key.

 2. Setup an environment variable (`MAPZEN_API_KEY`) to point to your API key.
    ```bash
    export MAPZEN_API_KEY=YOUR_API_KEY
    ```

## Build ##

 1. Download zlib sources from https://zlib.net/

 2. Download curl sources fom https://curl.haxx.se/download.html

### Clang ###

```cmake -DCMAKE_C_COMPILER=clang-cl.exe -DCMAKE_CXX_COMPILER=clang-cl.exe -DCMAKE_BUILD_TYPE=Release -G "NMake Makefiles"  ../..```

D:\prg\_git\tangram-es\core\deps\harfbuzz-icu-freetype\icu\common\ucnv2022.cpp(751,9):  error: ISO C++17 does not allow
      'register' storage class specifier [-Wregister]

### MSVC ###

 1. Launch Visual C++ 2015 x86 Native Build Tools Command Prompt

 2. Enter zlib directory and run ```nmake -f win32/Makefile.msc```
 
 3. Enter <curl dir>/winbuild and run nmake /f Makefile.vc mode=<static or dll>
 
 4. For static builds, go to <curl dir>\builds\libcurl-vc-x86-release-static-ipv6-sspi-winssl\lib and change libcurl_a.lib to libcurl.lib
 
 5. Setup local variable `CMAKE_PREFIX_PATH` so that it includes ```<curl dir>\builds\libcurl-vc-x86-release-static-ipv6-sspi-winssl``` and ```<zlib dir>```. If ```echo %CMAKE_PREFIX_PATH%``` doesn't reflect changes, you must log out and log in to Windows.

 6. Create directory build/windows under tangram-es root directory.

 7. Enter command prompt and go to newly created directory.

 8. Setup CMake: ```cmake ../.. -G "NMake Makefiles"```

 9. Run build via nmake: ```nmake```

### MinGW ###

 1. Create directory build/windows under tangram-es root directory.

 2. Enter command prompt and go to newly created directory.

 3. Setup CMake, forcing MinGW toolchain: ```cmake ../.. -G "MinGW Makefiles"```

 4. Run build via make: ```make```

You can optionally use `make -j` to parallelize the build and append `DEBUG=1` or `RELEASE=1` to choose the build type.

## Running app ##

App should now reside in `build/windows/bin directory`. If you built curl/zlib as dynamic libraries, you'll have to supply zlib.dll and curl.dll into same directory as .exe file.

You can open a different YAML scene file by dragging and dropping it into the window, or passing it as an argument:

```tangram -f /path/to/your/scene.yaml
```
