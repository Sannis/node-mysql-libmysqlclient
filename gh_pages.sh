#!/bin/bash

BRANCH=`git branch 2> /dev/null | grep "*" | sed 's#*\ \(.*\)#\1#'`
if [ "$BRANCH" != "master" ]
then
    exit 1
fi

DESC=`git describe`

if [ "$TMPDIR" = "" ]
then
    TMPDIR=~/tmp
fi

TMP=$TMPDIR/`mktemp temp.XXXXXX`.$$
mkdir -p $TMP
cp ./doc/* $TMP
rm -f $TMP/*~
git checkout gh-pages
cp $TMP/* ./
git add ./*.js ./*.html
git ci -m "Update docs to $DESC"
git checkout master
rm -rf $TMP

