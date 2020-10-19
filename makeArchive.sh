#!/bin/bash
#
# 
# Submodule stuff from Khaja Minhajuddin
# File name: makeArchive.sh
# cd qdaq-root-folder; ./makeArchive.sh

gitversion=`git describe --always --tags  | sed -e 's/-/./g' | sed -e 's/g//g'`
prefix="qdaq-"$gitversion"/"
filename="../qdaq-"$gitversion
ROOT_ARCHIVE_DIR="$(pwd)"

echo "> create root archive"
git archive --format=tar --prefix=$prefix --output $ROOT_ARCHIVE_DIR"/repo-output.tar" HEAD 

echo "> add gitversion.txt"
mkdir "qdaq-"$gitversion
echo "$gitversion" > "qdaq-"$gitversion"/"gitversion.txt
tar rf $ROOT_ARCHIVE_DIR"/repo-output.tar" "qdaq-"$gitversion"/"gitversion.txt
rm -r "qdaq-"$gitversion

echo "> append submodule archives"
pushd gui/qconsolewidget
git archive --format=tar --prefix=$prefix"/gui/qconsolewidget/" --output $ROOT_ARCHIVE_DIR"/repo-output-sub1.tar" HEAD
popd
tar --concatenate --file repo-output.tar repo-output-sub1.tar
pushd gui/qmatplotwidget
git archive --format=tar --prefix=$prefix"/gui/qmatplotwidget/" --output $ROOT_ARCHIVE_DIR"/repo-output-sub2.tar" HEAD
popd
tar --concatenate --file repo-output.tar repo-output-sub2.tar
echo "> remove all sub tars"
rm -rf repo-output-sub*.tar

echo "> gzip final tar"
gzip --force --verbose repo-output.tar

echo "> move output file"
mv repo-output.tar.gz $filename".tar.gz"

echo "> done"









