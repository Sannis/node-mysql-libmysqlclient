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

// Create DB connection
var conn = mysql_libmysqlclient.createConnectionSync();

process.on('exit', function () {
  sys.debug("onExit");
});

sys.debug("Start");

conn.connectSync(cfg.host, cfg.user, cfg.password);
sys.debug("After first connectSync()");
conn.closeSync();
sys.debug("After first closeSync()");

conn.connectSync(cfg.host, cfg.user, cfg.password);
sys.debug("After second connectSync()");
conn.closeSync();
sys.debug("After second closeSync()");

conn.connect(cfg.host, cfg.user, cfg.password, function (err) {
  if (err) {
    throw err;
  }
  
  sys.debug("In first connect()");
  conn.closeSync();
  sys.debug("After first connect().closeSync()");
  
  conn.connect(cfg.host, cfg.user, cfg.password, function (err) {
    if (err) {
      throw err;
    }
    
    sys.debug("In second connect()");
    conn.closeSync();
    sys.debug("After second connect().closeSync()");
    
    conn.connect(cfg.host, cfg.user, cfg.password, function (err) {
      if (err) {
        throw err;
      }
      
      sys.debug("In third connect()");
      conn.closeSync();
      sys.debug("After third connect().closeSync()");
    });
  });
});

sys.debug("Finish");

