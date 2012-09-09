/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Load configuration
var cfg = require('../config.js');

exports.New = function (test) {
  test.expect(1);
  
  test.throws(function () {
    var stmt = new cfg.mysql_bindings.MysqlStatement();
  }, TypeError, "new mysql_bindings.MysqlStatement() should throw exception from JS code");
  
  test.done();
};

var testAttrGetAndSetSync = function (test) {
  test.expect(6);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    stmt;

  stmt = conn.initStatementSync();
  
  test.equals(typeof stmt.attrGetSync(cfg.mysql_libmysqlclient.STMT_ATTR_UPDATE_MAX_LENGTH), "boolean", "Value of STMT_ATTR_UPDATE_MAX_LENGTH attribute is boolean");
  test.equals(typeof stmt.attrGetSync(cfg.mysql_libmysqlclient.STMT_ATTR_CURSOR_TYPE), "number", "Value of STMT_ATTR_CURSOR_TYPE attribute is number");
  test.equals(typeof stmt.attrGetSync(cfg.mysql_libmysqlclient.STMT_ATTR_PREFETCH_ROWS), "number", "Value of STMT_ATTR_PREFETCH_ROWS attribute is number");
  
  test.equals(stmt.attrGetSync(cfg.mysql_libmysqlclient.STMT_ATTR_PREFETCH_ROWS), 1, "Get default value of STMT_ATTR_PREFETCH_ROWS");
  stmt.attrSetSync(cfg.mysql_libmysqlclient.STMT_ATTR_PREFETCH_ROWS, 100);
  test.equals(stmt.attrGetSync(cfg.mysql_libmysqlclient.STMT_ATTR_PREFETCH_ROWS), 100, "Get new value of STMT_ATTR_PREFETCH_ROWS");
  stmt.attrSetSync(cfg.mysql_libmysqlclient.STMT_ATTR_PREFETCH_ROWS, 4294967295);
  test.equals(stmt.attrGetSync(cfg.mysql_libmysqlclient.STMT_ATTR_PREFETCH_ROWS), 4294967295, "Get new value of STMT_ATTR_PREFETCH_ROWS");
  
  conn.closeSync();
  
  test.done();
};

var testBindParamsAndExecuteSync = function (test) {
  test.expect(30);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
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

  // Check inserting for INT
  
  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
  test.strictEqual(res, true);
  
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
  row = res.fetchRowSync({"asArray": true});
  test.same(row, [3, 0], "conn.querySync('SELECT ... WHERE random_boolean=0').fetchRowSync()");
  
  stmt.closeSync();
  
  // Check inserting for VARCHAR, DOUBLE and NULL
  
  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
  test.strictEqual(res, true);
  
  res = conn.querySync("ALTER TABLE " + cfg.test_table + " ADD title VARCHAR(255), ADD number DOUBLE, ADD for_null INT NULL;");
  test.strictEqual(res, true);

  stmt2 = conn.initStatementSync();
  test.ok(stmt2);
  
  test.ok(stmt2.prepareSync("INSERT INTO " + cfg.test_table + " (title, number, for_null) VALUES (?, ?, ?);"));
  test.equals(stmt2.paramCount, 3);
  
  test.ok(stmt2.bindParamsSync([test_string, test_double, null]), "stmt.bindParamSync([test_string, test_double, null])");
  test.ok(stmt2.executeSync(), "stmt.bindParamSync([test_string, test_double, null]).executeSync()");

  res = conn.querySync("SELECT title, number, for_null from " + cfg.test_table + ";");
  rows = res.fetchAllSync();
  test.same(rows, [{title: test_string, number: test_double, for_null: null}], "conn.querySync('SELECT title, number, for_null ... ').fetchAllSync()");
  
  res = conn.querySync("ALTER TABLE " + cfg.test_table + " DROP title, DROP number, DROP for_null;");
  test.strictEqual(res, true);

  stmt2.closeSync();
  
  // Check inserting for VARCHAR, DOUBLE and NULL
  
  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
  test.strictEqual(res, true);
  
  res = conn.querySync("ALTER TABLE " + cfg.test_table + " ADD date DATE, ADD time TIME, ADD datetime DATETIME, ADD timestamp TIMESTAMP;");
  test.strictEqual(res, true);

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
  test.strictEqual(res, true);

  stmt3.closeSync();
  
  conn.closeSync();
  
  test.done();
};

exports.ParamCountGetter = function (test) {
  test.expect(4);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    stmt;

  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
  test.strictEqual(res, true);
  
  stmt = conn.initStatementSync();
  test.ok(stmt);
  test.ok(stmt.prepareSync("INSERT INTO " + cfg.test_table + " (random_number, random_boolean) VALUES (?, ?);"));
  test.equals(stmt.paramCount, 2, "Param count in INSERT INTO test_table + (random_number, random_boolean) VALUES (?, ?) query");
  stmt.closeSync();
  
  conn.closeSync();
  
  test.done();
};

exports.AffectedRowsSync = function (test) {
  test.expect(7);

  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    random_number,
    random_boolean,
    stmt,
    affected_rows,
    i;

  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
  test.strictEqual(res, true);

  for (i = 0; i < cfg.insert_rows_count; i += 1) {
    random_number = Math.round(Math.random() * 1000000);
    random_boolean = (Math.random() > 0.5) ? 1 : 0;
    res = conn.querySync("INSERT INTO " + cfg.test_table +
      " (random_number, random_boolean) VALUES ('" + random_number +
      "', '" + random_boolean + "');") && res;
  }
  test.ok(res);

  setTimeout(function () {
    stmt = conn.initStatementSync();
    test.ok(stmt);
    test.ok(stmt.prepareSync("UPDATE " + cfg.test_table + " SET random_number=?;"));
    test.ok(stmt.bindParamsSync([1]), "stmt.bindParamSync([1])");

    test.ok(stmt.executeSync(), "stmt.bindParamSync([1]).executeSync()");

    affected_rows = stmt.affectedRowsSync();
    test.equals(affected_rows, cfg.insert_rows_count, "stmt.affectedRowsSync()");

    conn.closeSync();

    test.done();
  }, cfg.delay);
};

exports.AttrGetSync = function (test) {
  testAttrGetAndSetSync(test);
};

exports.AttrSetSync = function (test) {
  testAttrGetAndSetSync(test);
};

exports.BindParamsSync = function (test) {
  testBindParamsAndExecuteSync(test);
};

exports.CloseSync = function (test) {
  test.expect(3);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    stmt;

  stmt = conn.initStatementSync();
  test.ok(stmt);
  test.ok(stmt.prepareSync("INSERT INTO " + cfg.test_table + " (random_number, random_boolean) VALUES (?, ?);"));

  stmt.closeSync();
  
  test.throws(function () {
    stmt.closeSync();
  }, Error, "Statement not initialized");
  
  conn.closeSync();
  
  test.done();
};

exports.DataSeekSync = function (test) {
  test.expect(4);

  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    random_number,
    random_boolean,
    stmt,
    i;

  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
  test.strictEqual(res, true);

  for (i = 0; i < cfg.insert_rows_count; i += 1) {
    random_number = Math.round(Math.random() * 1000000);
    random_boolean = 1;
    res = conn.querySync("INSERT INTO " + cfg.test_table +
      " (random_number, random_boolean) VALUES ('" + random_number +
      "', '" + random_boolean + "');") && res;
  }
  test.ok(res);

  setTimeout(function () {
    stmt = conn.initStatementSync();
    stmt.prepareSync("SELECT random_boolean FROM " + cfg.test_table + ";");
    stmt.executeSync();
    stmt.storeResultSync();

    test.throws(function () {
      stmt.dataSeekSync(-1);
    }, "Invalid row offset -1");

    test.throws(function () {
      stmt.dataSeekSync(-100);
    }, "Invalid row offset -100");

    stmt.closeSync();

    conn.closeSync();

    test.done();
  }, cfg.delay);
};

exports.LastInsertIdSync = function (test) {
  test.expect(10);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    stmt,
    lastInsertId;

  res = conn.querySync("TRUNCATE " + cfg.test_table + ";");
  test.strictEqual(res, true);
  
  stmt = conn.initStatementSync();
  test.ok(stmt);
  test.ok(stmt.prepareSync("INSERT INTO " + cfg.test_table + " (random_number, random_boolean) VALUES (?, ?);"));

  test.ok(stmt.bindParamsSync([1, 1]), "stmt.bindParamSync([1, 1])");
  test.ok(stmt.executeSync(), "stmt.bindParamSync(1, 1).executeSync()");
  
  test.ok(stmt.bindParamsSync([2, 1]), "stmt.bindParamSync([2, 1])");
  test.ok(stmt.executeSync(), "stmt.bindParamSync(1, 1).executeSync()");
  
  test.ok(stmt.bindParamsSync([3, 0]), "stmt.bindParamSync([3, 0])");
  test.ok(stmt.executeSync(), "stmt.bindParamSync(1, 1).executeSync()");
  
  lastInsertId = stmt.lastInsertIdSync();
  test.equals(lastInsertId, 3, "Last insert id after 3 rows inserded");
  
  stmt.closeSync();
  
  conn.closeSync();
  
  test.done();
};

exports.ResultMetadataSync = function (test) {
  test.expect(4);

  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    stmt,
    resultMetadata;

  stmt = conn.initStatementSync();
  test.ok(stmt);

  test.ok(stmt.prepareSync("SELECT 1 AS one, 2 AS two;"));

  test.ok(stmt.executeSync(), "stmt.bindParamSync([1]).executeSync()");

  resultMetadata = stmt.resultMetadataSync();
  test.ok(resultMetadata instanceof cfg.mysql_bindings.MysqlResult, "resultMetadata instanceof MysqlResult");

  stmt.closeSync();

  conn.closeSync();

  test.done();
};

exports.SendLongDataSync = function (test) {
  test.expect(10);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    stmt,
    res,
    rows,
    test_message1 = "MySQL -- The most popular Open Source database",
    test_message2 = "\n Second chunk of message";

  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
  test.strictEqual(res, true);
  
  res = conn.querySync("ALTER TABLE " + cfg.test_table + " ADD message TEXT;");
  test.strictEqual(res, true);

  stmt = conn.initStatementSync();
  test.ok(stmt);
  
  res = stmt.prepareSync("INSERT INTO " + cfg.test_table + " (message) VALUES (?);");
  test.strictEqual(res, true);
  
  res = stmt.bindParamsSync([""]);
  test.strictEqual(res, true);
  
  test.ok(stmt.sendLongDataSync(0, test_message1), "stmt.sendLongDataSync(0, test_message1)");
  test.ok(stmt.sendLongDataSync(0, test_message2), "stmt.sendLongDataSync(0, test_message2)");
  
  res = stmt.executeSync();
  test.strictEqual(res, true);
  
  res = conn.querySync("SELECT message from " + cfg.test_table + " WHERE message !='';");
  rows = res.fetchAllSync();
  test.same(rows, [{message: test_message1 + test_message2}], "conn.querySync('SELECT message ... ').fetchAllSync()");
  
  res = conn.querySync("ALTER TABLE " + cfg.test_table + " DROP message;");
  test.strictEqual(res, true);

  stmt.closeSync();

  conn.closeSync();
  
  test.done();
};

exports.SqlStateSync = function (test) {
  test.expect(1);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    stmt;

  stmt = conn.initStatementSync();
  test.equals(stmt.sqlStateSync(), "00000", "stmt.sqlStateSync()");

  stmt.closeSync();

  conn.closeSync();
  
  test.done();
};
