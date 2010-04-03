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

exports.Connect_WithoutDb = function (test) {
  test.expect(1);
  
  var conn = mysql_libmysqlclient.createConnection();
  test.ok(conn.connect(host, user, password), "conn.connect() without database selection");
  conn.close();
  
  test.done();
};

exports.Connect_ManyTimes = function (test) {
  test.expect(1);
  
  var conn = mysql_libmysqlclient.createConnection();
  for (i = 1; i <= reconnect_count; i += 1) {
    conn.connect(host, user, password);
    conn.close();
  }
  test.ok(conn.connect(host, user, password), "conn.connect() after many times connect");
  conn.close();
  
  test.done();
};

/*
// TODO: how to write this test?
unittest.test('conn.close()', function () {
  var conn = mysql_libmysqlclient.createConnection(host, user, password, database);
  conn.close();
  unittest.assert(conn);
});
*/

exports.Connect_AllowedDb = function (test) {
  test.expect(1);
  
  var conn = mysql_libmysqlclient.createConnection();
  test.ok(conn.connect(host, user, password, database), "conn.connect() for allowed database");
  conn.close();
  
  test.done();
};

exports.Connect_DeniedDb = function (test) {
  test.expect(1);
  
  var conn = mysql_libmysqlclient.createConnection();
  test.ok(!conn.connect(host, user, password, database_denied), "conn.connect() for denied database");
  
  test.done();
};

