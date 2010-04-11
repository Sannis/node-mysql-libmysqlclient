/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

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
  charset: "utf8",

  // Operations count for continuous tests
  reconnect_count: 10000,
  insert_rows_count: 10000,
  insert_rows_count_speedtest: 100000
};

