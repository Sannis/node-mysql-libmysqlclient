#!/usr/bin/env node
/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Mixin settings
process.mixin(require("./settings"));

// Require modules
var sys = require("sys"),
  mysql_sync = require("../mysql-sync");

exports.escape = function(test){
  var conn = mysql_sync.createConnection(host, user, password, database),
    strings_to_escape = [
    ["test string", "test string"],
    ["test\\string", "test\\\\string"],
    ["test\nstring", "test\\nstring"],
    ["test\rstring", "test\\rstring"],
    ["test\"string", "test\\\"string"],
    ["test\'string", "test\\'string"],
    ["test \x00", "test \\0"]
  ],
  str,
  str_esc_theor,
  str_esc_real,
  i;
  
  test.expect(strings_to_escape.length + 1);
  
  test.ok(conn, "mysql_sync.createConnection(host, user, password, database)");
  
  for (i in strings_to_escape) {
    if (typeof i === 'string') {
      str = strings_to_escape[i][0];
      str_esc_theor = strings_to_escape[i][1];
      str_esc_real = conn.escape(strings_to_escape[i][0]);

      test.equals(str_esc_real, str_esc_theor, "conn.escape()");
    }
  }
  
  conn.close();
  
  test.done();
};

/*
unittest.test('conn.query("SHOW TABLES;")', function () {
  var conn = mysql_sync.createConnection(host, user, password, database),
    res;
  
  res = conn.query("SHOW TABLES;");
  unittest.assert(res);
  
  conn.close();
});

unittest.test('conn.query("SHOW TABLES;").fetchResult()', function () {
  var conn = mysql_sync.createConnection(host, user, password, database),
    res,
    tables;
  
  res = conn.query("SHOW TABLES;");
  tables = res.fetchResult();
  
  unittest.assert(tables);
  
  conn.close();
});

unittest.test('Drop table ' + test_table, function () {
  var conn = mysql_sync.createConnection(host, user, password, database),
    res;
  
  res = conn.query("DROP TABLE IF EXISTS " + test_table + ";");

  unittest.assertEqual(res, true);
  
  conn.close();
});

unittest.test('Create table ' + test_table, function () {
  var conn = mysql_sync.createConnection(host, user, password, database),
    res;
  
  res = conn.query("CREATE TABLE " + test_table +
    " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
    " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
    " PRIMARY KEY (autoincrement_id));");

  unittest.assertEqual(res, true);
  
  conn.close();
});

unittest.test('Insert ' + insert_rows_count + ' rows into table ' + test_table, function () {
  var conn = mysql_sync.createConnection(host, user, password, database),
    res = true,
    random_number,
    random_boolean,
    i;
  
  for (i = 0; i < insert_rows_count; i += 1)
  {
    random_number = Math.round(Math.random() * 1000000);
    random_boolean = (Math.random() > 0.5) ? 1 : 0;
    res = conn.query("INSERT INTO " + test_table +
      " (random_number, random_boolean) VALUES ('" + random_number +
      "', '" + random_boolean + "');") && res;
  }

  unittest.assertEqual(res, true);
  
  conn.close();
});

unittest.test('conn.lastInsertId()', function () {
  var conn = mysql_sync.createConnection(host, user, password, database),
    res = true,
    random_number,
    random_boolean,
    last_insert_id,
    i;
  
  for (i = 0; i < insert_rows_count; i += 1)
  {
    random_number = Math.round(Math.random() * 1000000);
    random_boolean = (Math.random() > 0.5) ? 1 : 0;
    res = conn.query("INSERT INTO " + test_table +
      " (random_number, random_boolean) VALUES ('" + random_number +
      "', '" + random_boolean + "');") && res;
  }

  last_insert_id = conn.lastInsertId();
  
  unittest.assertEqual(last_insert_id, 2 * insert_rows_count);
  
  conn.close();
});
*/

