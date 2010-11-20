/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Load configuration
var cfg = require("../config").cfg;

// Require modules
var
  mysql_libmysqlclient = require("../../mysql-libmysqlclient"),
  mysql_bindings = require("../../mysql_bindings");

exports.Connect = function (test) {
  test.expect(1);
  
  var conn = new mysql_bindings.MysqlConnection();
  
  conn.connect(cfg.host, cfg.user, cfg.password, cfg.database, function (error) {
    test.ok(error === null, "conn.connect() for allowed database");
    conn.closeSync();
    
    test.done();
  });
};

exports.ConnectWithError = function (test) {
  test.expect(3);
  
  var conn = new mysql_bindings.MysqlConnection();
  
  conn.connect(cfg.host, cfg.user, cfg.password, cfg.database_denied, function (error) {
    test.ok(error === 1044, "conn.connect() to denied database");
    
    test.equals(conn.connectErrno, 1044, "conn.connectErrno");
    test.ok(conn.connectError.match(new RegExp("Access denied for user '(" + cfg.user + "|)'@'.*' to database '" + cfg.database_denied + "'")), "conn.connectError");
    
    test.done();
  });
};

exports.Query = function (test) {
  test.expect(3);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res;
  
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  conn.query("SHOW TABLES", function (err, result) {
    test.ok(result.fieldCount === 1, "show results field count === 1");
    var res = result.fetchAllSync();
    test.ok(res.some(function (r) {
      return r['Tables_in_' + cfg.database] === cfg.test_table;
    }), "find the test table in results");
    conn.closeSync();
    test.done();
  });
};

exports.QueryWithError = function (test) {
  test.expect(3);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  conn.query("SHOW TABLESaagh", function (err, result) {
    test.ok(err, "Error object is present");
    test.ok(!result, "Result is not defined");
    conn.closeSync();
    test.done();
  });
};

exports.QueryWithoutCallback = function (test) {
  test.expect(6);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
  test.ok(res, "conn.querySync('DELETE FROM cfg.test_table')");
  
  test.doesNotThrow(function () {
    conn.query("INSERT INTO " + cfg.test_table + " (random_number, random_boolean) VALUES ('1', '0');");
  });
  
  conn.query("SELECT * FROM " + cfg.test_table + ";", function (err, result) {
    test.ok(result, "Result is defined");
    test.ok(!err, "Error object is not present");
    
    var res = result.fetchAllSync();
    test.same(res, [{random_number: 1, random_boolean: 0}],
              "Right result, one row, [{random_number: 1, random_boolean: 0}]]");
    conn.closeSync();
    test.done();
  });
};

