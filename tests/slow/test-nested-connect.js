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

exports.NestedConnect = function (test) {
  test.expect(2);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    test_string = "";
  
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  test_string += "s";
  conn.connect(cfg.host, cfg.user, cfg.password, function (err) {
    if (err) {
      throw err;
    }
    
    test_string += "1";
    conn.closeSync();
    
    conn.connect(cfg.host, cfg.user, cfg.password, function (err) {
      if (err) {
        throw err;
      }
      
      test_string += "2";
      conn.closeSync();
      
      conn.connect(cfg.host, cfg.user, cfg.password, function (err) {
        if (err) {
          throw err;
        }
        
        test_string += "3";
        conn.closeSync();
        
        test.equals(test_string, "sf123");
        test.done();
      });
    });
  });
  
  test_string += "f";
};

