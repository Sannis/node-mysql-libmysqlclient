#!/usr/bin/env node
/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Mixin settings
/*global host, user, password, database, database_denied, test_table, insert_rows_count */
process.mixin(require("./settings"));

// Require modules
var
  sys = require("sys"),
  mysql_sync = require("../mysql-libmysqlclient");

var
  conn = mysql_sync.createConnection(host, user, password, database);

sys.puts("mysql_sync.createConnection(host, user, password, database)");
sys.puts(sys.inspect(conn));

sys.puts("Before run conn.async()");

conn.async(function () {
  sys.puts("In conn.async() callback");
  sys.puts("End of conn.async() callback");
});

sys.puts("After run conn.async()");

sys.puts("Before run conn.queryAsync();");

conn.queryAsync("SHOW TABLES;", function (result) {
  sys.puts("In conn.queryAsync();");
  sys.puts("arguments: " + sys.inspect(arguments));
  sys.puts("result: " + sys.inspect(result));
  sys.puts("result.fetchResult(): " + sys.inspect(result.fetchResult()));
  conn.close();
  sys.puts("End of conn.queryAsync();");
});

sys.puts("After run conn.queryAsync();");

