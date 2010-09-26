#!/usr/bin/env node
/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Load configuration
var cfg = require("../tests/config").cfg;

// Require modules
var
  sys = require("sys"),
  events = require('events'),
  mysql_libmysqlclient = require("../mysql-libmysqlclient"),
  mysql_bindings = require("../mysql_bindings"),
  conn,
  res;

function debug(title, obj) {
  var i = 0;
  
  sys.puts("Debug '" + title + "':");
  sys.puts("  Inspect:");
  sys.puts(sys.inspect(obj).replace(/^/gm, "    "));
  sys.puts("  Foreach exclude EventEmitter.prototype:");
  for (i in obj) {
    if ((typeof i === "string") && (typeof events.EventEmitter.prototype[i] === "undefined")) {
      sys.puts("    " + i + " (typeof " + (typeof obj[i]) + "): " + obj[i].toString());
    }
  }
}

sys.print("Show debug information for NodeJS MySQL/libmysqlclient bindings...\n");

conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database);

res = conn.querySync("SHOW TABLES;");

debug("mysql_bindings", mysql_bindings);
debug("mysql_libmysqlclient", mysql_libmysqlclient);
debug("conn", conn);
debug("res", res);

conn.closeSync();

