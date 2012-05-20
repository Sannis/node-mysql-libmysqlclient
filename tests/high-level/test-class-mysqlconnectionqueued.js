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

exports.ConnectWithCallback = function (test) {
  test.expect(0);

  var conn = cfg.mysql_libmysqlclient.createConnectionQueuedSync();

  conn.connect(cfg.host, cfg.user, cfg.password, cfg.database, function () {
    test.done();
  });
};

exports.ConnectWithQueryInCallback = function (test) {
  test.expect(0);

  var conn = cfg.mysql_libmysqlclient.createConnectionQueuedSync();

  conn.connect(cfg.host, cfg.user, cfg.password, cfg.database, function () {
    conn.query("SELECT 1;", function (err) {
      if (err) {
        throw err;
      }

      test.done();
    });
  });
};

exports.ConnectWithoutCallbackAndThenQueryWithCallback = function (test) {
  test.expect(0);

  var conn = cfg.mysql_libmysqlclient.createConnectionQueuedSync();

  conn.connect(cfg.host, cfg.user, cfg.password, cfg.database);

  conn.query("SELECT 1;", function (err) {
    if (err) {
      throw err;
    }

    test.done();
  });
};

exports.ConnectWithoutCallbackAndThenTwoQueriesWithCallbacks = function (test) {
  test.expect(0);

  var conn = cfg.mysql_libmysqlclient.createConnectionQueuedSync();

  conn.connect(cfg.host, cfg.user, cfg.password, cfg.database);

  conn.query("SELECT 1;", function (err) {
    if (err) {
      throw err;
    }
  });

  conn.query("SELECT 2;", function (err) {
    if (err) {
      throw err;
    }

    test.done();
  });
};

exports.QueryParallel = function (test) {
  test.expect(0);

  var
    conn = cfg.mysql_libmysqlclient.createConnectionQueuedSync(cfg.host, cfg.user, cfg.password, cfg.database),
    i = 0;

  conn.query("SELECT 1;", function (err, result) {
    if (err) {
      throw err;
    }

    i++;

    if (i == 2) {
      test.done();
    }
  });

  conn.query("SELECT 2;", function (err, result) {
    if (err) {
      throw err;
    }

    i++;

    if (i == 2) {
      test.done();
    }
  });
};

exports.QuerySendParallel = function (test) {
  test.expect(0);

  var
    conn = cfg.mysql_libmysqlclient.createConnectionQueuedSync(cfg.host, cfg.user, cfg.password, cfg.database),
    i = 0;

  conn.querySend("SELECT 1;", function (err, result) {
    if (err) {
      throw err;
    }

    i++;

    if (i == 2) {
      test.done();
    }
  });

  conn.querySend("SELECT 2;", function (err, result) {
    if (err) {
      throw err;
    }

    i++;

    if (i == 2) {
      test.done();
    }
  });
};

exports.QueryAndQuerySendParallel = function (test) {
  test.expect(0);

  var
    conn = cfg.mysql_libmysqlclient.createConnectionQueuedSync(cfg.host, cfg.user, cfg.password, cfg.database),
    i = 0;

  conn.query("SELECT 1;", function (err, result) {
    if (err) {
      throw err;
    }

    i++;

    if (i == 4) {
      test.done();
    }
  });

  conn.querySend("SELECT 2;", function (err, result) {
    if (err) {
      throw err;
    }

    i++;

    if (i == 4) {
      test.done();
    }
  });
  
  conn.query("SELECT 3;", function (err, result) {
    if (err) {
      throw err;
    }

    i++;

    if (i == 4) {
      test.done();
    }
  });
  
  conn.querySend("SELECT 4;", function (err, result) {
    if (err) {
      throw err;
    }

    i++;

    if (i == 4) {
      test.done();
    }
  });
};

