#!/bin/bash
#
# 
# Submodule stuff from Khaja Minhajuddin
# File name: makeArchive.sh
# cd qdaq-root-folder; ./makeArchive.sh

gitversion=`git describe --always --tags  | sed -e 's/-/./g' | sed -e 's/g//g'`
# prefix="qdaq-"$gitversion"/"
prefix="qdaq/"
filename="../qdaq-"$gitversion
# ROOT_ARCHIVE_DIR="$(pwd)"
ROOT_ARCHIVE_DIR="$(mktemp -d)"

echo "> create root archive"
git archive --format=tar --prefix=$prefix --output $ROOT_ARCHIVE_DIR"/repo-output.tar" HEAD 

echo "> add gitversion.txt"
pushd $ROOT_ARCHIVE_DIR
mkdir qdaq
echo "$gitversion" > qdaq/gitversion.txt
tar rf repo-output.tar qdaq/gitversion.txt
rm -r qdaq
popd 

echo "> append submodule archives"

pushd src/gui/qconsolewidget
git archive --format=tar --prefix=$prefix"src/gui/qconsolewidget/" --output $ROOT_ARCHIVE_DIR"/repo-output-sub1.tar" HEAD
popd
pushd $ROOT_ARCHIVE_DIR
tar --concatenate --file repo-output.tar repo-output-sub1.tar
popd

pushd src/gui/qmatplotwidget
git archive --format=tar --prefix=$prefix"src/gui/qmatplotwidget/" --output $ROOT_ARCHIVE_DIR"/repo-output-sub2.tar" HEAD
popd
pushd $ROOT_ARCHIVE_DIR
tar --concatenate --file repo-output.tar repo-output-sub2.tar
popd

pushd src/core/qthdf5
git archive --format=tar --prefix=$prefix"src/core/qthdf5/" --output $ROOT_ARCHIVE_DIR"/repo-output-sub3.tar" HEAD
popd
pushd $ROOT_ARCHIVE_DIR
tar --concatenate --file repo-output.tar repo-output-sub3.tar
popd

pushd $ROOT_ARCHIVE_DIR
echo "> remove all sub tars"
rm -rf repo-output-sub*.tar

echo "> gzip final tar"
gzip --force --verbose repo-output.tar
popd

echo "> move output file"
mv $ROOT_ARCHIVE_DIR"/repo-output.tar.gz" $filename".tar.gz"

echo "> done"









