/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

exports.cfg = {
  // Database connection settings
  host: "localhost",
  user: "test",
  password: "",
  database: "test",
  database_denied: "mysql",
  test_table: "test_table",
  test_table2: "test_table2",
  test_table_notexists: "test_table_notexists",
  charset: "utf8",

  // Operations count for continuous tests
  reconnect_count: 10000,
  insert_rows_count: 10000,
  insert_rows_count_speedtest: 100000,

  // Operations count for slow tests
  slow_connects_nested: 20000,
  slow_connects_inloop: 150,
  slow_inserts_count: 20000,
  slow_fetches_nested: 20000,
  slow_fetches_inloop: 20000
};

