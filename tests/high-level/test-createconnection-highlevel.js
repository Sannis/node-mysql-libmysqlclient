/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Load configuration
var cfg = require('../config');

exports.mysql_libmysqlclient_createConnectionHighlevelSync_0 = function (test) {
  test.expect(3);

  var conn = cfg.mysql_libmysqlclient.createConnectionHighlevelSync();
  test.ok(conn instanceof cfg.mysql_bindings.MysqlConnection);
  test.ok(conn instanceof cfg.mysql_libmysqlclient.MysqlConnectionQueued);
  test.ok(conn instanceof cfg.mysql_libmysqlclient.MysqlConnectionHighlevel);
  if (conn.connectedSync()) {
    conn.closeSync();
  }
  test.done();
};

exports.mysql_libmysqlclient_createConnectionHighlevelSync_4 = function (test) {
  test.expect(4);

  var conn = cfg.mysql_libmysqlclient.createConnectionHighlevelSync(cfg.host, cfg.user, cfg.password, cfg.database);
  test.ok(conn instanceof cfg.mysql_bindings.MysqlConnection);
  test.ok(conn instanceof cfg.mysql_libmysqlclient.MysqlConnectionQueued);
  test.ok(conn instanceof cfg.mysql_libmysqlclient.MysqlConnectionHighlevel);

  isConnected = conn.connectedSync();
  test.ok(isConnected, "cfg.mysql_libmysqlclient.createConnectionHighlevelSync(host, user, password, database) connects");

  if (!isConnected) {
    // Extra debug output
    console.log("Error:" + conn.connectError);
  } else {
    conn.closeSync();
  }

  test.done();
};

exports.mysql_libmysqlclient_createConnectionHighlevelSync_DSN = function (test) {
  test.expect(4);

  var
    dsn = require('util').format("mysql://%s:%s@%s:%s/%s/zxcvbn?qwerty=1234", cfg.user, cfg.password, cfg.host, cfg.port, cfg.database),
    conn = cfg.mysql_libmysqlclient.createConnectionHighlevelSync(dsn),
    isConnected;
  test.ok(conn instanceof cfg.mysql_bindings.MysqlConnection);
  test.ok(conn instanceof cfg.mysql_libmysqlclient.MysqlConnectionQueued);
  test.ok(conn instanceof cfg.mysql_libmysqlclient.MysqlConnectionHighlevel);

  isConnected = conn.connectedSync();
  test.ok(isConnected, "cfg.mysql_libmysqlclient.createConnectionHighlevelSync(dsn(user, password, host, port, database)) connects");

  if (!isConnected) {
    // Extra debug output
    console.log("Error:" + conn.connectError);
  } else {
    conn.closeSync();
  }

  test.done();
};

exports.mysql_libmysqlclient_createConnectionHighlevel_0 = function (test) {
  test.expect(1);

  test.throws(function () {
    cfg.mysql_libmysqlclient.createConnectionHighlevel();
  }, "require('mysql-libmysqlclient').createConnectionHighlevel() must get callback as last argument");

  test.done();
};

exports.mysql_libmysqlclient_createConnectionHighlevel_1 = function (test) {
  test.expect(4);

  cfg.mysql_libmysqlclient.createConnectionHighlevel(function (err, conn) {
    test.ok(err == null, "Error object is not present");
    test.ok(conn instanceof cfg.mysql_bindings.MysqlConnection);
    test.ok(conn instanceof cfg.mysql_libmysqlclient.MysqlConnectionQueued);
    test.ok(conn instanceof cfg.mysql_libmysqlclient.MysqlConnectionHighlevel);

    if (conn.connectedSync()) {
      conn.closeSync();
    }

    test.done();
  });
};

exports.mysql_libmysqlclient_createConnectionHighlevel_5 = function (test) {
  test.expect(5);

  cfg.mysql_libmysqlclient.createConnectionHighlevel(cfg.host, cfg.user, cfg.password, cfg.database, function (err, conn) {
    test.ok(err === null, "Error object is not present");
    test.ok(conn instanceof cfg.mysql_bindings.MysqlConnection);
    test.ok(conn instanceof cfg.mysql_libmysqlclient.MysqlConnectionQueued);
    test.ok(conn instanceof cfg.mysql_libmysqlclient.MysqlConnectionHighlevel);

    isConnected = conn.connectedSync();
    test.ok(isConnected, "cfg.mysql_libmysqlclient.createConnectionHighlevel(host, user, password, database, callback) connects");

    if (!isConnected) {
      // Extra debug output
      console.log("Error:" + conn.connectError);
    } else {
      conn.closeSync();
    }

    test.done();
  });
};

exports.mysql_libmysqlclient_createConnectionHighlevel_DSN = function (test) {
  test.expect(5);

  var dsn = require('util').format("mysql://%s:%s@%s:%s/%s/zxcvbn?qwerty=1234", cfg.user, cfg.password, cfg.host, cfg.port, cfg.database);

  cfg.mysql_libmysqlclient.createConnectionHighlevel(dsn, function (err, conn) {
    test.ok(err === null, "Error object is not present");
    test.ok(conn instanceof cfg.mysql_bindings.MysqlConnection);
    test.ok(conn instanceof cfg.mysql_libmysqlclient.MysqlConnectionQueued);
    test.ok(conn instanceof cfg.mysql_libmysqlclient.MysqlConnectionHighlevel);

    isConnected = conn.connectedSync();
    test.ok(isConnected, "cfg.mysql_libmysqlclient.createConnectionHighlevel(dsn(user, password, host, port, database), callback) connects");

    if (!isConnected) {
      // Extra debug output
      console.log("Error:" + conn.connectError);
    } else {
      conn.closeSync();
    }

    test.done();
  });
};
