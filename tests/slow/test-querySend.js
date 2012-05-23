/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Load configuration
var cfg = require('../config');

exports.Setup = function (test) {
  test.expect(0);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database);
  
  conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  conn.querySync("CREATE TABLE " + cfg.test_table +
    " (number INT(8) NOT NULL) ENGINE=MEMORY;");
  
  conn.closeSync();
  
  test.done();
};

exports.QuerySendNested = function (test) {
  test.expect(2);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    test_result,
    test_order = "";
  
  test_order += "s";
  
  conn.querySend("INSERT INTO " + cfg.test_table + " (number) VALUES ('1');", function (err, res) {
    if (err) {
      throw err;
    }
    
    test_order += "1";
    
    conn.querySend("INSERT INTO " + cfg.test_table + " (number) VALUES ('2');", function (err, res) {
      if (err) {
        throw err;
      }
      
      test_order += "2";
      
      conn.querySend("INSERT INTO " + cfg.test_table + " (number) VALUES ('3');", function (err, res) {
        if (err) {
          throw err;
        }
        
        test_order += "3";
        
        (function () {
          test.equals(test_order, "sf123");
          
          test_result = conn.querySync("SELECT number FROM " + cfg.test_table + ";").fetchAllSync();
          test.same(test_result, [{number: 1}, {number: 2}, {number: 3}]);
          
          conn.closeSync();
          test.done();
        }());
      });
    });
  });
  
  test_order += "f";
};

exports.QuerySendNestedDeeper = function (test) {
  test.expect(1);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    helper,
    test_result,
    i = 0;
  
  helper = function () {
    i += 1;
    if (i <= cfg.slow_inserts_count) {
      conn.querySend("INSERT INTO " + cfg.test_table + " (number) VALUES ('" + i + "');", function (err, res) {
        if (err) {
          throw err;
        }
        
        helper();
      });
    } else {
      test_result = conn.querySync("SELECT COUNT(number) AS c FROM " + cfg.test_table + ";").fetchAllSync()[0].c;
      test.equals(test_result, cfg.slow_inserts_count + 3);
      
      conn.closeSync();
      test.done();
    }
  };
  
  helper();
};

exports.QuerySendParallelShouldFail = function (test) {
  test.expect(0);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database);

  conn.querySend("SELECT SLEEP(2);", function (err, res) {
    if (err) {
      throw err;
    }
  });

  conn.querySend("SELECT SLEEP(1);", function (err, res) {
    if (err) {
      test.done();
    }
  });
};
