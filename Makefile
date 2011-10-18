#!/bin/sh

MLF=./tools/run-memoryleaks-finder.js
NI=./node_modules/.bin/node-inspector
NI_DEBUG_PORT=5858
NI_WEB_PORT=8888
WEB_BROWSER=google-chrome

all: ./mysql_bindings.node

./mysql_bindings.node: ./src/*
		node-waf build

clean:
		rm -rf ./build
		rm -f ./mysql_bindings.node

conf: clean
		node-waf configure

./node_modules/.bin/nodeunit:
		npm install nodeunit

test: ./node_modules/.bin/nodeunit
		./node_modules/.bin/nodeunit tests/simple tests/complex

test-all: ./node_modules/.bin/nodeunit
		./node_modules/.bin/nodeunit tests/simple tests/complex tests/slow

test-profile: ./node_modules/.bin/nodeunit
		rm -f v8.log
		/usr/bin/env node --prof ./node_modules/.bin/nodeunit tests/simple tests/complex
		/usr/bin/env linux-tick-processor v8.log > v8.processed.log

${NI}:
		npm install node-inspector
		npm install v8-profiler

inspector: ${NI}
		${NI} --web-port=${NI_WEB_PORT} &
		${WEB_BROWSER} http://127.0.0.1:${NI_WEB_PORT}/debug?port=${NI_DEBUG_PORT}

mlf: ${MLF} ./mysql-libmysqlclient.js ./mysql_bindings.node
		/usr/bin/env node --expose-gc --debug ${MLF} #--debugger_port=${NI_DEBUG_PORT} ${MLF}

.PHONY: all clean conf test test-all test-profile inspector mlf

