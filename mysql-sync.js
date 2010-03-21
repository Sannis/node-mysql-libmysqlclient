/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

var binding = require("./mysql_sync_bindings");
var MysqlSyncConn = binding.MysqlSyncConn;
var sys = require("sys");

function createConnection(servername, user, password, dbname, port, socket)
{
  var db = new MysqlSyncConn();
  db.connect(servername, user, password, dbname, port, socket);
  
  return db;
}

exports.createConnection = createConnection;

