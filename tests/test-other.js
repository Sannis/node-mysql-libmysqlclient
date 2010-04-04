/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Mixin settings
/*global host, user, password, database, test_table, insert_rows_count */
process.mixin(require("./settings"));

// Require modules
var
  sys = require("sys"),
  mysql_libmysqlclient = require("../mysql-libmysqlclient");

exports.query_FetchAll = function (test) {
  test.expect(5);
  
  var conn = mysql_libmysqlclient.createConnection(host, user, password, database),
    res,
    rows;
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user, password, database)");
  
  res = conn.query("DROP TABLE IF EXISTS " + test_table + ";");
  res = conn.query("CREATE TABLE " + test_table +
    " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
    " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
    " PRIMARY KEY (autoincrement_id)) TYPE=MEMORY;") && res;
  test.ok(res, "conn.query('DELETE FROM test_table')");
  
  res = conn.query("INSERT INTO " + test_table +
                   " (random_number, random_boolean) VALUES ('1', '1');") && res;
  res = conn.query("INSERT INTO " + test_table +
                    " (random_number, random_boolean) VALUES ('2', '1');") && res;
  res = conn.query("INSERT INTO " + test_table +
                   " (random_number, random_boolean) VALUES ('3', '0');") && res;
  test.ok(res, "conn.query('INSERT INTO test_table ...')");
  
  res = conn.query("SELECT random_number from " + test_table +
                   " WHERE random_boolean='0';");
  test.ok(res, "conn.query('SELECT ...')");
  rows = res.fetchAll();
  test.same(rows, [{random_number: 3}], "conn.query('SELECT ...').fetchAll()")
  
  conn.close();
  
  test.done();
};

exports.query_DropTestTable = function (test) {
  test.expect(2);
  
  var conn = mysql_libmysqlclient.createConnection(host, user, password, database),
    res;
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user, password, database)");
  res = conn.query("DROP TABLE IF EXISTS " + test_table + ";");
  test.equals(res, true, "conn.query('DROP TABLE IF EXISTS ...;'");
  conn.close();
  
  test.done();
};

exports.query_CreateTestTable = function (test) {
  test.expect(2);
  
  var conn = mysql_libmysqlclient.createConnection(host, user, password, database),
    res;
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user, password, database)");
  res = conn.query("CREATE TABLE " + test_table +
    " (autoincrement_id BIGINT NOT NULL AUTO_INCREMENT," +
    " random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL," +
    " PRIMARY KEY (autoincrement_id)) TYPE=MEMORY;");
  test.equals(res, true, "conn.query('CREATE TABLE ...;'");
  conn.close();
  
  test.done();
};

exports.query_InsertIntoTestTable = function (test) {
  test.expect(2);
  
  var conn = mysql_libmysqlclient.createConnection(host, user, password, database),
    res = true,
    random_number,
    random_boolean,
    i;
  
  test.ok(conn, "mysql_libmysqlclient.createConnection(host, user, password, database)");
  
  for (i = 0; i < insert_rows_count; i += 1)
  {
    random_number = Math.round(Math.random() * 1000000);
    random_boolean = (Math.random() > 0.5) ? 1 : 0;
    res = conn.query("INSERT INTO " + test_table +
      " (random_number, random_boolean) VALUES ('" + random_number +
      "', '" + random_boolean + "');") && res;
  }

  test.equals(res, true, "Insert " + insert_rows_count + " rows into table " + test_table);
  conn.close();
  
  test.done();
};



/*
// TODO: Rewrite these tests

for( var i in show_tables_result )
{
  sys.puts(i + ": " + JSON.stringify(show_tables_result[i]));
}

sys.print("Test conn.lastInsertId(): ");
var insert_id = conn.lastInsertId();
if( insert_id == insert_rows_count )
{
  sys.print(" OK\n");
}
else
{
  sys.print(" FAILED [" + insert_id + " != " + insert_rows_count + "]\n");
}

sys.print("Run end get results of query 'SELECT * FROM " + test_table + " ORDER BY RAND() LIMIT 10': ");
conn.query("SELECT * FROM " + test_table + " ORDER BY RAND() LIMIT 10;");
var select_limit_result = conn.fetchAll();
sys.print("OK\n");
for( var i in select_limit_result )
{
  sys.puts(i + ": " + JSON.stringify(select_limit_result[i]));
}
*/

