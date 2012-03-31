/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Load configuration
var cfg = require("../config");

exports.createTestTableForIssue110 = function (test) {
  test.expect(3);

  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    tables;

  conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  conn.querySync("CREATE TABLE " + cfg.test_table +
    "(`int` int NOT NULL," +
    " `bigint` bigint, " + 
    " `decimal` decimal(40)) " + cfg.store_engine + ";");
  res = conn.querySync("SHOW TABLES");
  tables = res.fetchAllSync();

  test.ok(res.fieldCount === 1, "SHOW TABLES result field count === 1");
  test.ok(tables.some(function (r) {
    return r['Tables_in_' + cfg.database] === cfg.test_table;
  }), "Find the test_table in results");

  res = conn.querySync("INSERT INTO " + cfg.test_table +
                   " (`int`, `bigint`, `decimal`)" +
                   " VALUES (1, 1234, 1234567890), (2, 1234567890123456789, 12345678901234567890);");
  test.ok(res, "conn.querySync('INSERT INTO test_table ...')");

  conn.closeSync();
  test.done();
};

exports.issue110 = function (test) {
  test.expect(3);

  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    rows;

  res = conn.querySync("SELECT * FROM " + cfg.test_table + ";");

  test.ok(res, "conn.querySync('SELECT * FROM test_table;')");

  rows = res.fetchAllSync();

  test.ok(typeof rows[0]['bigint'] === 'string', "BIGINT returns as string");
  
  test.ok(rows[1]['bigint'] === '1234567890123456789', "BIGINT returns as correct string");
  
  /*test.same(rows,
           [ { 'int': 1, 'bigint': '1234', 'decimal': '1234567890' },
             { 'int': 2, 'bigint': '1234567890123456789', 'decimal': '12345678901234567890' } ],
           "SELECT result is correct"
  );*/

  test.done();
};
