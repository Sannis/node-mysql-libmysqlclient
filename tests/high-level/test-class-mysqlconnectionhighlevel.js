/*
Copyright by Oleg Efimov and node-mysql-libmysqlclient contributors
See contributors list in README

See license text in LICENSE file
*/

// Load configuration
var cfg = require('../config');

exports.Setup = function (test) {
  test.expect(0);

  var conn = cfg.mysql_libmysqlclient.createConnectionSync(cfg.host, cfg.user, cfg.password, cfg.database);

  conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table + ";");
  conn.querySync("CREATE TABLE " + cfg.test_table + " (number INT(8) NOT NULL) ENGINE=MEMORY;");

  conn.closeSync();

  test.done();
};

exports.New = function (test) {
  test.expect(1);

  test.doesNotThrow(function () {
    (new cfg.mysql_libmysqlclient.MysqlConnectionHighlevel());
  });

  test.done();
};

exports.ConnectWithCallback = function (test) {
  test.expect(2);

  var conn = cfg.mysql_libmysqlclient.createConnectionHighlevelSync();

  conn.connect(cfg.host, cfg.user, cfg.password, cfg.database, function (err) {
    test.ok(err === null, "Error object is not present");
    test.ok(arguments.length <= 1, "Only error arguments is possible.");

    test.done();
  });
};

exports.ConnectWithQueryInCallback = function (test) {
  test.expect(1);

  var conn = cfg.mysql_libmysqlclient.createConnectionHighlevelSync();

  conn.connect(cfg.host, cfg.user, cfg.password, cfg.database, function () {
    conn.query("SELECT 1;", function (err, res) {
      if (err) {
        throw err;
      }

      test.ok(res instanceof cfg.mysql_bindings.MysqlResult);

      test.done();
    });
  });
};

exports.ConnectWithoutCallbackAndThenQueryWithCallback = function (test) {
  test.expect(1);

  var conn = cfg.mysql_libmysqlclient.createConnectionHighlevelSync();

  conn.connect(cfg.host, cfg.user, cfg.password, cfg.database);

  conn.query("SELECT 1;", function (err, res) {
    if (err) {
      throw err;
    }

    test.ok(res instanceof cfg.mysql_bindings.MysqlResult);

    test.done();
  });
};

exports.ConnectWithoutCallbackAndThenTwoQueriesWithCallbacks = function (test) {
  test.expect(2);

  var conn = cfg.mysql_libmysqlclient.createConnectionHighlevelSync();

  conn.connect(cfg.host, cfg.user, cfg.password, cfg.database);

  conn.query("SELECT 1;", function (err, res) {
    if (err) {
      throw err;
    }

    test.ok(res instanceof cfg.mysql_bindings.MysqlResult);
  });

  conn.query("SELECT 2;", function (err, res) {
    if (err) {
      throw err;
    }

    test.ok(res instanceof cfg.mysql_bindings.MysqlResult);

    test.done();
  });
};

exports.QueryParallel = function (test) {
  test.expect(4);

  var
    conn = cfg.mysql_libmysqlclient.createConnectionHighlevelSync(cfg.host, cfg.user, cfg.password, cfg.database),
    i = 0;

  conn.query("SELECT 1;", function (err, res) {
    if (err) {
      throw err;
    }

    test.ok(res instanceof cfg.mysql_bindings.MysqlResult);

    i++;

    if (i == 4) {
      test.done();
    }
  });

  conn.query("SELECT 2;", function (err, res) {
    if (err) {
      throw err;
    }

    test.ok(res instanceof cfg.mysql_bindings.MysqlResult);

    i++;

    if (i == 4) {
      test.done();
    }
  });
  
  conn.query("SELECT 3;", function (err, res) {
    if (err) {
      throw err;
    }

    test.ok(res instanceof cfg.mysql_bindings.MysqlResult);

    i++;

    if (i == 4) {
      test.done();
    }
  });
  
  conn.query("SELECT 4;", function (err, res) {
    if (err) {
      throw err;
    }

    test.ok(res instanceof cfg.mysql_bindings.MysqlResult);

    i++;

    if (i == 4) {
      test.done();
    }
  });
};

exports.QueryParallelWithSetQueryType = function (test) {
  test.expect(6);

  var
    conn = cfg.mysql_libmysqlclient.createConnectionHighlevelSync(cfg.host, cfg.user, cfg.password, cfg.database),
    i = 0;

  conn.query("SELECT 1;", function (err, res) {
    if (err) {
      throw err;
    }

    test.ok(res instanceof cfg.mysql_bindings.MysqlResult);

    i++;

    if (i == 6) {
      test.done();
    }
  });

  conn.setQueryTypeSync('query');

  conn.query("SELECT 2;", function (err, res) {
    if (err) {
      throw err;
    }

    test.ok(res instanceof cfg.mysql_bindings.MysqlResult);

    i++;

    if (i == 6) {
      test.done();
    }
  });

  conn.setQueryTypeSync('querySend');

  conn.query("SELECT 3;", function (err, res) {
    if (err) {
      throw err;
    }

    test.ok(res instanceof cfg.mysql_bindings.MysqlResult);

    i++;

    if (i == 6) {
      test.done();
    }
  });

  conn.setQueryTypeSync('query');

  conn.query("SELECT 4;", function (err, res) {
    if (err) {
      throw err;
    }

    test.ok(res instanceof cfg.mysql_bindings.MysqlResult);

    i++;

    if (i == 6) {
      test.done();
    }
  });

  conn.setQueryTypeSync('querySend');

  conn.query("SELECT 5;", function (err, res) {
    if (err) {
      throw err;
    }

    test.ok(res instanceof cfg.mysql_bindings.MysqlResult);

    i++;

    if (i == 6) {
      test.done();
    }
  });

  conn.setQueryTypeSync('query');

  conn.query("SELECT 6;", function (err, res) {
    if (err) {
      throw err;
    }

    test.ok(res instanceof cfg.mysql_bindings.MysqlResult);

    i++;

    if (i == 6) {
      test.done();
    }
  });
};
