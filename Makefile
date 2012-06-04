#!/bin/sh

all: npm-install

npm-install: npm-install-stamp

npm-install-stamp: ./binding.gyp ./src/*
		npm install
		touch npm-install-stamp

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
		rm -f webkit-devtools-agent-stamp

clean-all: clean
		rm -rf ./node_modules

test: npm-install
		./node_modules/.bin/nodeunit --reporter=minimal tests/low-level-sync tests/low-level-async tests/high-level tests/complex tests/issues

test-slow: npm-install
		./node_modules/.bin/nodeunit --reporter=minimal tests/slow

test-all: npm-install
		./node_modules/.bin/nodeunit --reporter=minimal tests/low-level-sync tests/low-level-async tests/high-level tests/complex tests/issues tests/slow

test-profile: npm-install
		rm -f v8.log
		/usr/bin/env node --prof ./node_modules/.bin/nodeunit tests/simple tests/complex tests/issues
		/usr/bin/env linux-tick-processor v8.log > v8.processed.log

lint: npm-install
		cpplint ./src/*.h ./src/*.cc
		./node_modules/.bin/nodelint --config ./nodelint.conf ./package.json ./mysql-libmysqlclient.js ./doc ./tools/*.js
		./node_modules/.bin/nodelint --config ./nodelint.conf ./tests

webkit-devtools-agent: webkit-devtools-agent-stamp

webkit-devtools-agent-stamp:
		npm install webkit-devtools-agent@0.0.4
		touch webkit-devtools-agent-stamp

mlf: npm-install webkit-devtools-agent
		./tools/memory-leaks-finder-repl.js

valgrind: npm-install webkit-devtools-agent $(time)
 		valgrind \
 		  --leak-check=full --error-limit=no \
 		  --track-origins=yes -v \
 		  --log-file=valgrind.log -- \
 		  node --expose-gc ./tools/memory-usage-show.js


.PHONY: all npm-install waf clean clean-all test test-slow test-all test-profile lint mlf
