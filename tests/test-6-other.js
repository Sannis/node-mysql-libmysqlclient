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

exports.query_FetchAllSync = function (test) {
  test.expect(5);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res,
    rows;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  res = conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  res = conn.querySync("CREATE TABLE " + cfg.test_table +
    " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
    " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
    " PRIMARY KEY (autoincrement_id)) TYPE=MEMORY;") && res;
  test.ok(res, "conn.querySync('DELETE FROM cfg.test_table')");
  
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('1', '1');") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                    " (random_number, random_boolean) VALUES ('2', '1');") && res;
  res = conn.querySync("INSERT INTO " + cfg.test_table +
                   " (random_number, random_boolean) VALUES ('3', '0');") && res;
  test.ok(res, "conn.querySync('INSERT INTO cfg.test_table ...')");
  
  res = conn.querySync("SELECT random_number from " + cfg.test_table +
                   " WHERE random_boolean='0';");
  test.ok(res, "conn.querySync('SELECT ...')");
  rows = res.fetchAllSync();
  test.same(rows, [{random_number: 3}], "conn.query('SELECT ...').fetchAll()");
  
  conn.closeSync();
  
  test.done();
};

exports.query_DropTestTableSync = function (test) {
  test.expect(2);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res;
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user, password, database)");
  res = conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  test.equals(res, true, "conn.querySync('DROP TABLE IF EXISTS ...;'");
  conn.closeSync();
  
  test.done();
};

exports.query_CreateTestTableSync = function (test) {
  test.expect(2);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res;
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  res = conn.querySync("CREATE TABLE " + cfg.test_table +
    " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
    " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
    " PRIMARY KEY (autoincrement_id)) TYPE=MEMORY;");
  test.equals(res, true, "conn.querySync('CREATE TABLE ...;'");
  conn.closeSync();
  
  test.done();
};

exports.query_InsertIntoTestTableSync = function (test) {
  test.expect(2);
  
  var conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    res = true,
    random_number,
    random_boolean,
    i;
  
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  for (i = 0; i < cfg.insert_rows_count; i += 1)
  {
    random_number = Math.round(Math.random() * 1000000);
    random_boolean = (Math.random() > 0.5) ? 1 : 0;
    res = conn.querySync("INSERT INTO " + cfg.test_table +
      " (random_number, random_boolean) VALUES ('" + random_number +
      "', '" + random_boolean + "');") && res;
  }

  test.equals(res, true, "Insert " + cfg.insert_rows_count + " rows into table " + cfg.test_table);
  conn.closeSync();
  
  test.done();
};

