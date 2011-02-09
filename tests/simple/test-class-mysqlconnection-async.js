/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Load configuration
var cfg = require('../config');

exports.Connect = function (test) {
  test.expect(1);
  
  var conn = new cfg.mysql_bindings.MysqlConnection();
  
  conn.connect(cfg.host, cfg.user, cfg.password, cfg.database, function (error) {
    test.ok(error === null, "conn.connect() for allowed database");
    conn.closeSync();
    
    test.done();
  });
};

exports.ConnectWithError = function (test) {
  test.expect(3);
  
  var conn = new cfg.mysql_bindings.MysqlConnection();
  
  conn.connect(cfg.host, cfg.user, cfg.password, cfg.database_denied, function (err) {
    var
      errno = conn.connectErrno,
      error = conn.connectError;
    
    test.equals(errno, 1044, "conn.connectErrno");
    test.ok(error.match(new RegExp("Access denied for user '(" + cfg.user + "|)'@'.*' to database '" + cfg.database_denied + "'")), "conn.connectError");
    
    test.equals(err.message, "Connection error #" + errno + ": " + error, "Callback exception in conn.connect() to denied database");
    
    test.done();
  });
};

exports.Query = function (test) {
  test.expect(3);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database);
  
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  conn.query("SHOW TABLES", function (err, res) {
    test.ok(res.fieldCount === 1, "show results field count === 1");
    var rows = res.fetchAllSync();
    res.freeSync();
    test.ok(rows.some(function (r) {
      return r['Tables_in_' + cfg.database] === cfg.test_table;
    }), "find the test table in results");
    conn.closeSync();
    test.done();
  });
};

exports.QueryWithLastInsertIdAndAffectedRows = function (test) {
  test.expect(9);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res;
  
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
  test.ok(res, "conn.querySync('DELETE FROM test_table')");
  
  res = conn.querySync("ALTER TABLE " + cfg.test_table + " AUTO_INCREMENT = 1;");
  test.ok(res, "conn.querySync('ALTER TABLE test_table AUTO_INCREMENT = 1;')");
  
  conn.query("INSERT INTO " + cfg.test_table + " (random_number, random_boolean) VALUES ('1', '0');", function (err, info) {
    test.equals(info.insertId, 1, "Last insert id");
    
    conn.query("INSERT INTO " + cfg.test_table + " (random_number, random_boolean) VALUES ('2', '0'), ('3', '0');", function (err, info) {
      // For MySQL version >= 5.1.12
      test.equals(info.insertId, 2, "Last insert id");
      
      conn.query("INSERT INTO " + cfg.test_table + " (random_number, random_boolean) VALUES ('4', '1'), ('5', '1');", function (err, info) {
        // For MySQL version >= 5.1.12
        test.equals(info.insertId, 4, "Last insert id");
        
        var insertIdSync = conn.lastInsertIdSync();
        test.equals(insertIdSync, 4, "Last insert id");
        
        conn.query("UPDATE " + cfg.test_table + " SET random_number = '0' WHERE random_boolean='0';", function (err, info) {
          test.equals(info.affectedRows, 3, "Affected rows count");
          
          conn.query("DELETE FROM " + cfg.test_table + " WHERE random_boolean='1';", function (err, info) {
            test.equals(info.affectedRows, 2, "Affected rows count");
            
            conn.closeSync();
            test.done();
          });
        });
      });
    });
  });
};

exports.QueryWithError = function (test) {
  test.expect(6);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res;
  
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  conn.query("SHOW TABLESaagh", function (err, res) {
    test.ok(err, "Error object is present");
    test.ok(!res, "Result is not defined");
    
    var
      errno = conn.errnoSync(),
      error = conn.errorSync();
    
    test.equals(errno, 1064, "conn.connectErrno");
    test.ok(error.match(new RegExp("You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near 'TABLESaagh'")), "conn.connectError");
    
    test.equals(err.message, "Query error #" + errno + ": " + error, "Callback exception in conn.query()");
    
    conn.closeSync();
    test.done();
  });
};

exports.QueryWithoutCallback = function (test) {
  test.expect(6);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res;
  
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DELETE FROM " + cfg.test_table + ";");
  test.ok(res, "conn.querySync('DELETE FROM cfg.test_table')");
  
  test.doesNotThrow(function () {
    conn.query("INSERT INTO " + cfg.test_table + " (random_number, random_boolean) VALUES ('1', '0');");
  });
  
  conn.query("SELECT random_number, random_boolean FROM " + cfg.test_table + ";", function (err, result) {
    test.ok(result, "Result is defined");
    test.ok(!err, "Error object is not present");
    
    var res = result.fetchAllSync();
    test.same(res, [{random_number: 1, random_boolean: 0}],
              "Right result, one row, [{random_number: 1, random_boolean: 0}]]");
    conn.closeSync();
    test.done();
  });
};

