#!/usr/bin/env node
/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Load configuration
var cfg = require("./config").cfg;

// Require modules
var
  assert = require("assert"),
  sys = require("sys"),
  mysql_libmysqlclient = require("../mysql-libmysqlclient"),
  conn = mysql_libmysqlclient.createConnection(cfg.host, cfg.user, cfg.password, cfg.database),
  res,
  rows;

function writeTest() {
  var
    start_time,
    total_time,
    i;
  
  start_time = new Date();
  
  for (i = 0; i < cfg.insert_rows_count_speedtest; i += 1) {
    res = conn.query("INSERT INTO " + cfg.test_table + " VALUES (1, 'hello', 3.141);");
  }
  
  total_time = ((new Date()) - start_time) / 1000;
  
  sys.puts("**** " + cfg.insert_rows_count_speedtest + " insertions in " + total_time + "s (" + (cfg.insert_rows_count_speedtest / total_time) + "/s)");
}

function readTest() {
  var
    start_time,
    total_time;
  
  start_time = new Date();
  
  res = conn.query("SELECT * FROM " + cfg.test_table + ";", true);
  rows = res.fetchAll();
  assert.equal(rows.length, cfg.insert_rows_count_speedtest);
  assert.deepEqual(rows[0], {alpha: 1, beta: 'hello', pi: 3.141});
  
  total_time = ((new Date()) - start_time) / 1000;
  
  sys.puts("**** " + cfg.insert_rows_count_speedtest + " rows in " + total_time + "s (" + (cfg.insert_rows_count_speedtest / total_time) + "/s)");
}

/*
> mysql -u root
CREATE DATABASE test  DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;
GRANT ALL ON test.* TO test@localhost IDENTIFIED BY "";

See current connection options in ./settings.js
*/

if (!conn.connected()) {
  sys.puts("Connection error: " + conn.connectErrno() + ", " + conn.connectError());
}

sys.puts("mysql_libmysqlclient.createConnection() result: " + sys.inspect(conn));

res = conn.query("DROP TABLE IF EXISTS " + cfg.test_table + ";");
sys.puts("'DROP TABLE IF EXISTS' result: " + sys.inspect(res));

res = conn.query("SET max_heap_table_size=128M;");
sys.puts("'SET max_heap_table_size=128M' result: " + sys.inspect(res));

res = conn.query("CREATE TABLE " + cfg.test_table + " (alpha INTEGER, beta VARCHAR(255), pi FLOAT) TYPE=MEMORY;");
sys.puts("'CREATE TABLE' result: " + sys.inspect(res));

writeTest();
readTest();

