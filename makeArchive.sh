#!/bin/sh
gitversion=`git describe --always --tags`
prefix="qdaq-"$gitversion"/"
filename="../qdaq-"$gitversion".tar.gz"
git archive --format=tar.gz --prefix=$prefix HEAD > $filename