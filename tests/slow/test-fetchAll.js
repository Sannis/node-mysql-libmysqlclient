/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Load configuration
var cfg = require('../config');

exports.FetchesNested = function (test) {
  test.expect(5);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    t1, t2, t3,
    test_order = "";
  
  test_order += "s";
  
  conn.querySync("SHOW TABLES;").fetchAll(function (err, tables) {
    if (err) {
      throw err;
    }
    
    t1 = tables;
    
    test_order += "1";
    
    conn.querySync("SHOW TABLES;").fetchAll(function (err, tables) {
      if (err) {
        throw err;
      }
      
      t2 = tables;
      
      test_order += "2";
      
      conn.querySync("SHOW TABLES;").fetchAll(function (err, tables) {
        if (err) {
          throw err;
        }
        
        t3 = tables;
        
        test_order += "3";
        
        (function () {
          test.equals(test_order, "sf123");
          
          tables = conn.querySync("SHOW TABLES;").fetchAllSync();
          test.ok(tables.some(function (r) {
            return r['Tables_in_' + cfg.database] === cfg.test_table;
          }), "Find the test table in result");
          test.same(t1, tables);
          test.same(t2, tables);
          test.same(t3, tables);
          
          conn.closeSync();
          
          test.done();
        }());
      });
    });
  });
  
  test_order += "f";
};

exports.ManyFetchesNested = function (test) {
  test.expect(1);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    helper,
    initial_result,
    test_result,
    i = 0;
  
  initial_result = conn.querySync("SHOW TABLES;").fetchAllSync();
  
  helper = function () {
    i += 1;
    if (i <= cfg.slow_fetches_nested) {
      conn.querySync("SHOW TABLES;").fetchAll(function (err, tables) {
        if (err) {
          throw err;
        }
        
        helper();
      });
    } else {
      test_result = conn.querySync("SHOW TABLES;").fetchAllSync();
      test.same(test_result, initial_result);
      
      if (cfg.slow_fetches_nested <= 0) {
        console.log("ManyFetchesNested test ignored, cfg.slow_fetches_nested <=0");
      }
      
      conn.closeSync();
      test.done();
    }
  };
  
  helper();
};

exports.ManyFetchesInLoop = function (test) {
  test.expect(1);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    initial_result,
    test_result,
    i = 0, ci = 0;
  
  if (cfg.slow_fetches_inloop <= 0) {
    console.log("ManyFetchesInLoop test ignored, cfg.slow_fetches_inloop <=0");
    conn.closeSync();
    test.done();
    return;
  }
  
  initial_result = conn.querySync("SHOW TABLES;").fetchAllSync();
  
  for (i = 0; i < cfg.slow_fetches_inloop; i += 1) {
    conn.querySync("SHOW TABLES;").fetchAll(function (err, tables) {
      if (err) {
        throw err;
      }
      
      ci += 1;
      
      if (ci === cfg.slow_fetches_inloop) {
        test_result = conn.querySync("SHOW TABLES;").fetchAllSync();
        test.same(test_result, initial_result);
        
        conn.closeSync();
        test.done();
      }
    });
  }
};

