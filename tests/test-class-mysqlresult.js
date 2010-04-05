/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Mixin settings
/*global host, user, password, database, database_denied, charset, test_table */
process.mixin(require("./settings"));

// Require modules
var
  sys = require("sys"),
  mysql_libmysqlclient = require("../mysql-libmysqlclient");

exports.NumRows = function (test) {
  test.expect(9);
  
  var conn = mysql_libmysqlclient.createConnection(host, user, password, database),
    res,
    rows;
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user, password, database)");
  
  res = conn.query("DROP TABLE IF EXISTS " + test_table + ";");
  res = conn.query("CREATE TABLE " + test_table +
    " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
    " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
    " PRIMARY KEY (autoincrement_id)) TYPE=MEMORY;") && res;
  test.ok(res, "conn.query('DELETE FROM test_table')");
  
  res = conn.query("INSERT INTO " + test_table +
                   " (random_number, random_boolean) VALUES ('1', '1');") && res;
  res = conn.query("INSERT INTO " + test_table +
                    " (random_number, random_boolean) VALUES ('2', '1');") && res;
  res = conn.query("INSERT INTO " + test_table +
                   " (random_number, random_boolean) VALUES ('3', '0');") && res;
  test.ok(res, "conn.query('INSERT INTO test_table ...')");
  
  res = conn.query("SELECT random_number from " + test_table +
                   " WHERE random_boolean='0';", 1);
  test.ok(res, "conn.query('SELECT ... 1')");
  rows = res.numRows();
  test.equals(rows, 1, "conn.query('SELECT ... 1').numRows()");
  
  res = conn.query("SELECT random_number from " + test_table +
                   " WHERE random_boolean='1';", 1);
  test.ok(res, "conn.query('SELECT ... 2')");
  rows = res.numRows();
  test.equals(rows, 2, "conn.query('SELECT ... 2').numRows()");
  
  res = conn.query("SELECT random_number from " + test_table +
                   " WHERE random_number>'0';", 1);
  test.ok(res, "conn.query('SELECT ... 3')");
  rows = res.numRows();
  test.equals(rows, 3, "conn.query('SELECT ... 3').numRows()");
  
  conn.close();
  
  test.done();
};

exports.FetchAll = function (test) {
  test.expect(3);
  
  var conn = mysql_libmysqlclient.createConnection(host, user, password, database),
    res,
    tables;
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user, password, database)");
  res = conn.query("SHOW TABLES;");
  test.ok(res, "conn.query('SHOW TABLES;')");
  tables = res.fetchAll();
  test.ok(tables, "res.fetchAll()");
  conn.close();
  
  test.done();
};

exports.FetchArray = function (test) {
  test.expect(5);
  
  var conn = mysql_libmysqlclient.createConnection(host, user, password, database),
    res,
    row;
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user, password, database)");
  
  res = conn.query("DROP TABLE IF EXISTS " + test_table + ";");
  res = conn.query("CREATE TABLE " + test_table +
    " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
    " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
    " PRIMARY KEY (autoincrement_id)) TYPE=MEMORY;") && res;
  test.ok(res, "conn.query('DELETE FROM test_table')");
  
  res = conn.query("INSERT INTO " + test_table +
                   " (random_number, random_boolean) VALUES ('1', '1');") && res;
  res = conn.query("INSERT INTO " + test_table +
                    " (random_number, random_boolean) VALUES ('2', '1');") && res;
  res = conn.query("INSERT INTO " + test_table +
                   " (random_number, random_boolean) VALUES ('3', '0');") && res;
  test.ok(res, "conn.query('INSERT INTO test_table ...')");
  
  res = conn.query("SELECT random_number, random_boolean from " + test_table +
                   " WHERE random_boolean='0';", 1);
  test.ok(res, "conn.query('SELECT ...')");
  row = res.fetchArray();
  test.same(row, [3, 0], "conn.query('SELECT ...').fetchArray()");
  
  conn.close();
  
  test.done();
};

exports.FetchLengths = function (test) {
  test.expect(7);
  
  var conn = mysql_libmysqlclient.createConnection(host, user, password, database),
    res,
    row,
    lengths;
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user, password, database)");
  
  res = conn.query("DROP TABLE IF EXISTS " + test_table + ";");
  res = conn.query("CREATE TABLE " + test_table +
    " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
    " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
    " PRIMARY KEY (autoincrement_id)) TYPE=MEMORY;") && res;
  test.ok(res, "conn.query('DELETE FROM test_table')");
  
  res = conn.query("INSERT INTO " + test_table +
                   " (random_number, random_boolean) VALUES ('123456', '0');") && res;
  res = conn.query("INSERT INTO " + test_table +
                    " (random_number, random_boolean) VALUES ('7', '1');") && res;
  test.ok(res, "conn.query('INSERT INTO test_table ...')");
  
  res = conn.query("SELECT random_number, random_boolean from " + test_table +
                   " WHERE random_boolean='0';", 1);
  test.ok(res, "conn.query('SELECT ... 1')");
  row = res.fetchArray();
  lengths = res.fetchLengths();
  test.same(lengths, [6, 1], "conn.query('SELECT ... 1').fetchArray()");

  res = conn.query("SELECT random_number, random_boolean from " + test_table +
                   " WHERE random_boolean='1';", 1);
  test.ok(res, "conn.query('SELECT ... 2')");
  row = res.fetchArray();
  lengths = res.fetchLengths();
  test.same(lengths, [1, 1], "conn.query('SELECT ... 2').fetchArray()");

  conn.close();
  
  test.done();
};

exports.FetchObject = function (test) {
  test.expect(5);
  
  var conn = mysql_libmysqlclient.createConnection(host, user, password, database),
    res,
    row;
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user, password, database)");
  
  res = conn.query("DROP TABLE IF EXISTS " + test_table + ";");
  res = conn.query("CREATE TABLE " + test_table +
    " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
    " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
    " PRIMARY KEY (autoincrement_id)) TYPE=MEMORY;") && res;
  test.ok(res, "conn.query('DELETE FROM test_table')");
  
  res = conn.query("INSERT INTO " + test_table +
                   " (random_number, random_boolean) VALUES ('1', '1');") && res;
  res = conn.query("INSERT INTO " + test_table +
                    " (random_number, random_boolean) VALUES ('2', '1');") && res;
  res = conn.query("INSERT INTO " + test_table +
                   " (random_number, random_boolean) VALUES ('3', '0');") && res;
  test.ok(res, "conn.query('INSERT INTO test_table ...')");
  
  res = conn.query("SELECT random_number, random_boolean from " + test_table +
                   " WHERE random_boolean='0';", 1);
  test.ok(res, "conn.query('SELECT ...')");
  row = res.fetchObject();
  test.same(row, {random_number: 3, random_boolean: 0}, "conn.query('SELECT ... 1').fetchObject()");
  
  conn.close();
  
  test.done();
};

