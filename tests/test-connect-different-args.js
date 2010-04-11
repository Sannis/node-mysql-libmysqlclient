/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Load configuration
var cfg = require("./config").cfg;

// Require modules
var
  sys = require("sys"),
  mysql_libmysqlclient = require("../mysql-libmysqlclient");

exports.Connect_WithoutDb = function (test) {
  test.expect(1);
  
  var conn = mysql_libmysqlclient.createConnection();
  test.ok(conn.connect(cfg.host, cfg.user, cfg.password), "conn.connect() without database selection");
  conn.close();
  
  test.done();
};

exports.Connect_ManyTimes = function (test) {
  test.expect(1);
  
  var conn = mysql_libmysqlclient.createConnection(), i;
  for (i = 1; i <= cfg.reconnect_count; i += 1) {
    conn.connect(cfg.host, cfg.user, cfg.password);
    conn.close();
  }
  test.ok(conn.connect(cfg.host, cfg.user, cfg.password), "conn.connect() after many times connect");
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
  test.ok(conn.connect(cfg.host, cfg.user, cfg.password, cfg.database), "conn.connect() for allowed database");
  conn.close();
  
  test.done();
};

exports.Connect_DeniedDb = function (test) {
  test.expect(1);
  
  var conn = mysql_libmysqlclient.createConnection();
  test.ok(!conn.connect(cfg.host, cfg.user, cfg.password, cfg.database_denied), "conn.connect() for denied database");
  
  test.done();
};

