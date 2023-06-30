#! /usr/bin/env sh

# input the folder where qdaq.exe resides
# mingw64-deploy.sh install_folder/ 

# the script does the following
#   1. find all /ucrt64 dlls that qdaq and its libraries depend on
#   2. copies them to qdaq's folder
#   3. runs the 'windeployqt' tool 

INSTALLPATH=$1
CURFLDR=$PWD

cd $INSTALLPATH 

printf "ldd qdaq.exe\n"
list=$(ldd ./qdaq.exe | sed 's/[^\/]*\(\/[^ ]*\)/\1\n/' | grep ucrt64)
for dll in $list;
do
  dll_lst="$dll_lst $dll"
done

printf "ldd ./libQDaqFilters.dll\n"
list=$(ldd ./libQDaqFilters.dll | sed 's/[^\/]*\(\/[^ ]*\)/\1\n/' | grep ucrt64)
for dll in $list;
do
  dll_lst="$dll_lst $dll"
done

printf "ldd ./libQDaqInterfaces.dll\n"
list=$(ldd ./libQDaqInterfaces.dll | sed 's/[^\/]*\(\/[^ ]*\)/\1\n/' | grep ucrt64)
for dll in $list;
do
  dll_lst="$dll_lst $dll"
done

printf "ldd designer/qdaqwidgetsplugin.dll\n"
list=$(ldd designer/qdaqwidgetsplugin.dll | sed 's/[^\/]*\(\/[^ ]*\)/\1\n/' | grep ucrt64)
for dll in $list;
do
  dll_lst="$dll_lst $dll"
done

printf "ldd script/QDaqInterfacesPlugin.dll\n"
list=$(ldd script/QDaqInterfacesPlugin.dll | sed 's/[^\/]*\(\/[^ ]*\)/\1\n/' | grep ucrt64)
for dll in $list;
do
  dll_lst="$dll_lst $dll"
done

printf "ldd script/QDaqFiltersPlugin.dll\n"
list=$(ldd script/QDaqFiltersPlugin.dll | sed 's/[^\/]*\(\/[^ ]*\)/\1\n/' | grep ucrt64)
for dll in $list;
do
  dll_lst="$dll_lst $dll"
done

# remove duplicates
dll_lst=`echo $dll_lst | tr ' ' '\n' | sort | uniq`

for dll in $dll_lst;
do
  cp $dll .
done

# run Qt windeploy tool 
# fetches (again) Qt dlls + some important folders
windeployqt .


