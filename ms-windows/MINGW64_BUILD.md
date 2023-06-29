# Windows build with MSYS2 / MINGW64

## Obtain source code

Clone the project from gitlab

```
git clone --recurse-submodules git@gitlab.com:qdaq/qdaq.git
```

To later update your copy

```
git pull --recurse-submodules
```

## required MSYS2 packages

Tested platform: UCRT64

Install the following to prepare the build environment

```
pacman -S mingw-w64-ucrt-x86_64-toolchain
pacman -S mingw-w64-ucrt-x86_64-qt5-base
pacman -S mingw-w64-ucrt-x86_64-qt5-serialport
pacman -S mingw-w64-ucrt-x86_64-qt5-script
pacman -S mingw-w64-ucrt-x86_64-qt5-tools
```

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
- QtSolutions - propertybrowser    
  Had to create the package for mingw
  Found similar package in AUR and started from there
- filters
    - gsl    
      `mingw-w64-ucrt-x86_64-gsl`
- interfaces
  - modbus     
    `mingw-w64-ucrt-x86_64-libmodbus-git`


## build

```
mkdir build 
cd build    
qmake ../qdaq/qdaq.pro CONFIG+=debug && make qmake_all    
make    
```

All binary files (*.exe & *.dlls) go into the build/bin/ folder.

designer plugin goes in build/bin/designer/

script plugins go into build/bin/script/

## deploy

copy the contents of build/bin/ to the installation folder

run the script ms-windows/mingw64-deploy.sh
```
> ./ms-windows/mingw64-deploy.sh path_to_installation_folder
``` 

This will copy all required dlls and then run the `windeployqt` tool.


