/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Load configuration
var cfg = require("../config").cfg;

// Require modules
var
  mysql_libmysqlclient = require("../../mysql-libmysqlclient");

exports.createTestTableSimple = function (test) {
  test.expect(3);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    tables;
  
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, cfg.database)");
  conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  conn.querySync("CREATE TABLE " + cfg.test_table +
    " (random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL) TYPE=MEMORY;");
  res = conn.querySync("SHOW TABLES");
  test.ok(res.fieldCount === 1, "SHOW TABLES result field count === 1");
  tables = res.fetchAllSync();
  test.ok(tables.some(function (r) {
    return r['Tables_in_' + cfg.database] === cfg.test_table;
  }), "Find the test table in result");
  
  conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table_notexists + ";");
  
  conn.closeSync();
  
  test.done();
};

