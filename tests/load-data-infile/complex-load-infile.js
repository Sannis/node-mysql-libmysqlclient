var cfg = require('../config.js');
var fs = require('fs');
var createLoadInfileCapableConnection = function () {
  var conn =  new cfg.mysql_libmysqlclient.bindings.MysqlConnection();
  conn.initSync();
  conn.setOptionSync(cfg.mysql_libmysqlclient.MYSQL_OPT_LOCAL_INFILE);
  conn.realConnectSync(cfg.host, cfg.user, cfg.password, cfg.database);
  if (conn.connectErrno) {
    console.log("Failed to connect as " + cfg.user + "@" + cfg.host + ": " + conn.connectError);
  }
  return conn;
};
var prepareData = function () {
  var i,
    data = [];
  for (i = 0; i < 1000; i++) {
    data.push(i + ',zzzz' + i);
  }
  return data.join('\n');
};
module.exports = {
  setUp: function (callback) {
    this.conn = createLoadInfileCapableConnection();
    this.conn.querySync("DROP TABLE IF EXISTS " + cfg.test_table);
    this.conn.querySync("CREATE TABLE " + cfg.test_table + " (field1 int, field2 text)");
    this.exampleData = new Buffer(prepareData());
    this.exampleQuery = 'LOAD DATA LOCAL INFILE \'/nothing\' INTO TABLE ' + cfg.test_table +
      " FIELDS TERMINATED BY ',' LINES TERMINATED BY '\n'";
    callback();
  },
  tearDown: function (callback) {
    if (this.conn && this.conn.connectedSync()) {
      this.conn.closeSync();
    }
    callback();
  },
  syncCloseSync:  function (test) {
    test.expect(2);
    test.ok(this.conn.querySync(this.exampleQuery, this.exampleData), "load infile syncronous");
    this.conn.closeSync();
    test.throws(
      this.conn.query.bind(
        this.conn,
        this.exampleQuery,
        this.exampleData,
        function (result) {
          test.ok(false, "there has to be exception if calling query after close");
          test.done();
        }.bind(this)
      ),
      "there has to be exception in query after closeSync"
    );
    test.done();
  },
  asyncCloseAsync: function (test) {
    test.expect(2);
    try {
      this.conn.query(
        this.exampleQuery,
        this.exampleData,
        function (error) {
          test.ok(error === null, "asyncronous load data infile breaks");
          this.conn.closeSync();
          test.throws(
            this.conn.query.bind(
              this.conn,
              this.exampleQuery,
              this.exampleData,
              function (error) {
                test.ok(false, "there has to be exception in query after closeSync");
                test.done();
              }
            )
          );
          test.done();
        }.bind(this)
      );
    } catch (e) {
      test.ok(false, "asyncronous query LOAD DATA INFILE from Buffer breaks");
      test.done();
    }
  },
  syncAsyncSync: function (test) {
    test.expect(4);
    test.ok(
      this.conn.querySync(
        this.exampleQuery,
        this.exampleData
      ),
      'synchronous load data infile query'
    );
    try {
      this.conn.query(
        this.exampleQuery,
        this.exampleData,
        function (error) {
          var result;
          test.ok(error === null, "Asynchronous load data infile after syncronous");
          result = this.conn.querySync("select count(*) as count from " + cfg.test_table);
          test.ok(result, "syncronous query after sync + async");
          result = result && result.fetchAllSync();
          result = result && result[0] && result[0].count;
          test.ok(result > 0, "Load infile fails, table is empty");
          test.done();
        }.bind(this)
      );
    } catch (e) {
      test.ok(false, "Asynchronous load data infile after syncronous");
      test.done();
    }
  },
  asyncSyncAsync: function (test) {
    test.expect(4);
    try {
      this.conn.query(
        this.exampleQuery,
        this.exampleData,
        function (error) {
          test.ok(error === null, "Asynchronous load data infile fails");
          test.ok(
            this.conn.querySync(
              this.exampleQuery,
              this.exampleData
            ),
            "synchronous data load fails"
          );
          try {
            this.conn.query(
              "SELECT count(*) as count from " + cfg.test_table,
              function (error, result) {
                test.ok(error === null, "Asyncronous query after synchronous fails");
                result = result && result.fetchAllSync();
                result = result && result[0] && result[0].count;
                test.ok(result > 0, "Load data infile asynchounous and syncronous fails");
                test.done();
              }.bind(this)
            );
          } catch (e) {
            test.ok(false, "Asyncronous query after synchronous fails");
            test.done();
          }
        }.bind(this)
      );
    } catch (e) {
      test.ok(false, "Asynchronous load data infile after syncronous");
      test.done();
    }
  }
};
