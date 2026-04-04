#! /usr/bin/env sh

# input the folder where qdaq.exe resides, e.g.
# > mingw64-deploy.sh install_folder/ 

# the script does the following
#   1. find all ${MSYSTEM,,} dlls, i.e. belonging to ['ucrt64', 'mingw32', ...], that qdaq and its libraries depend on
#   2. copies them to qdaq's folder
#   3. runs the 'windeployqt' tool 

# ldd/ntldd usage
# - use -R flag to recursively find all dependencies
# - use grep to filter only lines with "=>", i.e. the ones with missing dll paths
# - use grep to filter only ${MSYSTEM,,} dlls, i.e. belonging to ['ucrt64', 'mingw32', ...]
# - use sed to extract the dll path from the output, e.g. "C:/msys64/mingw64/bin/libgcc_s_seh-1.dll"

INSTALLPATH=$1
CURFLDR=$PWD

cd $INSTALLPATH 

printf "ntldd qdaq.exe\n"
list=$(ntldd -R ./qdaq.exe | grep "=>" | grep ${MSYSTEM,,} | sed 's/.* => \([^ ]*\) .*/\1/')
for dll in $list
do
  dll_lst="$dll_lst $dll"
done

printf "ntldd ./libQDaqFilters.dll\n"
list=$(ntldd -R ./libQDaqFilters.dll | grep "=>" | grep ${MSYSTEM,,} | sed 's/.* => \([^ ]*\) .*/\1/')
for dll in $list;
do
  dll_lst="$dll_lst $dll"
done

printf "ntldd ./libQDaqInterfaces.dll\n"
list=$(ntldd -R ./libQDaqInterfaces.dll | grep "=>" | grep ${MSYSTEM,,} | sed 's/.* => \([^ ]*\) .*/\1/')
for dll in $list;
do
  dll_lst="$dll_lst $dll"
done

printf "ntldd designer/qdaqwidgetsplugin.dll\n"
list=$(ntldd -R designer/qdaqwidgetsplugin.dll | grep "=>" | grep ${MSYSTEM,,} | sed 's/.* => \([^ ]*\) .*/\1/')
for dll in $list;
do
  dll_lst="$dll_lst $dll"
done

printf "ntldd script/QDaqInterfacesPlugin.dll\n"
list=$(ntldd -R script/QDaqInterfacesPlugin.dll | grep "=>" | grep ${MSYSTEM,,} | sed 's/.* => \([^ ]*\) .*/\1/')
for dll in $list;
do
  dll_lst="$dll_lst $dll"
done

printf "ntldd script/QDaqFiltersPlugin.dll\n"
list=$(ntldd -R script/QDaqFiltersPlugin.dll | grep "=>" | grep ${MSYSTEM,,} | sed 's/.* => \([^ ]*\) .*/\1/')
for dll in $list;
do
  dll_lst="$dll_lst $dll"
done

# remove duplicates
dll_lst=`echo $dll_lst | tr ' ' '\n' | sort | uniq`

for dll in $dll_lst;
do
  printf "$dll\n"
  cp $dll .
done

# run Qt windeploy tool 
# fetches (again) Qt dlls + some important folders
windeployqt .


