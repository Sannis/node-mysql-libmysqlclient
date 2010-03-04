/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

var binding = require("./mysql_sync_bindings");
var MysqlDbSync = binding.MysqlDbSync;
var sys = require("sys");

function createConnection (servername, user, password, database, port, socket)
{
  var db = new MysqlDbSync;
  db.connect(servername, user, password, database, port, socket);
  
  return db;
}

exports.createConnection = createConnection;

