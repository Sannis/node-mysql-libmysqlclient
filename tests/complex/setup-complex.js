/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Load configuration
var cfg = require("../config").cfg;

// Require modules
var
  mysql_libmysqlclient = require("../../mysql-libmysqlclient");

exports.createTestTableComplex = function (test) {
  test.expect(6);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    tables;
  
  conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  conn.querySync("CREATE TABLE " + cfg.test_table +
    " (size ENUM('small', 'medium', 'large')," +
    "  colors SET('red', 'green', 'blue')) TYPE=MEMORY;");
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
  
  conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table2 + ";");
  conn.querySync("CREATE TABLE " + cfg.test_table2 +
    " (size ENUM('small', 'medium', 'large')," +
    "  colors VARCHAR(32)) TYPE=MEMORY;");
  res = conn.querySync("SHOW TABLES");
  tables = res.fetchAllSync();
  
  test.ok(res.fieldCount === 1, "SHOW TABLES result field count === 1");
  test.ok(tables.some(function (r) {
    return r['Tables_in_' + cfg.database] === cfg.test_table2;
  }), "Find the test_table2 in results");
  
  res = conn.querySync("INSERT INTO " + cfg.test_table2 +
                   " (size, colors) VALUES ('small', 'red');");
  res = conn.querySync("INSERT INTO " + cfg.test_table2 +
                   " (size, colors) VALUES ('small', 'orange');");
  res = conn.querySync("INSERT INTO " + cfg.test_table2 +
                    " (size, colors) VALUES ('medium', 'black');") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table2 +
                   " (size, colors) VALUES ('large', 'deep purple');") && res;
  test.ok(res, "conn.querySync('INSERT INTO test_table ...')");
  
  conn.closeSync();
  test.done();
};

