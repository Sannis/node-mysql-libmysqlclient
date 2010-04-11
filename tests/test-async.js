/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Load configuration
var cfg = require("./config").cfg;

// Require modules
var
  sys = require("sys"),
  mysql_libmysqlclient = require("../mysql-libmysqlclient");

exports.async = function (test) {
  var conn = mysql_libmysqlclient.createConnection(cfg.host, cfg.user, cfg.password, cfg.database);
  
  conn.async(function () {
    conn.close();
    test.done();
  });
};

exports.queryAsync = function (test) {
  var conn = mysql_libmysqlclient.createConnection(cfg.host, cfg.user, cfg.password, cfg.database);
  
  conn.queryAsync("SHOW TABLES;", function (result) {
    conn.close();
    test.done();
  });
};

