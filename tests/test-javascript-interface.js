/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Mixin settings
/*global host, user, password, database, database_denied */
process.mixin(require("./settings"));

// Require modules
var
  sys = require("sys"),
  mysql_libmysqlclient = require("../mysql-libmysqlclient");

exports.mysql_libmysqlclient_createConnection_0 = function (test) {
  test.expect(1);
  
  var conn = mysql_libmysqlclient.createConnection();
  test.ok(conn, "mysql_libmysqlclient.createConnection()");
  conn.close();
  
  test.done();
};

exports.mysql_libmysqlclient_createConnection_1 = function (test) {
  test.expect(2);
  
  var conn = mysql_libmysqlclient.createConnection(host);
  test.ok(conn, "mysql_libmysqlclient.createConnection(host)");
  test.ok(conn.connected(), "mysql_libmysqlclient.createConnection(host).connected()");
  conn.close();
  
  test.done();
};

exports.mysql_libmysqlclient_createConnection_2 = function (test) {
  test.expect(2);
  
  var conn = mysql_libmysqlclient.createConnection(host, user);
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user)");
  test.ok(conn.connected(), "mysql_libmysqlclient.createConnection(host, user).connected()");
  conn.close();
  
  test.done();
};

exports.mysql_libmysqlclient_createConnection_3 = function (test) {
  test.expect(2);
  
  var conn = mysql_libmysqlclient.createConnection(host, user, password);
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user, password)");
  test.ok(conn.connected(), "mysql_libmysqlclient.createConnection(host, user, password).connected()");
  conn.close();
  
  test.done();
};

exports.mysql_libmysqlclient_createConnection_4_allowed = function (test) {
  test.expect(2);
  
  var conn = mysql_libmysqlclient.createConnection(host, user, password, database);
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user, password, database)");
  test.ok(conn.connected(), "mysql_libmysqlclient.createConnection(host, user, password, database).connected()");
  conn.close();
  
  test.done();
};

exports.mysql_libmysqlclient_createConnection_4_denied = function (test) {
  test.expect(1);
  
  var conn = mysql_libmysqlclient.createConnection(host, user, password, database_denied);
  test.ok(!conn.connected(), "!mysql_libmysqlclient.createConnection(host, user, password, database_denied).connected()");
  
  test.done();
};
