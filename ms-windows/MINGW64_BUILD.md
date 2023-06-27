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

USE UCRT64

- core
    - muparser    
      the MSYS2 package is build without unicode support
    - hdf5
- gui
    - qwt   
    The include path is not correctly set. Had to add `$$[QT_INSTALL_HEADERS]/qwt-qt5` in the include path
- QtSolutions - propertybrowser    
  Had to create the package for mingw
  Found similar package in AUR and started from there
- filters
    - gsl


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


