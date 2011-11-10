#!/bin/sh

MLF=./tools/run-memoryleaks-finder.js
NI=./node_modules/.bin/node-inspector
NI_DEBUG_PORT=5858
NI_WEB_PORT=8888
WEB_BROWSER=google-chrome

all: conf build

conf: clean conf-stamp

conf-stamp:
		touch conf-stamp
		node-waf configure

clean:
		rm -rf ./build
		rm -f ./mysql_bindings.node
		rm -f conf-stamp
		rm -f build-stamp

cleanall: clean
		rm -f devdependencies-stamp

build: build-stamp

build-stamp: ./src/*
		touch build-stamp
		node-waf build

test: devdependencies
		./node_modules/.bin/nodeunit tests/simple tests/complex tests/issues

test-all: devdependencies
		./node_modules/.bin/nodeunit tests/simple tests/complex tests/slow tests/issues

test-profile: devdependencies
		rm -f v8.log
		/usr/bin/env node --prof ./node_modules/.bin/nodeunit tests/simple tests/complex tests/issues
		/usr/bin/env linux-tick-processor v8.log > v8.processed.log

inspector: devdependencies
		${NI} --web-port=${NI_WEB_PORT} &
		${WEB_BROWSER} http://127.0.0.1:${NI_WEB_PORT}/debug?port=${NI_DEBUG_PORT}

mlf: devdependencies build ./mysql-libmysqlclient.js
		/usr/bin/env node --expose-gc --debug ${MLF} #--debugger_port=${NI_DEBUG_PORT} ${MLF}

devdependencies: devdependencies-stamp

devdependencies-stamp:
		touch devdependencies-stamp
		npm install --dev .

.PHONY: all build clean conf test test-all test-profile inspector mlf

