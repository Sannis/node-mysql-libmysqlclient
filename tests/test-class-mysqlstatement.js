/*
Copyright (C) 2010, Oleg Efimov <efimovov@gmail.com>

See license text in LICENSE file
*/

// Mixin settings
/*global host, user, password, database, database_denied, charset, reconnect_count */
process.mixin(require("./settings"));

// Require modules
var
  sys = require("sys"),
  mysql_libmysqlclient = require("../mysql-libmysqlclient");

