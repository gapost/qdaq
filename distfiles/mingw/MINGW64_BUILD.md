# Windows build with MSYS2 / MINGW64

## Obtain source code

Clone or download the project from gitlab

```
git clone git@gitlab.com:qdaq/qdaq.git
```

## Install required MSYS2 packages

Tested platform: UCRT64, MINGW32

Install the following to prepare the build environment

```
pacman -S mingw-w64-ucrt-x86_64-toolchain
pacman -S mingw-w64-ucrt-x86_64-qt5-base
pacman -S mingw-w64-ucrt-x86_64-qt5-serialport
pacman -S mingw-w64-ucrt-x86_64-qt5-script
pacman -S mingw-w64-ucrt-x86_64-qt5-tools
pacman -S mingw-w64-ucrt-x86_64-qt5-svg
pacman -S mingw-w64-ucrt-x86_64-qwt-qt5
pacman -S mingw-w64-ucrt-x86_64-gsl
pacman -S mingw-w64-ucrt-x86_64-hdf5  
pacman -S mingw-w64-ucrt-x86_64-muparser
pacman -S mingw-w64-ucrt-x86_64-libmodbus
```

Notes:
- core
    - muparser    
      `mingw-w64-ucrt-x86_64-muparser`    
      the MSYS2 package is build without unicode support    
    - hdf5     
      `mingw-w64-ucrt-x86_64-hdf5`
- gui
    - qwt    
      `mingw-w64-ucrt-x86_64-qwt-qt5`   
      The include path is not correctly set. Had to add `$$[QT_INSTALL_HEADERS]/qwt-qt5` in the include path
- filters
    - gsl    
      `mingw-w64-ucrt-x86_64-gsl`
- interfaces
  - modbus     
    `mingw-w64-ucrt-x86_64-libmodbus`

## GPIB

Typically, you would need to have NI-488.2 installed or just the required files for C language development.

The files are: 
For Win10+
```
NI/include/ni4882.h
NI/lib64/msvc/ni4882.obj
NI/lib32/msvc/ni4882.obj
```

For Win7 32bit
```
NI/include/decl-32.h
NI/lib32/gpib-32.obj
```

If you have the NI-488.2 lib installed in default location, then it will be discovered by `cmake`.

Otherwise, set the path to the folder containing the NI files in the environment variable `NIEXTCCOMPILERSUPP` before calling cake to configure 

    NIEXTCCOMPILERSUPP=/path/to/NI cmake ...

## Build required libraries

The following libraries are used by `QDaq` and have to be built first:
- QMatplotWidget, https://gitlab.com/qdaq/qmatplotwidget
- QConsoleWidget, https://gitlab.com/qdaq/qconsolewidget
- QtPropertyBrowser, https://gitlab.com/qdaq/qtpropertybrowser
- QtHDF5, https://gitlab.com/qdaq/qthdf5

For each of them follow the same procedure:
```bash
git clone git@gitlab.com:qdaq/libname.git
cd libname
mkdir .build
cd .build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$HOME/.local/${MSYSTEM,,} ..
cmake --build . --target install
``` 

With the above code these libs are installed in a local subfolder under the user's subdirectory, different for each MSYS environment, e.g. `.local/ucrt`. 

## Build QDaq

```bash
git clone git@gitlab.com:qdaq/qdaq.git
cd qdaq
mkdir .winbuild 
cd .winbuild    
NIEXTCCOMPILERSUPP=/path/to/NI cmake -DCMAKE_BUILD_TYPE=Release  -DCMAKE_PREFIX_PATH=$HOME/.local/${MSYSTEM,,} ..  
cmake --build . --target install 
```

This will create a folder `.winbuild/QDaq` which contains `qdaq.exe` and all DLLs.

The Designer plugin goes in `.winbuild/QDaq/designer/`.

Qt-Script plugins go into `.winbuild/QDaq/script/`.

## Deploy QDaq

In order for qdaq.exe to be able to run from the windows shell, all required MSYS DLLs have to be copied to the `.winbuild/QDaq` folder

This is achieved with the script `distfiles/mingw/mingw64-deploy.sh`
```
> distfiles/mingw/mingw64-deploy.sh .winbuild/QDaq
``` 

The script copies all required DLLs and then runs the `windeployqt` tool. Test by double clicking `qdaq.exe` from Windows Explorer.

Finally, `Inno Setup` can be used under Windows with the `distfiles/mingw/innosetup.iss` script to create an installer.

## Notes for Windows 7 32bit

Status as of March 2026.

MSYS2 support for Windows 7 has been dropped. However, one can use the `mingw32` environment to build 32-bit programs targeting older windows versions.

Not all packages are currently built for `mingw32`. But there is a package archive available at https://repo.msys2.org/mingw/mingw32/. There is a complete win32 Qt version 5.15.14 which is used to build QDaq.

Only the package `qwt-qt5` is not available. We build that as an MSYS package using the PKGBUILD recipe at `distfiles/mingw-w64-qwt` taken from Arch Linux.


