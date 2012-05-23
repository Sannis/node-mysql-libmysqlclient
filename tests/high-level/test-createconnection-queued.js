/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Load configuration
var cfg = require('../config');

exports.mysql_libmysqlclient_createConnectionQueuedSync_0 = function (test) {
  test.expect(2);

  var conn = cfg.mysql_libmysqlclient.createConnectionQueuedSync();
  test.ok(conn instanceof cfg.mysql_bindings.MysqlConnection);
  test.ok(conn instanceof cfg.mysql_libmysqlclient.MysqlConnectionQueued);
  if (conn.connectedSync()) {
    conn.closeSync();
  }
  test.done();
};

exports.mysql_libmysqlclient_createConnectionQueuedSync_4 = function (test) {
  test.expect(3);

  var conn = cfg.mysql_libmysqlclient.createConnectionQueuedSync(cfg.host, cfg.user, cfg.password, cfg.database);
  test.ok(conn instanceof cfg.mysql_bindings.MysqlConnection);
  test.ok(conn instanceof cfg.mysql_libmysqlclient.MysqlConnectionQueued);

  isConnected = conn.connectedSync();
  test.ok(isConnected, "cfg.mysql_libmysqlclient.createConnectionQueuedSync(host, user, password, database) connects");

  if (!isConnected) {
    // Extra debug output
    console.log("Error:" + conn.connectError);
  } else {
    conn.closeSync();
  }

  test.done();
};

exports.mysql_libmysqlclient_createConnectionQueued_0 = function (test) {
  test.expect(1);

  test.throws(function () {
    cfg.mysql_libmysqlclient.createConnectionQueued();
  }, "require('mysql-libmysqlclient').createConnectionQueued() must get callback as last argument");

  test.done();
};

exports.mysql_libmysqlclient_createConnectionQueued_1 = function (test) {
  test.expect(3);

  cfg.mysql_libmysqlclient.createConnectionQueued(function (err, conn) {
    test.ok(err == null, "Error object is not present");
    test.ok(conn instanceof cfg.mysql_bindings.MysqlConnection);
    test.ok(conn instanceof cfg.mysql_libmysqlclient.MysqlConnectionQueued);

    if (conn.connectedSync()) {
      conn.closeSync();
    }

    test.done();
  });
};

exports.mysql_libmysqlclient_createConnectionQueued_5 = function (test) {
  test.expect(4);

  cfg.mysql_libmysqlclient.createConnectionQueued(cfg.host, cfg.user, cfg.password, cfg.database, function (err, conn) {
    test.ok(err === null, "Error object is not present");
    test.ok(conn instanceof cfg.mysql_bindings.MysqlConnection);
    test.ok(conn instanceof cfg.mysql_libmysqlclient.MysqlConnectionQueued);

    isConnected = conn.connectedSync();
    test.ok(isConnected, "cfg.mysql_libmysqlclient.createConnectionQueued(host, user, password, database, callback) connects");

    if (!isConnected) {
      // Extra debug output
      console.log("Error:" + conn.connectError);
    } else {
      conn.closeSync();
    }

    test.done();
  });
};
