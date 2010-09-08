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

function AttrGetAndSetSync(test) {
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
}

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

exports.XXXTest = function (test) {
  test.expect(5);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    stmt,
    res,
    row;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
  test.ok(res, "conn.querySync('DELETE FROM cfg.test_table')");
  
  stmt = conn.initStatementSync();
  test.ok(stmt);
  test.ok(stmt.prepareSync("INSERT INTO " + cfg.test_table + " (random_number, random_boolean) VALUES ('?', '?');"));
  
  test.ok(stmt.bindParamSync(1, 1), "stmt.bindParamSync(1, 1)");
  test.ok(stmt.executeSync(), "stmt.bindParamSync(1, 1).executeSync()");
  
  test.ok(stmt.bindParamSync(2, 1), "stmt.bindParamSync(2, 1)");
  test.ok(stmt.executeSync(), "stmt.bindParamSync(1, 1).executeSync()");
  
  test.ok(stmt.bindParamSync(3, 0), "stmt.bindParamSync(3, 0)");
  test.ok(stmt.executeSync(), "stmt.bindParamSync(1, 1).executeSync()");
  
  res = conn.querySync("SELECT random_number, random_boolean from " + cfg.test_table + " WHERE random_boolean='0';");
  row = res.fetchArraySync();
  test.same(row, [3, 0], "conn.querySync('SELECT ...').fetchArraySync()");
  
  conn.closeSync();
  
  test.done();
};

