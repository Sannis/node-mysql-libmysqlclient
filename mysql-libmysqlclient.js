/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

var binding = require("./mysql_bindings");

exports.createConnection = function () {
  var db = new binding.MysqlConn();
  db.connect.apply(db, Array.prototype.slice.call(arguments, 0, 6));
  return db;
};
