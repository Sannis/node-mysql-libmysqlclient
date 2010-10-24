/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Load configuration
var cfg = require("../config").cfg;

// Require modules
var
  sys = require("sys"),
  mysql_libmysqlclient = require("../../mysql-libmysqlclient"),
  mysql_bindings = require("../../mysql_bindings");

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

exports.New = function (test) {
  test.expect(1);
  
  test.throws(function () {
    var res = new mysql_bindings.MysqlResult();
  }, TypeError, "new mysql_bindings.MysqlResult() should throw exception from JS code");
  
  test.done();
};

exports.DataSeekSync = function (test) {
  test.expect(6);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    row;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
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

exports.FetchAll = function (test) {
  test.expect(4);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("SHOW TABLES;");
  test.ok(res, "conn.querySync('SHOW TABLES;')");
  
  res.fetchAll(function (err, tables) {
    test.ok(err === null, "res.fetchAll() err===null");
    test.ok(tables, "res.fetchAll() result");
    conn.closeSync();
    
    test.done();
  });
};

exports.FetchAllWithArrayOption = function (test) {
  test.expect(5);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("SHOW TABLES;");
  test.ok(res, "conn.querySync('SHOW TABLES;')");
  
  res.fetchAll(true,function (err, tables) {
    test.ok(err === null, "res.fetchAll() err===null");
    test.ok(tables, "res.fetchAll() result");
    test.ok(Array.isArray(tables[0]), "Result returns an array of array");
    conn.closeSync();
    
    test.done();
  });
};

exports.FetchAllSync = function (test) {
  test.expect(7);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    tables,
    rows;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("SHOW TABLES;");
  test.ok(res, "conn.querySync('SHOW TABLES;')");
  tables = res.fetchAllSync();
  test.ok(tables, "res.fetchAllSync()");
  res = false;
  tables.forEach(function (table) {
    if (table.Tables_in_test === cfg.test_table) {
      res = true;
    }
  });
  test.ok(res, "res.fetchAllSync() show test table");
  
  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('1', '1');") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                    " (random_number, random_boolean) VALUES ('2', '1');") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('3', '0');") && res;
  test.ok(res, "INSERT");
  
  res = conn.querySync("SELECT random_number from " + cfg.test_table +
                   " WHERE random_boolean='0';");
  test.ok(res, "SELECT");
  rows = res.fetchAllSync();
  test.same(rows, [{random_number: 3}], "conn.querySync('SELECT ...').fetchAllSync()");
  
  conn.closeSync();
  
  test.done();
};

exports.FetchAllSyncWithArrayOption = function (test) {
  test.expect(8);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    tables,
    rows;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("SHOW TABLES;");
  test.ok(res, "conn.querySync('SHOW TABLES;')");
  tables = res.fetchAllSync();
  test.ok(tables, "res.fetchAllSync()");
  res = false;
  tables.forEach(function (table) {
    if (table.Tables_in_test === cfg.test_table) {
      res = true;
    }
  });
  test.ok(res, "res.fetchAllSync() show test table");
  
  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('1', '1');") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                    " (random_number, random_boolean) VALUES ('2', '1');") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('3', '0');") && res;
  test.ok(res, "INSERT");
  
  res = conn.querySync("SELECT random_number from " + cfg.test_table +
                   " WHERE random_boolean='0';");
  test.ok(res, "SELECT");
  rows = res.fetchAllSync(true);
  test.ok(Array.isArray(rows[0]), "Result returns an array of array");
  test.same(rows, [[3]], "conn.querySync('SELECT ...').fetchAllSync()");
  
  conn.closeSync();
  
  test.done();
};

exports.FetchArraySync = function (test) {
  test.expect(5);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    row;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
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
  
  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
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
  
  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
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

exports.FieldCountGetter = function (test) {
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
  test.equals(res.fieldCount, 2, "conn.querySync('SELECT ...') && res.fieldCount");

  conn.closeSync();
  
  test.done();
};

exports.FieldSeekSync = function (test) {
  testFieldSeekAndTellAndFetchAndFetchDirectAndFetchFieldsSync(test);
};

exports.FieldTellSync = function (test) {
  testFieldSeekAndTellAndFetchAndFetchDirectAndFetchFieldsSync(test);
};

exports.FreeSync = function (test) {
  test.expect(6);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    rows;
  
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
  test.ok(res, "conn.querySync('DELETE FROM cfg.test_table')");
  
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('1', '1');") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                    " (random_number, random_boolean) VALUES ('2', '1');") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('3', '0');") && res;
  test.ok(res, "conn.querySync('INSERT INTO cfg.test_table ...')");
  
  res = conn.querySync("SELECT random_number from " + cfg.test_table + ";");
  test.ok(res, "conn.querySync('SELECT ... 1')");
  rows = res.numRowsSync();
  test.equals(rows, 3, "conn.querySync('SELECT * ...').numRowsSync()");
  
  res.freeSync();
  
  test.throws(function () {
    rows = res.numRowsSync();
  }, "res.numRowsSync() after res.freeSync()");
  
  conn.closeSync();
  
  test.done();
};

exports.NumRowsSync = function (test) {
  test.expect(9);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    rows;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
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
  test.equals(rows, 1, "conn.querySync('SELECT ... 1').numRowsSync()");
  
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

