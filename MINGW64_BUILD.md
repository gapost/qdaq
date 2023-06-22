# Windows build with MSYS2 / MINGW64

## Obtain source code

Clone the project from gitlab

``
git clone --recurse-submodules git@gitlab.com:qdaq/qdaq.git
``

To later update your copy

``
git pull --recurse-submodules
``

## required MSYS2 packages

USE UCRT64

- muparser (the MSYS2 package is build without unicode support)
- hdf5

## build

``
mkdir build
cd build
qmake ../qdaq/qdaq.pro CONFIG+=debug && mingw32-make qmake_all
mingw32-make
``