/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Load configuration
var cfg = require('../config');

exports.fetchDateAndTimeValues = function (test) {
  test.expect(17);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    rows;
  
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  rows = conn.querySync("SELECT CAST('2 2:50' AS TIME) as time;").fetchAllSync();
  test.ok(rows[0].time instanceof Date, "SELECT CAST('2 2:50' AS TIME) is Date");
  test.equals(rows[0].time.toUTCString(), "Sat, 03 Jan 1970 02:50:00 GMT", "SELECT CAST('2 2:50' AS TIME) is correct");
  
  rows = conn.querySync("SELECT CAST('2000-01-01' AS DATE) as date;").fetchAllSync();
  test.ok(rows[0].date instanceof Date, "SELECT CAST('2000-01-01' AS DATE) is Date");
  test.equals(rows[0].date.toUTCString(), "Sat, 01 Jan 2000 00:00:00 GMT", "SELECT CAST('2000-01-01' AS DATE) is correct");
  
  rows = conn.querySync("SELECT CAST('1988-10-25 06:34' AS DATETIME) as datetime;").fetchAllSync();
  test.ok(rows[0].datetime instanceof Date, "SELECT CAST('1988-10-25 06:34' AS DATETIME) is Date");
  test.equals(rows[0].datetime.toUTCString(), "Tue, 25 Oct 1988 06:34:00 GMT", "SELECT CAST('1988-10-25 06:34' AS DATETIME) is correct");
  
  rows = conn.querySync("SELECT CAST('1988-10-25' AS DATETIME) as datetime;").fetchAllSync();
  test.ok(rows[0].datetime instanceof Date, "SELECT CAST('1988-10-25' AS DATETIME) is Date");
  test.equals(rows[0].datetime.toUTCString(), "Tue, 25 Oct 1988 00:00:00 GMT", "SELECT CAST('1988-10-25' AS DATETIME) is correct");
  
  rows = conn.querySync("SELECT CAST('1988-10-25 2:50' AS DATETIME) as datetime;").fetchAllSync();
  test.ok(rows[0].datetime instanceof Date, "SELECT CAST('1988-10-25 2:50' AS DATETIME) is Date");
  test.equals(rows[0].datetime.toUTCString(), "Tue, 25 Oct 1988 02:50:00 GMT", "SELECT CAST('1988-10-25 2:50' AS TIME) is correct");
  
  rows = conn.querySync("SELECT CAST('1988-10-25 3:50' AS DATETIME) as datetime;").fetchAllSync();
  test.ok(rows[0].datetime instanceof Date, "SELECT CAST('1988-10-25 3:50' AS DATETIME) is Date");
  test.equals(rows[0].datetime.toUTCString(), "Tue, 25 Oct 1988 03:50:00 GMT", "SELECT CAST('1988-10-25 3:50' AS TIME) is correct");
  
  rows = conn.querySync("SELECT CAST('1988-10-25 23:50' AS DATETIME) as datetime;").fetchAllSync();
  test.ok(rows[0].datetime instanceof Date, "SELECT CAST('1988-10-25 23:50' AS DATETIME) is Date");
  test.equals(rows[0].datetime.toUTCString(), "Tue, 25 Oct 1988 23:50:00 GMT", "SELECT CAST('1988-10-25 23:50' AS TIME) is correct");
  
  rows = conn.querySync("SELECT CAST('1988-10-25 18:50' AS DATETIME) as datetime;").fetchAllSync();
  test.ok(rows[0].datetime instanceof Date, "SELECT CAST('1988-10-25 18:50' AS DATETIME) is Date");
  test.equals(rows[0].datetime.toUTCString(), "Tue, 25 Oct 1988 18:50:00 GMT", "SELECT CAST('1988-10-25 18:50' AS TIME) is correct");
  
  conn.closeSync();
  
  test.done();
};

exports.fetchSetValues = function (test) {
  test.expect(5);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    rows;
  
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  rows = conn.querySync("SELECT size, colors FROM " + cfg.test_table + " WHERE size='small';").fetchAllSync();
  test.ok(rows[0].colors instanceof Array, "SET fetched result is Array");
  test.same(rows[0].colors, ['red'], "SET fetched result is correct");
  
  rows = conn.querySync("SELECT size, colors FROM " + cfg.test_table + " WHERE size='medium';").fetchAllSync();
  test.ok(rows[0].colors instanceof Array, "SET fetched result is Array");
  test.same(rows[0].colors, ['red', 'green', 'blue'], "SET fetched result is correct");
  
  conn.closeSync();
  
  test.done();
};

