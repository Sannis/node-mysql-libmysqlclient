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

exports.fetchDateAndTimeValues = function (test) {
  test.expect(9);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    rows;
  
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  rows = conn.querySync("SELECT CAST('2 2:50' AS TIME) as time;").fetchAllSync();
  test.ok(rows[0].time instanceof Date, "SELECT CAST('2 2:50' AS TIME) is Date");
  test.equals(rows[0].time.toUTCString(), "Sat, 03 Jan 1970 02:50:00 GMT", "SELECT CAST('2 2:50' AS TIME) is correct");
  //console.log(rows[0].time.toUTCString());
  //console.log("Expected Sat, 03 Jan 1970 02:50:00 GMT");

  rows = conn.querySync("SELECT CAST('2000-01-01' AS DATE) as date;").fetchAllSync();
  test.ok(rows[0].date instanceof Date, "SELECT CAST('2000-01-01' AS DATE) is Date");
  test.equals(rows[0].date.toUTCString(), "Sat, 01 Jan 2000 00:00:00 GMT", "SELECT CAST('2000-01-01' AS DATE) is correct");
  //console.log(rows[0].date.toUTCString());
  //console.log("Expected Sat, 01 Jan 2000 00:00:00 GMT");

  rows = conn.querySync("SELECT CAST('1988-10-25 06:34' AS DATETIME) as datetime;").fetchAllSync();
  test.ok(rows[0].datetime instanceof Date, "SELECT CAST('1988-10-25 06:34' AS DATETIME) is Date");
  test.equals(rows[0].datetime.toUTCString(), "Tue, 25 Oct 1988 06:34:00 GMT", "SELECT CAST('1988-10-25 06:34' AS DATETIME) is correct");
  //console.log(rows[0].datetime.toUTCString());
  //console.log("Expected Tue, 25 Oct 1988 06:34:00 GMT");

  rows = conn.querySync("SELECT CAST('1988-10-25' AS DATETIME) as datetime;").fetchAllSync();
  test.ok(rows[0].datetime instanceof Date, "SELECT CAST('1988-10-25' AS DATETIME) is Date");
  test.equals(rows[0].datetime.toUTCString(), "Tue, 25 Oct 1988 00:00:00 GMT", "SELECT CAST('1988-10-25' AS DATETIME) is correct");
  //console.log(rows[0].datetime.toUTCString());
  //console.log("Expected Tue, 25 Oct 1988 00:00:00 GMT");

  conn.closeSync();
  
  test.done();
};

