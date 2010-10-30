/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Load configuration
var cfg = require("../config").cfg;

// Require modules
var
  mysql_libmysqlclient = require("../../mysql-libmysqlclient"),
  mysql_bindings = require("../../mysql_bindings");

var initAndRealConnectSync = function (test) {
  test.expect(2);
  
  var conn = mysql_libmysqlclient.createConnectionSync();
  
  conn.initSync();
  test.ok(!conn.connectedSync(), "conn.connectedSync() after conn.initSync()");
  conn.realConnectSync(cfg.host, cfg.user, cfg.password);
  test.ok(conn.connectedSync(), "conn.connectedSync() after conn.realConnectSync()");
  conn.closeSync();
  
  test.done();
};

var multiRealQueryAndNextAndMoreSync = function (test) {
  test.expect(5);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
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
  test.ok(res instanceof mysql_bindings.MysqlResult, "SELECT query res instanceof mysql_bindings.MysqlResult");
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
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    r1, r2, r3;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password)");
  
  conn.realQuerySync("SHOW TABLES;");
  res = conn.storeResultSync();
  test.ok(res instanceof mysql_bindings.MysqlResult, "SHOW TABLES query res instanceof mysql_bindings.MysqlResult");
  test.ok(res, "conn.realQuerySync('SHOW TABLES;') and conn.storeResultSync()");
  r1 = res.fetchAllSync();
  conn.realQuerySync("SHOW TABLES;");
  res = conn.useResultSync();
  test.ok(res, "conn.realQuerySync('SHOW TABLES;') and conn.useResultSync()");
  r2 = res.fetchAllSync();
  r3 = conn.querySync("SHOW TABLES;").fetchAllSync();
  test.same(r1, r3, "conn.realQuerySync('SHOW TABLES;') + conn.storeResultSync() === conn.querySync('SHOW TABLES;')");
  test.same(r2, r3, "conn.realQuerySync('SHOW TABLES;') + conn.useResultSync() === conn.querySync('SHOW TABLES;')");
  conn.closeSync();
  
  test.done();
};

exports.New = function (test) {
  test.expect(1);
  
  var conn = new mysql_bindings.MysqlConnection();
  test.ok(conn, "var conn = new mysql_bindings.MysqlConnection()");
  
  test.done();
};

exports.OptionsConstants = function (test) {
  test.expect(12);
  
  var conn = mysql_libmysqlclient.createConnectionSync();
  
  test.equals(conn.MYSQL_INIT_COMMAND, 3);
  test.equals(conn.MYSQL_OPT_COMPRESS, 1);
  test.equals(conn.MYSQL_OPT_CONNECT_TIMEOUT, 0);
  test.equals(conn.MYSQL_OPT_LOCAL_INFILE, 8);
  test.equals(conn.MYSQL_OPT_PROTOCOL, 9);
  test.equals(conn.MYSQL_OPT_READ_TIMEOUT, 11);
  test.equals(conn.MYSQL_OPT_RECONNECT, 20);
  test.equals(conn.MYSQL_OPT_WRITE_TIMEOUT, 12);
  test.equals(conn.MYSQL_READ_DEFAULT_FILE, 4);
  test.equals(conn.MYSQL_READ_DEFAULT_GROUP, 5);
  test.equals(conn.MYSQL_SET_CHARSET_DIR, 6);
  test.equals(conn.MYSQL_SET_CHARSET_NAME, 7);
  
  test.done();
};

exports.ConnectErrnoGetter = function (test) {
  test.expect(1);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database_denied);
  test.equals(conn.connectErrno, 1044, "conn.connectErrno");
  
  test.done();
};

exports.ConnectErrorGetter = function (test) {
  test.expect(1);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database_denied);

  test.ok(conn.connectError.match(new RegExp("Access denied for user '(" + cfg.user + "|)'@'.*' to database '" + cfg.database_denied + "'")), "conn.connectError");
  
  test.done();
};

exports.AffectedRowsSync = function (test) {
  test.expect(5);

  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res = true,
    random_number,
    random_boolean,
    affected_rows,
    i;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
  test.ok(res, "conn.querySync('DELETE FROM test_table')");
  
  for (i = 0; i < cfg.insert_rows_count; i += 1) {
    random_number = Math.round(Math.random() * 1000000);
    random_boolean = (Math.random() > 0.5) ? 1 : 0;
    res = conn.querySync("INSERT INTO " + cfg.test_table +
      " (random_number, random_boolean) VALUES ('" + random_number +
      "', '" + random_boolean + "');") && res;
  }
  test.equals(res, true, "Insert " + cfg.insert_rows_count + " rows into table " + cfg.test_table);

  res = conn.querySync("UPDATE " + cfg.test_table + " SET random_number=1;");
  test.equals(res, true, "Update " + cfg.insert_rows_count + " rows in table " + cfg.test_table);
  
  affected_rows = conn.affectedRowsSync();
  
  test.equals(affected_rows, cfg.insert_rows_count, "conn.affectedRowsSync()");
  
  conn.closeSync();
  
  test.done();
};

exports.ChangeUserSync = function (test) {
  test.expect(9);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password);
  
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

exports.Connect = function (test) {
  test.expect(1);
  
  var conn = new mysql_bindings.MysqlConnection();
  
  conn.connect(cfg.host, cfg.user, cfg.password, cfg.database, function (error) {
    test.ok(error === null, "conn.connect() for allowed database");
    conn.closeSync();
    
    test.done();
  });
};

exports.ConnectWithError = function (test) {
  test.expect(3);
  
  var conn = new mysql_bindings.MysqlConnection();
  
  conn.connect(cfg.host, cfg.user, cfg.password, cfg.database_denied, function (error) {
    test.ok(error === 1044, "conn.connect() to denied database");
    
    test.equals(conn.connectErrno, 1044, "conn.connectErrno");
    test.ok(conn.connectError.match(new RegExp("Access denied for user '(" + cfg.user + "|)'@'.*' to database '" + cfg.database_denied + "'")), "conn.connectError");
    
    test.done();
  });
};

exports.ConnectSync = function (test) {
  test.expect(2);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database);
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  conn.closeSync();
  test.ok(conn.connectSync(cfg.host, cfg.user, cfg.password), "conn.connectSync() without database selection");
  conn.closeSync();
  
  test.done();
};

exports.ConnectedSync = function (test) {
  test.expect(5);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database);
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
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database);
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  test.ok(conn.connectedSync(), "conn.connectedSync() after connect");
  conn.closeSync();
  test.ok(!conn.connectedSync(), "conn.connectedSync() after close");
  
  test.throws(function () {
    conn.closeSync();
  }, "conn.closeSync() with closed connection");
  
  test.done();
};

exports.ErrnoSync = function (test) {
  test.expect(3);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password)");
  
  res = conn.querySync("USE " + cfg.database_denied + ";");
  test.equals(conn.errnoSync(), 1044, "conn.errnoSync");
  res = conn.querySync("SELECT * FROM " + cfg.test_table_notexists + ";");
  test.equals(conn.errnoSync(), 1146, "conn.errnoSync");


  test.done();
};

exports.ErrorSync = function (test) {
  test.expect(3);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password)");
  
  res = conn.querySync("USE " + cfg.database_denied + ";");
  test.ok(conn.errorSync().match(new RegExp("Access denied for user '(" + cfg.user + "|)'@'.*' to database '" + cfg.database_denied + "'")), "conn.errorSync");
  res = conn.querySync("SELECT * FROM " + cfg.test_table_notexists + ";");
  test.equals(conn.errorSync(), "Table '" + cfg.database + "." + cfg.test_table_notexists + "' doesn't exist", "conn.errorSync");
  
  
  test.done();
};

exports.EscapeSync = function (test) {
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
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
  
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
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
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    row,
    field_count;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
  test.ok(res, "conn.querySync('DELETE FROM cfg.test_table')");
  
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('123456', '0');") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                    " (random_number, random_boolean) VALUES ('7', '1');") && res;
  test.ok(res, "conn.querySync('INSERT INTO cfg.test_table ...')");
  
  res = conn.querySync("SELECT random_number, random_boolean from " + cfg.test_table +
                   " WHERE random_boolean='0';", 1);
  test.ok(res, "conn.querySync('SELECT ... 1')");
  test.equals(conn.fieldCountSync(), 2, "conn.querySync('SELECT ...') && conn.fieldCountSync()");

  conn.closeSync();
  
  test.done();
};

exports.GetCharsetSync = function (test) {
  test.expect(4);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password),
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
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password);
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password)");
  
  test.ok(conn.setCharsetSync(cfg.charset), "conn.setCharsetSync()");
  test.equals(conn.getCharsetNameSync(), cfg.charset, "conn.getCharsetNameSync()");
  conn.closeSync();
  
  test.done();
};

exports.GetInfoSync = function (test) {
  test.expect(2);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database);
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  test.ok(conn.getInfoSync(), "conn.getInfoSync() after connection to allowed database");
  
  test.done();
};

exports.GetInfoStringSync = function (test) {
  test.expect(4);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database), res;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
  test.equals(res, true);
  res = conn.querySync("ALTER TABLE " + cfg.test_table + " ADD INDEX (random_number)");
  test.equals(res, true);
  test.equals(conn.getInfoStringSync(), "Records: 0  Duplicates: 0  Warnings: 0",
                                    "conn.getInfoStringSync() after ALTER TABLE");
  conn.closeSync();

  test.done();
};

exports.GetWarningsSync = function (test) {
  test.expect(2);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database), res;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table_notexists + ";");
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
  test.expect(6);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res = true,
    random_number,
    random_boolean,
    last_insert_id,
    i;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
  test.ok(res, "conn.querySync('DELETE FROM cfg.test_table')");
  
  res = conn.querySync("ALTER TABLE " + cfg.test_table + " ADD id BIGINT AUTO_INCREMENT, ADD INDEX (id);");
  test.ok(res, "conn.querySync('ALTER TABLE cfg.test_table ADD id BIGINT AUTO_INCREMENT, ADD INDEX (id)')");
  
  for (i = 0; i < cfg.insert_rows_count; i += 1) {
    random_number = Math.round(Math.random() * 1000000);
    random_boolean = (Math.random() > 0.5) ? 1 : 0;
    res = conn.querySync("INSERT INTO " + cfg.test_table +
      " (random_number, random_boolean) VALUES ('" + random_number +
      "', '" + random_boolean + "');") && res;
  }
  
  test.equals(res, true, "Insert " + cfg.insert_rows_count + " rows into table " + cfg.test_table);
  last_insert_id = conn.lastInsertIdSync();
  test.equals(last_insert_id, cfg.insert_rows_count, "conn.lastInsertIdSync()");
  
  res = conn.querySync("ALTER TABLE " + cfg.test_table + " DROP id;");
  test.ok(res, "conn.querySync('ALTER TABLE cfg.test_table  DROP id')");
  
  conn.closeSync();
  
  test.done();
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

exports.Query = function (test) {
  test.expect(3);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  conn.query("SHOW TABLES", function (err, result) {
    test.ok(result.fieldCount === 1, "show results field count === 1");
    var res = result.fetchAllSync();
    test.ok(res.some(function (r) {
      return r['Tables_in_' + cfg.database] === cfg.test_table;
    }), "find the test table in results");
    conn.closeSync();
    test.done();
  });
};

exports.QueryWithError = function (test) {
  test.expect(3);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  conn.query("SHOW TABLESaagh", function (err, result) {
    test.ok(err, "Error object is present");
    test.ok(!result, "Result is not defined");
    conn.closeSync();
    test.done();
  });
};

exports.QueryWithQuerySync = function (test) {
  test.expect(4);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database);
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  conn.query("SELECT SLEEP(2)", function (err, result) {
    test.ok(result, "Result is defined");
    test.ok(!err, "Error object is not present");
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
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("SHOW TABLES");
  test.ok(res instanceof mysql_bindings.MysqlResult, "SHOW TABLES query res instanceof mysql_bindings.MysqlResult");
  
  res = conn.querySync("SHOW TABLESaagh");
  test.equals(res, false, "SHOW TABLESaagh wrong query res === false");
  
  res = conn.querySync("INSERT INTO " + cfg.test_table + " (random_number, random_boolean) VALUES (0, 1);");
  test.equals(res, true, "INSERT query res === true");
  
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
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password);
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password)");
  
  test.ok(conn.selectDbSync(cfg.database), "conn.selectDbSync() for allowed database");
  test.ok(!conn.selectDbSync(cfg.database_denied), "conn.selectDbSync() for denied database");
  conn.closeSync();
  
  test.done();
};

exports.SetCharsetSync = function (test) {
  test.expect(2);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password);
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password)");
  
  test.ok(conn.setCharsetSync(cfg.charset), "conn.setCharsetSync()");
  conn.closeSync();
  
  test.done();
};

exports.SetOptionSync = function (test) {
  test.expect(2);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(),
    charset_map = {utf8: 'cp1251', cp1251: 'utf8', other: 'binary'},
    default_cs,
    other_cs;
  
  conn.connectSync(cfg.host, cfg.user, cfg.password);
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync().connect(host, user, password)");
  
  default_cs = conn.querySync("SHOW VARIABLES LIKE 'character_set_connection';").fetchAllSync()[0].Value;
  other_cs = charset_map[default_cs] ? charset_map[default_cs] : charset_map.other;
  conn.closeSync();
  conn.initSync();
  conn.setOptionSync(conn.MYSQL_INIT_COMMAND, "SET NAMES " + other_cs + ";");
  conn.realConnectSync(cfg.host, cfg.user, cfg.password);
  test.equals(conn.querySync("SHOW VARIABLES LIKE 'character_set_connection';").fetchAllSync()[0].Value, other_cs, "setOptionSync(MYSQL_INIT_COMMAND. 'SET NAMES')");
  conn.closeSync();
  
  test.done();
};

exports.SqlStateSync = function (test) {
  test.expect(2);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  test.equals(conn.sqlStateSync(), "00000", "conn.sqlStateSync() after connection to allowed database");
  conn.closeSync();
  
  test.done();
};

exports.StatSync = function (test) {
  test.expect(2);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res;
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
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database);
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
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database), res;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table_notexists + ";");
  test.equals(conn.warningCountSync(), 1, "conn.getWarningsSync() after DROP TABLE IF EXISTS test_table_notexists");
  conn.closeSync();
  
  test.done();
};

