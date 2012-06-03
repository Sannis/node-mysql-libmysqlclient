#!/bin/sh

all: npm-install

npm-install: npm-install-stamp

npm-install-stamp: ./binding.gyp wscript ./src/*
		touch npm-install-stamp
		npm install

waf: waf-stamp
		@echo "NOTICE: Please note that node-waf building is deprecated for this module"

waf-stamp: ./wscript ./src/*
		touch waf-stamp
		node-waf configure build

clean:
		rm -rf ./build
		rm -f ./mysql_bindings.node
		rm -f npm-install-stamp
		rm -f waf-stamp

clean-all: clean
		rm -f devdependencies-stamp

test: devdependencies
		./node_modules/.bin/nodeunit --reporter=minimal tests/low-level-sync tests/low-level-async tests/high-level tests/complex tests/issues

test-slow: devdependencies
		./node_modules/.bin/nodeunit --reporter=minimal tests/slow

test-all: devdependencies
		./node_modules/.bin/nodeunit --reporter=minimal tests/low-level-sync tests/low-level-async tests/high-level tests/complex tests/issues tests/slow

test-profile: devdependencies
		rm -f v8.log
		/usr/bin/env node --prof ./node_modules/.bin/nodeunit tests/simple tests/complex tests/issues
		/usr/bin/env linux-tick-processor v8.log > v8.processed.log

lint: devdependencies
		cpplint ./src/*.h ./src/*.cc
		./node_modules/.bin/nodelint --config ./nodelint.conf ./package.json ./mysql-libmysqlclient.js ./doc ./tools/*.js
		./node_modules/.bin/nodelint --config ./nodelint.conf ./tests

mlf: devdependencies build ./lib
		./tools/memory-leaks-finder-repl.js

devdependencies: devdependencies-stamp

devdependencies-stamp:
		touch devdependencies-stamp
		npm install --dev .

.PHONY: all npm-install waf clean clean-all test test-slow test-all test-profile lint mlf
