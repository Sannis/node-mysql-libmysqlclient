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
    test.ok(err === null, "Error object is not present");
    test.ok(conn instanceof cfg.mysql_bindings.MysqlConnection);

    if (conn.connectedSync()) {
      conn.closeSync();
    }

    test.done();
  });
};

exports.mysql_libmysqlclient_createConnection_2 = function (test) {
  test.expect(3);

  cfg.mysql_libmysqlclient.createConnection(cfg.host, function (err, conn) {
    test.ok(err instanceof Error, "Error object is present");
    test.ok(!conn, "Not connected");

    test.ok(err.message.match(/Connection error.*Access denied for user/));

    test.done();
  });
};

exports.mysql_libmysqlclient_createConnection_3_Function = function (test) {
  test.expect(2);

  cfg.mysql_libmysqlclient.createConnection(cfg.host, cfg.user, function (err, conn) {
    test.ok(err instanceof Error, "Error object is present");

    var error = "Access denied for user '" + cfg.user + "'@'" + cfg.host + "' (using password: NO)";
    var errno = 1045;

    test.equals(err.message, "Connection error #" + errno + ": " + error, "Callback exception");

    if (conn && conn.connectedSync()) {
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
    test.ok(err === null, "Error object is not present");
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
    test.ok(err === null, "Error object is not present");
    test.ok(conn instanceof cfg.mysql_bindings.MysqlConnection);

    var isConnected = conn.connectedSync();
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

exports.mysql_libmysqlclient_createConnection_6 = function (test) {
  test.expect(3);

  cfg.mysql_libmysqlclient.createConnection(cfg.host, cfg.user, cfg.password, cfg.database, cfg.port, function (err, conn) {
    test.ok(err === null, "Error object is not present");
    test.ok(conn instanceof cfg.mysql_bindings.MysqlConnection);

    var isConnected = conn.connectedSync();
    test.ok(isConnected, "cfg.mysql_libmysqlclient.createConnection(host, user, password, database, flags, callback)");

    if (!isConnected) {
      // Extra debug output
      console.log("Error:" + conn.connectError);
    } else {
      conn.closeSync();
    }

    test.done();
  });
};

exports.mysql_libmysqlclient_createConnection_8 = function (test) {
  test.expect(3);

  var compress_flag = cfg.mysql_bindings.CLIENT_COMPRESS;

  cfg.mysql_libmysqlclient.createConnection(cfg.host, cfg.user, cfg.password, cfg.database, null, null, compress_flag, function (err, conn) {
    test.ok(err === null, "Error object is not present");
    test.ok(conn instanceof cfg.mysql_bindings.MysqlConnection);

    var isConnected = conn.connectedSync();
    test.ok(isConnected, "cfg.mysql_libmysqlclient.createConnection(host, user, password, database, flags, callback)");

    if (!isConnected) {
      // Extra debug output
      console.log("Error:" + conn.connectError);
    } else {
      conn.closeSync();
    }

    test.done();
  });
};

exports.mysql_libmysqlclient_createConnectionSync_DSN_1 = function (test) {
  test.expect(3);

  var dsn = require('util').format("mysql://%s:%s@%s:%s", cfg.user, cfg.password, cfg.host, cfg.port);

  cfg.mysql_libmysqlclient.createConnection(dsn, function (err, conn) {
    test.ok(err === null, "Error object is not present");
    test.ok(conn instanceof cfg.mysql_bindings.MysqlConnection);

    var isConnected = conn.connectedSync();
    test.ok(isConnected, "cfg.mysql_libmysqlclient.createConnection(dsn(user, password, host, port), callback)");

    if (!isConnected) {
      // Extra debug output
      console.log("Error:" + conn.connectError);
    } else {
      conn.closeSync();
    }

    test.done();
  });
};

exports.mysql_libmysqlclient_createConnectionSync_DSN_2 = function (test) {
  test.expect(3);

  var dsn = require('util').format("mysql://%s:%s@%s:%s?qwerty=1234", cfg.user, cfg.password, cfg.host, cfg.port);

  cfg.mysql_libmysqlclient.createConnection(dsn, function (err, conn) {
    test.ok(err === null, "Error object is not present");
    test.ok(conn instanceof cfg.mysql_bindings.MysqlConnection);

    var isConnected = conn.connectedSync();
    test.ok(isConnected, "cfg.mysql_libmysqlclient.createConnection(dsn(user, password, host, port), callback)");

    if (!isConnected) {
      // Extra debug output
      console.log("Error:" + conn.connectError);
    } else {
      conn.closeSync();
    }

    test.done();
  });
};

exports.mysql_libmysqlclient_createConnectionSync_DSN_3 = function (test) {
  test.expect(3);

  var dsn = require('util').format("mysql://%s:%s@%s:%s/%s/zxcvbn?qwerty=1234", cfg.user, cfg.password, cfg.host, cfg.port, cfg.database);

  cfg.mysql_libmysqlclient.createConnection(dsn, function (err, conn) {
    test.ok(err === null, "Error object is not present");
    test.ok(conn instanceof cfg.mysql_bindings.MysqlConnection);

    var isConnected = conn.connectedSync();
    test.ok(isConnected, "cfg.mysql_libmysqlclient.createConnection(dsn(user, password, host, port, database), callback)");

    if (!isConnected) {
      // Extra debug output
      console.log("Error:" + conn.connectError);
    } else {
      conn.closeSync();
    }

    test.done();
  });
};
