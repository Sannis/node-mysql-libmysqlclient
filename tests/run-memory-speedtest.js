#!/usr/bin/env node
/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Mixin settings
/*global host, user, password, database, test_table, insert_rows_count */
process.mixin(require("./settings"));

// Require modules
var
  assert = require("assert"),
  sys = require("sys"),
  mysql_sync = require("../mysql-sync"),
  conn = mysql_sync.createConnection(host, user, password, database),
  res,
  rows;

function writeTest() {
  var
    start_time,
    total_time,
    i;
  
  start_time = new Date();
  
  for (i = 0; i < insert_rows_count; i += 1) {
    res = conn.query("INSERT INTO " + test_table + " VALUES (1, 'hello', 3.141);");
  }
  
  total_time = ((new Date()) - start_time) / 1000;
  
  sys.puts("**** " + insert_rows_count + " insertions in " + total_time + "s (" + (insert_rows_count / total_time) + "/s)");
}

function readTest() {
  var
    start_time,
    total_time;
  
  start_time = new Date();
  
  res = conn.query("SELECT * FROM " + test_table + ";", true);
  rows = res.fetchResult();
  assert.equal(rows.length, insert_rows_count);
  assert.deepEqual(rows[0], {alpha: 1, beta: 'hello', pi: 3.141});
  
  total_time = ((new Date()) - start_time) / 1000;
  
  sys.puts("**** " + insert_rows_count + " rows in " + total_time + "s (" + (insert_rows_count / total_time) + "/s)");
}
 


sys.puts("mysql_sync.createConnection() result: " + sys.inspect(conn));

res = conn.query("DROP TABLE IF EXISTS " + test_table + ";");
sys.puts("'DROP TABLE IF EXISTS' result: " + sys.inspect(res));

res = conn.query("SET max_heap_table_size=128M;");
sys.puts("'SET max_heap_table_size=128M' result: " + sys.inspect(res));

res = conn.query("CREATE TABLE " + test_table + " (alpha INTEGER, beta VARCHAR(255), pi FLOAT) TYPE=MEMORY;");
sys.puts("'CREATE TABLE' result: " + sys.inspect(res));

writeTest();
readTest();

