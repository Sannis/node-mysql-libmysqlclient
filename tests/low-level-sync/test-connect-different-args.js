/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Load configuration
var cfg = require("../config.js");

exports.ConnectSync_WithoutDb = function (test) {
  test.expect(1);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync();
  test.ok(conn.connectSync(cfg.host, cfg.user, cfg.password), "conn.connect() without database selection");
  conn.closeSync();
  
  test.done();
};

exports.ConnectSync_ManyTimes = function (test) {
  test.expect(1);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync(), i;
  for (i = 1; i <= cfg.reconnect_count; i += 1) {
    conn.connectSync(cfg.host, cfg.user, cfg.password);
    conn.closeSync();
  }
  test.ok(conn.connectSync(cfg.host, cfg.user, cfg.password), "conn.connect() after many times connect");
  conn.closeSync();
  
  test.done();
};

exports.ConnectSync_AllowedDb = function (test) {
  test.expect(1);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync();
  test.ok(conn.connectSync(cfg.host, cfg.user, cfg.password, cfg.database), "conn.connect() for allowed database");
  conn.closeSync();
  
  test.done();
};

exports.ConnectSync_DeniedDb = function (test) {
  test.expect(1);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync();
  test.ok(!conn.connectSync(cfg.host, cfg.user, cfg.password, cfg.database_denied), "conn.connect() for denied database");
  
  test.done();
};

exports.ConnectSync_DeniedFollowedByAllowedDb = function (test) {
  test.expect(2);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync();
  test.ok(!conn.connectSync(cfg.host, cfg.user, cfg.password, cfg.database_denied), "conn.connect() for denied database");
  test.ok(conn.connectSync(cfg.host, cfg.user, cfg.password, cfg.database), "conn.connect() for allowed database");
  conn.closeSync();
  
  test.done();
};

