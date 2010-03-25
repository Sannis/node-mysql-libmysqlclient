#!/bin/bash

DIR=`dirname $0`

cd $DIR
node ./nodeunit/lib/testrunner.js test-*.js
cd -
