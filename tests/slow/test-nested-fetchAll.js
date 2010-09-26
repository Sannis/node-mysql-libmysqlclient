/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Load configuration
var cfg = require("../config").cfg;

// Require modules
var
  sys = require("sys"),
  mysql_libmysqlclient = require("../../mysql-libmysqlclient");

exports.NestedFetchAll = function (test) {
  test.expect(5);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    test_string = "";
  
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("SHOW TABLES;");
  
  test_string += "s";
  res.fetchAll(function (err, tables) {
    if (err) {
      throw err;
    }
    
    test.ok(tables.some(function (r) {
      return r['Tables_in_' + cfg.database] === cfg.test_table;
    }), "Find the test table in result");
    
    test_string += "1";
    res = conn.querySync("SHOW TABLES;");
    
    res.fetchAll(function (err, tables) {
      if (err) {
        throw err;
      }
      
      test.ok(tables.some(function (r) {
        return r['Tables_in_' + cfg.database] === cfg.test_table;
      }), "Find the test table in result");
      
      test_string += "2";
      res = conn.querySync("SHOW TABLES;");
      
      res.fetchAll(function (err, tables) {
        if (err) {
          throw err;
        }
        
        test.ok(tables.some(function (r) {
          return r['Tables_in_' + cfg.database] === cfg.test_table;
        }), "Find the test table in result");
        
        test_string += "3";
        res = conn.querySync("SHOW TABLES;");
        
        conn.closeSync();
        test.equals(test_string, "sf123");
        test.done();
      });
    });
  });
  
  test_string += "f";
};

