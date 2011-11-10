/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Load configuration
var cfg = require("../config");

exports.createTestTableForIssue102 = function (test) {
  test.expect(3);

  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    tables;

  conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  conn.querySync("CREATE TABLE " + cfg.test_table +
    "(`user_` int(11) NOT NULL," +
    " `karma` int(11) DEFAULT '0'," +
    " `energy` int(11) DEFAULT '0'," +
    " `zachet` int(11) DEFAULT '0'," +
    " `hvost` int(11) DEFAULT '0'," +
    " `digits` int(11) DEFAULT '0'," +
    " `level` int(11) DEFAULT '1'," +
    " PRIMARY KEY (`user_`)," +
    " KEY `level_karma` (`level`,`karma`)) " + cfg.store_engine + ";");
  res = conn.querySync("SHOW TABLES");
  tables = res.fetchAllSync();

  test.ok(res.fieldCount === 1, "SHOW TABLES result field count === 1");
  test.ok(tables.some(function (r) {
    return r['Tables_in_' + cfg.database] === cfg.test_table;
  }), "Find the test_table in results");

  res = conn.querySync("INSERT INTO " + cfg.test_table +
                   " (`user_`, `karma`, `energy`, `zachet`, `hvost`, `digits`, `level`)" +
                   " VALUES (1057691, 0, 400, 5, 5, 0, 1);");
  test.ok(res, "conn.querySync('INSERT INTO test_table ...')");

  conn.closeSync();
  test.done();
};

exports.issue102 = function (test) {
  test.expect(2);

  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    rows;

  res = conn.querySync("SELECT * FROM " + cfg.test_table + ";");

  test.ok(res, "conn.querySync('SELECT * FROM test_table;')");

  rows = res.fetchAllSync();

  test.equals(rows.length, 1, "1 row selected");

  test.done();
};
