/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Load configuration
var cfg = require("../config").cfg;

// Require modules
var
  sys = require("sys"),
  mysql_libmysqlclient = require("../../mysql-libmysqlclient"),
  mysql_bindings = require("../../mysql_bindings");

exports.New = function (test) {
  test.expect(1);
  
  var stmt;
  
  try {
    stmt = new mysql_bindings.MysqlStatement();
  } catch (e) {
    test.equals(e.toString(), "TypeError: Argument 0 invalid");
  }
  
  test.done();
};

