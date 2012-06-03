#!/usr/bin/env node --expose-gc
/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Load configuration
var cfg = require("../tests/config");

// Require modules
var mysql_libmysqlclient = require("../lib/mysql-libmysqlclient"), tries = 1000;

function showMemoryUsage() {
  gc();gc();gc();gc();gc();
  gc();gc();gc();gc();gc();
  gc();gc();gc();gc();gc();
  gc();gc();gc();gc();gc();
  gc();gc();gc();gc();gc();
  console.log(require("util").inspect(process.memoryUsage()));
}

function showConnectionsMemoryUsage() {
  var connections = [], i;

  console.log("Initial value:");
  showMemoryUsage();

  for (i = 0; i < cfg.slow_connects_nested; i++) {
    connections[i] = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database);
  }

  console.log("Connections created:");
  showMemoryUsage();

  for (i = 0; i < cfg.slow_connects_nested; i++) {
    if (connections[i].connectedSync()) {
      connections[i].closeSync();
    }
    connections[i] = null;
  }

  connections = null;

  console.log("Connections closed:");
  showMemoryUsage();
}

// Start!
console.log("Show memory usage information for NodeJS MySQL/libmysqlclient bindings...");

for (var i = 0; i < tries; i++) {
  console.log("\n" + i + ":\n");
  showConnectionsMemoryUsage();
}
