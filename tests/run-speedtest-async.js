#!/usr/bin/env node
/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Mixin settings
/*global host, user, password, database, test_table, insert_rows_count_speedtest */
process.mixin(require("./settings"));

// Require modules
var
  sys = require("sys"),
  puts = sys.puts,
  inspect = sys.inspect,
  mysql_libmysqlclient = require("../mysql-libmysqlclient"),
  conn = mysql_libmysqlclient.createConnection(host, user, password, database),
  res,
  t0;


function readTest(db) {
  var t0 = new Date();
  conn.queryAsync("SELECT * FROM " + test_table + ";", function (result) {
    var
      rows = result.fetchResult(),
      d = ((new Date()) - t0) / 1000;
      
    puts("**** " + rows.length + " rows in " + d + "s (" + (rows.length / d) + "/s)");
      
    conn.close();
  });
}

function writeTest(db, i, callback) {
  conn.queryAsync("INSERT INTO " + test_table + " VALUES (1);", function (result) {
    i -= 1;
    if (!i) {
      // end of results
      var dt = ((new Date()) - t0) / 1000;
      puts("**** " + insert_rows_count_speedtest + " insertions in " + dt + "s (" + (insert_rows_count_speedtest / dt) + "/s)");

      if (callback) {
        callback(db);
      }
    }
    else {
      writeTest(db, i, callback);
    }
  });
}

/*
> mysql -u root
CREATE DATABASE test  DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;
GRANT ALL ON test.* TO test@localhost IDENTIFIED BY "";

See current connection options in ./settings.js
*/


sys.puts("mysql_libmysqlclient.createConnection() result: " + sys.inspect(conn));

res = conn.query("DROP TABLE IF EXISTS " + test_table + ";");
puts("'DROP TABLE IF EXISTS' result: " + inspect(res));

res = conn.query("SET max_heap_table_size=128M;");
puts("'SET max_heap_table_size=128M' result: " + inspect(res));

conn.queryAsync("CREATE TABLE " + test_table + " (alpha INTEGER) TYPE=MEMORY;", function () {
  puts("In 'CREATE TABLE' callback: " + inspect(arguments));
  t0 = new Date();
  writeTest(conn, insert_rows_count_speedtest, readTest);
});

