/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Load configuration
var cfg = require('../config');

/**
 * Reference:
 * http://dev.mysql.com/doc/refman/5.1/en/create-procedure.html
 * http://dev.mysql.com/doc/refman/5.1/en/call.html
 */

exports.CallStoredProcedureSelectSync = function (test) {
  test.expect(5);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(),
    res,
    num = 1234,
    numFromProcedure;
  
  conn.connectSync(cfg.host, cfg.user, cfg.password, cfg.database, null, null, conn.CLIENT_MULTI_RESULTS);
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");

  res = conn.querySync("DROP PROCEDURE IF EXISTS test_procedure;");
  test.ok(res, "DROP PROCEDURE IF EXISTS test_procedure;");
  
  res = conn.querySync("CREATE PROCEDURE test_procedure() SELECT " + num + " AS num;");
  test.ok(res, "CREATE PROCEDURE test_procedure");
  
  res = conn.querySync("CALL test_procedure();");
  test.ok(res, "CALL test_procedure();");
  
  numFromProcedure = res.fetchAllSync()[0].num;
  res.freeSync();
  
  test.equals(numFromProcedure, num, "numFromProcedure == num");
  
  conn.closeSync();
  test.done();
};

exports.CallStoredProcedureSelectTwiceSync = function (test) {
  test.expect(7);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(),
    res,
    num = 1234,
    numFromProcedure;
  
  conn.connectSync(cfg.host, cfg.user, cfg.password, cfg.database, null, null, conn.CLIENT_MULTI_RESULTS);
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");

  res = conn.querySync("DROP PROCEDURE IF EXISTS test_procedure;");
  test.ok(res, "DROP PROCEDURE IF EXISTS test_procedure;");
  
  res = conn.querySync("CREATE PROCEDURE test_procedure() SELECT " + num + " AS num;");
  test.ok(res, "CREATE PROCEDURE test_procedure");
  
  res = conn.querySync("CALL test_procedure();");
  test.ok(res, "CALL test_procedure();");
  
  numFromProcedure = res.fetchAllSync()[0].num;
  res.freeSync();
  
  test.equals(numFromProcedure, num, "numFromProcedure == num");
  
  // We need to get second result from set, that contains execution status
  while (conn.multiMoreResultsSync()) {
    // Go to next result in set
    conn.multiNextResultSync();
    // Store result
    res = conn.storeResultSync();
    if (typeof res.freeSync === 'function') {
      // Free result if it is not simple true/false
      res.freeSync();
    }
  }
  
  res = conn.querySync("CALL test_procedure();");
  
  test.ok(res, "CALL test_procedure();");

  numFromProcedure = res.fetchAllSync()[0].num;
  res.freeSync();
  
  test.equals(numFromProcedure, num, "numFromProcedure == num");
  
  // We need to get second result from set, that contains execution status
  while (conn.multiMoreResultsSync()) {
    // Go to next result in set
    conn.multiNextResultSync();
    // Store result
    res = conn.storeResultSync();
    if (typeof res.freeSync === 'function') {
      // Free result if it is not simple true/false
      res.freeSync();
    }
  }
  
  conn.closeSync();
  test.done();
};

exports.CallStoredProcedureSelectIntoSync = function (test) {
  test.expect(6);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(),
    res,
    num = 1234,
    numFromProcedure;
  
  conn.connectSync(cfg.host, cfg.user, cfg.password, cfg.database, null, null, conn.CLIENT_MULTI_RESULTS);
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");

  res = conn.querySync("DROP PROCEDURE IF EXISTS test_procedure;");
  test.ok(res, "DROP PROCEDURE IF EXISTS test_procedure;");
  
  res = conn.querySync("CREATE PROCEDURE test_procedure(OUT num INT) SELECT " + num + " INTO num;");
  test.ok(res, "CREATE PROCEDURE test_procedure");
  
  res = conn.querySync("CALL test_procedure(@num);");
  test.ok(res, "CALL test_procedure(@num);");
  
  res = conn.querySync("SELECT @num AS num;");
  test.ok(res, "SELECT @num AS num;");
  
  numFromProcedure = res.fetchAllSync()[0].num;
  res.freeSync();
  
  test.equals(numFromProcedure, num, "numFromProcedure == num");
  
  conn.closeSync();
  test.done();
};

exports.CallStoredProcedureSelectIntoTwiceSync = function (test) {
  test.expect(9);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(),
    res,
    num = 1234,
    numFromProcedure;
  
  conn.connectSync(cfg.host, cfg.user, cfg.password, cfg.database, null, null, conn.CLIENT_MULTI_RESULTS);
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");

  res = conn.querySync("DROP PROCEDURE IF EXISTS test_procedure;");
  test.ok(res, "DROP PROCEDURE IF EXISTS test_procedure;");
  
  res = conn.querySync("CREATE PROCEDURE test_procedure(OUT num INT) SELECT " + num + " INTO num;");
  test.ok(res, "CREATE PROCEDURE test_procedure");
  
  res = conn.querySync("CALL test_procedure(@num);");
  test.ok(res, "CALL test_procedure(@num);");
  
  res = conn.querySync("SELECT @num AS num;");
  test.ok(res, "SELECT @num AS num;");
  
  numFromProcedure = res.fetchAllSync()[0].num;
  res.freeSync();
  
  test.equals(numFromProcedure, num, "numFromProcedure == num");
  
  // We need to get second result from set, that contains execution status
  while (conn.multiMoreResultsSync()) {
    // Go to next result in set
    conn.multiNextResultSync();
    // Store result
    res = conn.storeResultSync();
    if (typeof res.freeSync === 'function') {
      // Free result if it is not simple true/false
      res.freeSync();
    }
  }
  
  res = conn.querySync("CALL test_procedure(@num);");
  test.ok(res, "CALL test_procedure(@num);");
  
  res = conn.querySync("SELECT @num AS num;");
  test.ok(res, "SELECT @num AS num;");
  
  numFromProcedure = res.fetchAllSync()[0].num;
  res.freeSync();
  
  test.equals(numFromProcedure, num, "numFromProcedure == num");
  
  // We need to get second result from set, that contains execution status
  while (conn.multiMoreResultsSync()) {
    // Go to next result in set
    conn.multiNextResultSync();
    // Store result
    res = conn.storeResultSync();
    if (typeof res.freeSync === 'function') {
      // Free result if it is not simple true/false
      res.freeSync();
    }
  }
  
  conn.closeSync();
  test.done();
};

exports.CallStoredFunctionSync = function (test) {
  test.expect(5);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(),
    res,
    error,
    num = 1234,
    numFromFunction;
  
  conn.connectSync(cfg.host, cfg.user, cfg.password, cfg.database, null, null, conn.CLIENT_MULTI_RESULTS);
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DROP FUNCTION IF EXISTS test_function;");
  test.ok(res, "DROP FUNCTION IF EXISTS test_function;");
  
  res = conn.querySync("CREATE FUNCTION test_function() RETURNS INT DETERMINISTIC RETURN " + num + ";");
  test.ok(res, "CREATE FUNCTION test_function");
  if (!res) {
    error = conn.errorSync();
    if (error.match(new RegExp("You do not have the SUPER privilege and binary logging is enabled"))) {
      console.log("Known MySQL bug: run `SET GLOBAL log_bin_trust_function_creators = 1;` "
                + "in MySQL console to fix this.");
    }
  }
  
  res = conn.querySync("SELECT test_function() AS num;");
  test.ok(res, "SELECT test_function() AS num;");

  numFromFunction = res.fetchAllSync()[0].num;
  res.freeSync();
  
  test.equals(numFromFunction, num, "numFromFunction == num");
  
  conn.closeSync();
  test.done();
};

exports.CallStoredFunctionTwiceSync = function (test) {
  test.expect(7);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(),
    res,
    error,
    num = 1234,
    numFromFunction;
  
  conn.connectSync(cfg.host, cfg.user, cfg.password, cfg.database, null, null, conn.CLIENT_MULTI_RESULTS);
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DROP FUNCTION IF EXISTS test_function;");
  test.ok(res, "DROP FUNCTION IF EXISTS test_function;");
  
  res = conn.querySync("CREATE FUNCTION test_function() RETURNS INT DETERMINISTIC RETURN " + num + ";");
  test.ok(res, "CREATE FUNCTION test_function");
  if (!res) {
    error = conn.errorSync();
    if (error.match(new RegExp("You do not have the SUPER privilege and binary logging is enabled"))) {
      console.log("Known MySQL bug: run `SET GLOBAL log_bin_trust_function_creators = 1;` "
                + "in MySQL console to fix this.");
    }
  }
  
  res = conn.querySync("SELECT test_function() AS num;");
  test.ok(res, "SELECT test_function() AS num;");

  numFromFunction = res.fetchAllSync()[0].num;
  res.freeSync();
  
  test.equals(numFromFunction, num, "numFromFunction == num");
  
  res = conn.querySync("SELECT test_function() AS num;");
  test.ok(res, "SELECT test_function() AS num;");

  numFromFunction = res.fetchAllSync()[0].num;
  res.freeSync();
  
  test.equals(numFromFunction, num, "numFromFunction == num");
  
  conn.closeSync();
  test.done();
};

exports.CallStoredProcedureSelectNested = function (test) {
  test.expect(8);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(),
    res,
    num = 1234,
    numFromProcedure;
  
  conn.connectSync(cfg.host, cfg.user, cfg.password, cfg.database, null, null, conn.CLIENT_MULTI_RESULTS);
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");

  res = conn.querySync("DROP PROCEDURE IF EXISTS test_procedure;");
  test.ok(res, "DROP PROCEDURE IF EXISTS test_procedure;");
  
  res = conn.querySync("CREATE PROCEDURE test_procedure() SELECT " + num + " AS num;");
  test.ok(res, "CREATE PROCEDURE test_procedure");
  
  conn.query("CALL test_procedure();", function (err, res) {
    test.ok(err === null, "conn.query() err===null");
    
    numFromProcedure = res.fetchAllSync()[0].num;
    res.freeSync();
    
    test.equals(numFromProcedure, num, "numFromProcedure == num");
    
    // We need to get second result from set, that contains execution status
    while (conn.multiMoreResultsSync()) {
      // Go to next result in set
      conn.multiNextResultSync();
      // Store result
      res = conn.storeResultSync();
      if (typeof res.freeSync === 'function') {
        // Free result if it is not simple true/false
        res.freeSync();
      }
    }
    
    conn.query("CALL test_procedure();", function (err, res) {
      test.ok(err === null, "conn.query() err===null");
      
      test.ok(res, "CALL test_procedure();");
    
      numFromProcedure = res.fetchAllSync()[0].num;
      res.freeSync();
      
      test.equals(numFromProcedure, num, "numFromProcedure == num");
      
      // We need to get second result from set, that contains execution status
      while (conn.multiMoreResultsSync()) {
        // Go to next result in set
        conn.multiNextResultSync();
        // Store result
        res = conn.storeResultSync();
        if (typeof res.freeSync === 'function') {
          // Free result if it is not simple true/false
          res.freeSync();
        }
      }
      
      conn.closeSync();
      test.done();
    });
  });
};