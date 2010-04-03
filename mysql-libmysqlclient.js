/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

var
  binding = require("./mysql_bindings"),
  sys = require("sys");

function createConnection(servername, user, password, dbname, port, socket)
{
  var db = new binding.MysqlConn();
  
  if (typeof socket !== "undefined") {
    db.connect(servername, user, password, dbname, port, socket);
  } else {
    if (typeof port !== "undefined") {
      db.connect(servername, user, password, dbname, port);
    } else {
      if (typeof dbname !== "undefined") {
        db.connect(servername, user, password, dbname);
      } else {
        if (typeof password !== "undefined") {
          db.connect(servername, user, password);
        } else {
          if (typeof user !== "undefined") {
            db.connect(servername, user);
          } else {
            if (typeof servername !== "undefined") {
              db.connect(servername);
            }
          }
        }
      }
    }
  }
  
  return db;
}

exports.createConnection = createConnection;

