/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Load configuration
var cfg = require("../config");

exports.issue157_query = function (test) {
  test.expect(3);

  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database);

  conn.query('SELECT SLEEP(5);', function (err) {
    test.ok(err instanceof Error);
    test.ok(err.message.indexOf("Connection is closed by closeSync()") === 0);

    test.done();
  });

  conn.closeSync();

  test.ok(true);
};

exports.issue157_querySend = function (test) {
  test.expect(3);

  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database);

  conn.querySend('SELECT SLEEP(5);', function (err) {
    test.ok(err instanceof Error);
    test.ok(err.message.indexOf("Connection is closed by closeSync()") === 0);

    test.done();
  });

  conn.closeSync();

  test.ok(true);
};
