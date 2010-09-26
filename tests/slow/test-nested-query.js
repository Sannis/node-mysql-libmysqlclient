/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Load configuration
var cfg = require("../config").cfg;

// Require modules
var
  sys = require("sys"),
  mysql_libmysqlclient = require("../../mysql-libmysqlclient");

exports.NestedQueryMany = function (test) {
  test.expect(2);
  
  var
    conn = mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database),
    random_boolean,
    i, ci = 0;
  
  test.ok(conn, "mysql_libmysqlclient.createConnectionSync(host, user, password, database)");
  
  conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  conn.querySync("CREATE TABLE " + cfg.test_table +
    " (random_number INT(8) NOT NULL, random_boolean BOOLEAN NOT NULL) TYPE=MEMORY;");
  
  for (i = 0; i < cfg.insert_rows_count; i += 1) {
    random_boolean = (Math.random() > 0.5) ? 1 : 0;
    
    conn.query("INSERT INTO " + cfg.test_table +
      " (random_number, random_boolean) VALUES ('" + i +
      "', '" + random_boolean + "');", function (err, result) {
        if (err) {
          sys.puts(conn.errorSync())
          throw err;
        }
        
        ci += 1;
        
        if (ci === cfg.insert_rows_count) {
          conn.query("SELECT COUNT(random_number) AS c FROM " + cfg.test_table + ";", function (err, result) {
            if (err) {
              throw err;
            }
            
            test.equals(result.fetchAllSync()[0].c, cfg.insert_rows_count);
            conn.closeSync();
            test.done();
          });
        }
      });
  }
};

