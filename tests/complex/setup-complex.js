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

exports.createTestTableComplex = function (test) {
  test.expect(3);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    tables;

  conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  conn.querySync("CREATE TABLE " + cfg.test_table +
    " (size ENUM('small', 'medium', 'large')," +
    " colors SET('red', 'green', 'blue')) TYPE=MEMORY;");
  res = conn.querySync("SHOW TABLES");
  tables = res.fetchAllSync();
  
  test.ok(res.fieldCount === 1, "SHOW TABLES result field count === 1");
  test.ok(tables.some(function (r) {
    return r['Tables_in_' + cfg.database] === cfg.test_table;
  }), "Find the test table in result");
  
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

