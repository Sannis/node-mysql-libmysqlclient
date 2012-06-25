/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Load configuration
var cfg = require('../config');

exports.createTestTableComplex2BinaryBlobs = function (test) {
  test.expect(3);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    tables;
  
  conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table2 + ";");
  conn.querySync("CREATE TABLE " + cfg.test_table2 +
    " (id INT AUTO_INCREMENT, vc VARCHAR(8), vbi VARBINARY(8), bi BINARY(8), t TEXT, b BLOB, PRIMARY KEY (id)) " + cfg.store_engine + ";");
  res = conn.querySync("SHOW TABLES");
  tables = res.fetchAllSync();
  
  test.ok(res.fieldCount === 1, "SHOW TABLES result field count === 1");
  test.ok(tables.some(function (r) {
    return r['Tables_in_' + cfg.database] === cfg.test_table2;
  }), "Find the test_table2 in results");
  
  res = conn.querySync("INSERT INTO " + cfg.test_table2 +
                   " (vc, vbi, bi, t, b) VALUES ('qwerty', 'qwerty', 'qwerty', 'qwerty', 'qwerty');");
  res = conn.querySync("INSERT INTO " + cfg.test_table2 +
                   " (vc, vbi, bi, t, b) VALUES ('qwe\\0\\0', 'qwe\\0\\0', 'qwe\\0\\0', 'qwe\\0\\0', 'qwe\\0\\0');") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table2 +
                   " (vc, vbi, bi, t) VALUES ('12\\0\\0', '12\\0\\0', '12\\0\\0', '12\\0\\0');") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table2 +
                   " (vc, vbi, bi, b) VALUES ('34\\0\\0', '34\\0\\0', '34\\0\\0', '34\\0\\0');") && res;
  test.ok(res, "conn.querySync('INSERT INTO test_table2 ...')");
  
  // Ensure all rows are inserted
  setTimeout(function () {
    conn.closeSync();
    test.done();
  }, 100);
};

exports.FetchAllSyncWithBinaryFields = function (test) {
  test.expect(2);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    rows,
    rowsExpected;

  res = conn.querySync("SELECT vc, vbi, bi, t, b FROM " + cfg.test_table2 + " ORDER BY id;");
  test.ok(res instanceof cfg.mysql_bindings.MysqlResult);
  
  rows = res.fetchAllSync(true);

  rowsExpected = [ [ 'qwerty',          new Buffer('qwerty'),  new Buffer('qwerty\0\0'),     'qwerty',          new Buffer('qwerty') ],
                   [ 'qwe\u0000\u0000', new Buffer('qwe\0\0'), new Buffer('qwe\0\0\0\0\0'),  'qwe\u0000\u0000', new Buffer('qwe\0\0') ],
                   [ '12\u0000\u0000',  new Buffer('12\0\0'),  new Buffer('12\0\0\0\0\0\0'), '12\u0000\u0000',  null ],
                   [ '34\u0000\u0000',  new Buffer('34\0\0'),  new Buffer('34\0\0\0\0\0\0'), null,              new Buffer('34\0\0') ] ];

  test.same(cfg.util.inspect(rows), cfg.util.inspect(rowsExpected), "conn.querySync('SELECT ...').fetchAllSync(true)");

  res.freeSync();
  
  conn.closeSync();
  
  test.done();
};

