/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Mixin settings
/*global host, user, password, database, database_denied, test_table */
process.mixin(require("./settings"));

// Require modules
var sys = require("sys"),
  mysql_sync = require("../mysql-sync");

exports.sqlState_AllowedDb = function (test) {
  test.expect(2);
  
  var conn = mysql_sync.createConnection(host, user, password, database);
  test.ok(conn, "mysql_sync.createConnection(host, user, password, database)");
  test.equals(conn.sqlState(), "00000", "conn.sqlState() after connection to allowed database");
  conn.close();
  
  test.done();
};

exports.sqlState_DeniedDb = function (test) {
  test.expect(2);
  
  var conn = mysql_sync.createConnection(host, user, password, database_denied);
  test.ok(conn, "mysql_sync.createConnection(host, user, password, database)");
  test.equals(conn.sqlState(), "42000", "conn.sqlState() after connection to denied database");
  
  test.done();
};

exports.getInfo = function (test) {
  test.expect(2);
  
  var conn = mysql_sync.createConnection(host, user, password, database);
  test.ok(conn, "mysql_sync.createConnection(host, user, password, database)");
  test.ok(conn.getInfo(), "conn.getInfo() after connection to allowed database");
  
  test.done();
};

exports.getInfoString = function (test) {
  test.expect(4);
  
  var conn = mysql_sync.createConnection(host, user, password, database), res;
  res = conn.query("DROP TABLE IF EXISTS " + test_table + ";");
  test.equals(res, true);
  res = conn.query("CREATE TABLE " + test_table +
    " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
    " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
    " PRIMARY KEY (autoincrement_id)) TYPE=MEMORY;");
  test.equals(res, true);
  res = conn.query("ALTER TABLE " + test_table + " ADD INDEX (random_number)");
  test.equals(res, true);
  test.equals(conn.getInfoString(), "Records: 0  Duplicates: 0  Warnings: 0",
                                    "conn.getInfoString() after ALTER TABLE");
  conn.close();

  test.done();
};

exports.getWarnings = function (test) {
  test.expect(2);
  
  var conn = mysql_sync.createConnection(host, user, password, database), res;  
  res = conn.query("DROP TABLE IF EXISTS " + test_table + ";");
  test.same(conn.getWarnings(), [],
            "conn.getWarnings() after DROP TABLE IF EXISTS");
  res = conn.query("DROP TABLE IF EXISTS " + test_table + ";");
  test.same(conn.getWarnings(),
            [{errno: 1051, reason: "Unknown table '" + test_table + "'" }],
            "conn.getWarnings() after double DROP TABLE IF EXISTS");
  conn.close();
  
  test.done();
};

