#!/usr/bin/env node
/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Mixin settings
process.mixin(require("./settings"));

// Require modules
var sys = require("sys"),
  mysql_sync = require("../mysql-sync");

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


