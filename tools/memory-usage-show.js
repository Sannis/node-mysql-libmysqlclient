#!/usr/bin/env node --expose-gc
/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

var
// Require modules
  mysql = require("../"),
  helper = require("./memory-helper"),
// Load configuration
  cfg = require("../tests/config"),
// Params
  tries = 10,
  i;

function showMemoryUsage() {
  helper.heavyGc();
  helper.showMemoryUsage();
}

function showConnectionsMemoryUsage() {
  var connections = [], i;

  console.log("Initial value:");
  showMemoryUsage();

  for (i = 0; i < cfg.slow_connects_inloop; i++) {
    connections[i] = mysql.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database);
  }

  console.log("Connections created:");
  showMemoryUsage();

  for (i = 0; i < cfg.slow_connects_inloop; i++) {
    if (connections[i].connectedSync()) {
      connections[i].closeSync();
    }
    connections[i] = null;
  }

  connections = null;

  console.log("Connections closed:");
  showMemoryUsage();
}

function showQueriesMemoryUsage() {
  var connection, i, res, rows;

  console.log("Initial value:");
  showMemoryUsage();

  connection = mysql.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database);

  for (i = 0; i < cfg.slow_fetches_inloop; i++) {
    res = connection.querySync("SELECT 'some string' as str;");
    rows = res.fetchAllSync();
  }

  rows = null;
  res = null;
  connection = null;

  console.log("Connection closed:");
  showMemoryUsage();
}

// Start!
console.log("Show memory usage information for NodeJS MySQL/libmysqlclient bindings...");

for (i = 0; i < tries; i++) {
  console.log("\n" + i + ":\n");
  showConnectionsMemoryUsage();
}

for (i = 0; i < tries; i++) {
  console.log("\n" + i + ":\n");
  showQueriesMemoryUsage();
}

setTimeout(function () {
  showMemoryUsage();

  setTimeout(function () {
    showMemoryUsage();

    setTimeout(function () {
      showMemoryUsage();
    }, 5000);
  }, 5000);
}, 5000);
