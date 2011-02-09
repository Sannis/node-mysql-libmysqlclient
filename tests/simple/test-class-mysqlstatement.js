/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Load configuration
var cfg = require('../config');

exports.New = function (test) {
  test.expect(1);
  
  test.throws(function () {
    var stmt = new cfg.mysql_bindings.MysqlStatement();
  }, TypeError, "new mysql_bindings.MysqlStatement() should throw exception from JS code");
  
  test.done();
};

