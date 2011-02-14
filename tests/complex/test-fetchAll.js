/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Load configuration
var cfg = require('../config');

exports.createTestTableComplex2 = function (test) {
  test.expect(3);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    tables;
  
  conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table2 + ";");
  conn.querySync("CREATE TABLE " + cfg.test_table2 +
    " (size ENUM('small', 'medium', 'large')," +
    "  colors VARCHAR(32)) " + cfg.store_engine + ";");
  res = conn.querySync("SHOW TABLES");
  tables = res.fetchAllSync();
  
  test.ok(res.fieldCount === 1, "SHOW TABLES result field count === 1");
  test.ok(tables.some(function (r) {
    return r['Tables_in_' + cfg.database] === cfg.test_table2;
  }), "Find the test_table2 in results");
  
  res = conn.querySync("INSERT INTO " + cfg.test_table2 +
                   " (size, colors) VALUES ('small', 'red');");
  res = conn.querySync("INSERT INTO " + cfg.test_table2 +
                   " (size, colors) VALUES ('small', 'orange');") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table2 +
                    " (size, colors) VALUES ('medium', 'black');") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table2 +
                   " (size, colors) VALUES ('large', 'deep purple');") && res;
  test.ok(res, "conn.querySync('INSERT INTO test_table2 ...')");
  
  conn.closeSync();
  test.done();
};

exports.FetchAllWithBooleanOption = function (test) {
  test.expect(9);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
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
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
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
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
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
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
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
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("SELECT t1.size, t1.colors, t2.size, t2.colors " +
                       "FROM " + cfg.test_table + " t1, " + cfg.test_table2 + " t2 " +
                       "WHERE t1.size = t2.size AND t1.size != 'medium';");
  test.ok(res, "SELECT");
  
  res.fetchAll({'structured': false}, function (err, rows) {
    test.ok(err === null, "res.fetchAll() err===null");
    test.same(rows,
      [{size: 'small', colors: 'red'},
       {size: 'small', colors: 'orange'},
       {size: 'large', colors: 'deep purple'},
       {size: 'large', colors: 'deep purple'}
      ], "conn.querySync('SELECT ...').fetchAllSync({'structured': false})");
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
        [{t1: {size: 'small', colors: ['red']},
          t2: {size: 'small', colors: 'red'}
         },
         {t1: { size: 'small', colors: ['red']},
          t2: { size: 'small', colors: 'orange'}
         },
         {t1: { size: 'large', colors: ['red', 'blue'] },
          t2: { size: 'large', colors: 'deep purple' }
         }
        ], "conn.querySync('SELECT ...').fetchAll({'structured': true})");
      res.freeSync();
      
      conn.closeSync();
      test.done();
    });
  });
};

exports.FetchAllSyncWithObjectStructuredOption = function (test) {
  test.expect(7);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    rows;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("SELECT t1.size, t1.colors, t2.size, t2.colors " +
                       "FROM " + cfg.test_table + " t1, " + cfg.test_table2 + " t2 " +
                       "WHERE t1.size = t2.size AND t1.size != 'medium';");
  test.ok(res, "SELECT");
  
  rows = res.fetchAllSync({'structured': false});
  test.same(rows,
    [{size: 'small', colors: 'red'},
     {size: 'small', colors: 'orange'},
     {size: 'large', colors: 'deep purple'},
     {size: 'large', colors: 'deep purple'}
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
    [{t1: { size: 'small', colors: ['red']},
      t2: { size: 'small', colors: 'red'}
     },
     {t1: { size: 'small', colors: ['red']},
      t2: { size: 'small', colors: 'orange'}
     },
     {t1: { size: 'large', colors: ['red', 'blue']},
      t2: { size: 'large', colors: 'deep purple'}
     }
    ], "conn.querySync('SELECT ...').fetchAllSync({'structured': true})");
  res.freeSync();
  
  conn.closeSync();
  test.done();
};

exports.FetchAllWithObjectOptionsConflicted = function (test) {
  test.expect(3);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
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
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
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

exports.setOptionSyncQueryFetchAll = function (test) {
  test.expect(5);
  
  var conn = cfg.mysql_libmysqlclient.createConnectionSync();
  
  conn.initSync();
  conn.setOptionSync(conn.MYSQL_OPT_RECONNECT, 1);
  conn.setOptionSync(conn.MYSQL_SET_CHARSET_NAME, "utf8");
  conn.realConnectSync(cfg.host, cfg.user, cfg.password, cfg.database);
  test.ok(conn.connectedSync(), "conn.realConnectSync(host, user, password, database)");
  
  conn.query("SELECT size, colors FROM " + cfg.test_table + " WHERE size;", function (err, res) {
    test.ok(err === null, "conn.query() err===null");
    res.fetchAll(function (err, rows, fields) {
      test.ok(err === null, "res.fetchAll() err===null");
      
      test.same(rows,
                [{size: 'small', colors: ['red']},
                 {size: 'medium', colors: ['red', 'green', 'blue']},
                 {size: 'large', colors: ['green']},
                 {size: 'large', colors: ['red', 'blue']}],
                "conn.querySync('SELECT ...').fetchAll()");
      test.same(fields, res.fetchFieldsSync(), "Callback fields argument == res.fetchFieldsSync()");
      
      res.freeSync();
      conn.closeSync();
      
      test.done();
    });
  });
};

