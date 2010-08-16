/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Load configuration
var cfg = require("./config").cfg;

// Require modules
var
  sys = require("sys"),
  mysql_libmysqlclient = require("../mysql-libmysqlclient");

exports.async = function (test) {
  var conn = mysql_libmysqlclient.createConnection(cfg.host, cfg.user, cfg.password, cfg.database);
  
  conn.async(function () {
    conn.close();
    test.done();
  });
};

exports.queryAsync = function (test) {
  var conn = mysql_libmysqlclient.createConnection(cfg.host, cfg.user, cfg.password, cfg.database), res;

  res = conn.query("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  res = conn.query("CREATE TABLE " + cfg.test_table +
    " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
    " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
    " PRIMARY KEY (autoincrement_id)) TYPE=MEMORY;");

  test.expect(2);
  conn.queryAsync("SHOW TABLES", function (result) {
    test.ok(result.fieldCount() === 1, "show results field count === 1");      
    var res = result.fetchAll(); 
    test.ok(res.some(function (r) {
      return r['Tables_in_' + cfg.database] === cfg.test_table;
    }), "find the test table in results");      
    conn.close();
    test.done();
  });
};

