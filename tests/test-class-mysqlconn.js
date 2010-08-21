/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Load configuration
var cfg = require("./config").cfg;

// Require modules
var
  sys = require("sys"),
  mysql_libmysqlclient = require("../mysql-libmysqlclient"),
  mysql_bindings = require("../mysql_bindings");

exports.New = function (test) {
  test.expect(1);
  
  var db = new mysql_bindings.MysqlConn();
  test.ok(db, "var db = new mysql_bindings.MysqlConn()");
  
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
  
  res = conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  res = conn.querySync("CREATE TABLE " + cfg.test_table +
    " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
    " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
    " PRIMARY KEY (autoincrement_id)) TYPE=MEMORY;");

  test.ok(res, "conn.querySync('DELETE FROM cfg.test_table')");
  
  for (i = 0; i < cfg.insert_rows_count; i += 1)
  {
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
  
  if (affected_rows !== cfg.insert_rows_count) {
    res = conn.querySync("UPDATE " + cfg.test_table + " SET random_number=1;");
  
    affected_rows = conn.affectedRowsSync();
  }
  
  test.equals(affected_rows, cfg.insert_rows_count, "conn.affectedRowsSync()");
  
  conn.closeSync();
  
  test.done();
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

exports.ConnectErrnoSync = function (test) {
  test.expect(2);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password);
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password)");
  conn.closeSync();
  conn.connectSync(cfg.host, cfg.user, cfg.password, cfg.database_denied);
  test.equals(conn.connectErrnoSync(), 1044, "conn.connectErrnoSync()");
  
  test.done();
};

exports.ConnectErrorSync = function (test) {
  test.expect(2);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password), error_str;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password)");
  conn.closeSync();
  conn.connectSync(cfg.host, cfg.user, cfg.password, cfg.database_denied);
  
  error_str = conn.connectErrorSync();

  if (error_str.match(new RegExp("Access denied for user '(" + cfg.user + "|)'@'.*' to database '" + cfg.database_denied + "'"))) {
    test.ok(true, "conn.connectErrorSync()");  
  } else {
    test.ok(false, "conn.connectErrorSync()");  
  }
  
  test.done();
};

exports.EscapeSync = function (test) {
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    strings_to_escape = [
    ["test string", "test string"],
    ["test\\string", "test\\\\string"],
    ["test\nstring", "test\\nstring"],
    ["test\rstring", "test\\rstring"],
    ["test\"string", "test\\\"string"],
    ["test\'string", "test\\'string"],
    ["test \x00", "test \\0"]
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

      test.equals(str_esc_real, str_esc_theor, "conn.escapeSync()");
    }
  }
  
  conn.closeSync();
  
  test.done();
};

exports.GetCharsetSync = function (test) {
  test.expect(4);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password),
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
  res = conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  test.equals(res, true);
  res = conn.querySync("CREATE TABLE " + cfg.test_table +
    " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
    " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
    " PRIMARY KEY (autoincrement_id)) TYPE=MEMORY;");
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
  res = conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  test.same(conn.getWarningsSync(), [],
            "conn.getWarningsSync() after DROP TABLE IF EXISTS");
  res = conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  test.same(conn.getWarningsSync(),
            [{errno: 1051, reason: "Unknown table '" + cfg.test_table + "'" }],
            "conn.getWarningsSync() after double DROP TABLE IF EXISTS");
  conn.closeSync();
  
  test.done();
};

exports.LastInsertIdSync = function (test) {
  test.expect(4);

  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res = true,
    random_number,
    random_boolean,
    last_insert_id,
    i;
  
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  res = conn.querySync("CREATE TABLE " + cfg.test_table +
    " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
    " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
    " PRIMARY KEY (autoincrement_id)) TYPE=MEMORY;");
  
  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
  test.ok(res, "conn.querySync('DELETE FROM cfg.test_table')");
  
  for (i = 0; i < cfg.insert_rows_count; i += 1)
  {
    random_number = Math.round(Math.random() * 1000000);
    random_boolean = (Math.random() > 0.5) ? 1 : 0;
    res = conn.querySync("INSERT INTO " + cfg.test_table +
      " (random_number, random_boolean) VALUES ('" + random_number +
      "', '" + random_boolean + "');") && res;
  }

  test.equals(res, true, "Insert " + cfg.insert_rows_count + " rows into table " + cfg.test_table);
  last_insert_id = conn.lastInsertIdSync();
  test.equals(last_insert_id, cfg.insert_rows_count, "conn.lastInsertIdSync()");
  conn.closeSync();
  
  test.done();
};

exports.QuerySync = function (test) {
  test.expect(2);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  res = conn.querySync("SHOW TABLES;");
  test.ok(res, "conn.querySync('SHOW TABLES;'");
  conn.closeSync();
  
  test.done();
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

exports.SqlStateSync = function (test) {
  test.expect(2);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database), res;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  test.equals(conn.sqlStateSync(), "00000", "conn.sqlStateSync() after connection to allowed database");
  conn.closeSync();
  
  test.done();
};

exports.WarningCountSync = function (test) {
  test.expect(1);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database), res;  
  res = conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  res = conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  test.equals(conn.warningCountSync(), 1, "conn.getWarningsSync() after double DROP TABLE IF EXISTS");
  conn.closeSync();
  
  test.done();
};

