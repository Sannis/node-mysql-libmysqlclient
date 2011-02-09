/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Load configuration
var cfg = require('../config');

exports.mysql_libmysqlclient_createConnectionSync_0 = function (test) {
  test.expect(1);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync();
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync()");
  if (conn.connectedSync()) {
    conn.closeSync();
  }
  test.done();
};

exports.mysql_libmysqlclient_createConnectionSync_1 = function (test) {
  test.expect(2);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host);
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host)");
  test.ok(conn.connectedSync(), "mysql_libmysqlclient.createConnectionSync(host).connectedSync()");
  if (!conn.connectedSync()) {
    console.log("Error:" + conn.connectError);
  } else {
    conn.closeSync();
  }
  test.done();
};

exports.mysql_libmysqlclient_createConnectionSync_2 = function (test) {
  test.expect(2);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user);
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user)");
  
  // This depends on password, connect(host, user) === connect(host, user, NULL)
  if (!cfg.password) {
    test.ok(conn.connectedSync(), "mysql_libmysqlclient.createConnectionSync(host, user).connectedSync()");
  } else {
    test.equals(conn.connectError,
                "Access denied for user '" + cfg.user + "'@'" + cfg.host + "' (using password: NO)",
                "mysql_libmysqlclient.createConnectionSync(host, user).connectedSync()");
  }
  
  if (conn.connectedSync()) {
    conn.closeSync();
  }
  test.done();
};

exports.mysql_libmysqlclient_createConnectionSync_3 = function (test) {
  test.expect(2);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password);
  test.ok(conn, "cfg.mysql_libmysqlclient.createConnectionSync(host, user, password)");
  test.ok(conn.connectedSync(), "cfg.mysql_libmysqlclient.createConnectionSync(host, user, password).connectedSync()");
  if (!conn.connectedSync()) {
    console.log("Error:" + conn.connectError);
  } else {
    conn.closeSync();
  }
  test.done();
};

exports.mysql_libmysqlclient_createConnectionSync_4_allowed = function (test) {
  test.expect(2);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database);
  test.ok(conn, "cfg.mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  test.ok(conn.connectedSync(), "cfg.mysql_libmysqlclient.createConnectionSync(host, user, password, database).connectedSync()");
  if (!conn.connectedSync()) {
    console.log("Error:" + conn.connectError);
  } else {
    conn.closeSync();
  }
  test.done();
};

exports.mysql_libmysqlclient_createConnectionSync_4_denied = function (test) {
  test.expect(2);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database_denied);
  test.ok(!conn.connectedSync(), "!cfg.mysql_libmysqlclient.createConnectionSync(host, user, password, database_denied).connectedSync()");
  test.equals(conn.connectError,
              "Access denied for user '" + cfg.user + "'@'" + cfg.host + "' to database '" + cfg.database_denied + "'",
              "cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database_denied).connectError");
  if (conn.connectedSync()) {
    conn.closeSync();
  }
  test.done();
};

