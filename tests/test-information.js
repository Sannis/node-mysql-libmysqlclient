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
  charset = "utf8",

  // Operations count for continuous tests
  reconnect_count = 10000,
  insert_rows_count = 10000,

  // Require modules
  sys = require("sys"),
  mysql_sync = require("../mysql-sync"),

  conn;

exports.sqlStateAllowed = function(test){
  test.expect(2);
  
  var conn = mysql_sync.createConnection(host, user, password, database);
  test.ok(conn, "mysql_sync.createConnection(host, user, password, database)");
  test.ok(conn.sqlState(), "00000", "conn.sqlState() after connection to allowed database");
  conn.close();
  
  test.done();
};

exports.sqlStateDenied = function(test){
  test.expect(2);
  
  var conn = mysql_sync.createConnection(host, user, password, database_denied);
  test.ok(conn, "mysql_sync.createConnection(host, user, password, database)");
  test.ok(conn.sqlState(), "42000", "conn.sqlState() after connection to denied database");
  
  test.done();
};


