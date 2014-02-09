#!/bin/bash

BRANCH=`git branch 2> /dev/null | grep "*" | sed 's#*\ \(.*\)#\1#'`
if [ "$BRANCH" != "master" ]
then
    exit 1
fi

DESC=`git describe --tags`

# Update gh-pages branch
TMP=`mktemp -d ${TMPDIR}temp.XXXXXXXXXX`
cp -r ./doc/* $TMP
rm -f $TMP/*~
git checkout gh-pages
git reset --hard
git clean -df
cp -r $TMP/* ./
git add .
git ci -m "Update docs to $DESC"
git checkout master
rm -rf $TMP
