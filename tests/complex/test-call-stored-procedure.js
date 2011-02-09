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
exports.CallStoredProcedureSync = function (test) {
  test.expect(12);
  
  var
    conn = cfg.mysql_libmysqlclient.createConnectionSync(),
    res,
    max_num,
    max_num_proc,
    max_num_func,
    rows;
  
  conn.connectSync(cfg.host, cfg.user, cfg.password, cfg.database, null, null, conn.CLIENT_MULTI_RESULTS);
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  // Insert some rows
  res = conn.querySync("INSERT INTO " + cfg.test_table + " (num) VALUES (1);");
  res = conn.querySync("INSERT INTO " + cfg.test_table + " (num) VALUES (2);") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table + " (num) VALUES (654);") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table + " (num) VALUES (3415);") && res;
  test.ok(res, "conn.querySync('INSERT INTO test_table ...')");
  
  // Calc MAX(num) with simple SELECT query
  res = conn.querySync("SELECT MAX(num) FROM " + cfg.test_table + ";");
  test.ok(res, "SELECT MAX(num) FROM test_table;')");
  
  max_num = res.fetchAllSync()[0]['MAX(num)'];
  res.freeSync();
  
  // Calc MAX(num) with stored procedure
  res = conn.querySync("DROP PROCEDURE IF EXISTS maxnum_proc;");
  test.ok(res, "DROP PROCEDURE IF EXISTS maxnum_proc");
  
  res = conn.querySync("CREATE PROCEDURE maxnum_proc(OUT max_num INT) SELECT MAX(num) FROM " + cfg.test_table + " INTO max_num;");
  test.ok(res, "CREATE PROCEDURE maxnum_proc");
  
  res = conn.querySync("CALL maxnum_proc(@max_num);");
  test.ok(res, "CALL maxnum_proc(@max_num);");
  
  res = conn.querySync("SELECT @max_num;");
  test.ok(res, "SELECT @max_num;");
  
  max_num_proc = res.fetchAllSync()[0]['@max_num'];
  res.freeSync();
  
  /*
  // Calc MAX(num) with stored function
  res = conn.querySync("DROP FUNCTION IF EXISTS maxnum_func;");
  test.ok(res, "DROP FUNCTION IF EXISTS maxnum_func");
  
  res = conn.querySync("CREATE FUNCTION maxnum_func() RETURNS INT DETERMINISTIC RETURN 1;");
  test.ok(res, "CREATE FUNCTION maxnum_func");
  if (!res) {
    error = conn.errorSync();
    console.log(error);
    if (error.match(new RegExp("You do not have the SUPER privilege and binary logging is enabled"))) {
      console.log("Known MySQL bug: run `SET GLOBAL log_bin_trust_function_creators = 1;` in MySQL console to fix this.");
    }
  }
  
  res = conn.querySync("SELECT maxnum_func();");
  test.ok(res, "SELECT maxnum_func();");
  if (!res) {
    console.log(conn.errorSync());
  }
  console.log(res.fetchAllSync());
  max_num_func = res.fetchAllSync()[0]['maxnum_func()'];
  res.freeSync();
  */
  
  // max_num_proc should equals max_num
  test.equals(max_num_proc, max_num, "max_num_proc == max_num");
  // max_num_func should equals max_num
  //test.equals(max_num_func, max_num, "max_num_func == max_num");
  
  // Select all rows with stored procedure
  res = conn.querySync("DROP PROCEDURE IF EXISTS select_num_proc;");
  test.ok(res, "DROP PROCEDURE IF EXISTS select_num_proc");
  
  res = conn.querySync("CREATE PROCEDURE select_num_proc() SELECT num FROM " + cfg.test_table + " WHERE num > 2;");
  test.ok(res, "CREATE PROCEDURE select_num_proc");
  
  res = conn.querySync("CALL select_num_proc();");
  test.ok(res, "CALL select_num_proc()");
  
  rows = res.fetchAllSync();
  test.same(rows, [{num: 654}, {num: 3415}], "Select rows by stored procedure");
  
  res.freeSync();
  
  conn.closeSync();
  test.done();
};


