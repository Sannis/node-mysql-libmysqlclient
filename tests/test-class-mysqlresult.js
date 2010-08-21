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
var testFieldSeekAndTellAndFetchAndFetchDirectAndFetchFieldsSync = function (test) {
  test.expect(9);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    row,
    field1,
    field2,
    fields,
    field_tell;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  res = conn.querySync("CREATE TABLE " + cfg.test_table +
    " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
    " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
    " PRIMARY KEY (autoincrement_id)) TYPE=MEMORY;") && res;
  test.ok(res, "conn.querySync('DELETE FROM cfg.test_table')");
  
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('123456', '0');") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                    " (random_number, random_boolean) VALUES ('7', '1');") && res;
  test.ok(res, "conn.querySync('INSERT INTO cfg.test_table ...')");
  
  res = conn.querySync("SELECT random_number, random_boolean from " + cfg.test_table +
                   " WHERE random_boolean='0';", 1);
  test.ok(res, "conn.querySync('SELECT ... 1')");

  res.fieldSeekSync(1);
  field_tell = res.fieldTellSync();
  test.equals(field_tell, 1, "res.fieldTellSync()");
  field1 = res.fetchFieldSync();
  test.ok(field1, "res.fetchFieldSync()");
  fields = res.fetchFieldsSync();
  test.same(field1, fields[1], "res.fetchFieldsSync() same check");

  res.fieldSeekSync(0);
  field1 = res.fetchFieldSync();
  test.ok(field1, "res.fetchFieldSync()");
  field2 = res.fetchFieldDirectSync(0);
  test.same(field1, field2, "res.fetchFieldDirectSync() same check");
  
  conn.closeSync();
  
  test.done();
};

exports.DataSeekSync = function (test) {
  test.expect(6);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    row;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  res = conn.querySync("CREATE TABLE " + cfg.test_table +
    " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
    " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
    " PRIMARY KEY (autoincrement_id)) TYPE=MEMORY;") && res;
  test.ok(res, "conn.querySync('DELETE FROM cfg.test_table')");
  
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('1', '1');") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                    " (random_number, random_boolean) VALUES ('2', '1');") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('3', '0');") && res;
  test.ok(res, "conn.querySync('INSERT INTO cfg.test_table ...')");
  
  res = conn.querySync("SELECT random_number, random_boolean from " + cfg.test_table +
                   " WHERE random_boolean='1' ORDER BY random_number ASC;");
  test.ok(res, "conn.querySync('SELECT ...')");
  res.dataSeekSync(1);
  row = res.fetchArraySync();
  test.same(row, [2, 1], "conn.querySync('SELECT ...').dataSeekSync().fetchArraySync()");
  res.dataSeekSync(0);
  row = res.fetchArraySync();
  test.same(row, [1, 1], "conn.querySync('SELECT ...').dataSeekSync().fetchArraySync()");
  
  conn.closeSync();
  
  test.done();
};

exports.FetchAllSync = function (test) {
  test.expect(3);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    tables;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  res = conn.querySync("SHOW TABLES;");
  test.ok(res, "conn.querySync('SHOW TABLES;')");
  tables = res.fetchAllSync();
  test.ok(tables, "res.fetchAllSync()");
  conn.closeSync();
  
  test.done();
};

exports.FetchArraySync = function (test) {
  test.expect(5);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    row;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  res = conn.querySync("CREATE TABLE " + cfg.test_table +
    " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
    " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
    " PRIMARY KEY (autoincrement_id)) TYPE=MEMORY;") && res;
  test.ok(res, "conn.querySync('DELETE FROM cfg.test_table')");
  
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('1', '1');") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                    " (random_number, random_boolean) VALUES ('2', '1');") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('3', '0');") && res;
  test.ok(res, "conn.querySync('INSERT INTO cfg.test_table ...')");
  
  res = conn.querySync("SELECT random_number, random_boolean from " + cfg.test_table +
                   " WHERE random_boolean='0';", 1);
  test.ok(res, "conn.querySync('SELECT ...')");
  row = res.fetchArraySync();
  test.same(row, [3, 0], "conn.querySync('SELECT ...').fetchArraySync()");
  
  conn.closeSync();
  
  test.done();
};

exports.FetchFieldSync = function (test) {
  testFieldSeekAndTellAndFetchAndFetchDirectAndFetchFieldsSync(test);
};

exports.FetchFieldDirectSync = function (test) {
  testFieldSeekAndTellAndFetchAndFetchDirectAndFetchFieldsSync(test);
};

exports.FetchFieldsSync = function (test) {
  testFieldSeekAndTellAndFetchAndFetchDirectAndFetchFieldsSync(test);
};

exports.FetchLengthsSync = function (test) {
  test.expect(7);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    row,
    lengths;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  res = conn.querySync("CREATE TABLE " + cfg.test_table +
    " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
    " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
    " PRIMARY KEY (autoincrement_id)) TYPE=MEMORY;") && res;
  test.ok(res, "conn.querySync('DELETE FROM cfg.test_table')");
  
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('123456', '0');") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                    " (random_number, random_boolean) VALUES ('7', '1');") && res;
  test.ok(res, "conn.querySync('INSERT INTO cfg.test_table ...')");
  
  res = conn.querySync("SELECT random_number, random_boolean from " + cfg.test_table +
                   " WHERE random_boolean='0';");
  test.ok(res, "conn.querySync('SELECT ... 1')");
  row = res.fetchArraySync();
  lengths = res.fetchLengthsSync();
  test.same(lengths, [6, 1], "conn.querySync('SELECT ... 1').fetchArraySync()");

  res = conn.querySync("SELECT random_number, random_boolean from " + cfg.test_table +
                   " WHERE random_boolean='1';");
  test.ok(res, "conn.querySync('SELECT ... 2')");
  row = res.fetchArraySync();
  lengths = res.fetchLengthsSync();
  test.same(lengths, [1, 1], "conn.querySync('SELECT ... 2').fetchArraySync()");

  conn.closeSync();
  
  test.done();
};

exports.FetchObjectSync = function (test) {
  test.expect(5);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    row;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  res = conn.querySync("CREATE TABLE " + cfg.test_table +
    " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
    " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
    " PRIMARY KEY (autoincrement_id)) TYPE=MEMORY;") && res;
  test.ok(res, "conn.querySync('DELETE FROM cfg.test_table')");
  
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('1', '1');") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                    " (random_number, random_boolean) VALUES ('2', '1');") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('3', '0');") && res;
  test.ok(res, "conn.querySync('INSERT INTO cfg.test_table ...')");
  
  res = conn.querySync("SELECT random_number, random_boolean from " + cfg.test_table +
                   " WHERE random_boolean='0';", 1);
  test.ok(res, "conn.querySync('SELECT ...')");
  row = res.fetchObjectSync();
  test.same(row, {random_number: 3, random_boolean: 0}, "conn.querySync('SELECT ... 1').fetchObjectSync()");
  
  conn.closeSync();
  
  test.done();
};

exports.FieldCountSync = function (test) {
  test.expect(6);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    row,
    field_count;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  res = conn.querySync("CREATE TABLE " + cfg.test_table +
    " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
    " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
    " PRIMARY KEY (autoincrement_id)) TYPE=MEMORY;") && res;
  test.ok(res, "conn.querySync('DELETE FROM cfg.test_table')");
  
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('123456', '0');") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                    " (random_number, random_boolean) VALUES ('7', '1');") && res;
  test.ok(res, "conn.querySync('INSERT INTO cfg.test_table ...')");
  
  res = conn.querySync("SELECT random_number, random_boolean from " + cfg.test_table +
                   " WHERE random_boolean='0';", 1);
  test.ok(res, "conn.querySync('SELECT ... 1')");
  field_count = conn.fieldCountSync();
  test.equals(field_count, 2, "conn.querySync('SELECT ...') && conn.fieldCountSync()");
  field_count = res.fieldCountSync();
  test.equals(field_count, 2, "conn.querySync('SELECT ...') && res.fieldCountSync()");

  conn.closeSync();
  
  test.done();
};

exports.FieldSeekSync = function (test) {
  testFieldSeekAndTellAndFetchAndFetchDirectAndFetchFieldsSync(test);
};

exports.FieldTellSync = function (test) {
  testFieldSeekAndTellAndFetchAndFetchDirectAndFetchFieldsSync(test);
};

exports.NumRowsSync = function (test) {
  test.expect(9);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    rows;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  res = conn.querySync("CREATE TABLE " + cfg.test_table +
    " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
    " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
    " PRIMARY KEY (autoincrement_id)) TYPE=MEMORY;") && res;
  test.ok(res, "conn.querySync('DELETE FROM cfg.test_table')");
  
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('1', '1');") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                    " (random_number, random_boolean) VALUES ('2', '1');") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('3', '0');") && res;
  test.ok(res, "conn.querySync('INSERT INTO cfg.test_table ...')");
  
  res = conn.querySync("SELECT random_number from " + cfg.test_table +
                   " WHERE random_boolean='0';");
  test.ok(res, "conn.querySync('SELECT ... 1')");
  rows = res.numRowsSync();
  test.equals(rows, 1, "conn.querySync('SELECT ... 1').numRows()");
  
  res = conn.querySync("SELECT random_number from " + cfg.test_table +
                   " WHERE random_boolean='1';");
  test.ok(res, "conn.querySync('SELECT ... 2')");
  rows = res.numRowsSync();
  test.equals(rows, 2, "conn.querySync('SELECT ... 2').numRowsSync()");
  
  res = conn.querySync("SELECT random_number from " + cfg.test_table +
                   " WHERE random_number>'0';");
  test.ok(res, "conn.querySync('SELECT ... 3')");
  rows = res.numRowsSync();
  test.equals(rows, 3, "conn.querySync('SELECT ... 3').numRowsSync()");
  
  conn.closeSync();
  
  test.done();
};

