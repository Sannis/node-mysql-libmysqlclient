/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Load configuration
var cfg = require("../config");

exports.createTestTableComplex1 = function (test) {
  test.expect(3);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    tables;
  
  conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  conn.querySync("CREATE TABLE " + cfg.test_table +
    " (size ENUM('small', 'medium', 'large') NULL," +
    "  colors SET('red', 'green', 'blue') NULL," + 
    "  num INT(8) NOT NULL DEFAULT 0) " + cfg.store_engine + ";");
  res = conn.querySync("SHOW TABLES");
  tables = res.fetchAllSync();
  
  test.ok(res.fieldCount === 1, "SHOW TABLES result field count === 1");
  test.ok(tables.some(function (r) {
    return r['Tables_in_' + cfg.database] === cfg.test_table;
  }), "Find the test_table in results");
  
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                   " (size, colors) VALUES ('small', 'red');");
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                   " (size, colors) VALUES ('medium', 'red,green,blue');") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                    " (size, colors) VALUES ('large', 'green');") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                   " (size, colors) VALUES ('large', 'red,blue');") && res;
  test.ok(res, "conn.querySync('INSERT INTO test_table ...')");
  
  conn.closeSync();
  test.done();
};

