/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

/*
> mysql -u root
CREATE DATABASE test DEFAULT CHARACTER SET utf8 COLLATE utf8_unicode_ci;
GRANT ALL ON test.* TO test@localhost IDENTIFIED BY "";
*/

module.exports = {
  // Required modules
  mysql_libmysqlclient: require("../lib/mysql-libmysqlclient.js"),
  mysql_bindings: require("../lib/mysql-libmysqlclient.js").bindings,
  util: require("util"),

  // Database connection settings
  host:     process.env["TRAVIS"] ? "localhost" : "localhost",
  user:     process.env["TRAVIS"] ? "test_user" : "test",
  password: process.env["TRAVIS"] ? "1234"      : "",
  database: "test",
  database_denied: process.env["TRAVIS"] ? "test_denied" : "mysql",
  test_table: "test_table",
  test_table2: "test_table2",
  test_table_notexists: "test_table_notexists",
  charset: "utf8",
  store_engine: "ENGINE=MyISAM",

  // Operations count for continuous tests
  reconnect_count: 10000,
  insert_rows_count: 10000,

  // Operations count for slow tests
  slow_connects_nested: 10000,
  slow_connects_inloop: 100,
  slow_inserts_count:   10000,
  slow_fetches_nested:  10000,
  slow_fetches_inloop:  10000,

  // Pause before checks, to ensure MySQL inserts/update completed
  delay: 100
};
