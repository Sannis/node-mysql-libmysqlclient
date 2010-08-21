/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

var binding = require("./mysql_bindings");

exports.createConnectionSync = function () {
  var db = new binding.MysqlConn();
  if (arguments.length > 0) {
    db.connectSync.apply(db, Array.prototype.slice.call(arguments, 0, 6));
  }
  return db;
};
