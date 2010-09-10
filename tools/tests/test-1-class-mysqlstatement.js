/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Load configuration
var cfg = require("../config").cfg;

// Require modules
var
  sys = require("sys"),
  mysql_libmysqlclient = require("../../mysql-libmysqlclient");

var AttrGetAndSetSync = function(test) {
  test.expect(7);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    stmt;
  
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  stmt = conn.initStatementSync();
  
  test.equals(typeof stmt.attrGetSync(stmt.STMT_ATTR_UPDATE_MAX_LENGTH), "boolean", "Value of STMT_ATTR_UPDATE_MAX_LENGTH attribute is boolean");
  test.equals(typeof stmt.attrGetSync(stmt.STMT_ATTR_CURSOR_TYPE), "number", "Value of STMT_ATTR_CURSOR_TYPE attribute is number");
  test.equals(typeof stmt.attrGetSync(stmt.STMT_ATTR_PREFETCH_ROWS), "number", "Value of STMT_ATTR_PREFETCH_ROWS attribute is number");
  
  test.equals(stmt.attrGetSync(stmt.STMT_ATTR_PREFETCH_ROWS), 1, "Get default value of STMT_ATTR_PREFETCH_ROWS");
  stmt.attrSetSync(stmt.STMT_ATTR_PREFETCH_ROWS, 100)
  test.equals(stmt.attrGetSync(stmt.STMT_ATTR_PREFETCH_ROWS), 100, "Get new value of STMT_ATTR_PREFETCH_ROWS");
  stmt.attrSetSync(stmt.STMT_ATTR_PREFETCH_ROWS, 4294967295)
  test.equals(stmt.attrGetSync(stmt.STMT_ATTR_PREFETCH_ROWS), 4294967295, "Get new value of STMT_ATTR_PREFETCH_ROWS");
  
  
  conn.closeSync();
  
  test.done();
};

exports.ParamCountGetter = function (test) {
  test.expect(5);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    stmt;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
  test.ok(res, "conn.querySync('DELETE FROM cfg.test_table')");
  
  stmt = conn.initStatementSync();
  test.ok(stmt);
  test.ok(stmt.prepareSync("INSERT INTO " + cfg.test_table + " (random_number, random_boolean) VALUES (?, ?);"));
  test.equals(stmt.paramCount, 2, "Param count in INSERT INTO test_table + (random_number, random_boolean) VALUES (?, ?) query");
  stmt.closeSync();
  
  conn.closeSync();
  
  test.done();
};

exports.AttrGetSync = function (test) {
  AttrGetAndSetSync(test);
};

exports.AttrSetSync = function (test) {
  AttrGetAndSetSync(test);
};

exports.SqlStateSync = function (test) {
  test.expect(2);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    stmt;
  
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  stmt = conn.initStatementSync();
  test.equals(stmt.sqlStateSync(), "00000", "stmt.sqlStateSync()");
  
  conn.closeSync();
  
  test.done();
};

exports.BindParamsSync = function (test) {
  test.expect(22);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    stmt,
    stmt2,
    res,
    rows,
    row,
    test_string = "1234",
    test_double = 1e30;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
  test.ok(res, "conn.querySync('DELETE FROM cfg.test_table')");
  
  stmt = conn.initStatementSync();
  test.ok(stmt);
  test.ok(stmt.prepareSync("INSERT INTO " + cfg.test_table + " (random_number, random_boolean) VALUES (?, ?);"));
  test.equals(stmt.paramCount, 2, "Param count in INSERT INTO test_table + (random_number, random_boolean) VALUES (?, ?) query");
  
  test.ok(stmt.bindParamsSync([1, 1]), "stmt.bindParamSync(1, 1)");
  test.ok(stmt.executeSync(), "stmt.bindParamSync(1, 1).executeSync()");
  
  test.ok(stmt.bindParamsSync([2, 1]), "stmt.bindParamSync(2, 1)");
  test.ok(stmt.executeSync(), "stmt.bindParamSync(1, 1).executeSync()");
  
  test.ok(stmt.bindParamsSync([3, 0]), "stmt.bindParamSync(3, 0)");
  test.ok(stmt.executeSync(), "stmt.bindParamSync(1, 1).executeSync()");
  
  res = conn.querySync("SELECT random_number, random_boolean from " + cfg.test_table + " WHERE random_boolean=1;");
  rows = res.fetchAllSync();
  test.same(rows, [{random_number: 1, random_boolean: 1}, {random_number: 2, random_boolean: 1}], "conn.querySync('SELECT ... WHERE random_boolean=1').fetchAllSync()");
  
  res = conn.querySync("SELECT random_number, random_boolean from " + cfg.test_table + " WHERE random_boolean=0;");
  row = res.fetchArraySync();
  test.same(row, [3, 0], "conn.querySync('SELECT ... WHERE random_boolean=0').fetchArraySync()");
  
  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
  test.ok(res, "conn.querySync('DELETE FROM cfg.test_table')");
  
  res = conn.querySync("ALTER TABLE " + cfg.test_table + " ADD title VARCHAR(255), ADD number DOUBLE, ADD for_null INT;");
  test.ok(res, "conn.querySync('ALTER TABLE test_table ADD title VARCHAR(255)'), ADD number DOUBLE, ADD for_null INT");
  
  stmt2 = conn.initStatementSync();
  test.ok(stmt2);
  
  test.ok(stmt2.prepareSync("INSERT INTO " + cfg.test_table + " (title, number, for_null) VALUES (?, ?, ?);"));
  test.equals(stmt2.paramCount, 3, "Param count in INSERT INTO test_table (title, number, for_null) VALUES (?, ?, ?) query");
  
  test.ok(stmt2.bindParamsSync([test_string, test_double, null]), "stmt.bindParamSync(string)");
  test.ok(stmt2.executeSync(), "stmt.bindParamSync([test_string, test_double]).executeSync()");
  
  res = conn.querySync("SELECT title, number, for_null from " + cfg.test_table + ";");
  rows = res.fetchAllSync();
  test.same(rows, [{title: test_string, number: test_double, for_null: null}], "conn.querySync('SELECT title, number, for_null ... ').fetchAllSync()");
  
  res = conn.querySync("ALTER TABLE " + cfg.test_table + " DROP title, DROP number, DROP for_null;");
  test.ok(res, "conn.querySync('ALTER TABLE test_table  DROP title, DROP number, DROP for_null')");
  
  conn.closeSync();
  
  test.done();
};

