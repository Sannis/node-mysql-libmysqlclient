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
  mysql_libmysqlclient = require("../mysql-libmysqlclient");

exports.mysql_libmysqlclient_createConnection_1 = function (test) {
  test.expect(1);
  
  var conn = mysql_libmysqlclient.createConnection(host);
  test.ok(conn, "mysql_libmysqlclient.createConnection(host)");
  conn.close();
  
  test.done();
};

exports.mysql_libmysqlclient_createConnection_2 = function (test) {
  test.expect(1);
  
  var conn = mysql_libmysqlclient.createConnection(host, user);
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user)");
  conn.close();
  
  test.done();
};

exports.mysql_libmysqlclient_createConnection_3 = function (test) {
  test.expect(1);
  
  var conn = mysql_libmysqlclient.createConnection(host, user, password);
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user, password)");
  conn.close();
  
  test.done();
};

exports.mysql_libmysqlclient_createConnection_4 = function (test) {
  test.expect(1);
  
  var conn = mysql_libmysqlclient.createConnection(host, user, password, database);
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user, password, database)");
  conn.close();
  
  test.done();
};

exports.mysql_libmysqlclient_createConnection_4 = function (test) {
  test.expect(1);
  
  var conn = mysql_libmysqlclient.createConnection(host, user, password, database_denied);
  test.ok(!conn, "mysql_libmysqlclient.createConnection(host, user, password, database_denied)");
  conn.close();
  
  test.done();
};
