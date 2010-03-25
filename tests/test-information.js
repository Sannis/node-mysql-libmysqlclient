#!/usr/bin/env node
/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Mixin settings
/*global host, user, password, database, database_denied */
process.mixin(require("./settings"));

// Require modules
var sys = require("sys"),
  mysql_sync = require("../mysql-sync");

exports.sqlState_AllowedDb = function (test) {
  test.expect(2);
  
  var conn = mysql_sync.createConnection(host, user, password, database);
  test.ok(conn, "mysql_sync.createConnection(host, user, password, database)");
  test.ok(conn.sqlState(), "00000", "conn.sqlState() after connection to allowed database");
  conn.close();
  
  test.done();
};

exports.sqlState_DeniedDb = function (test) {
  test.expect(2);
  
  var conn = mysql_sync.createConnection(host, user, password, database_denied);
  test.ok(conn, "mysql_sync.createConnection(host, user, password, database)");
  test.ok(conn.sqlState(), "42000", "conn.sqlState() after connection to denied database");
  
  test.done();
};

/*
unittest.test('conn.getInfo()', function () {
  var conn = mysql_sync.createConnection(host, user, password, database),
    info = conn.getInfo();
  
  unittest.assert(info);
  
  conn.close();
});

unittest.test('conn.getInfoString()', function () {
  var conn = mysql_sync.createConnection(host, user, password, database),
    res;
  
  res = conn.query("DROP TABLE IF EXISTS " + test_table + ";");

  unittest.assertEqual(res, true);
  
  res = conn.query("CREATE TABLE " + test_table +
    " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
    " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
    " PRIMARY KEY (autoincrement_id));");

  unittest.assertEqual(res, true);
  
  res = conn.query("ALTER TABLE " + test_table + " ADD INDEX (random_number)");

  unittest.assertEqual(res, true);
  
  unittest.assertEqual("Records: 0  Duplicates: 0  Warnings: 0", conn.getInfoString());
  
  conn.close();
});

unittest.test('conn.getWarnings()', function () {
  var conn = mysql_sync.createConnection(host, user, password, database),
    res;
  
  res = conn.query("DROP TABLE IF EXISTS " + test_table + ";");

  unittest.assertInspectEqual([], conn.getWarnings());
  
  res = conn.query("DROP TABLE IF EXISTS " + test_table + ";");

  unittest.assertInspectEqual([{errno: 1051,
                                reason: "Unknown table '" + test_table + "'"
                               }], conn.getWarnings());
  
  conn.close();
});
*/

