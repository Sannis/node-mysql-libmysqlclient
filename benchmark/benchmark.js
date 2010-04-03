#!/usr/bin/env node
/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

var host = "localhost",
  user = "test",
  password = "",
  database = "test",
  test_table = "test_table",

  // Operations count for continuous benchmarks
  reconnects_count = 100000,
  escapes_count = 10000000,
  inserts_count = 200000,
  selects_count = 100,
  rows_to_select = 10,

  sys = require("sys"),
  mysql_libmysqlclient = require("../mysql-libmysqlclient"),
  
  start,
  finish,

  conn;


// Utility functions

function round2(number) {
  return Math.round(number * 100) / 100;
}

function doBenchmark(title, prepare_function, execute_function) {
  var start,
    finish,
    data;
    
  if (typeof prepare_function === "function") {
    data = prepare_function();
        
    start = new Date();
    execute_function(data);
    finish = new Date();
  } else {
    start = new Date();
    execute_function();
    finish = new Date();
  }
    
  sys.puts(title + ": " + round2((finish.valueOf() - start.valueOf()) / 1000) + "s");
}


// Initialize connection for benchmarks

sys.puts("Init...");

conn = mysql_libmysqlclient.createConnection(host, user, password, database);

start = new Date();


// Benchmark 1: connect and disconnect

doBenchmark("Connect and disconnect(" + reconnects_count + ")", 0, function () {
  var i;
  for (i = 0; i < reconnects_count; i += 1) {
    conn.close();
    conn.connect(host, user, password, database);
  }
});


// Benchmark 2: escape string

doBenchmark("Escape string(" + escapes_count + ")", 0, function () {
  // A bit utility code
  var string_to_escape = "test\\string\nwith\rmany\"symbols'and\x00nulls",
    escaped_string,
    i;

  for (i = 0; i < escapes_count; i += 1) {
    escaped_string = conn.escape(string_to_escape);
  }
});


// Benchmark 3: inserts

doBenchmark("Inserts(" + inserts_count + ")", function () {
  conn.query("DROP TABLE IF EXISTS " + test_table + ";");
  conn.query("CREATE TABLE " + test_table + " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT, random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL, PRIMARY KEY (autoincrement_id));");

  var rows = [],
    i;

  for (i = 0; i < inserts_count; i += 1) {
    rows[i] = {
      'random_number': Math.round(Math.random() * 1000000),
      'random_boolean': (Math.random() > 0.5) ? 1 : 0
    };
  }
    
  return rows;
}, function (rows) {
  var i;
  for (i = 0; i < inserts_count; i += 1) {
    conn.query("INSERT INTO " + test_table + " (random_number, random_boolean) VALUES ('" + rows[i].random_number + "', '" + rows[i].random_boolean + "');");
  }
});


// Benchmark 3: selects

doBenchmark("Selects(" + selects_count + ", " + rows_to_select + ")", function () {
  var limits = [],
    i;

  for (i = 0; i < selects_count; i += 1) {
    limits[i] = Math.round(Math.random() * (inserts_count - rows_to_select));
  }
    
  return limits;
}, function (limits) {
  var rows = new Array(selects_count),
    res,
    i;
    
  for (i = 0; i < selects_count; i += 1) {
    res = conn.query("SELECT * FROM " + test_table + " ORDER BY RAND() LIMIT " + limits[i] + ", " + rows_to_select + ";");
    rows[i] = res.fetchResult();
  }
});


// Finish benchmarks

finish = new Date();

sys.puts("Finish.");

conn.close();

sys.puts("Full time: " + round2((finish.valueOf() - start.valueOf()) / 1000) + "s");

