/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Load configuration
var cfg = require('../config.js');

// Require module
var path = require('path');

var initAndRealConnectSync = function (test) {
  test.expect(7);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync();
  
  // Test right code
  conn.initSync();
  test.ok(!conn.connectedSync(), "conn.connectedSync() after conn.initSync()");
  test.ok(
    conn.realConnectSync(cfg.host, cfg.user, cfg.password),
    "conn.realConnectSync(cfg.host, cfg.user, cfg.password)"
  );
  test.ok(conn.connectedSync(), "conn.connectedSync() after conn.realConnectSync()");
  conn.closeSync();
  
  // Test realConnectSync() without initSync()
  test.throws(function () {
    conn.realConnectSync(cfg.host, cfg.user, cfg.password);
  }, "Not initialized");
  
  // Test connectSync() after initSync()
  conn.initSync();
  test.ok(!conn.connectedSync(), "conn.connectedSync() after conn.initSync()");
  test.throws(function () {
    conn.connectSync(cfg.host, cfg.user, cfg.password);
  }, "Already initialized. Use conn.realConnectSync() after conn.initSync()");

  // Test realConnectSync() if already connected
  test.doesNotThrow(function () {
    conn.realConnectSync(cfg.host, cfg.user, cfg.password);
    conn.realConnectSync(cfg.host, cfg.user, cfg.password);
  });
  conn.closeSync();
  
  test.done();
};

var multiRealQueryAndNextAndMoreSync = function (test) {
  test.expect(5);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    rows,
    query;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password)");
  
  query  = "SELECT 1 as one;";
  query += "SELECT 2 as two;";
  query += "SELECT * FROM " + cfg.test_table_notexists + ";";
  // This statement will be ignored as the test_table_notexists doesn't exist
  // and the loop should quit
  query += "SHOW TABLES;";
  
  conn.multiRealQuerySync(query);
  // SELECT 1 as one;
  res = conn.storeResultSync();
  test.ok(res instanceof cfg.mysql_bindings.MysqlResult, "SELECT query res instanceof mysql_bindings.MysqlResult");
  rows = res.fetchAllSync();
  test.same(rows, [{one: 1}], "SELECT 1 as one;");
  conn.multiNextResultSync();
  // SELECT 2 as two;
  rows = conn.storeResultSync().fetchAllSync();
  test.same(rows, [{two: 2}], "SELECT 2 as two;");
  conn.multiNextResultSync();
  // SELECT * FROM test_table_notexists;
  test.equals(conn.errorSync(), "Table '" + cfg.database + "." + cfg.test_table_notexists + "' doesn't exist", "right error after select from notexists table");
  
  conn.closeSync();
  
  test.done();
};

var realQueryAndUseAndStoreResultSync = function (test) {
  test.expect(6);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    r1, r2, r3;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password)");
  
  conn.realQuerySync("SHOW TABLES;");
  res = conn.storeResultSync();
  test.ok(res instanceof cfg.mysql_bindings.MysqlResult, "SHOW TABLES query res instanceof mysql_bindings.MysqlResult");
  test.ok(res instanceof cfg.mysql_bindings.MysqlResult, "conn.realQuerySync('SHOW TABLES;') and conn.storeResultSync()");
  r1 = res.fetchAllSync();
  conn.realQuerySync("SHOW TABLES;");
  res = conn.useResultSync();
  test.ok(res instanceof cfg.mysql_bindings.MysqlResult, "conn.realQuerySync('SHOW TABLES;') and conn.useResultSync()");
  r2 = res.fetchAllSync();
  r3 = conn.querySync("SHOW TABLES;").fetchAllSync();
  test.same(r1, r3, "conn.realQuerySync('SHOW TABLES;') + conn.storeResultSync() === conn.querySync('SHOW TABLES;')");
  test.same(r2, r3, "conn.realQuerySync('SHOW TABLES;') + conn.useResultSync() === conn.querySync('SHOW TABLES;')");
  conn.closeSync();
  
  test.done();
};

exports.New = function (test) {
  test.expect(1);

  test.doesNotThrow(function () {
    var conn = new cfg.mysql_bindings.MysqlConnection();
  });

  test.done();
};

exports.ConnectFlagsConstants = function (test) {
  test.expect(9);

  var conn = cfg.mysql_libmysqlclient.createConnectionSync();

  test.equals(cfg.mysql_libmysqlclient.CLIENT_COMPRESS, 32);
  test.equals(cfg.mysql_libmysqlclient.CLIENT_FOUND_ROWS, 2);
  test.equals(cfg.mysql_libmysqlclient.CLIENT_IGNORE_SIGPIPE, 4096);
  test.equals(cfg.mysql_libmysqlclient.CLIENT_IGNORE_SPACE, 256);
  test.equals(cfg.mysql_libmysqlclient.CLIENT_INTERACTIVE, 1024);
  // Not yet implemented
  // test.equals(cfg.mysql_libmysqlclient.CLIENT_LOCAL_FILES, 128);
  test.equals(cfg.mysql_libmysqlclient.CLIENT_MULTI_RESULTS, 131072);
  test.equals(cfg.mysql_libmysqlclient.CLIENT_MULTI_STATEMENTS, 65536);
  test.equals(cfg.mysql_libmysqlclient.CLIENT_NO_SCHEMA, 16);
  // Known issue: cfg.mysql_libmysqlclient.CLIENT_REMEMBER_OPTIONS === -2147483648
  test.equals(cfg.mysql_libmysqlclient.CLIENT_REMEMBER_OPTIONS, -2147483648);

  conn.connectSync(cfg.host, cfg.user, cfg.password, cfg.database, null, null, cfg.mysql_libmysqlclient.CLIENT_REMEMBER_OPTIONS);
  conn.closeSync();

  test.done();
};

exports.SetOptionsConstants = function (test) {
  test.expect(11);

  test.equals(cfg.mysql_libmysqlclient.MYSQL_INIT_COMMAND, 3);
  test.equals(cfg.mysql_libmysqlclient.MYSQL_OPT_COMPRESS, 1);
  test.equals(cfg.mysql_libmysqlclient.MYSQL_OPT_CONNECT_TIMEOUT, 0);
  // Not yet implemented
  // test.equals(cfg.mysql_libmysqlclient.MYSQL_OPT_LOCAL_INFILE, 8);
  test.equals(cfg.mysql_libmysqlclient.MYSQL_OPT_PROTOCOL, 9);
  test.equals(cfg.mysql_libmysqlclient.MYSQL_OPT_READ_TIMEOUT, 11);
  test.equals(cfg.mysql_libmysqlclient.MYSQL_OPT_RECONNECT, 20);
  test.equals(cfg.mysql_libmysqlclient.MYSQL_OPT_WRITE_TIMEOUT, 12);
  test.equals(cfg.mysql_libmysqlclient.MYSQL_READ_DEFAULT_FILE, 4);
  test.equals(cfg.mysql_libmysqlclient.MYSQL_READ_DEFAULT_GROUP, 5);
  test.equals(cfg.mysql_libmysqlclient.MYSQL_SET_CHARSET_DIR, 6);
  test.equals(cfg.mysql_libmysqlclient.MYSQL_SET_CHARSET_NAME, 7);
  
  test.done();
};

exports.ConnectErrnoGetter = function (test) {
  test.expect(1);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database_denied);
  test.equals(conn.connectErrno, 1044, "conn.connectErrno");

  test.done();
};

exports.ConnectErrorGetter = function (test) {
  test.expect(1);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database_denied);

  var isRightError = conn.connectError.match(
    new RegExp("Access denied for user '(" + cfg.user + "|)'@'.*' to database '" + cfg.database_denied + "'")
  );

  if (!isRightError) {
    console.log("Wrong connect error message: '" + conn.connectError + "' for " + cfg.user + "@" + cfg.host);
  }

  test.ok(isRightError, "conn.connectError");
  
  test.done();
};

exports.AffectedRowsSync = function (test) {
  test.expect(5);

  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    random_number,
    random_boolean,
    affected_rows,
    i;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
  test.strictEqual(res, true);
  
  for (i = 0; i < cfg.insert_rows_count; i += 1) {
    random_number = Math.round(Math.random() * 1000000);
    random_boolean = (Math.random() > 0.5) ? 1 : 0;
    res = conn.querySync("INSERT INTO " + cfg.test_table +
      " (random_number, random_boolean) VALUES ('" + random_number +
      "', '" + random_boolean + "');") && res;
  }
  test.ok(res);

  setTimeout(function () {
    res = conn.querySync("UPDATE " + cfg.test_table + " SET random_number=1;");
    test.ok(res);

    affected_rows = conn.affectedRowsSync();
    test.equals(affected_rows, cfg.insert_rows_count, "conn.affectedRowsSync()");

    conn.closeSync();

    test.done();
  }, cfg.delay);
};

exports.ChangeUserSync = function (test) {
  test.expect(9);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password);
  
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password)");
  test.ok(conn.changeUserSync(cfg.user, cfg.password, cfg.database), "conn.changeUserSync() with database selection 1");
  test.ok(conn.changeUserSync(cfg.user, cfg.password), "conn.changeUserSync() without database selection");
  test.ok(!conn.changeUserSync(cfg.user, cfg.password, cfg.database_denied), "conn.changeUserSync() with denied database selection");
  test.ok(conn.changeUserSync(cfg.user, cfg.password, cfg.database), "conn.changeUserSync() with database selection 2");
  
  test.throws(function () {
    conn.changeUserSync(1, cfg.password);
  }, TypeError, "conn.changeUserSync() with not string user argument");
  
  test.throws(function () {
    conn.changeUserSync(cfg.user, 2);
  }, TypeError, "conn.changeUserSync() with not string password argument");
  
  test.throws(function () {
    conn.changeUserSync(cfg.user, cfg.password, 3);
  }, TypeError, "conn.changeUserSync() with not string database argument");
  
  test.throws(function () {
    conn.changeUserSync(cfg.user);
  }, TypeError, "conn.changeUserSync() without password argument");
  
  conn.closeSync();
  
  test.done();
};

exports.ConnectSync = function (test) {
  test.expect(1);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync();

  test.ok(conn.connectSync(cfg.host, cfg.user, cfg.password, cfg.database), "conn.connectSync()");

  conn.closeSync();
  
  test.done();
};

exports.ConnectSync2Times = function (test) {
  test.expect(2);

  var conn = cfg.mysql_libmysqlclient.createConnectionSync();

  test.ok(conn.connectSync(cfg.host, cfg.user, cfg.password, cfg.database), "conn.connectSync()");

  test.throws(function () {
    conn.connectSync(cfg.host, cfg.user, cfg.password, cfg.database);
  }, "Already initialized. Use conn.realConnectSync() after conn.initSync()");

  conn.closeSync();

  test.done();
};

exports.ConnectedSync = function (test) {
  test.expect(5);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database);
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  test.ok(conn.connectedSync(), "conn.connectedSync() after connect");
  conn.closeSync();
  test.ok(!conn.connectedSync(), "conn.connectedSync() after close");
  test.ok(conn.connectSync(cfg.host, cfg.user, cfg.password, cfg.database), "conn.connectSync()");
  test.ok(conn.connectedSync(), "conn.connectedSync() after reconnect");
  conn.closeSync();
  
  test.done();
};

exports.CloseSync = function (test) {
  test.expect(4);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database);
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  test.ok(conn.connectedSync(), "conn.connectedSync() after connect");
  conn.closeSync();
  test.ok(!conn.connectedSync(), "conn.connectedSync() after close");
  
  test.throws(function () {
    conn.closeSync();
  }, Error, "Not connected");
  
  test.done();
};

exports.ErrnoSync = function (test) {
  test.expect(4);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res;
  
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password)");
  
  test.strictEqual(conn.querySync("USE " + cfg.database_denied + ";"), false);
  test.equals(conn.errnoSync(), 1044, "conn.errnoSync");
  res = conn.querySync("SELECT * FROM " + cfg.test_table_notexists + ";");
  test.equals(conn.errnoSync(), 1146, "conn.errnoSync");

  test.done();
};

exports.ErrorSync = function (test) {
  test.expect(5);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database);
  
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password)");
  
  test.strictEqual(conn.querySync("USE " + cfg.database_denied + ";"), false);
  test.ok(conn.errorSync().match(new RegExp("Access denied for user '(" + cfg.user + "|)'@'.*' to database '" + cfg.database_denied + "'")), "conn.errorSync");
  test.strictEqual(conn.querySync("SELECT * FROM " + cfg.test_table_notexists + ";"), false);
  test.equals(conn.errorSync(), "Table '" + cfg.database + "." + cfg.test_table_notexists + "' doesn't exist", "conn.errorSync");

  test.done();
};

exports.EscapeSync = function (test) {
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    strings_to_escape = [
    ["test string", "test string"],
    ["test\\string", "test\\\\string"],
    ["test\nstring", "test\\nstring"],
    ["test\rstring", "test\\rstring"],
    ["test\"string", "test\\\"string"],
    ["test\'string", "test\\'string"],
    ["test \x00", "test \\0"],
    // utf8 test Français Größe
    ["utf8 test Fran\u00e7ais Gr\u00f6\u00dfe", "utf8 test Fran\u00e7ais Gr\u00f6\u00dfe"]
  ],
  str,
  str_esc_theor,
  str_esc_real,
  i;
  
  test.expect(strings_to_escape.length + 1);
  
  test.ok(conn, "cfg.mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  for (i in strings_to_escape) {
    if (typeof i === 'string') {
      str = strings_to_escape[i][0];
      str_esc_theor = strings_to_escape[i][1];
      str_esc_real = conn.escapeSync(strings_to_escape[i][0]);

      test.equals(str_esc_real, str_esc_theor, "conn.escapeSync(" + str + ") == " + str_esc_real + ", but must be " + str_esc_theor);
    }
  }
  
  conn.closeSync();
  
  test.done();
};

exports.FieldCountSync = function (test) {
  test.expect(5);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database), res;
  
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
  test.strictEqual(res, true);
  
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('123456', '0');") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                    " (random_number, random_boolean) VALUES ('7', '1');") && res;
  test.ok(res);
  
  res = conn.querySync("SELECT random_number, random_boolean from " + cfg.test_table +
                   " WHERE random_boolean='0';", 1);
  test.ok(res instanceof cfg.mysql_bindings.MysqlResult, "conn.querySync('SELECT ... 1')");
  test.equals(conn.fieldCountSync(), 2, "conn.querySync('SELECT ...') && conn.fieldCountSync()");

  conn.closeSync();
  
  test.done();
};

exports.GetCharsetSync = function (test) {
  test.expect(4);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password),
    charset_obj;
  
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password)");
  
  test.ok(conn.setCharsetSync(cfg.charset), "conn.setCharset()");
  charset_obj = conn.getCharsetSync();
  test.equals(charset_obj.charset, cfg.charset, "conn.getCharsetSync()");
  test.equals(charset_obj.collation.indexOf(cfg.charset), 0, "conn.getCharsetSync()");
  conn.closeSync();
  
  test.done();
};

exports.GetCharsetNameSync = function (test) {
  test.expect(3);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password);
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password)");
  
  test.ok(conn.setCharsetSync(cfg.charset), "conn.setCharsetSync()");
  test.equals(conn.getCharsetNameSync(), cfg.charset, "conn.getCharsetNameSync()");
  conn.closeSync();
  
  test.done();
};

exports.GetClientInfoSync = function (test) {
  test.expect(2);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(),
    client_info;
  
  client_info = conn.getClientInfoSync();
  
  test.ok(client_info.client_info, "conn.getClientInfoSync().client_info");
  test.ok(client_info.client_version, "conn.getClientInfoSync().client_version");
  
  test.done();
};

exports.GetInfoSync = function (test) {
  test.expect(2);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database);
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  test.ok(conn.getInfoSync(), "conn.getInfoSync() after connection to allowed database");
  
  test.done();
};

exports.GetInfoStringSync = function (test) {
  test.expect(4);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database), res;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
  test.strictEqual(res, true);

  res = conn.querySync("ALTER TABLE " + cfg.test_table + " ADD INDEX (random_number)");
  test.strictEqual(res, true);

  test.equals(conn.getInfoStringSync(), "Records: 0  Duplicates: 0  Warnings: 0",
                                    "conn.getInfoStringSync() after ALTER TABLE");
  conn.closeSync();

  test.done();
};

exports.GetWarningsSync = function (test) {
  test.expect(3);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database);
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  test.ok(conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table_notexists + ";"));
  test.same(conn.getWarningsSync(),
            [{errno: 1051, reason: "Unknown table '" + cfg.test_table_notexists + "'" }],
            "conn.getWarningsSync() after DROP TABLE IF EXISTS test_table_notexists");
  conn.closeSync();
  
  test.done();
};

exports.InitSync = function (test) {
  initAndRealConnectSync(test);
};

exports.LastInsertIdSync = function (test) {
  test.expect(5);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    random_number,
    random_boolean,
    last_insert_id,
    i;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
  test.strictEqual(res, true);
  
  res = conn.querySync("ALTER TABLE " + cfg.test_table + " AUTO_INCREMENT = 1;");
  test.strictEqual(res, true);
  
  for (i = 0; i < cfg.insert_rows_count; i += 1) {
    random_number = Math.round(Math.random() * 1000000);
    random_boolean = (Math.random() > 0.5) ? 1 : 0;
    res = conn.querySync("INSERT INTO " + cfg.test_table +
      " (random_number, random_boolean) VALUES ('" + random_number +
      "', '" + random_boolean + "');") && res;
  }
  test.ok(res);
  
  // Ensure all rows are inserted
  setTimeout(function () {
    last_insert_id = conn.lastInsertIdSync();
    test.equals(last_insert_id, cfg.insert_rows_count, "conn.lastInsertIdSync()");
    
    conn.closeSync();
    test.done();
  }, 100);
};

exports.MultiMoreResultsSync = function (test) {
  multiRealQueryAndNextAndMoreSync(test);
};

exports.MultiNextResultSync = function (test) {
  multiRealQueryAndNextAndMoreSync(test);
};

exports.MultiRealQuerySync = function (test) {
  multiRealQueryAndNextAndMoreSync(test);
};

exports.QueryWithQuerySync = function (test) {
  test.expect(4);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database);
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  conn.query("SELECT SLEEP(2)", function (err, res) {
    test.ok(res instanceof cfg.mysql_bindings.MysqlResult, "Result is defined");
    test.ok(err === null, "Error object is not present");
    test.done();
  });
  
  process.nextTick(function () {
    var res = conn.querySync("SHOW TABLES");
    test.ok(res.fetchAllSync(), "Synchronous result is defined");
  });
};

exports.QuerySync = function (test) {
  test.expect(4);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("SHOW TABLES");
  test.ok(res instanceof cfg.mysql_bindings.MysqlResult, "SHOW TABLES query res instanceof mysql_bindings.MysqlResult");
  
  res = conn.querySync("SHOW TABLESaagh");
  test.equals(res instanceof cfg.mysql_bindings.MysqlResult, false, "SHOW TABLESaagh wrong query res === false");
  
  res = conn.querySync("INSERT INTO " + cfg.test_table + " (random_number, random_boolean) VALUES (0, 1);");
  test.strictEqual(res, true);
  
  conn.closeSync();
  
  test.done();
};

exports.RealConnectSync = function (test) {
  initAndRealConnectSync(test);
};

exports.RealQuerySync = function (test) {
  realQueryAndUseAndStoreResultSync(test);
};

exports.SelectDbSync = function (test) {
  test.expect(3);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password);
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password)");
  
  test.ok(conn.selectDbSync(cfg.database), "conn.selectDbSync() for allowed database");
  test.ok(!conn.selectDbSync(cfg.database_denied), "conn.selectDbSync() for denied database");
  conn.closeSync();
  
  test.done();
};

exports.SetCharsetSync = function (test) {
  test.expect(2);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password);
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password)");
  
  test.ok(conn.setCharsetSync(cfg.charset), "conn.setCharsetSync()");
  conn.closeSync();
  
  test.done();
};

exports.SetOptionSync = function (test) {
  test.expect(2);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(),
    charset_map = {utf8: 'cp1251', cp1251: 'utf8', other: 'binary'},
    default_cs,
    other_cs;
  
  conn.connectSync(cfg.host, cfg.user, cfg.password);
  test.ok(conn.connectedSync(), "mysql_libmysqlclient.createConnectionSync().connectSync(host, user, password).connectedSync()");
  
  default_cs = conn.querySync("SHOW VARIABLES LIKE 'character_set_connection';").fetchAllSync()[0].Value;
  other_cs = charset_map[default_cs] ? charset_map[default_cs] : charset_map.other;
  conn.closeSync();
  
  // Test MYSQL_INIT_COMMAND
  conn.initSync();
  conn.setOptionSync(cfg.mysql_libmysqlclient.MYSQL_INIT_COMMAND, "SET NAMES " + other_cs + ";");
  conn.realConnectSync(cfg.host, cfg.user, cfg.password);
  test.equals(conn.querySync("SHOW VARIABLES LIKE 'character_set_connection';").fetchAllSync()[0].Value, other_cs, "setOptionSync(MYSQL_INIT_COMMAND. 'SET NAMES')");
  conn.closeSync();
  
  // Test MYSQL_OPT_RECONNECT
  conn.initSync();
  conn.setOptionSync(cfg.mysql_libmysqlclient.MYSQL_OPT_RECONNECT, 1);
  conn.realConnectSync(cfg.host, cfg.user, cfg.password);
  conn.closeSync();
  
  // Test MYSQL_OPT_RECONNECT (issue #90)
  conn.initSync();
  conn.setOptionSync(cfg.mysql_libmysqlclient.MYSQL_OPT_CONNECT_TIMEOUT, 3);
  conn.realConnectSync(cfg.host, cfg.user, cfg.password);
  conn.closeSync();
  
  test.done();
};

exports.SetSslSync = function (test) {
  test.expect(3);

  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(),
    key = path.resolve(__dirname, '../ssl-fixtures/client-key.pem'),
    cert = path.resolve(__dirname, '../ssl-fixtures/client-cert.pem'),
    ca = path.resolve(__dirname, '../ssl-fixtures/ca-cert.pem');

  conn.initSync();
  test.throws(function () {
    conn.setSslSync(key, cert, ca);
  });
  conn.setSslSync(key, cert, ca, "", "ALL");
  conn.realConnectSync(cfg.host, cfg.user, cfg.password, cfg.database);
  test.ok(conn.connectedSync());
  conn.closeSync();

  conn.initSync();
  conn.setSslSync(null, null, ca, "", "ALL");
  conn.realConnectSync(cfg.host, cfg.user, cfg.password, cfg.database);
  test.ok(conn.connectedSync());
  conn.closeSync();

  test.done();
};

exports.SqlStateSync = function (test) {
  test.expect(2);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database);
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  test.equals(conn.sqlStateSync(), "00000", "conn.sqlStateSync() after connection to allowed database");
  conn.closeSync();
  
  test.done();
};

exports.StatSync = function (test) {
  test.expect(2);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database);
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  test.equals(typeof conn.statSync(), "string", "typeof conn.statSync() is a string");
  conn.closeSync();
  
  test.done();
};

exports.StoreResultSync = function (test) {
  realQueryAndUseAndStoreResultSync(test);
};

exports.ThreadSafeSync = function (test) {
  test.expect(2);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database);
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  test.ok(conn.threadSafeSync(), "conn.threadSafeSync() === true");
  conn.closeSync();
  
  test.done();
};

exports.UseResultSync = function (test) {
  realQueryAndUseAndStoreResultSync(test);
};

exports.WarningCountSync = function (test) {
  test.expect(2);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database), res;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table_notexists + ";");
  test.equals(conn.warningCountSync(), 1, "conn.getWarningsSync() after DROP TABLE IF EXISTS test_table_notexists");
  conn.closeSync();
  
  test.done();
};

