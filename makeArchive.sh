#!/bin/sh
gitversion=`git describe --always --tags  | sed -e 's/-/./g' | sed -e 's/g//g'`
prefix="qdaq-"$gitversion"/"
filename="../qdaq-"$gitversion
git archive --format=tar --prefix=$prefix HEAD > $filename".tar"
mkdir "qdaq-"$gitversion
echo "$gitversion" > "qdaq-"$gitversion"/"gitversion.txt
tar rf $filename".tar" "qdaq-"$gitversion"/"gitversion.txt
gzip $filename".tar"
rm -r "qdaq-"$gitversion
