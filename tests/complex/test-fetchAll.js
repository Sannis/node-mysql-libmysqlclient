/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Load configuration
var cfg = require("../config").cfg;

// Require modules
var
  mysql_libmysqlclient = require("../../mysql-libmysqlclient");

exports.FetchAllWithBooleanOption = function (test) {
  test.expect(9);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("SELECT size, colors FROM " + cfg.test_table + " WHERE size='small';");
  test.ok(res, "SELECT");
  
  res.fetchAll(false, function (err, rows) {
    test.ok(err === null, "res.fetchAll() err===null");
    test.same(rows, [{size: 'small', colors: ['red']}], "conn.querySync('SELECT ...').fetchAllSync(false)");
    res.freeSync();
    
    res = conn.querySync("SELECT size, colors FROM " + cfg.test_table + " WHERE size='small';");
    test.ok(res, "SELECT");
    
    res.fetchAll(true, function (err, rows) {
      test.ok(err === null, "res.fetchAll() err===null");
      test.ok(Array.isArray(rows), "Result returns an array");
      test.ok(Array.isArray(rows[0]), "Result returns an array of arrays");
      test.same(rows,  [['small', ['red']]], "conn.querySync('SELECT ...').fetchAll(true)");
      res.freeSync();
      
      conn.closeSync();
      test.done();
    });
  });
};

exports.FetchAllSyncWithBooleanOption = function (test) {
  test.expect(7);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    rows;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("SELECT size, colors FROM " + cfg.test_table + " WHERE size='small';");
  test.ok(res, "SELECT");
  
  rows = res.fetchAllSync(false);
  test.same(rows, [{size: 'small', colors: ['red']}], "conn.querySync('SELECT ...').fetchAllSync(false)");
  res.freeSync();
  
  res = conn.querySync("SELECT size, colors FROM " + cfg.test_table + " WHERE size='small';");
  test.ok(res, "SELECT");
  
  rows = res.fetchAllSync(true);
  test.ok(Array.isArray(rows), "Result returns an array");
  test.ok(Array.isArray(rows[0]), "Result returns an array of arrays");
  test.same(rows, [['small', ['red']]], "conn.querySync('SELECT ...').fetchAllSync(true)");
  res.freeSync();
  
  conn.closeSync();
  test.done();
};

exports.FetchAllWithObjectArrayOption = function (test) {
  test.expect(9);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("SELECT size, colors FROM " + cfg.test_table + " WHERE size='small';");
  test.ok(res, "SELECT");
  
  res.fetchAll({'array': false}, function (err, rows) {
    test.ok(err === null, "res.fetchAll() err===null");
    test.same(rows, [{size: 'small', colors: ['red']}], "conn.querySync('SELECT ...').fetchAllSync({'array': false})");
    res.freeSync();
    
    res = conn.querySync("SELECT size, colors FROM " + cfg.test_table + " WHERE size='small';");
    test.ok(res, "SELECT");
    
    res.fetchAll({'array': true}, function (err, rows) {
      test.ok(err === null, "res.fetchAll() err===null");
      test.ok(Array.isArray(rows), "Result returns an array");
      test.ok(Array.isArray(rows[0]), "Result returns an array of arrays");
      test.same(rows, [['small', ['red']]], "conn.querySync('SELECT ...').fetchAll({'array': true})");
      res.freeSync();
      
      conn.closeSync();
      test.done();
    });
  });
};

exports.FetchAllSyncWithObjectArrayOption = function (test) {
  test.expect(7);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    rows;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("SELECT size, colors FROM " + cfg.test_table + " WHERE size='small';");
  test.ok(res, "SELECT");
  
  rows = res.fetchAllSync({'array': false});
  test.same(rows, [{size: 'small', colors: ['red']}], "conn.querySync('SELECT ...').fetchAllSync({'array': false})");
  res.freeSync();
  
  res = conn.querySync("SELECT size, colors FROM " + cfg.test_table + " WHERE size='small';");
  test.ok(res, "SELECT");
  
  rows = res.fetchAllSync({'array': true});
  test.ok(Array.isArray(rows), "Result returns an array");
  test.ok(Array.isArray(rows[0]), "Result returns an array of arrays");
  test.same(rows, [['small', ['red']]], "conn.querySync('SELECT ...').fetchAllSync({'array': true})");
  res.freeSync();
  
  conn.closeSync();
  test.done();
};

exports.FetchAllWithObjectStructuredOption = function (test) {
  test.expect(9);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("SELECT t1.size, t1.colors, t2.size, t2.colors " +
                       "FROM " + cfg.test_table + " t1, " + cfg.test_table2 + " t2 " +
                       "WHERE t1.size = t2.size AND t1.size != 'medium';");
  test.ok(res, "SELECT");
  
  res.fetchAll({'structured': false}, function (err, rows) {
    test.ok(err === null, "res.fetchAll() err===null");
    test.same(rows,
      [ { size: 'small', colors: 'red' }
      , { size: 'small', colors: 'orange' }
      , { size: 'large', colors: 'deep purple' }
      , { size: 'large', colors: 'deep purple' }
      ] , "conn.querySync('SELECT ...').fetchAllSync({'structured': false})");
    res.freeSync();
    
    res = conn.querySync("SELECT t1.size, t1.colors, t2.size, t2.colors " +
                         "FROM " + cfg.test_table + " t1, " + cfg.test_table2 + " t2 " +
                         "WHERE t1.size = t2.size AND t1.size != 'medium' AND t1.colors != 'green';");
    test.ok(res, "SELECT");
    
    res.fetchAll({'structured': true}, function (err, rows) {
      test.ok(err === null, "res.fetchAll() err===null");
      test.ok(Array.isArray(rows), "Result returns an array");
      test.ok(rows[0] instanceof Object, "Result returns an array of objects");
      test.same(rows,
        [ { t1: { size: 'small', colors: ['red'] }
          , t2: { size: 'small', colors: 'red' }
          }
        , { t1: { size: 'small', colors: ['red'] }
          , t2: { size: 'small', colors: 'orange' }
          }
        , { t1: { size: 'large', colors: ['red', 'blue'] }
          , t2: { size: 'large', colors: 'deep purple' }
          }
        ] , "conn.querySync('SELECT ...').fetchAll({'structured': true})");
      res.freeSync();
      
      conn.closeSync();
      test.done();
    });
  });
};

exports.FetchAllSyncWithObjectStructuredOption = function (test) {
  test.expect(7);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    rows;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("SELECT t1.size, t1.colors, t2.size, t2.colors " +
                       "FROM " + cfg.test_table + " t1, " + cfg.test_table2 + " t2 " +
                       "WHERE t1.size = t2.size AND t1.size != 'medium';");
  test.ok(res, "SELECT");
  
  rows = res.fetchAllSync({'structured': false});
  test.same(rows,
    [ { size: 'small', colors: 'red' }
    , { size: 'small', colors: 'orange' }
    , { size: 'large', colors: 'deep purple' }
    , { size: 'large', colors: 'deep purple' }
    ], "conn.querySync('SELECT ...').fetchAllSync({'structured': false})");
  res.freeSync();
  
  res = conn.querySync("SELECT t1.size, t1.colors, t2.size, t2.colors " +
                       "FROM " + cfg.test_table + " t1, " + cfg.test_table2 + " t2 " +
                       "WHERE t1.size = t2.size AND t1.size != 'medium' AND t1.colors != 'green';");
  test.ok(res, "SELECT");
  
  rows = res.fetchAllSync({'structured': true});
  test.ok(Array.isArray(rows), "Result returns an array");
  test.ok(rows[0] instanceof Object, "Result returns an array of objects");
  test.same(rows,
    [ { t1: { size: 'small', colors: ['red'] }
      , t2: { size: 'small', colors: 'red' }
      }
    , { t1: { size: 'small', colors: ['red'] }
      , t2: { size: 'small', colors: 'orange' }
      }
    , { t1: { size: 'large', colors: ['red', 'blue'] }
      , t2: { size: 'large', colors: 'deep purple' }
      }
    ] , "conn.querySync('SELECT ...').fetchAllSync({'structured': true})");
  res.freeSync();
  
  conn.closeSync();
  test.done();
};

exports.FetchAllWithObjectOptionsConflicted = function (test) {
  test.expect(3);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    rows;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("SELECT t1.size, t1.colors, t2.size, t2.colors " +
                       "FROM " + cfg.test_table + " t1, " + cfg.test_table2 + " t2 " +
                       "WHERE t1.size = t2.size AND t1.size != 'medium';");
  test.ok(res, "SELECT");
  
  test.throws(function () {
    res.fetchAll({'array': true, 'structured': true}, function (err, rows) {});
  }, Error, "You can't mix 'array' and 'structured' parameters");
  
  res.freeSync();
  
  conn.closeSync();
  test.done();
};

exports.FetchAllSyncWithObjectOptionsConflicted = function (test) {
  test.expect(3);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    rows;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("SELECT t1.size, t1.colors, t2.size, t2.colors " +
                       "FROM " + cfg.test_table + " t1, " + cfg.test_table2 + " t2 " +
                       "WHERE t1.size = t2.size AND t1.size != 'medium';");
  test.ok(res, "SELECT");
  
  test.throws(function () {
    rows = res.fetchAllSync({'array': true, 'structured': true});
  }, Error, "You can't mix 'array' and 'structured' parameters");
  
  res.freeSync();
  
  conn.closeSync();
  test.done();
};

