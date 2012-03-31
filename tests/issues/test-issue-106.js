/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Load configuration
var cfg = require("../config");

exports.createTestTableForIssue106 = function (test) {
  test.expect(3);

  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    tables;

  conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  conn.querySync("CREATE TABLE " + cfg.test_table +
    "(`id` int(11) NOT NULL," +
    " `value` varchar(128)) " + cfg.store_engine + ";");
  res = conn.querySync("SHOW TABLES");
  tables = res.fetchAllSync();

  test.ok(res.fieldCount === 1, "SHOW TABLES result field count === 1");
  test.ok(tables.some(function (r) {
    return r['Tables_in_' + cfg.database] === cfg.test_table;
  }), "Find the test_table in results");

  res = conn.querySync("INSERT INTO " + cfg.test_table +
                   " (`id`, `value`)" +
                   " VALUES (1, 'one'), (2, 'two'), (3, 'three'), (4, 'four');");
  test.ok(res, "conn.querySync('INSERT INTO test_table ...')");

  conn.closeSync();
  test.done();
};

exports.issue106 = function (test) {
  test.expect(10);

  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    queryTemplate = "SELECT * FROM " + cfg.test_table + " WHERE `value` = '%'",
    queries = [];
  
  queries.push(queryTemplate.replace('%', 'one'));
  queries.push(queryTemplate.replace('%', 'three'));

  conn.query(queries.join(' UNION ALL '), function (err, res) {
    test.ok(res, "Result is defined");
    test.ok(!err, "Error object is not present");

    res.fetchAll(function (err, rows) {
      test.ok(res, "Result is defined");
      test.ok(!err, "Error object is not present");

      test.deepEqual(rows, [ { id: 1, value: 'one' }, { id: 3, value: 'three' } ]);

      conn.query(queries.join(' UNION ALL '), function (err, res) {
        test.ok(res, "Result is defined");
        test.ok(!err, "Error object is not present");
    
        res.fetchAll(function (err, rows) {
          test.ok(res, "Result is defined");
          test.ok(!err, "Error object is not present");
    
          test.deepEqual(rows, [ { id: 1, value: 'one' }, { id: 3, value: 'three' } ]);

          test.done();
        });
      });
    });
  });
};
