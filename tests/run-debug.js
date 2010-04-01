#!/usr/bin/env node
/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Database connection parameters
var host = "localhost",
  user = "test",
  password = "",
  database = "test",
  database_denied = "mysql",
  test_table = "test_table",

  // Operations count for continuous tests
  reconnect_count = 10000,
  insert_rows_count = 10000,

  // Require modules
  sys = require("sys"),
  events = require('events'),
  mysql_libmysqlclient = require("../mysql-libmysqlclient"),

  conn,
  res;

function debug(title, obj) {
  var i = 0;
  
  sys.puts("Debug '" + title + "':");
  sys.puts("  Inspect:");
  sys.puts(sys.inspect(obj).replace(/^/, "    "));
  sys.puts("  Foreach exclude EventEmitter.prototype:");
  for (i in obj) {
    if ((typeof i === "string") && (typeof events.EventEmitter.prototype[i] === "undefined")) {
      sys.puts("    " + i + " (typeof " + (typeof obj[i]) + "): " + obj[i].toString());
    }
  }
}

sys.print("Show debug information for NodeJS libmysqlclientronous MySQL binding...\n");

conn = mysql_libmysqlclient.createConnection(host, user, password, database);

res = conn.query("SHOW TABLES;");

debug("mysql_libmysqlclient", mysql_libmysqlclient);
debug("conn", conn);
debug("res", res);
//debug("events.EventEmitter", events.EventEmitter.prototype);

conn.close();

