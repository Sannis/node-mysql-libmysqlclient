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

var testAttrGetAndSetSync = function (test) {
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
  stmt.attrSetSync(stmt.STMT_ATTR_PREFETCH_ROWS, 100);
  test.equals(stmt.attrGetSync(stmt.STMT_ATTR_PREFETCH_ROWS), 100, "Get new value of STMT_ATTR_PREFETCH_ROWS");
  stmt.attrSetSync(stmt.STMT_ATTR_PREFETCH_ROWS, 4294967295);
  test.equals(stmt.attrGetSync(stmt.STMT_ATTR_PREFETCH_ROWS), 4294967295, "Get new value of STMT_ATTR_PREFETCH_ROWS");
  
  conn.closeSync();
  
  test.done();
};

exports.ParamCountGetter = function (test) {
  test.expect(5);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
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
  testAttrGetAndSetSync(test);
};

exports.AttrSetSync = function (test) {
  testAttrGetAndSetSync(test);
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
  test.expect(31);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    stmt,
    stmt2,
    stmt3,
    res,
    rows,
    row,
    test_string = "1234",
    test_double = 1e30,
    date = new Date("Sat, 01 Jan 2000 00:00:00 GMT"),
    time_in = new Date("Sat, 03 Jan 1970 02:50:00 GMT"),
    time_out = new Date("Sat, 01 Jan 1970 02:50:00 GMT"),
    datetime = new Date("Tue, 25 Oct 1988 06:34:00 GMT"),
    timestamp = new Date("Tue, 25 Oct 1988 00:00:00 GMT");
  
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  // Check inserting for INT
  
  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
  test.ok(res, "conn.querySync('DELETE FROM cfg.test_table')");
  
  stmt = conn.initStatementSync();
  test.ok(stmt);
  test.ok(stmt.prepareSync("INSERT INTO " + cfg.test_table + " (random_number, random_boolean) VALUES (?, ?);"));
  test.equals(stmt.paramCount, 2, "Param count in INSERT INTO test_table + (random_number, random_boolean) VALUES (?, ?) query");
  
  test.ok(stmt.bindParamsSync([1, 1]), "stmt.bindParamSync([1, 1])");
  test.ok(stmt.executeSync(), "stmt.bindParamSync(1, 1).executeSync()");
  
  test.ok(stmt.bindParamsSync([2, 1]), "stmt.bindParamSync([2, 1])");
  test.ok(stmt.executeSync(), "stmt.bindParamSync(1, 1).executeSync()");
  
  test.ok(stmt.bindParamsSync([3, 0]), "stmt.bindParamSync([3, 0])");
  test.ok(stmt.executeSync(), "stmt.bindParamSync(1, 1).executeSync()");
  
  res = conn.querySync("SELECT random_number, random_boolean from " + cfg.test_table + " WHERE random_boolean=1;");
  rows = res.fetchAllSync();
  test.same(rows, [{random_number: 1, random_boolean: 1}, {random_number: 2, random_boolean: 1}], "conn.querySync('SELECT ... WHERE random_boolean=1').fetchAllSync()");
  
  res = conn.querySync("SELECT random_number, random_boolean from " + cfg.test_table + " WHERE random_boolean=0;");
  row = res.fetchArraySync();
  test.same(row, [3, 0], "conn.querySync('SELECT ... WHERE random_boolean=0').fetchArraySync()");
  
  // Check inserting for VARCHAR, DOUBLE and NULL
  
  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
  test.ok(res, "conn.querySync('DELETE FROM cfg.test_table')");
  
  res = conn.querySync("ALTER TABLE " + cfg.test_table + " ADD title VARCHAR(255), ADD number DOUBLE, ADD for_null INT;");
  test.ok(res, "conn.querySync('ALTER TABLE test_table ADD title VARCHAR(255)'), ADD number DOUBLE, ADD for_null INT");
  
  stmt2 = conn.initStatementSync();
  test.ok(stmt2);
  
  test.ok(stmt2.prepareSync("INSERT INTO " + cfg.test_table + " (title, number, for_null) VALUES (?, ?, ?);"));
  test.equals(stmt2.paramCount, 3, "Param count in INSERT INTO test_table (title, number, for_null) VALUES (?, ?, ?) query");
  
  test.ok(stmt2.bindParamsSync([test_string, test_double, null]), "stmt.bindParamSync([test_string, test_double, null])");
  test.ok(stmt2.executeSync(), "stmt.bindParamSync([test_string, test_double, null]).executeSync()");
  
  res = conn.querySync("SELECT title, number, for_null from " + cfg.test_table + ";");
  rows = res.fetchAllSync();
  test.same(rows, [{title: test_string, number: test_double, for_null: null}], "conn.querySync('SELECT title, number, for_null ... ').fetchAllSync()");
  
  res = conn.querySync("ALTER TABLE " + cfg.test_table + " DROP title, DROP number, DROP for_null;");
  test.ok(res, "conn.querySync('ALTER TABLE test_table  DROP title, DROP number, DROP for_null')");
  
  // Check inserting for VARCHAR, DOUBLE and NULL
  
  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
  test.ok(res, "conn.querySync('DELETE FROM cfg.test_table')");
  
  res = conn.querySync("ALTER TABLE " + cfg.test_table + " ADD date DATE, ADD time TIME, ADD datetime DATETIME, ADD timestamp TIMESTAMP;");
  test.ok(res, "conn.querySync('ALTER TABLE test_table ADD date DATE, ADD time TIME, ADD datetime DATETIME, ADD timestamp TIMESTAMP");
  
  stmt3 = conn.initStatementSync();
  test.ok(stmt3);
  
  test.ok(stmt3.prepareSync("INSERT INTO " + cfg.test_table + " (date, time, datetime, timestamp) VALUES (?, ?, ?, ?);"));
  test.equals(stmt3.paramCount, 4, "Param count in INSERT INTO test_table (date, time, datetime, timestamp) VALUES (?, ?, ?, ?) query");
  
  test.ok(stmt3.bindParamsSync([date, time_in, datetime, timestamp]), "stmt.bindParamSync([date, time_in, datetime, timestamp])");
  test.ok(stmt3.executeSync(), "stmt.bindParamSync([date, time_in, datetime, timestamp]).executeSync()");
  
  res = conn.querySync("SELECT date, time, datetime, timestamp from " + cfg.test_table + ";");
  rows = res.fetchAllSync();
  
  test.same(rows, [{date: date, time: time_out, datetime: datetime, timestamp: timestamp}], "conn.querySync('SELECT date, time, datetime, timestamp ... ').fetchAllSync()");
  
  res = conn.querySync("ALTER TABLE " + cfg.test_table + " DROP date, DROP time, DROP datetime, DROP timestamp;");
  test.ok(res, "conn.querySync('ALTER TABLE test_table DROP date, DROP time, DROP datetime, DROP timestamp')");
  
  conn.closeSync();
  
  test.done();
};

