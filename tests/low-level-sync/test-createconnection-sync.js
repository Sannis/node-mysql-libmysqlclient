/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Load configuration
var cfg = require('../config.js');

exports.mysql_libmysqlclient_createConnectionSync_0 = function (test) {
  test.expect(1);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync();
  test.ok(conn instanceof cfg.mysql_bindings.MysqlConnection, "mysql_libmysqlclient.createConnectionSync()");

  if (conn.connectedSync()) {
    conn.closeSync();
  }

  test.done();
};

exports.mysql_libmysqlclient_createConnectionSync_1 = function (test) {
  test.expect(2);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host);
  test.ok(conn instanceof cfg.mysql_bindings.MysqlConnection, "mysql_libmysqlclient.createConnectionSync(host)");

  test.ok(conn.connectError.match(/Access denied for user/));

  test.done();
};

exports.mysql_libmysqlclient_createConnectionSync_2 = function (test) {
  test.expect(2);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user), isConnected;
  test.ok(conn instanceof cfg.mysql_bindings.MysqlConnection, "mysql_libmysqlclient.createConnectionSync(host, user)");
  
  // This depends on password, connect(host, user) === connect(host, user, NULL)
  if (!cfg.password) {
    isConnected = conn.connectedSync();
    test.ok(isConnected, "mysql_libmysqlclient.createConnectionSync(host, user).connectedSync()");

    if (!isConnected) {
      // Extra debug output
      console.log("Error: " + conn.connectError);
    } else {
      conn.closeSync();
    }
  } else {
    test.equals(conn.connectError,
                "Access denied for user '" + cfg.user + "'@'" + cfg.host + "' (using password: NO)",
                "mysql_libmysqlclient.createConnectionSync(host, user).connectedSync()");
  }

  test.done();
};

exports.mysql_libmysqlclient_createConnectionSync_3 = function (test) {
  test.expect(2);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password), isConnected;
  test.ok(conn instanceof cfg.mysql_bindings.MysqlConnection, "cfg.mysql_libmysqlclient.createConnectionSync(host, user, password)");

  isConnected = conn.connectedSync();
  test.ok(isConnected, "cfg.mysql_libmysqlclient.createConnectionSync(host, user, password).connectedSync()");

  if (!isConnected) {
    // Extra debug output
    console.log("Error:" + conn.connectError);
  } else {
    conn.closeSync();
  }

  test.done();
};

exports.mysql_libmysqlclient_createConnectionSync_4_AccessAllowed = function (test) {
  test.expect(2);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database), isConnected;
  test.ok(conn instanceof cfg.mysql_bindings.MysqlConnection, "cfg.mysql_libmysqlclient.createConnectionSync(host, user, password, database)");

  isConnected = conn.connectedSync();
  test.ok(isConnected, "cfg.mysql_libmysqlclient.createConnectionSync(host, user, password, database) connects");

  if (!isConnected) {
    // Extra debug output
    console.log("Error:" + conn.connectError);
  } else {
    conn.closeSync();
  }

  test.done();
};

exports.mysql_libmysqlclient_createConnectionSync_4_AccessDenied = function (test) {
  test.expect(3);

  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database_denied), isConnected;
  test.ok(conn instanceof cfg.mysql_bindings.MysqlConnection, "cfg.mysql_libmysqlclient.createConnectionSync(host, user, password, database_denied)");

  isConnected = conn.connectedSync();
  test.ok(!isConnected, "cfg.mysql_libmysqlclient.createConnectionSync(host, user, password, database_denied) not connects");
  test.equals(conn.connectError,
              "Access denied for user '" + cfg.user + "'@'" + cfg.host + "' to database '" + cfg.database_denied + "'",
              "cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database_denied).connectError");

  if (isConnected) {
    conn.closeSync();
  }

  test.done();
};

exports.mysql_libmysqlclient_createConnectionSync_5 = function (test) {
  test.expect(2);

  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database, cfg.port),
    isConnected;
  test.ok(conn instanceof cfg.mysql_bindings.MysqlConnection, "cfg.mysql_libmysqlclient.createConnectionSync(host, user, password, database, null, port)");

  isConnected = conn.connectedSync();
  test.ok(isConnected, "cfg.mysql_libmysqlclient.createConnectionSync(host, user, password, database, null, port) connects");

  if (!isConnected) {
    // Extra debug output
    console.log("Error:" + conn.connectError);
  } else {
    conn.closeSync();
  }

  test.done();
};

exports.mysql_libmysqlclient_createConnectionSync_7 = function (test) {
  test.expect(2);

  var
    compress_flag = cfg.mysql_bindings.CLIENT_COMPRESS,
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database, null, null, compress_flag),
    isConnected;
  test.ok(conn instanceof cfg.mysql_bindings.MysqlConnection, "cfg.mysql_libmysqlclient.createConnectionSync(host, user, password, database, flags)");

  isConnected = conn.connectedSync();
  test.ok(isConnected, "cfg.mysql_libmysqlclient.createConnectionSync(host, user, password, database, flags) connects");

  if (!isConnected) {
    // Extra debug output
    console.log("Error:" + conn.connectError);
  } else {
    conn.closeSync();
  }

  test.done();
};

exports.mysql_libmysqlclient_createConnectionSync_DSN_1 = function (test) {
  test.expect(2);

  var
    dsn = require('util').format("mysql://%s:%s@%s:%s", cfg.user, cfg.password, cfg.host, cfg.port),
    conn = cfg.mysql_libmysqlclient.createConnectionSync(dsn),
    isConnected;
  test.ok(conn instanceof cfg.mysql_bindings.MysqlConnection, "cfg.mysql_libmysqlclient.createConnectionSync(dsn(user, password, host, port))");

  isConnected = conn.connectedSync();
  test.ok(isConnected, "cfg.mysql_libmysqlclient.createConnectionSync(dsn(user, password, host, port)) connects");

  if (!isConnected) {
    // Extra debug output
    console.log("Error:" + conn.connectError);
  } else {
    conn.closeSync();
  }

  test.done();
};

exports.mysql_libmysqlclient_createConnectionSync_DSN_2 = function (test) {
  test.expect(2);

  var
    dsn = require('util').format("mysql://%s:%s@%s:%s?qwerty=1234", cfg.user, cfg.password, cfg.host, cfg.port),
    conn = cfg.mysql_libmysqlclient.createConnectionSync(dsn),
    isConnected;
  test.ok(conn instanceof cfg.mysql_bindings.MysqlConnection, "cfg.mysql_libmysqlclient.createConnectionSync(dsn(user, password, host, port))");

  isConnected = conn.connectedSync();
  test.ok(isConnected, "cfg.mysql_libmysqlclient.createConnectionSync(dsn(user, password, host, port)) connects");

  if (!isConnected) {
    // Extra debug output
    console.log("Error:" + conn.connectError);
  } else {
    conn.closeSync();
  }

  test.done();
};

exports.mysql_libmysqlclient_createConnectionSync_DSN_3 = function (test) {
  test.expect(2);

  var
    dsn = require('util').format("mysql://%s:%s@%s:%s/%s/zxcvbn?qwerty=1234", cfg.user, cfg.password, cfg.host, cfg.port, cfg.database),
    conn = cfg.mysql_libmysqlclient.createConnectionSync(dsn),
    isConnected;
  test.ok(conn instanceof cfg.mysql_bindings.MysqlConnection, "cfg.mysql_libmysqlclient.createConnectionSync(dsn(user, password, host, port, database))");

  isConnected = conn.connectedSync();
  test.ok(isConnected, "cfg.mysql_libmysqlclient.createConnectionSync(dsn(user, password, host, port, database)) connects");

  if (!isConnected) {
    // Extra debug output
    console.log("Error:" + conn.connectError);
  } else {
    conn.closeSync();
  }

  test.done();
};
