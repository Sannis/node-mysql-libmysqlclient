/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Load configuration
var cfg = require("../config").cfg;

// Require modules
var
  sys = require("sys"),
  mysql_libmysqlclient = require("../../mysql-libmysqlclient");

exports.SqlStateSync = function (test) {
  test.expect(2);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    stmt;
  
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  stmt = conn.initStatementSync();
  test.equals(stmt.sqlStateSync(), "00000", "stmt.sqlStateSync()");
  
  conn.closeSync();
  
  test.done();
};
