/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Mixin settings
/*global host, user, password, database, database_denied, charset, reconnect_count */
process.mixin(require("./settings"));

// Require modules
var
  sys = require("sys"),
  mysql_sync = require("../mysql-sync");

exports.FetchResult = function (test) {
  test.expect(3);
  
  var conn = mysql_sync.createConnection(host, user, password, database),
    res,
    tables;
  test.ok(conn, "mysql_sync.createConnection(host, user, password, database)");
  res = conn.query("SHOW TABLES;");
  test.ok(res, "conn.query('SHOW TABLES;')");
  tables = res.fetchResult();
  test.ok(tables, "res.fetchResult()");
  conn.close();
  
  test.done();
};

