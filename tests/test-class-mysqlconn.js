/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Mixin settings
/*global host, user, password, database, database_denied, charset, test_table, insert_rows_count */
process.mixin(require("./settings"));

// Require modules
var
  sys = require("sys"),
  mysql_libmysqlclient = require("../mysql-libmysqlclient"),
  mysql_bindings = require("../mysql_bindings");

exports.New = function (test) {
  test.expect(1);
  
  var db = new mysql_bindings.MysqlConn();
  test.ok(db, "var db = new MysqllibmysqlclientConn()");
  
  test.done();
};

exports.AffectedRows = function (test) {
  test.expect(5);

  var conn = mysql_libmysqlclient.createConnection(host, user, password, database),
    res = true,
    random_number,
    random_boolean,
    affected_rows,
    i;
  
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user, password, database)");
  
  res = conn.query("DROP TABLE IF EXISTS " + test_table + ";");
  res = conn.query("CREATE TABLE " + test_table +
    " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
    " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
    " PRIMARY KEY (autoincrement_id)) TYPE=MEMORY;");

  test.ok(res, "conn.query('DELETE FROM test_table')");
  
  for (i = 0; i < insert_rows_count; i += 1)
  {
    random_number = Math.round(Math.random() * 1000000);
    random_boolean = (Math.random() > 0.5) ? 1 : 0;
    res = conn.query("INSERT INTO " + test_table +
      " (random_number, random_boolean) VALUES ('" + random_number +
      "', '" + random_boolean + "');") && res;
  }
  test.equals(res, true, "Insert " + insert_rows_count + " rows into table " + test_table);

  res = conn.query("UPDATE " + test_table + " SET random_number=1;");
  test.equals(res, true, "Update " + insert_rows_count + " rows in table " + test_table);
  
  affected_rows = conn.affectedRows();
  sys.puts(sys.inspect(affected_rows));
  test.equals(affected_rows, insert_rows_count, "conn.affectedRows()");
  
  conn.close();
  
  test.done();
};

exports.Connect = function (test) {
  test.expect(2);
  
  var conn = mysql_libmysqlclient.createConnection(host, user, password, database);
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user, password, database)");
  conn.close();
  test.ok(conn.connect(host, user, password), "conn.connect() without database selection");
  conn.close();
  
  test.done();
};

exports.ConnectErrno = function (test) {
  test.expect(2);
  
  var conn = mysql_libmysqlclient.createConnection(host, user, password);
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user, password)");
  conn.close();
  conn.connect(host, user, password, database_denied);
  test.equals(conn.connectErrno(), 1044, "conn.connectErrno()");
  
  test.done();
};

exports.ConnectError = function (test) {
  test.expect(2);
  
  var conn = mysql_libmysqlclient.createConnection(host, user, password);
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user, password)");
  conn.close();
  conn.connect(host, user, password, database_denied);
  test.equals(conn.connectError(), "Access denied for user ''@'" + host +
              "' to database '" + database_denied + "'", "conn.connectError()");
  
  test.done();
};

exports.Escape = function (test) {
  var conn = mysql_libmysqlclient.createConnection(host, user, password, database),
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
  
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user, password, database)");
  
  for (i in strings_to_escape) {
    if (typeof i === 'string') {
      str = strings_to_escape[i][0];
      str_esc_theor = strings_to_escape[i][1];
      str_esc_real = conn.escape(strings_to_escape[i][0]);

      test.equals(str_esc_real, str_esc_theor, "conn.escape()");
    }
  }
  
  conn.close();
  
  test.done();
};

exports.GetCharset = function (test) {
  test.expect(4);
  
  var conn = mysql_libmysqlclient.createConnection(host, user, password),
    charset_obj;
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user, password)");
  test.ok(conn.setCharset(charset), "conn.setCharset()");
  charset_obj = conn.getCharset();
  test.equals(charset_obj.charset, charset, "conn.getCharset()");
  test.equals(charset_obj.collation.indexOf(charset), 0, "conn.getCharset()");
  conn.close();
  
  test.done();
};

exports.GetCharsetName = function (test) {
  test.expect(3);
  
  var conn = mysql_libmysqlclient.createConnection(host, user, password);
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user, password)");
  test.ok(conn.setCharset(charset), "conn.setCharset()");
  test.equals(conn.getCharsetName(), charset, "conn.getCharsetName()");
  conn.close();
  
  test.done();
};

exports.GetInfo = function (test) {
  test.expect(2);
  
  var conn = mysql_libmysqlclient.createConnection(host, user, password, database);
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user, password, database)");
  test.ok(conn.getInfo(), "conn.getInfo() after connection to allowed database");
  
  test.done();
};

exports.GetInfoString = function (test) {
  test.expect(4);
  
  var conn = mysql_libmysqlclient.createConnection(host, user, password, database), res;
  res = conn.query("DROP TABLE IF EXISTS " + test_table + ";");
  test.equals(res, true);
  res = conn.query("CREATE TABLE " + test_table +
    " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
    " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
    " PRIMARY KEY (autoincrement_id)) TYPE=MEMORY;");
  test.equals(res, true);
  res = conn.query("ALTER TABLE " + test_table + " ADD INDEX (random_number)");
  test.equals(res, true);
  test.equals(conn.getInfoString(), "Records: 0  Duplicates: 0  Warnings: 0",
                                    "conn.getInfoString() after ALTER TABLE");
  conn.close();

  test.done();
};

exports.GetWarnings = function (test) {
  test.expect(2);
  
  var conn = mysql_libmysqlclient.createConnection(host, user, password, database), res;  
  res = conn.query("DROP TABLE IF EXISTS " + test_table + ";");
  test.same(conn.getWarnings(), [],
            "conn.getWarnings() after DROP TABLE IF EXISTS");
  res = conn.query("DROP TABLE IF EXISTS " + test_table + ";");
  test.same(conn.getWarnings(),
            [{errno: 1051, reason: "Unknown table '" + test_table + "'" }],
            "conn.getWarnings() after double DROP TABLE IF EXISTS");
  conn.close();
  
  test.done();
};

exports.LastInsertId = function (test) {
  test.expect(4);

  var conn = mysql_libmysqlclient.createConnection(host, user, password, database),
    res = true,
    random_number,
    random_boolean,
    last_insert_id,
    i;
  
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user, password, database)");
  
  res = conn.query("DROP TABLE IF EXISTS " + test_table + ";");
  res = conn.query("CREATE TABLE " + test_table +
    " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
    " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
    " PRIMARY KEY (autoincrement_id)) TYPE=MEMORY;");
  
  res = conn.query("DELETE FROM " + test_table + ";");
  test.ok(res, "conn.query('DELETE FROM test_table')");
  
  for (i = 0; i < insert_rows_count; i += 1)
  {
    random_number = Math.round(Math.random() * 1000000);
    random_boolean = (Math.random() > 0.5) ? 1 : 0;
    res = conn.query("INSERT INTO " + test_table +
      " (random_number, random_boolean) VALUES ('" + random_number +
      "', '" + random_boolean + "');") && res;
  }

  test.equals(res, true, "Insert " + insert_rows_count + " rows into table " + test_table);
  last_insert_id = conn.lastInsertId();
  test.equals(last_insert_id, insert_rows_count, "conn.lastInsertId()");
  conn.close();
  
  test.done();
};

exports.Query = function (test) {
  test.expect(2);
  
  var conn = mysql_libmysqlclient.createConnection(host, user, password, database),
    res;
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user, password, database)");
  res = conn.query("SHOW TABLES;");
  test.ok(res, "conn.query('SHOW TABLES;'");
  conn.close();
  
  test.done();
};

exports.SelectDb = function (test) {
  test.expect(3);
  
  var conn = mysql_libmysqlclient.createConnection(host, user, password);
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user, password)");
  test.ok(conn.selectDb(database), "conn.selectDb() for allowed database");
  test.ok(!conn.selectDb(database_denied), "conn.selectDb() for denied database");
  conn.close();
  
  test.done();
};

exports.SetCharset = function (test) {
  test.expect(2);
  
  var conn = mysql_libmysqlclient.createConnection(host, user, password);
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user, password)");
  test.ok(conn.setCharset(charset), "conn.setCharset()");
  conn.close();
  
  test.done();
};

exports.SqlState = function (test) {
  test.expect(4);
  
  var conn = mysql_libmysqlclient.createConnection(host, user, password, database), res;
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user, password, database)");
  test.equals(conn.sqlState(), "00000", "conn.sqlState() after connection to allowed database");
  conn.close();
  res = conn.connect(host, user, password, database_denied);
  test.ok(!res, "conn.connect(host, user, password, database_denied)");
  test.equals(conn.sqlState(), "42000", "conn.sqlState() after connection to denied database");
  
  test.done();
};

exports.WarningCount = function (test) {
  test.expect(1);
  
  var conn = mysql_libmysqlclient.createConnection(host, user, password, database), res;  
  res = conn.query("DROP TABLE IF EXISTS " + test_table + ";");
  res = conn.query("DROP TABLE IF EXISTS " + test_table + ";");
  test.equals(conn.warningCount(), 1, "conn.getWarnings() after double DROP TABLE IF EXISTS");
  conn.close();
  
  test.done();
};

