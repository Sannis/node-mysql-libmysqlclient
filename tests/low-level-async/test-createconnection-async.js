/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Load configuration
var cfg = require('../config.js');

exports.mysql_libmysqlclient_createConnection_0 = function (test) {
  test.expect(1);

  test.throws(function () {
    cfg.mysql_libmysqlclient.createConnection();
  }, "require('mysql-libmysqlclient').createConnection() must get callback as last argument");

  test.done();
};

exports.mysql_libmysqlclient_createConnection_1_NoFunction = function (test) {
  test.expect(1);

  test.throws(function () {
    cfg.mysql_libmysqlclient.createConnection(cfg.host);
  }, "require('mysql-libmysqlclient').createConnection() must get callback as last argument");

  test.done();
};

exports.mysql_libmysqlclient_createConnection_1_Function = function (test) {
  test.expect(2);

  cfg.mysql_libmysqlclient.createConnection(function (err, conn) {
    test.ok(!err, "Error object is not present");
    test.ok(conn instanceof cfg.mysql_bindings.MysqlConnection);

    if (conn.connectedSync()) {
      conn.closeSync();
    }

    test.done();
  });
};

exports.mysql_libmysqlclient_createConnection_2 = function (test) {
  test.expect(2);

  cfg.mysql_libmysqlclient.createConnection(cfg.host, function (err, conn) {
    test.ok(!err, "Error object is not present");
    test.ok(conn instanceof cfg.mysql_bindings.MysqlConnection);

    if (conn.connectedSync()) {
      conn.closeSync();
    }

    test.done();
  });
};

exports.mysql_libmysqlclient_createConnection_3_Function = function (test) {
  test.expect(2);

  cfg.mysql_libmysqlclient.createConnection(cfg.host, cfg.user, function (err, conn) {
    test.ok(!err, "Error object is not present");
    test.ok(conn instanceof cfg.mysql_bindings.MysqlConnection);

    if (conn.connectedSync()) {
      conn.closeSync();
    }

    test.done();
  });
};

exports.mysql_libmysqlclient_createConnection_3_NoFunction = function (test) {
  test.expect(1);

  test.throws(function () {
    cfg.mysql_libmysqlclient.createConnection(cfg.host, cfg.user, cfg.password);
  }, "require('mysql-libmysqlclient').createConnection() must get callback as last argument");

  test.done();
};

exports.mysql_libmysqlclient_createConnection_4 = function (test) {
  test.expect(2);

  cfg.mysql_libmysqlclient.createConnection(cfg.host, cfg.user, cfg.password, function (err, conn) {
    test.ok(!err, "Error object is not present");
    test.ok(conn instanceof cfg.mysql_bindings.MysqlConnection);

    if (conn.connectedSync()) {
      conn.closeSync();
    }

    test.done();
  });
};

exports.mysql_libmysqlclient_createConnection_5_AccessAllowed = function (test) {
  test.expect(3);

  cfg.mysql_libmysqlclient.createConnection(cfg.host, cfg.user, cfg.password, cfg.database, function (err, conn) {
    test.ok(!err, "Error object is not present");
    test.ok(conn instanceof cfg.mysql_bindings.MysqlConnection);

    isConnected = conn.connectedSync();
    test.ok(isConnected, "cfg.mysql_libmysqlclient.createConnection(host, user, password, database, callback)");

    if (!isConnected) {
      // Extra debug output
      console.log("Error:" + conn.connectError);
    } else {
      conn.closeSync();
    }

    test.done();
  });
};

exports.mysql_libmysqlclient_createConnection_5_AccessDenied = function (test) {
  test.expect(3);

  cfg.mysql_libmysqlclient.createConnection(cfg.host, cfg.user, cfg.password, cfg.database_denied, function (err, conn) {
    test.ok(err, "Error object is present");
    test.ok(!conn, "Connection is not defined");

    var error = "Access denied for user '" + cfg.user + "'@'" + cfg.host + "' to database '" + cfg.database_denied + "'";
    var errno = 1044;

    test.equals(err.message, "Connection error #" + errno + ": " + error, "Callback exception");

    test.done();
  });
};
