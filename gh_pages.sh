#!/bin/bash



BRANCH=`git branch 2> /dev/null | grep "*" | sed 's#*\ \(.*\)#\1#'`
if [ "$BRANCH" != "master" ]
then
    exit 1
fi

DESC=`git describe`

# Update master branch
node-waf doc
git add doc/*.js doc/*.html
git ci -m "Update docs to $DESC"

# Update gh-pages branch
TMP=`mktemp --tmpdir -d temp.XXXXXXXXXX`
cp ./doc/* $TMP
rm -f $TMP/*~
git checkout gh-pages
cp $TMP/* ./
git add ./*.js ./*.html
git ci -m "Update docs to $DESC"
git checkout master
rm -rf $TMP

