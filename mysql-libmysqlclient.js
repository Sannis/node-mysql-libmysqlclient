/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

var binding = require("./mysql_bindings");

exports.createConnection = function () {
  var db = new binding.MysqlConn();
  if (arguments.length > 0) {
    db.connect.apply(db, Array.prototype.slice.call(arguments, 0, 6));
  }
  return db;
};
