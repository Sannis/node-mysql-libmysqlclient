/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Load configuration
var cfg = require("./config").cfg;

// Require modules
var
  sys = require("sys"),
  mysql_libmysqlclient = require("../mysql-libmysqlclient");

/*
Complex test for methods:
res.fieldSeek();
res.fetchField();
res.fetchFieldDirect();
res.fetchFields();
*/
var testFieldSeekAndTellAndFetchAndFetchDirectAndFetchFields = function (test) {
  test.expect(9);
  
  var conn = mysql_libmysqlclient.createConnection(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    row,
    field1,
    field2,
    fields,
    field_tell;
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user, password, database)");
  
  res = conn.query("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  res = conn.query("CREATE TABLE " + cfg.test_table +
    " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
    " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
    " PRIMARY KEY (autoincrement_id)) TYPE=MEMORY;") && res;
  test.ok(res, "conn.query('DELETE FROM cfg.test_table')");
  
  res = conn.query("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('123456', '0');") && res;
  res = conn.query("INSERT INTO " + cfg.test_table +
                    " (random_number, random_boolean) VALUES ('7', '1');") && res;
  test.ok(res, "conn.query('INSERT INTO cfg.test_table ...')");
  
  res = conn.query("SELECT random_number, random_boolean from " + cfg.test_table +
                   " WHERE random_boolean='0';", 1);
  test.ok(res, "conn.query('SELECT ... 1')");

  res.fieldSeek(1);
  field_tell = res.fieldTell();
  test.equals(field_tell, 1, "res.fieldTell()");
  field1 = res.fetchField();
  test.ok(field1, "res.fetchField()");
  fields = res.fetchFields();
  test.same(field1, fields[1], "res.fetchFields() same check");

  res.fieldSeek(0);
  field1 = res.fetchField();
  test.ok(field1, "res.fetchField()");
  field2 = res.fetchFieldDirect(0);
  test.same(field1, field2, "res.fetchFieldDirect() same check");
  
  conn.close();
  
  test.done();
};

exports.DataSeek = function (test) {
  test.expect(6);
  
  var conn = mysql_libmysqlclient.createConnection(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    row;
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user, password, database)");
  
  res = conn.query("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  res = conn.query("CREATE TABLE " + cfg.test_table +
    " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
    " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
    " PRIMARY KEY (autoincrement_id)) TYPE=MEMORY;") && res;
  test.ok(res, "conn.query('DELETE FROM cfg.test_table')");
  
  res = conn.query("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('1', '1');") && res;
  res = conn.query("INSERT INTO " + cfg.test_table +
                    " (random_number, random_boolean) VALUES ('2', '1');") && res;
  res = conn.query("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('3', '0');") && res;
  test.ok(res, "conn.query('INSERT INTO cfg.test_table ...')");
  
  res = conn.query("SELECT random_number, random_boolean from " + cfg.test_table +
                   " WHERE random_boolean='1' ORDER BY random_number ASC;", 1);
  test.ok(res, "conn.query('SELECT ...')");
  res.dataSeek(1);
  row = res.fetchArray();
  test.same(row, [2, 1], "conn.query('SELECT ...').dataSeek().fetchArray()");
  res.dataSeek(0);
  row = res.fetchArray();
  test.same(row, [1, 1], "conn.query('SELECT ...').dataSeek().fetchArray()");
  
  conn.close();
  
  test.done();
};

exports.FetchAll = function (test) {
  test.expect(3);
  
  var conn = mysql_libmysqlclient.createConnection(cfg.host, cfg.user, cfg.password, cfg.database),
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
  
  var conn = mysql_libmysqlclient.createConnection(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    row;
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user, password, database)");
  
  res = conn.query("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  res = conn.query("CREATE TABLE " + cfg.test_table +
    " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
    " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
    " PRIMARY KEY (autoincrement_id)) TYPE=MEMORY;") && res;
  test.ok(res, "conn.query('DELETE FROM cfg.test_table')");
  
  res = conn.query("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('1', '1');") && res;
  res = conn.query("INSERT INTO " + cfg.test_table +
                    " (random_number, random_boolean) VALUES ('2', '1');") && res;
  res = conn.query("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('3', '0');") && res;
  test.ok(res, "conn.query('INSERT INTO cfg.test_table ...')");
  
  res = conn.query("SELECT random_number, random_boolean from " + cfg.test_table +
                   " WHERE random_boolean='0';", 1);
  test.ok(res, "conn.query('SELECT ...')");
  row = res.fetchArray();
  test.same(row, [3, 0], "conn.query('SELECT ...').fetchArray()");
  
  conn.close();
  
  test.done();
};

exports.FetchField = function (test) {
  testFieldSeekAndTellAndFetchAndFetchDirectAndFetchFields(test);
};

exports.FetchFieldDirect = function (test) {
  testFieldSeekAndTellAndFetchAndFetchDirectAndFetchFields(test);
};

exports.FetchFields = function (test) {
  testFieldSeekAndTellAndFetchAndFetchDirectAndFetchFields(test);
};

exports.FetchLengths = function (test) {
  test.expect(7);
  
  var conn = mysql_libmysqlclient.createConnection(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    row,
    lengths;
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user, password, database)");
  
  res = conn.query("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  res = conn.query("CREATE TABLE " + cfg.test_table +
    " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
    " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
    " PRIMARY KEY (autoincrement_id)) TYPE=MEMORY;") && res;
  test.ok(res, "conn.query('DELETE FROM cfg.test_table')");
  
  res = conn.query("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('123456', '0');") && res;
  res = conn.query("INSERT INTO " + cfg.test_table +
                    " (random_number, random_boolean) VALUES ('7', '1');") && res;
  test.ok(res, "conn.query('INSERT INTO cfg.test_table ...')");
  
  res = conn.query("SELECT random_number, random_boolean from " + cfg.test_table +
                   " WHERE random_boolean='0';", 1);
  test.ok(res, "conn.query('SELECT ... 1')");
  row = res.fetchArray();
  lengths = res.fetchLengths();
  test.same(lengths, [6, 1], "conn.query('SELECT ... 1').fetchArray()");

  res = conn.query("SELECT random_number, random_boolean from " + cfg.test_table +
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
  
  var conn = mysql_libmysqlclient.createConnection(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    row;
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user, password, database)");
  
  res = conn.query("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  res = conn.query("CREATE TABLE " + cfg.test_table +
    " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
    " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
    " PRIMARY KEY (autoincrement_id)) TYPE=MEMORY;") && res;
  test.ok(res, "conn.query('DELETE FROM cfg.test_table')");
  
  res = conn.query("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('1', '1');") && res;
  res = conn.query("INSERT INTO " + cfg.test_table +
                    " (random_number, random_boolean) VALUES ('2', '1');") && res;
  res = conn.query("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('3', '0');") && res;
  test.ok(res, "conn.query('INSERT INTO cfg.test_table ...')");
  
  res = conn.query("SELECT random_number, random_boolean from " + cfg.test_table +
                   " WHERE random_boolean='0';", 1);
  test.ok(res, "conn.query('SELECT ...')");
  row = res.fetchObject();
  test.same(row, {random_number: 3, random_boolean: 0}, "conn.query('SELECT ... 1').fetchObject()");
  
  conn.close();
  
  test.done();
};

exports.FieldCount = function (test) {
  test.expect(6);
  
  var conn = mysql_libmysqlclient.createConnection(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    row,
    field_count;
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user, password, database)");
  
  res = conn.query("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  res = conn.query("CREATE TABLE " + cfg.test_table +
    " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
    " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
    " PRIMARY KEY (autoincrement_id)) TYPE=MEMORY;") && res;
  test.ok(res, "conn.query('DELETE FROM cfg.test_table')");
  
  res = conn.query("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('123456', '0');") && res;
  res = conn.query("INSERT INTO " + cfg.test_table +
                    " (random_number, random_boolean) VALUES ('7', '1');") && res;
  test.ok(res, "conn.query('INSERT INTO cfg.test_table ...')");
  
  res = conn.query("SELECT random_number, random_boolean from " + cfg.test_table +
                   " WHERE random_boolean='0';", 1);
  test.ok(res, "conn.query('SELECT ... 1')");
  field_count = conn.fieldCount();
  test.equals(field_count, 2, "conn.query('SELECT ...') && conn.fieldCount()");
  field_count = res.fieldCount();
  test.equals(field_count, 2, "conn.query('SELECT ...') && res.fieldCount()");

  conn.close();
  
  test.done();
};

exports.FieldSeek = function (test) {
  testFieldSeekAndTellAndFetchAndFetchDirectAndFetchFields(test);
};

exports.FieldTell = function (test) {
  testFieldSeekAndTellAndFetchAndFetchDirectAndFetchFields(test);
};

exports.NumRows = function (test) {
  test.expect(9);
  
  var conn = mysql_libmysqlclient.createConnection(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    rows;
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user, password, database)");
  
  res = conn.query("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  res = conn.query("CREATE TABLE " + cfg.test_table +
    " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
    " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
    " PRIMARY KEY (autoincrement_id)) TYPE=MEMORY;") && res;
  test.ok(res, "conn.query('DELETE FROM cfg.test_table')");
  
  res = conn.query("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('1', '1');") && res;
  res = conn.query("INSERT INTO " + cfg.test_table +
                    " (random_number, random_boolean) VALUES ('2', '1');") && res;
  res = conn.query("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('3', '0');") && res;
  test.ok(res, "conn.query('INSERT INTO cfg.test_table ...')");
  
  res = conn.query("SELECT random_number from " + cfg.test_table +
                   " WHERE random_boolean='0';", 1);
  test.ok(res, "conn.query('SELECT ... 1')");
  rows = res.numRows();
  test.equals(rows, 1, "conn.query('SELECT ... 1').numRows()");
  
  res = conn.query("SELECT random_number from " + cfg.test_table +
                   " WHERE random_boolean='1';", 1);
  test.ok(res, "conn.query('SELECT ... 2')");
  rows = res.numRows();
  test.equals(rows, 2, "conn.query('SELECT ... 2').numRows()");
  
  res = conn.query("SELECT random_number from " + cfg.test_table +
                   " WHERE random_number>'0';", 1);
  test.ok(res, "conn.query('SELECT ... 3')");
  rows = res.numRows();
  test.equals(rows, 3, "conn.query('SELECT ... 3').numRows()");
  
  conn.close();
  
  test.done();
};

