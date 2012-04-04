#!/bin/sh

MLF=./tools/run-memoryleaks-finder.js
NI=./node_modules/.bin/node-inspector
NI_DEBUG_PORT=5858
NI_WEB_PORT=8888
WEB_BROWSER=google-chrome

all: waf

clean:
		rm -rf ./build
		rm -f ./mysql_bindings.node
		rm -f waf-stamp
		rm -f gyp-stamp

clean-all: clean
		rm -f devdependencies-stamp

waf: waf-stamp

waf-stamp: ./wscript ./src/*
		touch waf-stamp
		node-waf configure
		node-waf build

gyp: gyp-stamp

gyp-stamp: ./binding.gyp ./src/*
		touch gyp-stamp
		node-gyp configure
		node-gyp build

test: devdependencies
		# ldd ./build/Release/mysql_bindings.node || ldd ./build/default/Release/mysql_bindings.node || true
		./node_modules/.bin/nodeunit --reporter=minimal tests/low-level-sync tests/low-level-async tests/high-level tests/complex tests/issues

test-slow: devdependencies
		./node_modules/.bin/nodeunit --reporter=minimal tests/slow

test-all: test test-slow

test-profile: devdependencies
		rm -f v8.log
		/usr/bin/env node --prof ./node_modules/.bin/nodeunit tests/simple tests/complex tests/issues
		/usr/bin/env linux-tick-processor v8.log > v8.processed.log

#lint: devdependencies
		#cpplint ./src/*.h ./src/*.cc
		#./node_modules/.bin/nodelint --config ./nodelint.conf ./package.json ./mysql-libmysqlclient.js ./doc ./tools/*.js
		#./node_modules/.bin/nodelint --config ./nodelint.conf ./tests

inspector: devdependencies
		${NI} --web-port=${NI_WEB_PORT} &
		${WEB_BROWSER} http://127.0.0.1:${NI_WEB_PORT}/debug?port=${NI_DEBUG_PORT}

mlf: devdependencies build ./mysql-libmysqlclient.js
		/usr/bin/env node --expose-gc --debug ${MLF} #--debugger_port=${NI_DEBUG_PORT} ${MLF}

devdependencies: devdependencies-stamp

devdependencies-stamp:
		touch devdependencies-stamp
		npm install --dev .

.PHONY: all waf gyp clean clean-all test test-slow test-all test-profile lint inspector mlf
