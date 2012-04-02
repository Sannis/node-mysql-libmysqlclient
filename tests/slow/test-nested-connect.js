/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Load configuration
var cfg = require('../config');

exports.ConnectsNested = function (test) {
  test.expect(1);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    test_order = "";
  
  test_order += "s";
  
  conn.closeSync();
  conn.connect(cfg.host, cfg.user, cfg.password, function (err) {
    if (err) {
      throw err;
    }
    
    test_order += "1";
    
    conn.closeSync();
    conn.connect(cfg.host, cfg.user, cfg.password, function (err) {
      if (err) {
        throw err;
      }
      
      test_order += "2";
      
      conn.closeSync();
      conn.connect(cfg.host, cfg.user, cfg.password, function (err) {
        if (err) {
          throw err;
        }
        
        test_order += "3";
        
        (function () {
          test.equals(test_order, "sf123");
          
          conn.closeSync();
          test.done();
        }());
      });
    });
  });
  
  test_order += "f";
};

exports.ManyConnectsNested = function (test) {
  test.expect(0);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(),
    helper,
    i = 0;
  
  helper = function () {
    i += 1;
    if (i <= cfg.slow_connects_nested) {
      conn.connect(cfg.host, cfg.user, cfg.password, cfg.database, function (err) {
        if (err) {
          throw err;
        }
        
        conn.closeSync();
        
        helper();
      });
    } else {
      test.done();
    }
  };
  
  helper();
};

exports.ManyConnectsInLoop = function (test) {
  test.expect(0);
  
  var
    connections = [],
    test_result,
    i = 0, ci = 0;
  
  for (i = 0; i < cfg.slow_connects_inloop; i += 1) {
    connections[i] = cfg.mysql_libmysqlclient.createConnectionSync();
  }
  
  for (i = 0; i < cfg.slow_connects_inloop; i += 1) {
    connections[i].connect(cfg.host, cfg.user, cfg.password, cfg.database, function (err) {
      if (err) {
        console.log("Error:" + err);
        throw err;
      }
      
      ci += 1;
      
      if (ci === cfg.slow_connects_inloop) {
        for (i = 0; i < cfg.slow_connects_inloop; i += 1) {
          connections[i].closeSync();
        }
        
        test.done();
      }
    });
  }
};

