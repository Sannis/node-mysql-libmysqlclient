#!/usr/bin/env node
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
  events = require('events'),
  mysql_libmysqlclient = require("../lib/mysql-libmysqlclient"),
  mysql_bindings = mysql_libmysqlclient.bindings,
  conn,
  connQueued,
  connHighlevel,
  res;

function debug(title, obj) {
  var i;
  
  util.puts("Debug '" + title + "':");
  util.puts("  Inspect:");
  util.puts(util.inspect(obj).replace(/^/gm, "    "));
  util.puts("  Foreach exclude EventEmitter.prototype:");
  for (i in obj) {
    if ((typeof i === "string") && (typeof events.EventEmitter.prototype[i] === "undefined")) {
      util.puts("    " + i + " (typeof " + (typeof obj[i]) + "): " + obj[i].toString());
    }
  }
}

util.print("Show debug information for NodeJS MySQL/libmysqlclient bindings...\n");

conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database);
res = conn.querySync("SHOW TABLES;");

debug("mysql_bindings", mysql_bindings);
debug("mysql_libmysqlclient", mysql_libmysqlclient);
debug("conn", conn);
debug("res", res);

conn.closeSync();

connQueued = mysql_libmysqlclient.createConnectionQueuedSync(cfg.host, cfg.user, cfg.password, cfg.database);
connHighlevel = mysql_libmysqlclient.createConnectionHighlevelSync(cfg.host, cfg.user, cfg.password, cfg.database);

debug("connQueued", connQueued);
debug("connHighlevel", connHighlevel);

connQueued.closeSync();
connHighlevel.closeSync();
