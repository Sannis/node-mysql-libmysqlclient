/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

/*
mysql_query() cannot be used for statements that contain binary data; you must use mysql_real_query() instead.
(Binary data may contain the “\0” character, which mysql_query() interprets as the end of the statement string.)
In addition, mysql_real_query() is faster than mysql_query() because it does not call strlen() on the statement string.
*/

// Load configuration
var cfg = require("../config");

exports.Issue83Query = function (test) {
  test.expect(3);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database);

  res = conn.query("SELECT '1\u00002345\0' AS a;", function (err, res) {
    test.ok(res instanceof cfg.mysql_bindings.MysqlResult, "Result is defined");
    test.ok(err === null, "Error object is not present");
    
    rows = res.fetchAllSync();
    test.same(rows, [{a: "1\u00002345\0"}], "conn.querySync('SELECT ...').fetchAllSync()");
    
    conn.closeSync();
    
    test.done();
  });
};

exports.Issue83QuerySync = function (test) {
  test.expect(2);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res;

  res = conn.querySync("SELECT '1\u00002345\0' AS a;");
  test.ok(res instanceof cfg.mysql_bindings.MysqlResult, "SELECT '1\u00002345\0' AS a;");
  
  rows = res.fetchAllSync();
  test.same(rows, [{a: "1\u00002345\0"}], "conn.querySync('SELECT ...').fetchAllSync()");
  
  conn.closeSync();
  
  test.done();
};

exports.Issue83RealQuerySync = function (test) {
  test.expect(2);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res;

  conn.realQuerySync("SELECT '1\u00002345\0' AS a;");
  res = conn.storeResultSync();
  
  test.ok(res instanceof cfg.mysql_bindings.MysqlResult, "SELECT '1\u00002345\0' AS a;");
  
  rows = res.fetchAllSync();
  test.same(rows, [{a: "1\u00002345\0"}], "conn.querySync('SELECT ...').fetchAllSync()");
  
  conn.closeSync();
  
  test.done();
};

exports.Issue83MultyQuerySync = function (test) {
  test.expect(5);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res;

  conn.multiRealQuerySync("SELECT '1\u00002345\0' AS a; SELECT 'q\u0000werty\0' AS b;");
  res = conn.storeResultSync();
  test.ok(res instanceof cfg.mysql_bindings.MysqlResult, "SELECT '1\u00002345\0' AS a;");
  
  rows = res.fetchAllSync();
  test.same(rows, [{a: "1\u00002345\0"}], "conn.querySync('SELECT ...').fetchAllSync()");
  
  test.ok(conn.multiMoreResultsSync(), "conn.multiMoreResultsSync()");
  
  conn.multiNextResultSync();
  res = conn.storeResultSync();
  test.ok(res instanceof cfg.mysql_bindings.MysqlResult, "SELECT 'q\u0000werty\0' AS b;");
  
  rows = res.fetchAllSync();
  test.same(rows, [{b: "q\u0000werty\0"}], "conn.querySync('SELECT ...').fetchAllSync()");
  
  conn.closeSync();
  
  test.done();
};
