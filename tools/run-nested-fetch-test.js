#!/usr/bin/env node_g
/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Load configuration
var cfg = require("./config").cfg;

// Require modules
var
  sys = require("sys"),
  mysql_libmysqlclient = require("../mysql-libmysqlclient");


var
  conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
  res;

process.on('exit', function () {
  sys.debug("onExit");
  conn.closeSync();
});

sys.debug("Start");

sys.debug("In global scope");
res = conn.querySync("SHOW TABLES;");
sys.debug("After first querySync()");

res.fetchAll(function (err, tables) {
  if (err) {
    throw err;
  }
  
  sys.debug("In first fetchAll()");
  res = conn.querySync("SHOW TABLES;");
  sys.debug("After second querySync()");
  
  res.fetchAll(function (err, tables) {
    if (err) {
      throw err;
    }
    
    sys.debug("In second fetchAll()");
    res = conn.querySync("SHOW TABLES;");
    sys.debug("After second querySync()");
    
    res.fetchAll(function (err, tables) {
      if (err) {
        throw err;
      }
      sys.debug("In third fetchAll()");
    });
  });
});

sys.debug("Finish");

