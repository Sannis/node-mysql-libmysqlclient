#!/usr/bin/env node --expose-gc
/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Load configuration
var cfg = require("../tests/config");

// Require modules
var
  util = require("util"),
  mysql_libmysqlclient = require("../lib/mysql-libmysqlclient"),
  connections = [],
  i;

function showMemoryUsage() {
  gc();gc();gc();gc();gc();
  console.log(util.inspect(process.memoryUsage()));
}

// Start!
util.print("Show memory usage information for NodeJS MySQL/libmysqlclient bindings...\n");

showMemoryUsage();

for (i = 0; i < cfg.slow_connects_nested; i++) {
  connections[i] = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database);
}

showMemoryUsage();

for (i = 0; i < cfg.slow_connects_nested; i++) {
  if (connections[i].connectedSync()) {
    connections[i].closeSync();
  }
  connections[i] = null;
}

connections = null;

showMemoryUsage();
