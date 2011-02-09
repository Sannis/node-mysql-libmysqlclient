/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Load configuration
var cfg = require('../config');

/**
 * @url: https://github.com/Sannis/node-mysql-libmysqlclient/issues#issue/72
 *
 * Wrong gmt_delta calculation in  MysqlResult::GetFieldValue
 */
exports['gh-72'] = function (test) {
  test.expect(6);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    rows;
  
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  rows = conn.querySync("SELECT CAST('1988-10-25' AS DATETIME) as datetime;").fetchAllSync();
  test.equals(rows[0].datetime.toUTCString(), "Tue, 25 Oct 1988 00:00:00 GMT", "SELECT CAST('1988-10-25' AS DATETIME) is correct");
  
  rows = conn.querySync("SELECT CAST('1988-10-25 2:50' AS DATETIME) as datetime;").fetchAllSync();
  test.equals(rows[0].datetime.toUTCString(), "Tue, 25 Oct 1988 02:50:00 GMT", "SELECT CAST('1988-10-25 2:50' AS TIME) is correct");
  
  rows = conn.querySync("SELECT CAST('1988-10-25 3:50' AS DATETIME) as datetime;").fetchAllSync();
  test.equals(rows[0].datetime.toUTCString(), "Tue, 25 Oct 1988 03:50:00 GMT", "SELECT CAST('1988-10-25 3:50' AS TIME) is correct");
  
  rows = conn.querySync("SELECT CAST('1988-10-25 23:50' AS DATETIME) as datetime;").fetchAllSync();
  test.equals(rows[0].datetime.toUTCString(), "Tue, 25 Oct 1988 23:50:00 GMT", "SELECT CAST('1988-10-25 23:50' AS TIME) is correct");
  
  rows = conn.querySync("SELECT CAST('1988-10-25 18:50' AS DATETIME) as datetime;").fetchAllSync();
  test.equals(rows[0].datetime.toUTCString(), "Tue, 25 Oct 1988 18:50:00 GMT", "SELECT CAST('1988-10-25 18:50' AS TIME) is correct");
  
  conn.closeSync();
  
  test.done();
};

