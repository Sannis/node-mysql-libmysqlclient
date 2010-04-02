/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Mixin settings
/*global host, user, password, database, test_table, insert_rows_count */
process.mixin(require("./settings"));

// Require modules
var
  sys = require("sys"),
  mysql_libmysqlclient = require("../mysql-libmysqlclient");

exports.async = function (test) {
  var conn = mysql_libmysqlclient.createConnection(host, user, password, database);
  
  conn.async(function () {
    conn.close();
    test.done();
  });
};

exports.queryAsync = function (test) {
  var conn = mysql_libmysqlclient.createConnection(host, user, password, database);
  
  conn.queryAsync("SHOW TABLES;", function (result) {
    conn.close();
    test.done();
  });
};

