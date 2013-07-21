/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

/*
#See before_script commands in .travis.yml for actual version of this commends
> mysql -u root
DELETE FROM mysql.db WHERE Db = 'test\\_%';
DELETE FROM mysql.user WHERE Host = 'localhost' AND User = '';
FLUSH PRIVILEGES;
CREATE DATABASE test_allowed DEFAULT CHARACTER SET utf8 COLLATE utf8_unicode_ci;
GRANT ALL ON test_allowed.* TO test_user@localhost IDENTIFIED BY '1234';
CREATE DATABASE IF NOT EXISTS test_denied DEFAULT CHARACTER SET utf8 COLLATE utf8_unicode_ci;
*/

module.exports = {
  // Required modules
  mysql_libmysqlclient: require("../lib/mysql-libmysqlclient.js"),
  mysql_bindings: require("../lib/mysql-libmysqlclient.js").bindings,
  util: require("util"),

  // Database connection settings
  host:                 "localhost",
  port:                 3306,
  user:                 "test_user",
  password:             "1234",
  database:             "test_allowed",
  database_denied:      "test_denied",
  test_table:           "test_table",
  test_table2:          "test_table2",
  test_table_notexists: "test_table_notexists",
  charset:              "utf8",
  store_engine:         "ENGINE=MyISAM",

  // Operations count for continuous tests
  reconnect_count:   10000,
  insert_rows_count: 10000,

  // Operations count for slow tests
  slow_connects_nested: 10000,
  slow_connects_inloop: 100,
  slow_inserts_count:   10000,
  slow_fetches_nested:  10000,
  slow_fetches_inloop:  10000,

  // Pause before checks, to ensure MySQL inserts/update completed
  delay: 500
};
