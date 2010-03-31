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
  mysql_sync = require("../mysql-sync");

var
  conn = mysql_sync.createConnection(host, user, password, database);

sys.puts("mysql_sync.createConnection(host, user, password, database)");
sys.puts(sys.inspect(conn));

sys.puts("Before run conn.query();");

conn.query("SHOW TABLES;", function (result) {
  sys.puts("In conn.query();");
  sys.puts(sys.inspect(result));
  conn.close();
  sys.puts("End of conn.query();");
});

sys.puts("After run conn.query();");

