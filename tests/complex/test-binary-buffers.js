/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Load configuration
var cfg = require("../config").cfg;

// Require modules
var
  mysql_libmysqlclient = require("../../mysql-libmysqlclient");

exports.createTestTableComplex2BinaryBlobs = function (test) {
  test.expect(3);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    tables;
  
  conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table2 + ";");
  conn.querySync("CREATE TABLE " + cfg.test_table2 +
    " (vb VARBINARY(16), b BINARY, t TEXT) " + cfg.store_engine + ";");
  res = conn.querySync("SHOW TABLES");
  tables = res.fetchAllSync();
  
  test.ok(res.fieldCount === 1, "SHOW TABLES result field count === 1");
  test.ok(tables.some(function (r) {
    return r['Tables_in_' + cfg.database] === cfg.test_table2;
  }), "Find the test_table2 in results");
  
  res = conn.querySync("INSERT INTO " + cfg.test_table2 +
                   " (vb, b, t) VALUES ('qwerty', 'qwerty', 'qwerty');");
  res = conn.querySync("INSERT INTO " + cfg.test_table2 +
                   " (vb, b, t) VALUES ('qwe', 'qwe', 'qwe');") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table2 +
                   " (vb, b, t) VALUES ('qwe\\0', 'qwe\\0', 'qwe\\0');") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table2 +
                   " (vb, b, t) VALUES ('qwe\\0\\0', 'qwe\\0\\0', 'qwe\\0\\0');") && res;
  test.ok(res, "conn.querySync('INSERT INTO test_table2 ...')");
  
  conn.closeSync();
  test.done();
};

