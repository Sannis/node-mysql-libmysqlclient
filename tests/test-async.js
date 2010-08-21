/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Load configuration
var cfg = require("./config").cfg;

// Require modules
var
  sys = require("sys"),
  mysql_libmysqlclient = require("../mysql-libmysqlclient"),
  mysql_bindings = require("../mysql_bindings");

exports.connect = function (test) {
  test.expect(1);
  
  var conn = new mysql_bindings.MysqlConn();
  conn.connect(cfg.host, cfg.user, cfg.password, cfg.database, function (error) {
    test.ok(error === null, "conn.connect() for allowed database");
    
    conn.closeSync();
  
    test.done();
  });
};

exports.connectWithError = function (test) {
  test.expect(1);
  
  var conn = new mysql_bindings.MysqlConn();
  conn.connect(cfg.host, cfg.user, cfg.password, cfg.database_denied, function (error) {
    test.ok(error === 1044, "conn.connect() for denied database");
    
    conn.closeSync();
  
    test.done();
  });
};

exports.query = function (test) {
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database), res;

  res = conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  res = conn.querySync("CREATE TABLE " + cfg.test_table +
    " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
    " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
    " PRIMARY KEY (autoincrement_id)) TYPE=MEMORY;");

  test.expect(2);
  conn.query("SHOW TABLES", function (err, result) {
    test.ok(result.fieldCountSync() === 1, "show results field count === 1");
    var res = result.fetchAllSync();
    test.ok(res.some(function (r) {
      return r['Tables_in_' + cfg.database] === cfg.test_table;
    }), "find the test table in results");
    conn.closeSync();
    test.done();
  });
};

exports.queryWithError = function (test) {
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database), res;

  test.expect(2);
  conn.query("SHOW TABLESaagh", function (err, result) {
    test.ok(!result, "result is not defined");
    test.ok(err, "error object is present");
    conn.closeSync();
    test.done();
  });
};


