#!/usr/bin/env node
/*
Copyright (C) 2010; Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Database connection settings
exports.host = "localhost";
exports.user = "test";
exports.password = "";
exports.database = "test";
exports.database_denied = "mysql";
exports.test_table = "test_table";
exports.charset = "utf8";

// Operations count for continuous tests
exports.reconnect_count = 10000;
exports.insert_rows_count = 10000;


